//
// Created by max on 8/15/20.
//

#include <boost/test/unit_test.hpp>
#include <mabiphmo/ioc-container/Container.h>
#include "argStructs.h"

using namespace mabiphmo::ioc;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(args)

	BOOST_AUTO_TEST_CASE(lvalueSingletonInstance)
	{
		Container uut;
		unsigned arg = 3;
		auto holder = uut.RegisterType(
			TypeHolder<lvalueArgs>(
				Scope::Singleton,
				std::make_shared<lvalueArgs>(arg)));
		auto inst = holder->Get();
		BOOST_TEST(inst == holder->Get());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueSingletonFactory)
	{
		Container uut;
		unsigned arg = 3;
        auto holder =
            uut.RegisterType(
                TypeHolder<lvalueArgs>(
                    Scope::Singleton,
                    std::function<std::shared_ptr<lvalueArgs>()>(
                        [arg](){
                        	return std::make_shared<lvalueArgs>(arg);
                        })));
		auto inst = holder->Get();
		BOOST_TEST(inst == holder->Get());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueClassFactory)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueArgs>(
                Scope::Factory,
                std::function<std::shared_ptr<lvalueArgs>(unsigned)>(
                    [](unsigned val){
                    	return std::make_shared<lvalueArgs>(val);
                    })));
        auto inst = holder->Get(unsigned(arg));
        BOOST_TEST(inst != holder->Get(unsigned(arg)));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonInstance)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueRefArgs>(
                Scope::Singleton,
				std::make_shared<lvalueRefArgs>(arg)));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonFactory)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueRefArgs>(
                Scope::Singleton,
                std::function<std::shared_ptr<lvalueRefArgs>()>(
                    [&arg](){
                        return std::make_shared<lvalueRefArgs>(arg);
                    })));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceClassFactory)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueRefArgs>(
                Scope::Factory,
                std::function<std::shared_ptr<lvalueRefArgs>(unsigned &)>(
                    [](unsigned & val){
                        return std::make_shared<lvalueRefArgs>(val);
                    })));
        auto inst = holder->Get(static_cast<unsigned&>(arg));
        BOOST_TEST(inst != holder->Get(static_cast<unsigned&>(arg)));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonInstance)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueConstRefArgs>(
                Scope::Singleton,
				std::make_shared<lvalueConstRefArgs>(arg)));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonFactory)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueConstRefArgs>(
                Scope::Singleton,
                std::function<std::shared_ptr<lvalueConstRefArgs>()>(
                    [&arg](){
                        return std::make_shared<lvalueConstRefArgs>(arg);
                    })));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceClassFactory)
	{
        Container uut;
        unsigned arg = 3;
        auto holder = uut.RegisterType(
            TypeHolder<lvalueConstRefArgs>(
                Scope::Factory,
                std::function<std::shared_ptr<lvalueConstRefArgs>(const unsigned &)>(
                    [](const unsigned & val){
                        return std::make_shared<lvalueConstRefArgs>(val);
                    })));
        auto inst = holder->Get(arg);
        BOOST_TEST(inst != holder->Get(arg));
        BOOST_TEST(inst->a == (unsigned)3);
        BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonInstance)
	{
        Container uut;
        auto holder = uut.RegisterType(
            TypeHolder<rvalueRefArgs>(
                Scope::Singleton,
				std::make_shared<rvalueRefArgs>(3)));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonFactory)
	{
        Container uut;
        auto holder = uut.RegisterType(
            TypeHolder<rvalueRefArgs>(
                Scope::Singleton,
	            std::function<std::shared_ptr<rvalueRefArgs>()>(
                    [](){
                        return std::make_shared<rvalueRefArgs>(3);
                    })));
        auto inst = holder->Get();
        BOOST_TEST(inst == holder->Get());
        BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceClassFactory)
	{
        Container uut;
        auto holder = uut.RegisterType(
            TypeHolder<rvalueRefArgs>(
                Scope::Factory,
                std::function<std::shared_ptr<rvalueRefArgs>(unsigned &&)>(
                    [](unsigned && val){
                        return std::make_shared<rvalueRefArgs>(std::move(val));
                    })));
        auto inst = holder->Get((unsigned)3);
        BOOST_TEST(inst != holder->Get((unsigned)3));
        BOOST_TEST(inst->a == (unsigned)3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()