//
// Created by max on 8/15/20.
//

#ifndef IOC_ARGSTRUCTS_H
#define IOC_ARGSTRUCTS_H

#include <utility>

struct lvalueRefArgs{
	lvalueRefArgs(unsigned &a_) : a(a_) {}

	unsigned &a;
};

struct lvalueConstRefArgs{
	lvalueConstRefArgs(unsigned const &a_) : a(a_) {}

	unsigned const &a;
};

struct lvalueArgs{
	lvalueArgs(unsigned a_) : a(a_) {}
	unsigned a;
};

struct rvalueRefArgs{
	rvalueRefArgs(unsigned &&a_) : a(a_) {}
	unsigned a;
};

#endif //IOC_ARGSTRUCTS_H
