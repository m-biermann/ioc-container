//
// Created by max on 8/13/20.
//

#include <mabiphmo/ioc/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(singleton_factory)

	BOOST_AUTO_TEST_CASE(registerAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterType(Container::TypeHolder<A>(Container::Scope::Singleton, std::function<A()>([](){return A(3);})));
		auto holder = uut.GetTypeHolder<A>();
		auto inst = holder->Get();
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(inst == holder->Get());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		mabiphmo::ioc::Container uut;
        uut.RegisterType(Container::TypeHolder<B>(Container::Scope::Singleton, std::function<B()>([uut](){return B(uut.GetTypeHolder<A>()->Get(), 5);})));
        uut.RegisterType(Container::TypeHolder<A>(Container::Scope::Singleton, std::function<A()>([](){return A(3);})));
        auto aHolder = uut.GetTypeHolder<A>();
        auto bHolder = uut.GetTypeHolder<B>();
		std::shared_ptr<B> bInst = bHolder->Get();
		std::shared_ptr<A> aInst = aHolder->Get();
		BOOST_TEST(bInst->b == (unsigned)5);
		BOOST_TEST(bInst == bHolder->Get());
		BOOST_TEST(bInst->a->a == (unsigned)3);
		BOOST_TEST(bInst->a == aInst);
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterType(Container::TypeHolder<CImpl>(Container::Scope::Singleton, std::function<CImpl()>([](){return CImpl(10);})));
		auto holder = uut.GetTypeHolder<CImpl>();
		std::shared_ptr<IC> cInst = std::dynamic_pointer_cast<IC>(holder->Get());
		BOOST_TEST(cInst->C() == (unsigned)10);
		BOOST_TEST(cInst == std::dynamic_pointer_cast<IC>(holder->Get()));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		mabiphmo::ioc::Container uut;
        uut.RegisterType(Container::TypeHolder<D>(Container::Scope::Singleton, std::function<D()>([uut](){return D(uut.GetTypeHolder<B>()->Get(), std::dynamic_pointer_cast<IC>(uut.GetTypeHolder<CImpl>()->Get()), 2);})));
        uut.RegisterType(Container::TypeHolder<CImpl>(Container::Scope::Singleton, std::function<CImpl()>([](){return CImpl(10);})));
        uut.RegisterType(Container::TypeHolder<B>(Container::Scope::Singleton, std::function<B()>([uut](){return B(uut.GetTypeHolder<A>()->Get(), 5);})));
        uut.RegisterType(Container::TypeHolder<A>(Container::Scope::Singleton, std::function<A()>([](){return A(3);})));
        auto aHolder = uut.GetTypeHolder<A>();
        auto bHolder = uut.GetTypeHolder<B>();
        auto cHolder = uut.GetTypeHolder<CImpl>();
        auto dHolder = uut.GetTypeHolder<D>();
        auto dInst = dHolder->Get();
        auto cInst = cHolder->Get();
        auto bInst = bHolder->Get();
        auto aInst = aHolder->Get();
		BOOST_TEST(dInst->sum == (unsigned)20);
		BOOST_TEST(dInst->d == (unsigned)2);
		BOOST_TEST(dInst == dHolder->Get());
		BOOST_TEST(dInst->c->C() == (unsigned)10);
		BOOST_TEST(dInst->c == cInst);
		BOOST_TEST(dInst->b->b == (unsigned)5);
		BOOST_TEST(dInst->b == bInst);
		BOOST_TEST(dInst->b->a->a == (unsigned)3);
		BOOST_TEST(dInst->b->a == aInst);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()