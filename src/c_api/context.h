/*
 * unsafecontext.h
 *
 *  Created on: Apr 6, 2021
 *      Author: ovenror
 */

#ifndef C_API_CONTEXT_H_
#define C_API_CONTEXT_H_

#include <rhdl/construction/constructionexception.h>
#include <unordered_set>
#include <memory>

namespace rhdl {

class Handle;

namespace structural::builder { class Port; }

class Context {
public:
	~Context();

	Handle &make(structural::builder::Port &port);
	void checkContains(const Handle* handle) const;

private:
	class Hash;
	class Equal;

	bool contains(const Handle* handle) const;

	std::unordered_set<const Handle *> handles_;
};

}
#endif /* C_API_CONTEXT_H_ */
