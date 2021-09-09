//
// Created by max on 9/8/21.
//

#include <mabiphmo/ioc-container/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(Dependency)

	BOOST_AUTO_TEST_CASE(Simple)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::make_shared<A>(3u));
		uut->RegisterSingleton(std::function([](Container::Dependency<A> a, unsigned value){return std::make_shared<B>(a.value, value);}));
		auto inst = uut->Resolve<B>(5u);
		BOOST_TEST(inst->b == 5u);
		BOOST_TEST(inst->a->a == 3u);
	}

	BOOST_AUTO_TEST_CASE(Injection)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function([](unsigned val){return std::make_shared<CImpl>(val);}));
		uut->RegisterOnInterface<IC, CImpl>(5u);
		uut->RegisterSingleton(std::function([](Container::Injection<IC> c) {return std::make_shared<D>(std::make_shared<B>(std::make_shared<A>(3), 4), c.value, 8);}));
		auto inst = uut->Resolve<D>();
		BOOST_TEST(inst->sum == 20u);
		BOOST_TEST(inst->c->C() == 5u);
	}

	BOOST_AUTO_TEST_CASE(InjectionWithId)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterFactory(std::function([](unsigned val){return std::make_shared<CImpl>(val);}));
		uut->RegisterOnInterface<IC, CImpl, "5">(5u);
		uut->RegisterOnInterface<IC, CImpl, "3">(3u);
		uut->RegisterFactory(std::function([](Container::InjectionRuntimeResolved<IC> c) {return std::make_shared<D>(std::make_shared<B>(std::make_shared<A>(3), 4), c.value, 8);}));
		auto inst = uut->Resolve<D>(std::string("5"));
		BOOST_TEST(inst->sum == 20u);
		BOOST_TEST(inst->c->C() == 5u);
		inst = uut->Resolve<D>(std::string("3"));
		BOOST_TEST(inst->sum == 18u);
		BOOST_TEST(inst->c->C() == 3u);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()