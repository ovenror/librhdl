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
template <class H> class TypedHandle;

class Context {
public:
	~Context();

	template <class H, class... Args> TypedHandle<H> &make(Args&&... args);
	void checkContains(const Handle* handle) const;

private:
	class Hash;
	class Equal;

	bool contains(const Handle* handle) const;

	std::unordered_set<const Handle *> handles_;
};

} /* namespace rhdl */

#include "typedhandle.h"

namespace rhdl {

template <class H, class... Args>
TypedHandle<H> &Context::make(Args&&... args)
{
	auto *h = new TypedHandle<H>(*this, std::forward<Args>(args)...);
	handles_.insert(static_cast<const Handle *>(h));
	return *h;
}

}
#endif /* C_API_CONTEXT_H_ */
