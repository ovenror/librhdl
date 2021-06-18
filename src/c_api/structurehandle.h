/*
 * structurehandle.h
 *
 *  Created on: May 21, 2021
 *      Author: ovenror
 */

#ifndef C_API_STRUCTUREHANDLE_H_
#define C_API_STRUCTUREHANDLE_H_

#include <rhdl/construction/c/types.h>

#include "context.h"
#include "handle.h"
#include "wrapper.h"

#include <memory>

namespace rhdl {

class Entity;

namespace structural { namespace builder { class Structure; }}

class StructureHandle {
public:
	StructureHandle(const std::string name, int mode);

	Handle &makeComponent(const Entity &entity);
	void finalize();
	void abort();

private:
	Context context_;
	std::unique_ptr<structural::builder::Structure> structure_;

public:
	using C_Struct = rhdl_structure;

private:
	friend Wrapper<StructureHandle>;
	static constexpr unsigned long C_ID = 0x57300C7003E;
	Wrapper<StructureHandle> c_;
};

} /* namespace rhdl */

#endif /* C_API_STRUCTUREHANDLE_H_ */
