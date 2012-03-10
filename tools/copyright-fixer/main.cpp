/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include <util/iostream/readfile.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

const char* copyright =
"/*\n"
" * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>\n"
" */\n";

bool has_extension (const fs::path& p, const std::vector<std::string>& extensions)
{
	return std::find(extensions.begin(), extensions.end(),
						p.extension().string()) != extensions.end();
}

std::vector<std::string> split_entries_by_comma (const std::vector<std::string>& exts)
{
	std::vector<std::string> result;
	for (std::vector<std::string>::const_iterator iter = exts.begin();
			iter != exts.end(); ++iter)
	{
		typedef boost::char_separator<char> separator;
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

		tokenizer tok(*iter, separator(","));
		for (tokenizer::iterator iter = tok.begin();
				iter != tok.end(); ++iter)
		{
			result.push_back(*iter);
		}
	}

	return result;
}

int main (int argc, char* argv[])
{
	po::options_description opts("Options");
	opts.add_options()
		("help,h", "Shows this help")
		("ext,e", po::value<std::vector<std::string> >(), "Extensions to select")
		("base,b", po::value<std::string>(), "Base directory to search")
		("dry-run,n", "Do not change any files, just report what would be done.")
		("verbose,v", "Be verbose");

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, opts), vm);
		po::notify(vm);
	} catch (const std::exception& ex) {
		std::cerr << "Invalid command line: " << std::endl;
		std::cerr << "  " << ex.what() << std::endl;
		return 1;
	}

	if (vm.count("help")) {
		std::cout << opts << std::endl;
		return 0;
	}

	if (!vm.count("ext") || !vm.count("base")) {
		std::cerr << "Error missing argument: --ext or --base" << std::endl;
		return 1;
	}

	try {
		bool dry_run = !!vm.count("dry-run");
		bool verbose = !!vm.count("verbose");

		std::vector<std::string> exts = split_entries_by_comma(vm["ext"].as<std::vector<std::string> >());
		std::string base = vm["base"].as<std::string> ();

		fs::recursive_directory_iterator iter(base);
		fs::recursive_directory_iterator end;
		for (; iter != end; ++iter) {
			const fs::path& p = *iter;

			if (verbose)
				std::cerr << "Checking: " << p << std::endl;

			if (!fs::is_regular_file(iter.status())) {
				if (verbose)
					std::cerr << "  Not a regular file." << std::endl;
				continue;
			}

			if (!has_extension(p, exts)) {
				if (verbose)
					std::cerr << "  Wrong extension." << std::endl;
				continue;
			}

			std::vector<char> buf;
			if (!util::readfile(p.string(), buf)) {
				std::cerr << "Error: could not open: " << p << std::endl;
				continue;
			}

			std::size_t copyright_length = std::strlen(copyright);
			bool needs_fixing = false;
			if (buf.size() >= copyright_length) {
				if (!std::equal(buf.begin(), buf.begin() + copyright_length, copyright)) {
					if (verbose)
						std::cerr << "  Needs fixing: Does not start with copyright." << std::endl;
					needs_fixing = true;
				}
			} else {
				if (verbose)
					std::cerr << "  Needs fixing: Not big enough for correct copyright." << std::endl;
				needs_fixing = true;
			}

			if (needs_fixing) {
				std::cout << "Adding copyright to: " << p << std::endl;
				if (!dry_run) {
					std::ofstream file(p.string().c_str());
					if (!file.write(copyright, std::strlen(copyright))) {
						std::cerr << "  Could not write copyright." << std::endl;
						continue;
					}
					if (!file.write(&buf[0], buf.size())) {
						std::cerr << "  Could not write original content." << std::endl;
					}
				}
			} else if (verbose) {
				std::cerr << "  Has correct copyright." << std::endl;
			}
		}
	} catch (const std::exception& ex) {
		std::cerr << "ERROR: " << ex.what () << std::endl;
		return 1;
	}

	return 0;
}
