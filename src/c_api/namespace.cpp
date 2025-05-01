/*
 * namespace.cc
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#include "namespace.h"
#include "construction/library.h"
#include "util/dictionary/lexicaldictionary.h"

#include <memory>
#include <iostream>

namespace rhdl {

Namespace::Namespace(std::string name) : Super(
		name, std::make_unique<
				dictionary::LexicalDictionary<
						std::unique_ptr<const CObject>>>())
{
	setTypedMembers(c_strings().data());
}

Namespace::~Namespace() {}

void Namespace::setTypedMembers(const char *const *s) {
	c_ptr() -> members = s;
}

}
