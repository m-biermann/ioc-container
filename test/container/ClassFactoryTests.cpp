//
// Created by max on 8/14/20.
//

#include <mabiphmo/ioc-container/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(class_factory)

	BOOST_AUTO_TEST_CASE(registerAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterFactory(std::function<std::shared_ptr<A>(unsigned)>([](unsigned val){return std::make_shared<A>(val);}));
		auto aInst3 = uut->Get<A>((unsigned)3);
		auto aInst2 = uut->Get<A>((unsigned)2);
		BOOST_TEST(aInst3 != aInst2);
		BOOST_TEST(aInst3->a == (unsigned)3);
		BOOST_TEST(aInst2->a == (unsigned)2);
		BOOST_TEST(aInst3 != uut->Get<A>((unsigned)3));
		BOOST_TEST(aInst2 != uut->Get<A>((unsigned)2));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);}));
		uut->RegisterFactory(std::function<std::shared_ptr<B>(std::shared_ptr<A>, unsigned)>([](std::shared_ptr<A> a, unsigned val){return std::make_shared<B>(a, val);}));
        std::shared_ptr<B> bInst5 = uut->Get<B>((unsigned)5);
        std::shared_ptr<B> bInst6 = uut->Get<B>((unsigned)6);
		std::shared_ptr<A> aInst = uut->Get<A>();
		BOOST_TEST(bInst5 != bInst6);
		BOOST_TEST(bInst5->b == (unsigned)5);
		BOOST_TEST(bInst6->b == (unsigned)6);
		BOOST_TEST(bInst5 != uut->Get<B>((unsigned)5));
		BOOST_TEST(bInst6 != uut->Get<B>((unsigned)6));
		BOOST_TEST(bInst6->a == aInst);
		BOOST_TEST(bInst5->a == aInst);
		BOOST_TEST(aInst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterFactory(std::function<std::shared_ptr<CImpl>(unsigned)>([](unsigned val){return std::make_shared<CImpl>(val);}));
		uut->RegisterOnInterface<IC, CImpl, unsigned>();
		auto cInst5 = uut->Get<IC>((unsigned)5);
		auto cInst6 = uut->Get<IC>((unsigned)6);
		BOOST_TEST(cInst5 != cInst6);
		BOOST_TEST(cInst5->C() == (unsigned)5);
		BOOST_TEST(cInst6->C() == (unsigned)6);
		BOOST_TEST(cInst5 != uut->Get<IC>((unsigned)5));
		BOOST_TEST(cInst6 != uut->Get<IC>((unsigned)6));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterFactory(std::function<std::shared_ptr<D>(std::shared_ptr<B>, std::shared_ptr<IC>, unsigned)>([](std::shared_ptr<B> b, std::shared_ptr<IC> c, unsigned d){return std::make_shared<D>(b, c, d);}));
		uut->RegisterFactory(std::function<std::shared_ptr<CImpl>()>([](){return std::make_shared<CImpl>(10);}));
		uut->RegisterOnInterface<IC, CImpl>();
		uut->RegisterSingleton(std::function<std::shared_ptr<B>(std::shared_ptr<A>)>([](std::shared_ptr<A> a){return std::make_shared<B>(a, 5);}));
		uut->RegisterSingleton(std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);}));

		std::shared_ptr<D> dInst2 = uut->Get<D>((unsigned)2);
		std::shared_ptr<D> dInst3 = uut->Get<D>((unsigned)3);
		std::shared_ptr<IC> cInst = uut->Get<IC>();
		std::shared_ptr<B> bInst = uut->Get<B>();
		std::shared_ptr<A> aInst = uut->Get<A>();
		BOOST_TEST(dInst2 != dInst3);
		BOOST_TEST(dInst2->sum == (unsigned)20);
		BOOST_TEST(dInst3->sum == (unsigned)21);
		BOOST_TEST(dInst2->d == (unsigned)2);
		BOOST_TEST(dInst3->d == (unsigned)3);
		BOOST_TEST(dInst2 != uut->Get<D>((unsigned)2));
		BOOST_TEST(dInst3 != uut->Get<D>((unsigned)3));
		BOOST_TEST(dInst2->c != cInst);
		BOOST_TEST(dInst3->c != cInst);
		BOOST_TEST(cInst->C() == (unsigned)10);
		BOOST_TEST(dInst2->b == bInst);
		BOOST_TEST(dInst3->b == bInst);
		BOOST_TEST(bInst->b == (unsigned)5);
		BOOST_TEST(bInst->a == aInst);
		BOOST_TEST(aInst->a == (unsigned)3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()