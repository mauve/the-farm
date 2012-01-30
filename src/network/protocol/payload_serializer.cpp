/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "payload_serializer.hpp"

#include <stdexcept>

#include <boost/variant/apply_visitor.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/mpl/index_of.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/same_as.hpp>

namespace network {

namespace transport {

using network::protocol::payload;

namespace _detail {

// this little struct maps the type
// to an index in the payload_item_t
// variant, that index is then used
// to identify the type on the wire
template <typename PayloadItem>
struct prefix
{
	enum { constant = boost::mpl::index_of<
						protocol::payload_item_t::types,
						PayloadItem
					  >::type::value };

	static boost::uint8_t value()
	{
		return constant;
	}
};

/*
 * compute_size_visitor
 */

struct compute_size_visitor :
	public boost::static_visitor<std::size_t>
{
	const static std::size_t prefix = sizeof(boost::uint8_t);

	std::size_t operator() (bool) const
	{
		return prefix + sizeof(boost::uint8_t);
	}

	template <typename Integral>
	typename boost::enable_if<
		boost::is_integral<Integral>,
		std::size_t
	>::type operator() (Integral) const
	{
		return prefix + sizeof(Integral);
	}

	std::size_t operator() (const std::string& str) const
	{
		return prefix + sizeof(boost::uint16_t) + str.length();
	}

	std::size_t operator() (const protocol::node_item&) const
	{
		return prefix + sizeof(boost::uint32_t);
	}

	template <typename Integral>
	typename boost::enable_if<
		boost::is_integral<Integral>,
		std::size_t
	>::type
	operator() (const std::vector<Integral>& vec) const
	{
		return prefix + sizeof(boost::uint32_t) +
				sizeof(Integral) * vec.size();
	}

	std::size_t operator() (const std::vector<std::string>& vec) const
	{
		std::size_t size = prefix;
		for (std::vector<std::string>::const_iterator iter = vec.begin();
				iter != vec.end(); ++iter)
		{
			size += sizeof(boost::uint16_t) + iter->length();
		}
		return size;
	}
};

/*
 * serialize_visitor
 */

struct serialize_visitor :
	public boost::static_visitor<>
{
	mutable util::memory_output_stream& mos;

	serialize_visitor (util::memory_output_stream& _mos)
		: mos(_mos)
	{}

	void operator() (bool b) const
	{
		mos << prefix<bool>::value() <<
			static_cast<boost::uint8_t>(b);
	}

	template <typename Integral>
	typename boost::enable_if<
		boost::is_integral<Integral>
	>::type operator() (Integral integral) const
	{
		mos << prefix<Integral>::value() << integral;
	}

	void operator() (const std::string& str) const
	{
		mos << prefix<std::string>::value() <<
				static_cast<boost::uint16_t>(str.length());
		mos.write(str.c_str(), str.length());
	}

	void operator() (const protocol::node_item& node) const
	{
		union {
			struct {
				boost::uint16_t type;
				boost::uint16_t length;
			} parts;
			boost::uint32_t data;
		} value;

		value.parts.type = node.type();
		value.parts.length = node.length();

		mos << prefix<protocol::node_item>::value() << value.data;
	}

	template <typename Integral>
	typename boost::enable_if<
		boost::is_integral<Integral>
	>::type operator() (const std::vector<Integral>& vec) const
	{
		mos << prefix<std::vector<Integral> >::value() <<
				static_cast<boost::uint32_t>(vec.size());
		for (typename std::vector<Integral>::const_iterator iter = vec.begin();
				iter != vec.end(); ++iter)
		{
			mos << *iter;
		}
	}

	void operator() (const std::vector<std::string>& vec) const
	{
		mos << prefix<std::vector<std::string> >::value() <<
				static_cast<boost::uint32_t>(vec.size());
		for (std::vector<std::string>::const_iterator iter = vec.begin();
				iter != vec.end(); ++iter)
		{
			mos << static_cast<boost::uint16_t>(iter->length());
			mos.write(iter->c_str(), iter->length());
		}
	}
};

/*
 * helper for extraction of std::vector<>
 */
template <typename T>
struct vector_extractor
{
	static void extract (util::memory_input_stream& mis, payload& p)
	{
		boost::uint16_t length = mis.extract<boost::uint16_t>();
		const char* data = mis.reserve(sizeof(T) * length);

		// to avoid copying
		p.push_back(std::vector<T>());
		std::vector<T>& vec = boost::get<std::vector<T> >(p.back());
		std::memcpy(&vec[0], data, sizeof(T) * length);
	}
};

}  // namespace _detail

/*
 * serializer<payload>
 */

serializer<payload>::serializer (const payload& p)
	: _payload(p)
{}

std::size_t serializer<payload>::size () const
{
	std::size_t size = 0;
	for (payload::const_iterator iter = _payload.begin ();
			iter != _payload.end(); ++iter)
	{
		size += boost::apply_visitor( _detail::compute_size_visitor(), *iter );
	}
	return size;
}

void serializer<payload>::serialize (util::memory_output_stream& mos) const
{
	mos << static_cast<boost::uint16_t>(_payload.size());
	for (payload::const_iterator iter = _payload.begin ();
			iter != _payload.end(); ++iter)
	{
		boost::apply_visitor( _detail::serialize_visitor(mos), *iter );
	}
}

/*
 * deserializer<payload>
 */

void deserializer<payload>::deserialize (util::memory_input_stream& mis, payload& p)
{
	using _detail::prefix;

	boost::uint16_t num_items = 0;
	mis >> num_items;

	p.reserve(num_items);

	for (boost::uint16_t i = 0; i < num_items; ++i)
	{
		boost::uint8_t prefix_value = 0;
		mis >> prefix_value;

		/*
		 * should perhaps convert this to some
		 * awesome meta-programming stuff, could
		 * perhaps try boost::mpl::for_each
		 */
		switch (prefix_value)
		{
			case prefix<bool>::constant: {
				p.push_back(static_cast<bool>(mis.extract<uint8_t>()));
				break;
			}

			case prefix<boost::int32_t>::constant: {
				p.push_back(mis.extract<boost::int32_t>());
				break;
			}

			case prefix<boost::uint32_t>::constant: {
				p.push_back(mis.extract<boost::uint32_t>());
				break;
			}

			case prefix<boost::int64_t>::constant: {
				p.push_back(mis.extract<boost::int64_t>());
				break;
			}

			case prefix<boost::uint64_t>::constant: {
				p.push_back(mis.extract<boost::uint64_t>());
				break;
			}

			case prefix<std::string>::constant: {
				boost::uint16_t length = mis.extract<boost::uint16_t>();
				const char* str = mis.reserve(length);

				// to avoid copying
				p.push_back(std::string());
				boost::get<std::string>(p.back()).assign(str, length);
				break;
			}

			case prefix<std::vector<boost::uint8_t> >::constant: {
				_detail::vector_extractor<boost::uint8_t>::extract(mis, p);
				break;
			}

			case prefix<std::vector<boost::int32_t> >::constant: {
				_detail::vector_extractor<boost::int32_t>::extract(mis, p);
				break;
			}

			case prefix<std::vector<boost::uint32_t> >::constant: {
				_detail::vector_extractor<boost::uint32_t>::extract(mis, p);
				break;
			}

			case prefix<std::vector<boost::int64_t> >::constant: {
				_detail::vector_extractor<boost::int64_t>::extract(mis, p);
				break;
			}

			case prefix<std::vector<boost::uint64_t> >::constant: {
				_detail::vector_extractor<boost::uint64_t>::extract(mis, p);
				break;
			}

			case prefix<std::vector<std::string> >::constant: {
				boost::uint16_t length = mis.extract<boost::uint16_t>();

				// to avoid copying
				p.push_back(std::vector<std::string>());
				std::vector<std::string>& vec = boost::get<std::vector<std::string> >(p.back());

				// again to avoid copying
				vec.reserve(length);
				for (boost::uint16_t i = 0; i < length; ++i)
				{
					// and again to avoid copying
					vec.push_back(std::string());

					boost::uint16_t str_length = mis.extract<boost::uint16_t>();
					if (!str_length)
						continue;

					std::string& str = vec.back();
					str.assign(mis.reserve(str_length), str_length);
				}
				break;
			}

			default:
				// replace exception with custom
				throw std::runtime_error("invalid_data");
		}
	}
}

}  // namespace transport

}  // namespace network

