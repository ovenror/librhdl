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

#include "c_api/typedcobject.h"

#include "representation/structural/builder/structure.h"

#include <memory>

namespace rhdl {

class Entity;
class Namespace;

class StructureHandle : public TypedCObject<StructureHandle, rhdl_structure, false> {
public:
	StructureHandle(Namespace &ns, const std::string name, int mode);
	virtual ~StructureHandle();

	StructureHandle &cast() override {return *this;}

	Handle &makeComponent(const Entity &entity);
	void finalize();
	void abort();

private:
	Context context_;
	std::unique_ptr<structural::builder::Structure> structure_;
};

} /* namespace rhdl */

#endif /* C_API_STRUCTUREHANDLE_H_ */
