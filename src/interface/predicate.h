#ifndef PREDICATE_H
#define PREDICATE_H

#include "interface.h"
#include <functional>

namespace rhdl {

class Predicate_1st;

Predicate_1st Not(Predicate_1st &p);
Predicate_1st And(Predicate_1st &p1, Predicate_1st &p2);
Predicate_1st Or(Predicate_1st &p1, Predicate_1st &p2);

class Predicate_1st_Impl {
public:
	//operator Predicate_1st();

	virtual bool operator()(const Interface &) const {return false;}
	virtual bool operator()(const ISingle &i) const {return (*this)((Interface &) i);}
	virtual bool operator()(const IComposite &i) const {return (*this)((Interface &) i);}
	virtual bool operator()(const IPlaceholder &i) const {return (*this)((Interface &) i);}

protected:
	Predicate_1st_Impl();
	virtual ~Predicate_1st_Impl() {}

private:
	//void *operator new(size_t s);

	friend class Predicate_1st;
};


class Predicate_2nd;


class Predicate_1st {
public:
	Predicate_1st(Predicate_1st &&impl);

	~Predicate_1st();

	bool operator()(const Interface &i) const;
	bool operator()(const ISingle &i) const;
	bool operator()(const IComposite &i) const;
	bool operator()(const IPlaceholder &i) const;

	template <class T, class... Args>
	static Predicate_1st make(Args&&... args)
	{
		return Predicate_1st(new T(std::forward<Args>(args)...));
	}

protected:
	Predicate_1st(const Predicate_1st_Impl *impl);

private:
	friend class Predicate_1st_Impl;
	friend Predicate_1st Not(Predicate_1st &&p);
	friend Predicate_1st And(Predicate_1st &&p1, Predicate_1st &&p2);
	friend Predicate_1st Or(Predicate_1st &&p1, Predicate_1st &&p2);
	friend Predicate_2nd Exists(Predicate_1st &&p);
	friend Predicate_2nd ForAll(Predicate_1st &&p);

	const Predicate_1st_Impl *impl_;
};


class Predicate_2nd { //_Conc : public Predicate_2nd {
public:
	Predicate_2nd(Predicate_1st &&p, bool disjunction);
	Predicate_2nd(Predicate_2nd &&p);

	bool operator()(const Interface &i);
	bool operator()(const ISingle &i);
	bool operator()(const IComposite &i);
	bool operator()(const IPlaceholder &i);

	template <class IFACE>
	bool work(const IFACE &i);

	virtual bool done() {return done_;}
	virtual bool result() {return result_;}

private:
	Predicate_1st p_; //TODO: allow also for Predicate_2nd for optimization
	bool disjunction_;
	bool done_;
	bool result_;
};

template <class T>
Predicate_1st Predicate(const std::function<bool(const T &)> &f);

Predicate_2nd Exists(Predicate_1st &&p);
Predicate_2nd ForAll(Predicate_1st &&p);
Predicate_2nd Not(Predicate_2nd &&p);
Predicate_2nd And(Predicate_2nd &&p1, Predicate_2nd &&p2);
Predicate_2nd Or(const Predicate_2nd &p1, const Predicate_2nd &p2);

}

#endif // PREDICATE_H
