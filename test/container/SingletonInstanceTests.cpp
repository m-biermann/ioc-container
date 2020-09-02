//
// Created by max on 8/13/20.
//

#define BOOST_TEST_MODULE container
#include <mabiphmo/ioc/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(singleton_instance)

BOOST_AUTO_TEST_CASE(registerAndGet)
{
	mabiphmo::ioc::Container uut;
	uut.RegisterSingletonClass<A>(3);
	std::shared_ptr<A> aInst = uut.GetInstance<A>();
	BOOST_TEST(aInst->a == 3);
	BOOST_TEST(aInst == uut.GetInstance<A>());
}

BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
{
	mabiphmo::ioc::Container uut;
	uut.RegisterSingletonClass<A>(3);
	uut.RegisterSingletonClass<B, A>(5);
	std::shared_ptr<B> bInst = uut.GetInstance<B>();
	BOOST_TEST(bInst->b == 5);
	BOOST_TEST(bInst->a->a == 3);
	BOOST_TEST(uut.GetInstance<A>() == uut.GetInstance<B>()->a);
}

BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
{
	mabiphmo::ioc::Container uut;
	uut.RegisterSingletonClassOnInterface<IC, CImpl>(10);
	BOOST_TEST(uut.GetInstance<IC>()->C() == 10);
}

BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
{
	mabiphmo::ioc::Container uut;
	uut.RegisterSingletonClassOnInterface<IC, CImpl>(10);
	uut.RegisterSingletonClass<A>(3);
	uut.RegisterSingletonClass<B, A>(5);
	std::shared_ptr<B> bInst = uut.GetInstance<B>();
	std::shared_ptr<D> dInst = uut.RegisterSingletonClass<D, B, IC>(2);
	BOOST_TEST(dInst->sum == 20);
	BOOST_TEST(dInst->d == 2);
	BOOST_TEST(dInst == uut.GetInstance<D>());
	BOOST_TEST(dInst->c == uut.GetInstance<IC>());
	BOOST_TEST(dInst->b == uut.GetInstance<B>());
	BOOST_TEST(dInst->b->a == uut.GetInstance<A>());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()