//
// Created by max on 8/13/20.
//

#ifndef IOC_STRUCTS_H
#define IOC_STRUCTS_H

#include <memory>

struct A{
	unsigned a;
	explicit A(unsigned a_) : a(a_) {}
};

struct B{
	std::shared_ptr<A> a;
	unsigned b;
	B(std::shared_ptr<A> a_, unsigned b_) : a(std::move(a_)), b(b_) {}
};

struct IC{
	virtual unsigned C() = 0;
};

struct CImpl : public IC{
	explicit CImpl(unsigned c_) : IC(), c(c_) {}
	unsigned C() override
	{
		return c;
	}
private:
	unsigned c;
};

struct CImpl2 : public IC{
	explicit CImpl2(unsigned &c_) : IC(), c(c_) {}
	unsigned C() override
	{
		return c;
	}
	unsigned &c;
};

struct D{
	std::shared_ptr<B> b;
	std::shared_ptr<IC> c;
	unsigned d;
	unsigned sum;
	D(std::shared_ptr<B> b_, std::shared_ptr<IC> c_, unsigned d_) : b(std::move(b_)), c(std::move(c_)), d(d_), sum(b->b + b->a->a + c->C() + d) {}
};

#endif //IOC_STRUCTS_H
