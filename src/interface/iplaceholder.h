#ifndef IPLACEHOLDER_H
#define IPLACEHOLDER_H

#include "interface.h"
#include "isingle.h"
#include "icomposite.h"
#include <vector>

namespace rhdl {

class IPlaceholder : public Interface::VisitableChild<IPlaceholder>
{
public:
	IPlaceholder(const std::string &name = "");
	IPlaceholder(IComposite *composite);

	~IPlaceholder();

	const Interface *get(const std::string &name) const override;
	const char *const *ls() const override;

	virtual IPlaceholder *clone() const {return nullptr;} //you shouldn't

	virtual std::pair<const Interface *, const Interface *> find_connectibles(const Interface *to, const Predicate2 &predicate) const;
	virtual const Interface *find_connectible(const Interface *to, const Predicate2 &predicate) const override;

	virtual CResult eq_struct_int(const Interface &other, const Predicate2 &predicate) const;

	virtual bool eq_inner_names (const Interface &other) const;

	static const Interface *get_final(const Interface *interface);

	const Interface *realization() const {return realization_;}

	void finalize() const;

protected:
	virtual CResult eq_struct_int (const ISingle &other, const Predicate2 &predicate) const {return eq_struct_int((Interface &) other, predicate);}
	virtual CResult eq_struct_int (const IComposite &other, const Predicate2 &predicate) const {return eq_struct_int((Interface &) other, predicate );}

	const Interface *be_connectible(const Interface *to, const Interface::Predicate2 &predicate, CResult &cres) const;
	Interface *create_connectible(const Interface *to, const Interface::Predicate2 &predicate, CResult &cres) const;

	static const IPlaceholder *is_placeholder(const Interface *interface);

private:
	void copy_c_from_realization() const;
	C_Struct &mutable_c() const;
	void add(const Interface *iface) const;

	mutable IComposite *r_composite_;
	mutable Interface *realization_;
};

}

#endif // IPLACEHOLDER_H
