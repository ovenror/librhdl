/*
 * typedhandle.h
 *
 *  Created on: Apr 7, 2021
 *      Author: ovenror
 */

#ifndef C_API_TYPEDHANDLE_H_
#define C_API_TYPEDHANDLE_H_

#include <rhdl/construction/structure.h>
#include "handle.h"
#include "construction/library.h"
#include <cassert>

namespace rhdl {

class EntityHandle;
class EntityInterface;
class PartHandle;
class PartInterface;
class Context;

template <class T> class TypedHandle : public Handle {
public:
	template <class... Args>
	TypedHandle(Context &context, Args&&... args);
	virtual ~TypedHandle() {}

	void finalize();

	template <class H2>
	void typedConnect(const H2 *h) const;

	T theHandle_;

protected:
	Handle &select(const std::string &interfaceName) const override;

	void connect(const Handle &h) const override;
	void getConnectedFrom(const TypedHandle<Structure> &h) const override;
	void getConnectedFrom(const TypedHandle<EntityInterface> &h) const override;
	void getConnectedFrom(const TypedHandle<PartHandle> &h) const override;
	void getConnectedFrom(const TypedHandle<PartInterface> &h) const override;
};

template <class T>
inline void TypedHandle<T>::finalize()
{
	assert(0);
}

template <>
inline void TypedHandle<Structure>::finalize()
{
	theHandle_.finalize();
}

template <class T>
void TypedHandle<T>::connect(const Handle &h) const
{
	h.getConnectedFrom(*this);
}

template <class T>
void TypedHandle<T>::getConnectedFrom(const TypedHandle<Structure> &h) const
{
	h.typedConnect(this);
}

template <class T>
void TypedHandle<T>::getConnectedFrom(const TypedHandle<EntityInterface> &h) const
{
	h.typedConnect(this);
}

template <class T>
void TypedHandle<T>::getConnectedFrom(const TypedHandle<PartHandle> &h) const
{
	h.typedConnect(this);
}

template <class T>
void TypedHandle<T>::getConnectedFrom(const TypedHandle<PartInterface> &h) const
{
	h.typedConnect(this);
}

template <class T>
template <class H2>
inline void TypedHandle<T>::typedConnect(const H2 *h) const
{
	theHandle_ >> h -> theHandle_;
}

template <> template <> inline void TypedHandle<Structure>::typedConnect(const TypedHandle<Structure> *h) const {pt();}
template <> template <> inline void TypedHandle<Structure>::typedConnect(const TypedHandle<EntityInterface> *h) const {pt();}
template <> template <> inline void TypedHandle<EntityInterface>::typedConnect(const TypedHandle<Structure> *h) const {pt();}
template <> template <> inline void TypedHandle<EntityInterface>::typedConnect(const TypedHandle<EntityInterface> *h) const {pt();}

template <class T>
template <class... Args>
TypedHandle<T>::TypedHandle(Context &context, Args&&... args)
	: Handle(context), theHandle_(std::forward<Args>(args)...)
{
	c_.content().iface = c_ptr(*theHandle_.interface());
}

template <class T>
Handle &TypedHandle<T>::select(const std::string &interfaceName) const
{
	auto result = theHandle_[interfaceName];
	return context_.make<decltype(result)>(std::move(result));
}

} /* namespace rhdl */

#endif /* C_API_TYPEDHANDLE_H_ */
