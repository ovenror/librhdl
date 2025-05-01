/*
 * complexport.h
 *
 *  Created on: Jul 25, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTCONTAINER_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTCONTAINER_H_

#include "util/dictionary/dictionary.h"
#include <memory>
#include <set>

namespace rhdl {

class CompatibilityResult;
class ConnectionPredicate;

namespace structural {
namespace builder {

class Port;
class ExistingPort;
class ComplexPort;
class CBuilderPortContainerOps;

class PortContainer {
	using Dict = rhdl::dictionary::Dictionary<Port>;
	using DictPtr = std::unique_ptr<Dict>;

public:
	PortContainer();
	virtual ~PortContainer();

	static void tryConsolidate(PortContainer &from, PortContainer &to);
	virtual void tryConsolidate(ComplexPort &to) {}

	virtual Port &port() = 0;
	const Port &port() const;

	Dict &enclosed() {return *enclosed_;}
	const Dict &enclosed() const {return *enclosed_;}

protected:
	struct Less {
		using is_transparent = void;

		bool operator()(const Port &lhs, const Port &rhs) const;
		bool operator()(const Port &lhs, const std::string &rhs) const;
		bool operator()(const std::string &lhs, const Port &rhs) const;

		bool operator()(
				const std::unique_ptr<Port> &lhs,
				const std::unique_ptr<Port> &rhs) const
		{
			return (*this)(*lhs, *rhs);
		}

		bool operator()(
				const std::unique_ptr<Port> &lhs, const Port &rhs) const
		{
			return (*this)(*lhs, rhs);
		}

		bool operator()(
				const Port &lhs, const std::unique_ptr<Port> &rhs) const
		{
			return (*this)(lhs, *rhs);
		}

		bool operator()(
				const std::unique_ptr<Port> &lhs, const std::string &rhs) const
		{
			return (*this)(*lhs, rhs);
		}

		bool operator()(
				const std::string &lhs, const std::unique_ptr<Port> &rhs) const
		{
			return (*this)(lhs, *rhs);
		}

		bool operator()(
				const std::unique_ptr<ExistingPort> &lhs,
				const std::unique_ptr<ExistingPort> &rhs) const;
		bool operator()(
				const std::unique_ptr<ExistingPort> &lhs,
				const Port &rhs) const;
		bool operator()(
				const Port &lhs,
				const std::unique_ptr<ExistingPort> &rhs) const;
		bool operator()(
				const std::unique_ptr<ExistingPort> &lhs,
				const std::string &rhs) const;
		bool operator()(
				const std::string &lhs,
				const std::unique_ptr<ExistingPort> &rhs) const;
	};

	std::array<Port *, 2> findCompatibles(Port &peer, ConnectionPredicate p);
	Port *findCompatible(Port &peer, ConnectionPredicate p);

	void adopt(const ExistingPort &p);

	template <class DICT>
	void setDictionary(DICT &&dict) {setDictionary(static_cast<DictPtr>(std::make_unique<DICT>(std::move(dict))));}

private:
	void setDictionary(DictPtr dict) {enclosed_ = std::move(dict);}
	friend class CBuilderPortContainerOps;

	DictPtr enclosed_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTCONTAINER_H_ */
