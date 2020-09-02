//
// Created by max on 8/13/20.
//

#include <mabiphmo/ioc/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(singleton_factory)

	BOOST_AUTO_TEST_CASE(registerAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactory<A>(3);
		std::shared_ptr<A> aInst = uut.GetInstance<A>();
		BOOST_TEST(aInst->a == 3);
		BOOST_TEST(aInst == uut.GetInstance<A>());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactory<A>(3);
		uut.RegisterSingletonClassFactory<B, A>(5);
		std::shared_ptr<B> bInst = uut.GetInstance<B>();
		std::shared_ptr<A> aInst = uut.GetInstance<A>();
		BOOST_TEST(bInst->b == 5);
		BOOST_TEST(bInst == uut.GetInstance<B>());
		BOOST_TEST(bInst->a->a == 3);
		BOOST_TEST(bInst->a == aInst);
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactoryOnInterface<IC, CImpl>(10);
		std::shared_ptr<IC> cInst = uut.GetInstance<IC>();
		BOOST_TEST(cInst->C() == 10);
		BOOST_TEST(cInst == uut.GetInstance<IC>());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactoryOnInterface<IC, CImpl>(10);
		uut.RegisterSingletonClassFactory<A>(3);
		uut.RegisterSingletonClassFactory<B, A>(5);
		uut.RegisterSingletonClassFactory<D, B, IC>(2);
		std::shared_ptr<D> dInst = uut.GetInstance<D>();
		std::shared_ptr<IC> cInst = uut.GetInstance<IC>();
		std::shared_ptr<B> bInst = uut.GetInstance<B>();
		std::shared_ptr<A> aInst = uut.GetInstance<A>();
		BOOST_TEST(dInst->sum == 20);
		BOOST_TEST(dInst->d == 2);
		BOOST_TEST(dInst == uut.GetInstance<D>());
		BOOST_TEST(dInst->c->C() == 10);
		BOOST_TEST(dInst->c == cInst);
		BOOST_TEST(dInst->b->b == 5);
		BOOST_TEST(dInst->b == bInst);
		BOOST_TEST(dInst->b->a->a == 3);
		BOOST_TEST(dInst->b->a == aInst);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()