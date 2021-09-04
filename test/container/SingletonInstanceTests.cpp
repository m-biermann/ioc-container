//
// Created by max on 8/13/20.
//

#define BOOST_TEST_MODULE container
#include <mabiphmo/ioc-container/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(singleton_instance)

BOOST_AUTO_TEST_CASE(registerAndGet)
{
	auto uut = std::make_shared<Container>();
	uut->RegisterSingleton(std::make_shared<A>(3));
	std::shared_ptr<A> aInst = uut->Get<A>();
	BOOST_TEST(aInst->a == (unsigned)3);
	BOOST_TEST(aInst == uut->Get<A>());
}

BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
{
	auto uut = std::make_shared<Container>();
	uut->RegisterSingleton(std::make_shared<A>(3));
	uut->RegisterSingleton(std::function<std::shared_ptr<B>(std::shared_ptr<A>)>([](std::shared_ptr<A> a){return std::make_shared<B>(a, 5);}));
	std::shared_ptr<B> bInst = uut->Get<B>();
	BOOST_TEST(bInst->b == (unsigned)5);
	BOOST_TEST(bInst->a->a == (unsigned)3);
	BOOST_TEST(uut->Get<A>() == bInst->a);
}

BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
{
	auto uut = std::make_shared<Container>();
	uut->RegisterSingleton(std::make_shared<CImpl>(10));
	uut->RegisterSingleton(std::make_shared<A>(3));
	uut->RegisterSingleton(std::function<std::shared_ptr<B>(std::shared_ptr<A>)>([](std::shared_ptr<A> a){return std::make_shared<B>(a, 5);}));
	uut->RegisterSingleton(std::function<std::shared_ptr<D>(std::shared_ptr<B>, std::shared_ptr<IC>)>([](std::shared_ptr<B> b, std::shared_ptr<IC> c) {return std::make_shared<D>(b, c, 2);}));
	uut->RegisterOnInterface<IC, CImpl>();

	std::shared_ptr<B> bInst = uut->Get<B>();
	std::shared_ptr<D> dInst = uut->Get<D>();

	BOOST_TEST(dInst->sum == (unsigned)20);
	BOOST_TEST(dInst->d == (unsigned)2);
	BOOST_TEST(dInst == uut->Get<D>());
	BOOST_TEST(dInst->c == uut->Get<IC>());
	BOOST_TEST(dInst->b == uut->Get<B>());
	BOOST_TEST(dInst->b->a == uut->Get<A>());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()