//
// Created by max on 8/13/20.
//

#include <mabiphmo/ioc-container/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(singleton_factory)

	BOOST_AUTO_TEST_CASE(registerAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);}));
		auto inst = uut->Get<A>();
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(inst == uut->Get<A>());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		try{
			auto uut = std::make_shared<Container>();
			uut->RegisterSingleton(std::function<std::shared_ptr<B>(std::shared_ptr<A>)>([](std::shared_ptr<A> a){return std::make_shared<B>(a, 5);}));
			uut->RegisterSingleton(std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);}));
			auto bInst = uut->Get<B>();
			auto aInst = uut->Get<A>();
			BOOST_TEST(bInst->b == (unsigned)5);
			BOOST_TEST(bInst == uut->Get<B>());
			BOOST_TEST(bInst->a->a == (unsigned)3);
			BOOST_TEST(bInst->a == aInst);
		}
		catch (const std::out_of_range& oor) {
			std::cerr << "Out of Range error: " << oor.what() << '\n';
		}
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function<std::shared_ptr<CImpl>()>([](){return std::make_shared<CImpl>(10);}));
		uut->RegisterOnInterface<IC, CImpl>();
		auto cInst = uut->Get<IC>();
		BOOST_TEST(cInst->C() == (unsigned)10);
		BOOST_TEST(cInst == uut->Get<IC>());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function<std::shared_ptr<D>(std::shared_ptr<B>, std::shared_ptr<IC>)>([](std::shared_ptr<B> b, std::shared_ptr<IC> c){return std::make_shared<D>(b, c, 2);}));
		uut->RegisterSingleton(std::function<std::shared_ptr<CImpl>()>([](){return std::make_shared<CImpl>(10);}));
		uut->RegisterOnInterface<IC, CImpl>();
		uut->RegisterSingleton(std::function<std::shared_ptr<B>(std::shared_ptr<A>)>([](std::shared_ptr<A> a){return std::make_shared<B>(a, 5);}));
		uut->RegisterSingleton(std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);}));
        auto dInst = uut->Get<D>();
        auto cInst = uut->Get<IC>();
        auto bInst = uut->Get<B>();
        auto aInst = uut->Get<A>();
		BOOST_TEST(dInst->sum == (unsigned)20);
		BOOST_TEST(dInst->d == (unsigned)2);
		BOOST_TEST(dInst == uut->Get<D>());
		BOOST_TEST(dInst->c->C() == (unsigned)10);
		BOOST_TEST(dInst->c == cInst);
		BOOST_TEST(dInst->b->b == (unsigned)5);
		BOOST_TEST(dInst->b == bInst);
		BOOST_TEST(dInst->b->a->a == (unsigned)3);
		BOOST_TEST(dInst->b->a == aInst);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()