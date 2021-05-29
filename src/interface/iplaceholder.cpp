#include "interface/iplaceholder.h"
#include "interface/icomposite.h"
#include "interface/predicate.h"
#include "interface/cresult/ctrue.h"

#include <cassert>

namespace rhdl {

IPlaceholder::IPlaceholder(const std::string &name) :
	VisitableBase(name), r_composite_(nullptr), realization_(nullptr)
{
	c_.content().type = RHDL_UNSPECIFIED;
}

IPlaceholder::IPlaceholder(IComposite *composite) :
	VisitableBase(""), r_composite_(composite), realization_(composite)

{
	copy_c_from_realization();
}

void IPlaceholder::copy_c_from_realization() const {
	mutable_c() = *c_ptr(*realization_);
}

Interface::C_Struct &IPlaceholder::mutable_c() const {
	return const_cast<IPlaceholder *>(this) -> c_.content();
}

IPlaceholder::~IPlaceholder()
{
	//std::cerr << "IPlaceholder destruction for: " << this << std::endl;

	finalize();
}

static const char *const empty = nullptr;

const char* const * rhdl::IPlaceholder::ls() const {
	if (!realization_)
		return &empty;

	return realization_ -> ls();
}

void IPlaceholder::add(const Interface* iface) const {
	assert (r_composite_);

	r_composite_ -> add(iface);
	mutable_c().composite.interfaces = c_ptr(*realization_) -> composite.interfaces;
}

const Interface *IPlaceholder::get(const std::string &name) const
{
	const Interface *result = nullptr;

	if (realization_) {
		result = realization_ -> get(name);

		if (result || !r_composite_)
			return result;
	}
	else {
		r_composite_ = new IComposite(name_);
		realization_ = r_composite_;
		mutable_c().type = RHDL_COMPOSITE;
	}

	result = new IPlaceholder(name);
	add(result);
	return result;
}

std::pair<const Interface *, const Interface *> IPlaceholder::find_connectibles(const Interface *to, const Interface::Predicate2 &predicate) const
{
	std::pair<const Interface *, const Interface *> result;
	std::tie(result.second, result.first) = to -> find_connectibles(this, predicate.reversed());
	return result;
}

Interface *IPlaceholder::create_connectible(const Interface *to, const Interface::Predicate2 &predicate, CResult &cres) const
{
	//TODO: cloning only accidentally works for Entity<->Part here...
	//maybe add another parameter for the predicate, or another method 'complement()'
	Interface *cpy = to -> clone();

	cres = cpy -> eq_struct_int(*to, predicate);

	if (!cres -> success()) {
		delete cpy;
		return nullptr;
	}

	return cpy;
}

const IPlaceholder *IPlaceholder::is_placeholder(const Interface *interface)
{
	/* FIXME: Ugly
	 *
	 * Alternative 1: Add virtual method to class Interface that is overridden
	 * here in class IPlaceHolder.
	 *   - pollutes class Interface, which should know nothing of construction
	 *   + does so already by having IPlaceholder as a friend
	 *
	 * Alternative 2: Introduce a new container class similar to IComposite that
	 * can discern between placeholders and realized interfaces.
	 *   + nice and clean
	 *   - oof
	 */

	return dynamic_cast<const IPlaceholder *>(interface);
}

const Interface *IPlaceholder::be_connectible(const Interface *to, const Interface::Predicate2 &predicate, CResult &cres) const
{
	/* NOTE:
	 * if /to/ points to a composite interface, r_composite_ is not set accordingly, which means
	 * that we cannot auto-add additional components to it, if they are referenced
	 * => the composite interface is fixed/frozen, which is intended behaviour
	 * */

	Interface *cpy = create_connectible(to, predicate, cres);

	if (!cpy)
		return nullptr;

	//cpy -> name_ = name_;
	realization_ = cpy;

	//std::cerr << "setting name of new connectible realization to " << name_ << std::endl;

	realization_ -> name_ = name_;
	copy_c_from_realization();
	return realization_;
}


const Interface *IPlaceholder::find_connectible(const Interface *to, const Interface::Predicate2 &predicate) const
{
	//std::cerr << "IPlaceholder find connectible to " << typeid(*to).name() << std::endl;
	CResult dummy;

	if (realization_) {
		const Interface *result = nullptr;

		result = realization_ -> find_connectible(to, predicate);

		if (result || !r_composite_)
			return result;
	}
	else {
		return be_connectible(to, predicate, dummy);
	}

	Interface *result = create_connectible(to, predicate, dummy);

	if (!result)
		return nullptr;

	result -> name_ = Interface::anon_name;
	add(result);
	return result;
}

Interface::CResult IPlaceholder::eq_struct_int(const Interface &other, const Interface::Predicate2 &predicate) const
{
	//std::cerr << "IPlaceholder compare struct to " << (std::string) other << "@" << &other << "... ";

	if (realization_) {
		//std::cerr << "check realization" << std::endl;
		return realization_ -> eq_struct_int(other, predicate);
	}

	//std::cerr << "try creating fit... ";

	CResult cres;
	const Interface *connectible = be_connectible(&other, predicate, cres);

	if (!connectible) {
		//std::cerr << "FALI" << std::endl;
		return cres;
	}

	//std::cerr << "SUCCESS" << std::endl;
	return CResult(new CTrue());
}

bool IPlaceholder::eq_inner_names(const Interface &other) const
{
	std::ignore = other;
	return false;
}

void IPlaceholder::finalize() const
{
	//std::cerr << "Finalizing IPPlaceholder having realizations_: " << this->realization_ << ", " << this->r_composite_ << std::endl;

	if (!r_composite_)
		return;

	auto &components = r_composite_ -> components_;


	for (int i = 0; i < components.size();)
	{
		auto component = components.begin() + i;
		const IPlaceholder *ph_comp = is_placeholder(*component);

		if (!ph_comp) {
			++i;
			continue;
		}

		*component = ph_comp -> realization_;
		auto &c_strings = r_composite_ -> c_strings_;

		if (*component) {
			c_strings.at(i) = (*component) -> name().data();
			++i;
		}
		else {
			c_strings.erase(c_strings.begin() + i);
			components.erase(component);
		}

		//std::cerr << "IPlaceholder replaced by: " << typeid(*component).name() << " " << component -> name_ << std::endl;
		//std::cerr << "      deleting " << typeid(*ph_comp).name() << std::endl;
		delete ph_comp;
	}

	r_composite_ = 0;
}

const Interface *IPlaceholder::get_final(const Interface *interface)
{
	const IPlaceholder *ph = is_placeholder(interface);

	if (!ph)
		return interface;

	ph -> finalize();
	return ph -> realization_;
}

}
