#include "predicate.h"
#include "isingle.h"
#include "icomposite.h"
#include "iplaceholder.h"

namespace rhdl {



class P1st_Not : public Predicate_1st_Impl {
public:
	P1st_Not(Predicate_1st &&p) : p_(std::forward<Predicate_1st>(p)) {}
	~P1st_Not() {}

	bool operator()(const Interface &i) const override {return !p_(i);}
	bool operator()(const ISingle &i) const override {return !p_(i);}
	bool operator()(const IComposite &i) const override {return !p_(i);}
	bool operator()(const IPlaceholder &i) const override {return !p_(i);}

private:
	const Predicate_1st p_;
};

template <class FUNC>
class P1st_Func2 : public Predicate_1st_Impl {
public:
	P1st_Func2(Predicate_1st &&p1, Predicate_1st &&p2) : p1_(std::forward<Predicate_1st>(p1)), p2_(std::forward<Predicate_1st>(p2)) {}
	~P1st_Func2() {}

	bool operator()(const Interface &i) const override {return FUNC::eval(p1_(i), p2_(i));}
	bool operator()(const ISingle &i) const override {return FUNC::eval(p1_(i), p2_(i));}
	bool operator()(const IComposite &i) const override {return FUNC::eval(p1_(i), p2_(i));}
	bool operator()(const IPlaceholder &i) const override {return FUNC::eval(p1_(i), p2_(i));}

private:
	const Predicate_1st p1_;
	const Predicate_1st p2_;
};

struct Func_And {
	static bool eval(bool a, bool b) {return a && b;}
};

struct Func_Or {
	static bool eval(bool a, bool b) {return a || b;}
};

template <class IFACE>
bool Predicate_2nd::work(const IFACE &i)
{
	//std::cerr << "SECOND ORDER PREDICATE WORK " << typeid(i).name() << std::endl;

	if (done_)
		return result_;

	result_ = p_(i);

	if (result_ == disjunction_)
		done_ = true;

	return result_;
}

Predicate_2nd::Predicate_2nd(Predicate_1st &&p, bool disjunction) :
	p_(std::forward<Predicate_1st>(p)), disjunction_(disjunction), done_(false), result_(false)
{
}

Predicate_2nd::Predicate_2nd(Predicate_2nd &&dying) :
	Predicate_2nd(std::move(dying.p_), dying.disjunction_)
{
}

bool Predicate_2nd::operator()(const Interface &i)
{
	return work(i);
}

bool Predicate_2nd::operator()(const ISingle &i)
{
	return work(i);
}

bool Predicate_2nd::operator()(const IComposite &i)
{
	return work(i);
}

bool Predicate_2nd::operator()(const IPlaceholder &i)
{
	return work(i);
}


Predicate_1st Not(Predicate_1st &&p)
{
	return Predicate_1st::make<P1st_Not>(std::forward<Predicate_1st>(p));
}

Predicate_1st And(Predicate_1st &&p1, Predicate_1st &&p2)
{
	return Predicate_1st::make<P1st_Func2<Func_And> >(std::forward<Predicate_1st>(p1), std::forward<Predicate_1st>(p2));
}

Predicate_1st Or(Predicate_1st &&p1, Predicate_1st &&p2)
{
	return Predicate_1st::make<P1st_Func2<Func_Or> >(std::forward<Predicate_1st>(p1), std::forward<Predicate_1st>(p2));
}


template <class T>
class P1st_Functional : public Predicate_1st_Impl {
public:
	P1st_Functional(const std::function<bool(const T &)> &f) : f_(f) {}

	bool operator()(const T &i) const override
	{
		//std::cerr << "PREDICATE FUNCTION" << std::endl;
		return f_(i);
	}

private:
	std::function<bool(const T &)> f_;
};

template <class T, class... Args>
Predicate_1st Predicate(Args... args)
{
	return Predicate_1st::make<T>(args...);
}

template <class T>
Predicate_1st Predicate (const std::function<bool(const T &)> &f)
{
	return Predicate_1st::make<P1st_Functional<T> >(f);
}

template Predicate_1st Predicate(const std::function<bool(const Interface &)> &f);
template Predicate_1st Predicate(const std::function<bool(const ISingle &)> &f);
template Predicate_1st Predicate(const std::function<bool(const IComposite &)> &f);
template Predicate_1st Predicate(const std::function<bool(const IPlaceholder &)> &f);


Predicate_2nd Exists(Predicate_1st &&p)
{
	return Predicate_2nd(std::forward<Predicate_1st>(p), true);
}

Predicate_2nd ForAll(Predicate_1st &&p)
{
	return Predicate_2nd(std::forward<Predicate_1st>(p), false);
}

Predicate_1st::Predicate_1st(Predicate_1st &&dying_original) :
	Predicate_1st(dying_original.impl_)
{
	dying_original.impl_ = 0;
}

Predicate_1st::Predicate_1st(const Predicate_1st_Impl *impl) :
	impl_(impl)
{

}

Predicate_1st::~Predicate_1st()
{
	if (impl_)
		delete impl_;
}

bool Predicate_1st::operator()(const Interface &i) const
{
	return (*impl_)(i);
}

bool Predicate_1st::operator()(const ISingle &i) const
{
	return (*impl_)(i);
}

bool Predicate_1st::operator()(const IComposite &i) const
{
	return (*impl_)(i);
}

bool Predicate_1st::operator()(const IPlaceholder &i) const
{
	return (*impl_)(i);
}

Predicate_1st_Impl::Predicate_1st_Impl()
{
}

/*
void *Predicate_1st_Impl::operator new(size_t s)
{
	return ::operator new(s);
}
*/


}
