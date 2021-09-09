//
// Created by max on 9/8/21.
//

#include <mabiphmo/ioc-container/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(Scopes)

	BOOST_AUTO_TEST_CASE(SingletonInstance)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::make_shared<A>(3));
		std::shared_ptr<A> aInst = uut->Resolve<A>();
		BOOST_TEST(aInst == uut->Resolve<A>());
	}

	BOOST_AUTO_TEST_CASE(SingletonFactory)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);}));
		auto inst = uut->Resolve<A>();
		BOOST_TEST(inst == uut->Resolve<A>());
	}

	BOOST_AUTO_TEST_CASE(Transient)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterTransient(
				std::function<std::shared_ptr<A>(unsigned)>([](unsigned val) { return std::make_shared<A>(val); }));
		auto aInst3 = uut->Resolve<A>(3u);
		auto aInst2 = uut->Resolve<A>(2u);
		BOOST_TEST(aInst3 != aInst2);
		BOOST_TEST(aInst3 != uut->Resolve<A>(3u));
		BOOST_TEST(aInst2 != uut->Resolve<A>(2u));
	}

	BOOST_AUTO_TEST_CASE(Interface)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function<std::shared_ptr<CImpl>(unsigned)>([](unsigned val){return std::make_shared<CImpl>(val);}));
		uut->RegisterOnInterface<IC, CImpl, "", unsigned>();
		auto inst = uut->Resolve<IC>(5u);
		BOOST_TEST(inst->C() == 5u);
	}

	BOOST_AUTO_TEST_CASE(InterfaceDefinedValue)
	{
		auto uut = std::make_shared<Container>();
		auto value = 5u;
		uut->RegisterSingleton(std::function([](unsigned val){return std::make_shared<CImpl>(val);}));
		uut->RegisterOnInterface<IC, CImpl>(value);
		auto inst = uut->Resolve<IC>();
		BOOST_TEST(inst->C() == value);
	}

	BOOST_AUTO_TEST_CASE(InterfaceDefinedValueReference)
	{
		auto uut = std::make_shared<Container>();
		auto value = 5u;
		uut->RegisterSingleton(std::function([](unsigned &val){return std::make_shared<CImpl2>(val);}));
		uut->RegisterOnInterface<IC, CImpl2>(value);
		auto inst = uut->Resolve<IC>();
		BOOST_TEST(inst->C() == value);
		BOOST_TEST((value = 3u, inst->C() == 3u));
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()