//
// Created by max on 8/15/20.
//

#include <boost/test/unit_test.hpp>
#include <mabiphmo/ioc/Container.h>
#include "argStructs.h"

BOOST_AUTO_TEST_SUITE(container)
BOOST_AUTO_TEST_SUITE(args)

	BOOST_AUTO_TEST_CASE(lvalueSingletonInstance)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		std::shared_ptr<lvalueArgs> inst = uut.RegisterSingletonClass<lvalueArgs>(static_cast<unsigned>(arg));
		BOOST_TEST(inst == uut.GetInstance<lvalueArgs>());
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueSingletonFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterSingletonClassFactory<lvalueArgs>(static_cast<unsigned>(arg));
		std::shared_ptr<lvalueArgs> inst = uut.GetInstance<lvalueArgs>();
		BOOST_TEST(inst == uut.GetInstance<lvalueArgs>());
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueClassFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterClassFactory<lvalueArgs, std::tuple<unsigned>, std::tuple<>>();
		std::shared_ptr<lvalueArgs> inst = uut.GetInstance<lvalueArgs>(static_cast<unsigned>(arg));
		BOOST_TEST(inst != uut.GetInstance<lvalueArgs>(static_cast<unsigned>(arg)));
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a != &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonInstance)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		std::shared_ptr<lvalueRefArgs> inst = uut.RegisterSingletonClass<lvalueRefArgs>(static_cast<unsigned&>(arg));
		BOOST_TEST(inst == uut.GetInstance<lvalueRefArgs>());
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceSingletonFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterSingletonClassFactory<lvalueRefArgs>(arg);
		std::shared_ptr<lvalueRefArgs> inst = uut.GetInstance<lvalueRefArgs>();
		BOOST_TEST(inst == uut.GetInstance<lvalueRefArgs>());
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueReferenceClassFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterClassFactory<lvalueRefArgs, std::tuple<unsigned&>, std::tuple<>>();
		std::shared_ptr<lvalueRefArgs> inst = uut.GetInstance<lvalueRefArgs>(static_cast<unsigned&>(arg));
		BOOST_TEST(inst != uut.GetInstance<lvalueRefArgs>(static_cast<unsigned&>(arg)));
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonInstance)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		std::shared_ptr<lvalueConstRefArgs> inst = uut.RegisterSingletonClass<lvalueConstRefArgs>(static_cast<unsigned const&>(arg));
		BOOST_TEST(inst == uut.GetInstance<lvalueConstRefArgs>());
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceSingletonFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterSingletonClassFactory<lvalueConstRefArgs>(arg);
		std::shared_ptr<lvalueConstRefArgs> inst = uut.GetInstance<lvalueConstRefArgs>();
		BOOST_TEST(inst == uut.GetInstance<lvalueConstRefArgs>());
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(lvalueConstReferenceClassFactory)
	{
		mabiphmo::ioc::Container uut;
		unsigned arg = 3;
		uut.RegisterClassFactory<lvalueConstRefArgs, std::tuple<unsigned const&>, std::tuple<>>();
		std::shared_ptr<lvalueConstRefArgs> inst = uut.GetInstance<lvalueConstRefArgs>(static_cast<unsigned const&>(arg));
		BOOST_TEST(inst != uut.GetInstance<lvalueConstRefArgs>(static_cast<unsigned const&>(arg)));
		BOOST_TEST(inst->a == 3);
		BOOST_TEST(&inst->a == &arg);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonInstance)
	{
		mabiphmo::ioc::Container uut;
		std::shared_ptr<rvalueRefArgs> inst = uut.RegisterSingletonClass<rvalueRefArgs>(3);
		BOOST_TEST(inst == uut.GetInstance<rvalueRefArgs>());
		BOOST_TEST(inst->a == 3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceSingletonFactory)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterSingletonClassFactory<rvalueRefArgs>(3);
		std::shared_ptr<rvalueRefArgs> inst = uut.GetInstance<rvalueRefArgs>();
		BOOST_TEST(inst == uut.GetInstance<rvalueRefArgs>());
		BOOST_TEST(inst->a == 3);
	}

	BOOST_AUTO_TEST_CASE(rvalueReferenceClassFactory)
	{
		mabiphmo::ioc::Container uut;
		uut.RegisterClassFactory<rvalueRefArgs, std::tuple<unsigned>, std::tuple<>>();
		std::shared_ptr<rvalueRefArgs> inst = uut.GetInstance<rvalueRefArgs>((unsigned)3);
		BOOST_TEST(inst != uut.GetInstance<rvalueRefArgs>((unsigned)3));
		BOOST_TEST(inst->a == 3);
	}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()