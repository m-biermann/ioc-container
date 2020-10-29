//
// Created by max on 8/15/20.
//

#include <boost/test/unit_test.hpp>
#include <mabiphmo/ioc/Container.h>
#include "argStructs.h"

using namespace mabiphmo::ioc;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(args)

	BOOST_AUTO_TEST_CASE(lvalueSingletonInstance)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterType(Container::TypeHolder<lvalueArgs>(Container::Scope::Singleton, lvalueArgs(arg)));
		auto holder = uut.GetTypeHolder<lvalueArgs>();
		auto inst = holder->Get();
		BOOST_TEST(inst == holder->Get());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueSingletonFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueArgs>(Container::Scope::Singleton, std::function<lvalueArgs()>([arg](){return lvalueArgs(arg);})));
        auto holder = uut.GetTypeHolder<lvalueArgs>();
		auto inst = holder->Get();
		BOOST_TEST(inst == holder->Get());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueClassFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueArgs>(Container::Scope::Factory, std::function<lvalueArgs(unsigned)>([](unsigned val){return lvalueArgs(val);})));
        auto holder = uut.GetTypeHolder<lvalueArgs>();
        auto inst = holder->Get(unsigned(arg));
        BOOST_TEST(inst != holder->Get(unsigned(arg)));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonInstance)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueRefArgs>(Container::Scope::Singleton, lvalueRefArgs(arg)));
        auto holder = uut.GetTypeHolder<lvalueRefArgs>();
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueRefArgs>(Container::Scope::Singleton, std::function<lvalueRefArgs()>([&arg](){return lvalueRefArgs(arg);})));
        auto holder = uut.GetTypeHolder<lvalueRefArgs>();
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceClassFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueRefArgs>(Container::Scope::Factory, std::function<lvalueRefArgs(unsigned &)>([](unsigned & val){return lvalueRefArgs(val);})));
        auto holder = uut.GetTypeHolder<lvalueArgs>();
        auto inst = holder->Get(&arg);
        BOOST_TEST(inst != holder->Get(&arg));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonInstance)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueConstRefArgs>(Container::Scope::Singleton, lvalueConstRefArgs(arg)));
        auto holder = uut.GetTypeHolder<lvalueConstRefArgs>();
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueConstRefArgs>(Container::Scope::Singleton, std::function<lvalueConstRefArgs()>([arg](){return lvalueConstRefArgs(arg);})));
        auto holder = uut.GetTypeHolder<lvalueConstRefArgs>();
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceClassFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        uut.RegisterType(Container::TypeHolder<lvalueConstRefArgs>(Container::Scope::Factory, std::function<lvalueConstRefArgs(const unsigned &)>([](const unsigned & val){return lvalueConstRefArgs(val);})));
        auto holder = uut.GetTypeHolder<lvalueConstRefArgs>();
        auto inst = holder->Get(arg);
        BOOST_TEST(inst != holder->Get(arg));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonInstance)
	{
        mabiphmo::ioc::Container uut;
        uut.RegisterType(Container::TypeHolder<rvalueRefArgs>(Container::Scope::Singleton, rvalueRefArgs(3)));
        auto holder = uut.GetTypeHolder<rvalueRefArgs>();
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonFactory)
	{
        mabiphmo::ioc::Container uut;
        uut.RegisterType(Container::TypeHolder<rvalueRefArgs>(Container::Scope::Singleton, std::function<rvalueRefArgs()>([](){return rvalueRefArgs(3);})));
        auto holder = uut.GetTypeHolder<rvalueRefArgs>();
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceClassFactory)
	{
        mabiphmo::ioc::Container uut;
        uut.RegisterType(Container::TypeHolder<rvalueRefArgs>(Container::Scope::Factory, std::function<rvalueRefArgs(unsigned &&)>([](unsigned && val){return rvalueRefArgs(std::move(val));})));
        auto holder = uut.GetTypeHolder<rvalueRefArgs>();
        auto inst = holder->Get(3);
        BOOST_TEST(inst != holder->Get(3));
        BOOST_TEST(inst->a == (unsigned)3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()