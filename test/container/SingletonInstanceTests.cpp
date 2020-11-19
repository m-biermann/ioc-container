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
	Container uut;
	uut.RegisterType<A>(TypeHolder<A>(Scope::Singleton, std::make_shared<A>(3)));
	std::shared_ptr<A> aInst = uut.GetTypeHolder<A>()->Get();
	BOOST_TEST(aInst->a == (unsigned)3);
	BOOST_TEST(aInst == uut.GetTypeHolder<A>()->Get());
}

BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
{
	Container uut;
    uut.RegisterType(TypeHolder<A>(Scope::Singleton, std::make_shared<A>(3)));
    uut.RegisterType(TypeHolder<B>(Scope::Singleton, std::make_shared<B>(uut.GetTypeHolder<A>()->Get(), 5)));
	std::shared_ptr<B> bInst = uut.GetTypeHolder<B>()->Get();
	BOOST_TEST(bInst->b == (unsigned)5);
	BOOST_TEST(bInst->a->a == (unsigned)3);
	BOOST_TEST(uut.GetTypeHolder<A>()->Get() == bInst->a);
}

BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
{
	Container uut;
	uut.RegisterType(TypeHolder(Scope::Singleton, std::dynamic_pointer_cast<IC>(std::make_shared<CImpl>(10))));
	uut.RegisterType(TypeHolder(Scope::Singleton, std::make_shared<A>(3)));
	uut.RegisterType(TypeHolder(Scope::Singleton, std::make_shared<B>(uut.GetTypeHolder<A>()->Get(), 5)));
	uut.RegisterType(TypeHolder(Scope::Singleton, std::make_shared<D>(uut.GetTypeHolder<B>()->Get(), uut.GetTypeHolder<IC>()->Get(), 2)));

	std::shared_ptr<B> bInst = uut.GetTypeHolder<B>()->Get();
	std::shared_ptr<D> dInst = uut.GetTypeHolder<D>()->Get();

	BOOST_TEST(dInst->sum == (unsigned)20);
	BOOST_TEST(dInst->d == (unsigned)2);
	BOOST_TEST(dInst == uut.GetTypeHolder<D>()->Get());
	BOOST_TEST(dInst->c == uut.GetTypeHolder<IC>()->Get());
	BOOST_TEST(dInst->b == uut.GetTypeHolder<B>()->Get());
	BOOST_TEST(dInst->b->a == uut.GetTypeHolder<A>()->Get());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()