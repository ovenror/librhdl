/*
 * interfacemapping.h
 *
 *  Created on: Jun 7, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_INTERFACEMAPPING_H_
#define SRC_REPRESENTATION_INTERFACEMAPPING_H_

#include <cassert>

namespace rhdl_impl {

template <class ElementRef>
class InterfaceMapping {
public:
	InterfaceMapping();
	InterfaceMapping(const ElementRef target, bool open);
	~InterfaceMapping() {}

	ElementRef target() {return target_;}
	bool isOpen() const {return open_;}

private:
	const ElementRef target_;
	const bool open_;
};

/*
 * Default constructor for the sake of compilation.
 * Not to be used.
 */
template<class ElementRef>
inline InterfaceMapping<ElementRef>::InterfaceMapping()
{
	assert(0);
}

template<class ElementRef>
inline InterfaceMapping<ElementRef>::InterfaceMapping(
		const ElementRef target,	bool open)
	: target_(target), open_(open)
{}

} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_INTERFACEMAPPING_H_ */
