//
// Created by max on 8/15/20.
//

#define BOOST_TEST_MODULE container
#include <boost/test/unit_test.hpp>
#include <mabiphmo/ioc-container/Container.h>
#include "argStructs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(Args)

	BOOST_AUTO_TEST_CASE(lvalue)
	{
		auto uut = std::make_shared<Container>();
		auto arg = 3u;
		uut->RegisterSingleton(std::make_shared<lvalueArgs>(arg));
		auto inst = uut->Get<lvalueArgs>();
		BOOST_TEST(inst->a == 3u);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReference)
	{
		auto uut = std::make_shared<Container>();
		auto arg = 3u;
		uut->RegisterSingleton(std::make_shared<lvalueRefArgs>(arg));
		auto inst = uut->Get<lvalueRefArgs>();
		BOOST_TEST(inst->a == 3u);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReference)
	{
		auto uut = std::make_shared<Container>();
		auto arg = 3u;
		uut->RegisterSingleton(std::make_shared<lvalueConstRefArgs>(arg));
		auto inst = uut->Get<lvalueConstRefArgs>();
		BOOST_TEST(inst->a == 3u);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(rvalueReference)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::make_shared<rvalueRefArgs>(3));
		auto inst = uut->Get<rvalueRefArgs>();
		BOOST_TEST(inst->a == 3u);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()