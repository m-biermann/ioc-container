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
		auto holder = uut.RegisterType(
			Container::TypeHolder<lvalueArgs>(
				Container::Scope::Singleton,
				lvalueArgs(arg)));
		auto inst = holder->Get();
		BOOST_TEST(inst == holder->Get());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueSingletonFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
        auto holder =
            uut.RegisterType(
                Container::TypeHolder<lvalueArgs>(
                    Container::Scope::Singleton,
                    std::function<lvalueArgs()>(
                        [arg](){
                        	return lvalueArgs(arg);
                        })));
		auto inst = holder->Get();
		BOOST_TEST(inst == holder->Get());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueClassFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueArgs>(
                Container::Scope::Factory,
                std::function<lvalueArgs(unsigned)>(
                    [](unsigned val){
                    	return lvalueArgs(val);
                    })));
        auto inst = holder->Get(unsigned(arg));
        BOOST_TEST(inst != holder->Get(unsigned(arg)));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonInstance)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueRefArgs>(
                Container::Scope::Singleton,
                lvalueRefArgs(arg)));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueRefArgs>(
                Container::Scope::Singleton,
                std::function<lvalueRefArgs()>(
                    [&arg](){
                        return lvalueRefArgs(arg);
                    })));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceClassFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueRefArgs>(
                Container::Scope::Factory,
                std::function<lvalueRefArgs(unsigned &)>(
                    [](unsigned & val){
                        return lvalueRefArgs(val);
                    })));
        auto inst = holder->Get(static_cast<unsigned&>(arg));
        BOOST_TEST(inst != holder->Get(static_cast<unsigned&>(arg)));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonInstance)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueConstRefArgs>(
                Container::Scope::Singleton,
                lvalueConstRefArgs(arg)));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueConstRefArgs>(
                Container::Scope::Singleton,
                std::function<lvalueConstRefArgs()>(
                    [&arg](){
                        return lvalueConstRefArgs(arg);
                    })));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceClassFactory)
	{
        mabiphmo::ioc::Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            Container::TypeHolder<lvalueConstRefArgs>(
                Container::Scope::Factory,
                std::function<lvalueConstRefArgs(const unsigned &)>(
                    [](const unsigned & val){
                        return lvalueConstRefArgs(val);
                    })));
        auto inst = holder->Get(arg);
        BOOST_TEST(inst != holder->Get(arg));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonInstance)
	{
        mabiphmo::ioc::Container uut;
        auto holder = uut.RegisterType(
            Container::TypeHolder<rvalueRefArgs>(
                Container::Scope::Singleton,
                rvalueRefArgs(3)));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonFactory)
	{
        mabiphmo::ioc::Container uut;
        auto holder = uut.RegisterType(
            Container::TypeHolder<rvalueRefArgs>(
                Container::Scope::Singleton,
	            std::function<rvalueRefArgs()>(
                    [](){
                        return rvalueRefArgs(3);
                    })));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceClassFactory)
	{
        mabiphmo::ioc::Container uut;
        auto holder = uut.RegisterType(
            Container::TypeHolder<rvalueRefArgs>(
                Container::Scope::Factory,
                std::function<rvalueRefArgs(unsigned &&)>(
                    [](unsigned && val){
                        return rvalueRefArgs(std::move(val));
                    })));
        auto inst = holder->Get((unsigned)3);
        BOOST_TEST(inst != holder->Get((unsigned)3));
        BOOST_TEST(inst->a == (unsigned)3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()