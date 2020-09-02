//
// Created by max on 8/14/20.
//

#include <mabiphmo/ioc/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(class_factory)

	BOOST_AUTO_TEST_CASE(registerAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterClassFactory<A, std::tuple<unsigned>, std::tuple<>>();
		std::shared_ptr<A> aInst3 = uut.GetInstance<A>((unsigned)3);
		std::shared_ptr<A> aInst2 = uut.GetInstance<A>((unsigned)2);
		BOOST_TEST(aInst3 != aInst2);
		BOOST_TEST(aInst3->a == 3);
		BOOST_TEST(aInst2->a == 2);
		BOOST_TEST(aInst3 != uut.GetInstance<A>((unsigned)3));
		BOOST_TEST(aInst2 != uut.GetInstance<A>((unsigned)2));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactory<A>(3);
		uut.RegisterClassFactory<B, std::tuple<unsigned>, std::tuple<A>>();
		std::shared_ptr<B> bInst5 = uut.GetInstance<B>((unsigned)5);
		std::shared_ptr<B> bInst6 = uut.GetInstance<B>((unsigned)6);
		std::shared_ptr<A> aInst = uut.GetInstance<A>();
		BOOST_TEST(bInst5 != bInst6);
		BOOST_TEST(bInst5->b == 5);
		BOOST_TEST(bInst6->b == 6);
		BOOST_TEST(bInst5 != uut.GetInstance<B>((unsigned)5));
		BOOST_TEST(bInst6 != uut.GetInstance<B>((unsigned)6));
		BOOST_TEST(bInst6->a == aInst);
		BOOST_TEST(bInst5->a == aInst);
		BOOST_TEST(aInst->a == 3);
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterClassFactoryOnInterface<IC, CImpl, std::tuple<unsigned>, std::tuple<>>();
		std::shared_ptr<IC> cInst5 = uut.GetInstance<IC>((unsigned)5);
		std::shared_ptr<IC> cInst6 = uut.GetInstance<IC>((unsigned)6);
		BOOST_TEST(cInst5 != cInst6);
		BOOST_TEST(cInst5->C() == 5);
		BOOST_TEST(cInst6->C() == 6);
		BOOST_TEST(cInst5 != uut.GetInstance<IC>((unsigned)5));
		BOOST_TEST(cInst6 != uut.GetInstance<IC>((unsigned)6));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactoryOnInterface<IC, CImpl>(10);
		uut.RegisterSingletonClassFactory<A>(3);
		uut.RegisterSingletonClassFactory<B, A>(5);
		uut.RegisterClassFactory<D, std::tuple<unsigned>, std::tuple<B, IC>>();
		std::shared_ptr<D> dInst2 = uut.GetInstance<D>((unsigned)2);
		std::shared_ptr<D> dInst3 = uut.GetInstance<D>((unsigned)3);
		std::shared_ptr<IC> cInst = uut.GetInstance<IC>();
		std::shared_ptr<B> bInst = uut.GetInstance<B>();
		std::shared_ptr<A> aInst = uut.GetInstance<A>();
		BOOST_TEST(dInst2 != dInst3);
		BOOST_TEST(dInst2->sum == 20);
		BOOST_TEST(dInst3->sum == 21);
		BOOST_TEST(dInst2->d == 2);
		BOOST_TEST(dInst3->d == 3);
		BOOST_TEST(dInst2 != uut.GetInstance<D>((unsigned)2));
		BOOST_TEST(dInst3 != uut.GetInstance<D>((unsigned)3));
		BOOST_TEST(dInst2->c == cInst);
		BOOST_TEST(dInst3->c == cInst);
		BOOST_TEST(cInst->C() == 10);
		BOOST_TEST(dInst2->b == bInst);
		BOOST_TEST(dInst3->b == bInst);
		BOOST_TEST(bInst->b == 5);
		BOOST_TEST(bInst->a == aInst);
		BOOST_TEST(aInst->a == 3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()