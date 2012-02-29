/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_CONSOLE_AUTO_MODE_HPP_
#define UTIL_CONSOLE_AUTO_MODE_HPP_

#include <string>

#include <boost/lexical_cast.hpp>

#include <util/console/mode.hpp>
#include <util/functional/member-function-registry.hpp>

namespace util {

namespace console {

template <class Class>
class auto_mode :
	public mode
{
protected:
	auto_mode (Class* _this,
			const std::string& __default_prompt = "> ")
		: mode(__default_prompt), _target_this(_this)
	{}

	template <typename Function>
	void register_function (const std::string& name, Function f)
	{
		_registry.register_function(name, f);
	}

	bool handle_command (const std::string& command, const std::vector<std::string>& args)
	{
		if (!_registry.has_function (command))
			return false;

		ret_parser result;
		_registry.call(_target_this, command, result, args);
		if (result.has_been_set())
			out() << result.result() << std::endl;
		return true;
	}

private:
	typedef std::vector<std::string> arg_pack;

	class arg_parser
	{
	public:
		arg_parser (const arg_pack& args)
			: _args(args), _iter(_args.begin())
		{}

		template <typename Result>
		Result pop ()
		{
			if (_iter == _args.end())
				throw std::out_of_range("no more arguments");

			return boost::lexical_cast<Result> (*_iter++);
		}

	private:
		const arg_pack& _args;
		arg_pack::const_iterator _iter;
	};

	class ret_parser
	{
	public:
		typedef std::string return_type;

		ret_parser ()
			: _has_been_set(false)
		{}

		template <typename Type>
		void handle (const Type& v)
		{
			_result = boost::lexical_cast<std::string>(v);
			_has_been_set = true;
		}

		const std::string& result () const
		{
			return _result;
		}

		bool has_been_set () const
		{
			return _has_been_set;
		}

	private:
		bool _has_been_set;
		std::string _result;
	};

	Class* _target_this;
	functional::member_function_registry<Class,
		arg_parser, arg_pack, ret_parser,
		std::string> _registry;
};

}  // namespace console

}  // namespace util

#endif /* UTIL_CONSOLE_AUTO_MODE_HPP_ */
