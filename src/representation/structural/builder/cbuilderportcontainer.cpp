/*
 * cbuilder.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: ovenror
 */

#include "cbuilderportcontainer.h"
#include "cpeerdirection.h"
#include "builderport.h"
#include "interface/cresult/copdirection.h"

namespace rhdl {
namespace structural {
namespace builder {

Errorcode CBuilderPortContainerOps::fail_num(std::ostream &os) const
{
	os << "* different number of contained interfaces";
	return Errorcode::E_DIFFERENT_NUMBER_OF_COMPONENTS;
}

Errorcode CBuilderPortContainerOps::found_none(
		std::ostream &os, const Port &sub) const
{
	os<< "* no compatible partners for contained interface " << sub;
	return Errorcode::E_COMPATIBLE_INTERFACES_NOT_FOUND;
}

Errorcode CBuilderPortContainerOps::found_multiple(
		std::ostream &os, const Port &sub) const
{
	os << "* multiple compatible partners for contained interface " << sub;
	return Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES;
}


template<class RESULT>
inline void CBuilderPortContainerOps::eval_common(RESULT &result) const
{
	auto &ebuilder = lhs.enclosed();
	auto &econtainer = rhs.enclosed();

	/*
	* If Structure-Structure connections were allowed, violating this
	* assertion this would lead to  returning true.
	*/
	assert (!ebuilder.empty() || !econtainer.empty());

	if (ebuilder.empty()) {
		use<CPeerDirection>(result, rhs.port(), predicate);
		return;
	}

	if (ebuilder.size() != econtainer.size()) {
		issue(&CBuilderPortContainerOps::fail_num, result);
		return;
	}

	std::unordered_set<const Port *> remainingContainerSubs;

	for (const auto &containerSub : econtainer)
		remainingContainerSubs.insert(&containerSub);

	for (const auto &builderSub : ebuilder) {
		const Port *found = nullptr;

		for (auto *containerSub : remainingContainerSubs) {
			if (!builderSub.compatible(*containerSub, predicate))
				continue;

			if (found) {
				issue(&CBuilderPortContainerOps::found_multiple, result, builderSub);
				return;
			}

			found = containerSub;
			remainingContainerSubs.erase(containerSub);
		}

		if (!found) {
			issue(&CBuilderPortContainerOps::found_none, result, builderSub);
			return;
		}

		mapping_.emplace_back(std::array<const Port * ,2>{&builderSub, found});
	}
}

template void CBuilderPortContainerOps::eval_common(bool &) const;
template void CBuilderPortContainerOps::eval_common(const CBuilderPortContainer &) const;

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
