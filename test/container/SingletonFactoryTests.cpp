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
		mabiphmo::ioc_container::Container uut;
		uut.RegisterType(TypeHolder<A>(Scope::Singleton, std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);})));
		auto holder = uut.GetTypeHolder<A>();
		auto inst = holder->Get();
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(inst == holder->Get());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyAndGet)
	{
		try{
			mabiphmo::ioc_container::Container uut;
			auto bHolder = uut.RegisterType(
					TypeHolder<B>(
							Scope::Singleton,
							std::function<std::shared_ptr<B>()>(
									[&uut = std::as_const(uut)](){
										return std::make_shared<B>(uut.GetTypeHolder<A>()->Get(), 5);
									})));
			auto aHolder = uut.RegisterType(
					TypeHolder<A>(
							Scope::Singleton,
							std::function<std::shared_ptr<A>()>(
									[](){
										return std::make_shared<A>(3);
									})));
			std::shared_ptr<B> bInst = bHolder->Get();
			std::shared_ptr<A> aInst = aHolder->Get();
			BOOST_TEST(bInst->b == (unsigned)5);
			BOOST_TEST(bInst == bHolder->Get());
			BOOST_TEST(bInst->a->a == (unsigned)3);
			BOOST_TEST(bInst->a == aInst);
		}
		catch (const std::out_of_range& oor) {
			std::cerr << "Out of Range error: " << oor.what() << '\n';
		}
	}

	BOOST_AUTO_TEST_CASE(registerOnInterfaceAndGet)
	{
		mabiphmo::ioc_container::Container uut;
		auto holder = uut.RegisterType(TypeHolder(Scope::Singleton, std::function<std::shared_ptr<IC>()>([](){return std::make_shared<CImpl>(10);})));
		std::shared_ptr<IC> cInst = holder->Get();
		BOOST_TEST(cInst->C() == (unsigned)10);
		BOOST_TEST(cInst == holder->Get());
	}

	BOOST_AUTO_TEST_CASE(registerWithDependencyInjectionAndGet)
	{
		mabiphmo::ioc_container::Container uut;
		auto dHolder = uut.RegisterType(TypeHolder(Scope::Singleton, std::function<std::shared_ptr<D>()>([&uut = std::as_const(uut)](){return std::make_shared<D>(uut.GetTypeHolder<B>()->Get(), uut.GetTypeHolder<IC>()->Get(), 2);})));
		auto cHolder = uut.RegisterType(TypeHolder(Scope::Singleton, std::function<std::shared_ptr<IC>()>([](){return std::make_shared<CImpl>(10);})));
		auto bHolder = uut.RegisterType(TypeHolder(Scope::Singleton, std::function<std::shared_ptr<B>()>([&uut = std::as_const(uut)](){return std::make_shared<B>(uut.GetTypeHolder<A>()->Get(), 5);})));
		auto aHolder = uut.RegisterType(TypeHolder(Scope::Singleton, std::function<std::shared_ptr<A>()>([](){return std::make_shared<A>(3);})));
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