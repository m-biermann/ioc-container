//
// Created by max on 8/14/20.
//

#include <mabiphmo/ioc/Container.h>
#include <boost/test/unit_test.hpp>
#include "structs.h"

using namespace mabiphmo::ioc;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(class_factory)

	BOOST_AUTO_TEST_CASE(registerAndGet)
	{
		Container uut;
		uut.RegisterType(Container::TypeHolder<A>(Container::Scope::Factory, std::function<A(unsigned)>([](unsigned val){return A(val);})));
		std::shared_ptr<Container::TypeHolder<A>> aHolder = uut.GetTypeHolder<A>();
		std::shared_ptr<A> aInst3 = aHolder->Get((unsigned)3);
		std::shared_ptr<A> aInst2 = aHolder->Get((unsigned)2);
		BOOST_TEST(aInst3 != aInst2);
		BOOST_TEST(aInst3->a == (unsigned)3);
		BOOST_TEST(aInst2->a == (unsigned)2);
		BOOST_TEST(aInst3 != aHolder->Get((unsigned)3));
		BOOST_TEST(aInst2 != aHolder->Get((unsigned)2));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterType(Container::TypeHolder<A>(Container::Scope::Singleton, std::function<A()>([](){return A(3);})));
		uut.RegisterType(Container::TypeHolder<B>(Container::Scope::Factory, std::function<B(unsigned)>([&uut = std::as_const(uut)](unsigned val){return B(uut.GetTypeHolder<A>()->Get(), val);})));
		auto bHolder = uut.GetTypeHolder<B>();
        std::shared_ptr<B> bInst5 = bHolder->Get((unsigned)5);
        std::shared_ptr<B> bInst6 = bHolder->Get((unsigned)6);
		std::shared_ptr<A> aInst = uut.GetTypeHolder<A>()->Get();
		BOOST_TEST(bInst5 != bInst6);
		BOOST_TEST(bInst5->b == (unsigned)5);
		BOOST_TEST(bInst6->b == (unsigned)6);
		BOOST_TEST(bInst5 != bHolder->Get((unsigned)5));
		BOOST_TEST(bInst6 != bHolder->Get((unsigned)6));
		BOOST_TEST(bInst6->a == aInst);
		BOOST_TEST(bInst5->a == aInst);
		BOOST_TEST(aInst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterType(Container::TypeHolder<CImpl>(Container::Scope::Factory, std::function<CImpl(unsigned)>([](unsigned val){return CImpl(val);})));
		auto cHolder = uut.GetTypeHolder<CImpl>();
		auto cInst5 = std::dynamic_pointer_cast<IC>(cHolder->Get((unsigned)5));
		auto cInst6 = std::dynamic_pointer_cast<IC>(cHolder->Get((unsigned)6));
		BOOST_TEST(cInst5 != cInst6);
		BOOST_TEST(cInst5->C() == (unsigned)5);
		BOOST_TEST(cInst6->C() == (unsigned)6);
		BOOST_TEST(cInst5 != std::dynamic_pointer_cast<IC>(cHolder->Get((unsigned)5)));
		BOOST_TEST(cInst6 != std::dynamic_pointer_cast<IC>(cHolder->Get((unsigned)6)));
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		mabiphmo::ioc::Container uut;
        uut.RegisterType(Container::TypeHolder<CImpl>(Container::Scope::Factory, std::function<CImpl()>([](){return CImpl(10);})));
        uut.RegisterType(Container::TypeHolder<A>(Container::Scope::Singleton, std::function<A()>([](){return A(3);})));
        uut.RegisterType(Container::TypeHolder<B>(Container::Scope::Singleton, std::function<B()>([&uut = std::as_const(uut)](){return B(uut.GetTypeHolder<A>()->Get(), 5);})));
        uut.RegisterType(Container::TypeHolder<D>(Container::Scope::Factory, std::function<D(unsigned)>([&uut = std::as_const(uut)](unsigned val){return D(uut.GetTypeHolder<B>()->Get(), std::dynamic_pointer_cast<IC>(uut.GetTypeHolder<CImpl>()->Get()), val);})));

        auto aHolder = uut.GetTypeHolder<A>();
        auto bHolder = uut.GetTypeHolder<B>();
        auto cHolder = uut.GetTypeHolder<CImpl>();
        auto dHolder = uut.GetTypeHolder<D>();

		std::shared_ptr<D> dInst2 = dHolder->Get((unsigned)2);
		std::shared_ptr<D> dInst3 = dHolder->Get((unsigned)3);
		std::shared_ptr<IC> cInst = cHolder->Get();
		std::shared_ptr<B> bInst = bHolder->Get();
		std::shared_ptr<A> aInst = aHolder->Get();
		BOOST_TEST(dInst2 != dInst3);
		BOOST_TEST(dInst2->sum == (unsigned)20);
		BOOST_TEST(dInst3->sum == (unsigned)21);
		BOOST_TEST(dInst2->d == (unsigned)2);
		BOOST_TEST(dInst3->d == (unsigned)3);
		BOOST_TEST(dInst2 != dHolder->Get((unsigned)2));
		BOOST_TEST(dInst3 != dHolder->Get((unsigned)3));
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