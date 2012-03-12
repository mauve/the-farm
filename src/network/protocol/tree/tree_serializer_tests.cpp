
#include <boost/test/unit_test.hpp>

#include <network/protocol/payload_stream.hpp>
#include <network/protocol/tree/tree_serializer.hpp>
#include <network/protocol/tree/std/vector_tree_serializer.hpp>

#include <typeinfo>

BOOST_AUTO_TEST_SUITE(network)

BOOST_AUTO_TEST_SUITE(protocol)

BOOST_AUTO_TEST_SUITE(tree_tests)

BOOST_AUTO_TEST_CASE(serialize_simple)
{
	payload p;
	payload_output_stream pos(p);

	tree::serializer::serialize(boost::int32_t(5), pos);

	BOOST_CHECK_EQUAL(p.size(), 1);
	BOOST_CHECK(p[0].type() == typeid(boost::int32_t));
	BOOST_CHECK_EQUAL(boost::get<boost::int32_t>(p[0]), 5);
}

BOOST_AUTO_TEST_CASE(ensure_tree_serializer_isnt_used)
{
	payload p;
	payload_output_stream pos(p);

	tree::serializer::serialize(std::vector<boost::uint32_t>(), pos);

	BOOST_CHECK_EQUAL(p.size(), 1);
	BOOST_CHECK_MESSAGE(p[0].type() == typeid(std::vector<boost::uint32_t>),
			"invalid type serialized, got " << p[0].type().name() <<
			" expected " << typeid(std::vector<boost::uint32_t>).name());
}


BOOST_AUTO_TEST_SUITE_END() // tree_tests

BOOST_AUTO_TEST_SUITE_END() // protocol

BOOST_AUTO_TEST_SUITE_END() // network


