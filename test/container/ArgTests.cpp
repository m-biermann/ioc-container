//
// Created by max on 8/15/20.
//

#include <boost/test/unit_test.hpp>
#include <mabiphmo/ioc-container/Container.h>
#include "argStructs.h"

using namespace mabiphmo::ioc_container;

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(args)

	BOOST_AUTO_TEST_CASE(lvalueSingletonInstance)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton(std::make_shared<lvalueArgs>(arg));
		auto inst = uut->Get<lvalueArgs>();
		BOOST_TEST(inst == uut->Get<lvalueArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueSingletonFactoryPredefined)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton<lvalueArgs>(std::function<std::shared_ptr<lvalueArgs>()>(
				[arg](){
					return std::make_shared<lvalueArgs>(arg);
				}));
		auto inst = uut->Get<lvalueArgs>();
		BOOST_TEST(inst == uut->Get<lvalueArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueSingletonFactory)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton<lvalueArgs>(std::function<std::shared_ptr<lvalueArgs>(unsigned)>(
				[](unsigned arg){
					return std::make_shared<lvalueArgs>(arg);
				}));
		auto inst = uut->Get<lvalueArgs>(arg);
		BOOST_TEST(inst == uut->Get<lvalueArgs>(3));
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueClassFactory)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterFactory<lvalueArgs>(std::function<std::shared_ptr<lvalueArgs>(unsigned)>(
				[](unsigned arg){
					return std::make_shared<lvalueArgs>(arg);
				}));
		auto inst = uut->Get<lvalueArgs>((unsigned)arg);
		BOOST_TEST(inst != uut->Get<lvalueArgs>((unsigned)3));
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonInstance)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton(std::make_shared<lvalueRefArgs>(arg));
		auto inst = uut->Get<lvalueRefArgs>();
		BOOST_TEST(inst == uut->Get<lvalueRefArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonFactoryPredefined)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton<lvalueRefArgs>(std::function<std::shared_ptr<lvalueRefArgs>()>(
				[&arg](){
					return std::make_shared<lvalueRefArgs>(arg);
				}));
		auto inst = uut->Get<lvalueRefArgs>();
		BOOST_TEST(inst == uut->Get<lvalueRefArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonFactory)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton<lvalueRefArgs>(std::function<std::shared_ptr<lvalueRefArgs>(unsigned &)>(
				[](unsigned &val){
					return std::make_shared<lvalueRefArgs>(val);
				}));
		auto inst = uut->Get<lvalueRefArgs>(reinterpret_cast<unsigned&>(arg));
		BOOST_TEST(inst == uut->Get<lvalueRefArgs>(reinterpret_cast<unsigned&>(arg)));
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceClassFactory)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterFactory<lvalueRefArgs>(std::function<std::shared_ptr<lvalueRefArgs>(unsigned &)>(
				[](unsigned &arg){
					return std::make_shared<lvalueRefArgs>(arg);
				}));
		auto inst = uut->Get<lvalueRefArgs>(reinterpret_cast<unsigned&>(arg));
		BOOST_TEST(inst != uut->Get<lvalueRefArgs>(reinterpret_cast<unsigned&>(arg)));
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonInstance)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton(std::make_shared<lvalueConstRefArgs>(arg));
		auto inst = uut->Get<lvalueConstRefArgs>();
		BOOST_TEST(inst == uut->Get<lvalueConstRefArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonFactoryPredefined)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton<lvalueConstRefArgs>(std::function<std::shared_ptr<lvalueConstRefArgs>()>(
				[&arg](){
					return std::make_shared<lvalueConstRefArgs>(arg);
				}));
		auto inst = uut->Get<lvalueConstRefArgs>();
		BOOST_TEST(inst == uut->Get<lvalueConstRefArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonFactory)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterSingleton<lvalueConstRefArgs>(std::function<std::shared_ptr<lvalueConstRefArgs>(const unsigned &)>(
				[](const unsigned &arg){
					return std::make_shared<lvalueConstRefArgs>(arg);
				}));
		auto inst = uut->Get<lvalueConstRefArgs>(arg);
		BOOST_TEST(inst == uut->Get<lvalueConstRefArgs>(arg));
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceClassFactory)
	{
		auto uut = std::make_shared<Container>();
		unsigned arg = 3;
		uut->RegisterFactory<lvalueConstRefArgs>(std::function<std::shared_ptr<lvalueConstRefArgs>(const unsigned &)>(
				[](const unsigned &arg){
					return std::make_shared<lvalueConstRefArgs>(arg);
				}));
		auto inst = uut->Get<lvalueConstRefArgs>(arg);
		BOOST_TEST(inst != uut->Get<lvalueConstRefArgs>(arg));
		BOOST_TEST(inst->a == (unsigned)3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonInstance)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton(std::make_shared<rvalueRefArgs>(3));
		auto inst = uut->Get<rvalueRefArgs>();
		BOOST_TEST(inst == uut->Get<rvalueRefArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonFactoryPredefined)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton<rvalueRefArgs>(std::function<std::shared_ptr<rvalueRefArgs>()>(
				[](){
					return std::make_shared<rvalueRefArgs>(3);
				}));
		auto inst = uut->Get<rvalueRefArgs>();
		BOOST_TEST(inst == uut->Get<rvalueRefArgs>());
		BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonFactory)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterSingleton<rvalueRefArgs>(std::function<std::shared_ptr<rvalueRefArgs>(unsigned&&)>(
				[](unsigned &&arg){
					return std::make_shared<rvalueRefArgs>(std::move(arg));
				}));
		auto inst = uut->Get<rvalueRefArgs>((unsigned)3);
		BOOST_TEST(inst == uut->Get<rvalueRefArgs>((unsigned)3));
		BOOST_TEST(inst->a == (unsigned)3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceClassFactory)
	{
		auto uut = std::make_shared<Container>();
		uut->RegisterFactory<rvalueRefArgs>(std::function<std::shared_ptr<rvalueRefArgs>(unsigned&&)>(
				[](unsigned &&arg){
					return std::make_shared<rvalueRefArgs>(std::move(arg));
				}));
		auto inst = uut->Get<rvalueRefArgs>((unsigned)3);
		BOOST_TEST(inst != uut->Get<rvalueRefArgs>((unsigned)3));
		BOOST_TEST(inst->a == (unsigned)3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()