/*
 * dictionary.cpp
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#include <gtest/gtest.h>
#include <util/dictionary/fcfsdictionary.h>
#include <util/dictionary/lexicaldictionary.h>

using namespace rhdl;
using namespace rhdl::dictionary;

class Element {
public:
	Element(std::string name) : name_(name ) {}

	const std::string &name() const {return name_;}

private:
	std::string name_;
};

template <class T>
using L = LexicalDictionary<T>;

template <class T>
using F = FCFSDictionary<T>;

template <template <class Dict> class Tester>
void exec()
{
	Tester<L<Element *>>::exec();
	Tester<L<std::unique_ptr<Element>>>::exec();
	Tester<F<Element *>>::exec();
	Tester<F<std::unique_ptr<Element>>>::exec();
}

template <class DICT>
struct Maker {
	auto make(std::string name)
	{
		auto ptr = elements.emplace_back(std::make_unique<Element>(std::move(name))).get();
		return std::make_pair(ptr, ptr);
	}

	static bool same(const Element *element, const Element *ptr)
	{
		return element == ptr;
	}

	std::vector<std::unique_ptr<Element>> elements;
};

template <template <class, class> class DICT, class ORDER>
struct Maker<DICT<std::unique_ptr<Element>, ORDER>> {
	auto make(std::string name)
	{
		auto element = std::make_unique<Element>(std::move(name));
		auto ptr = element.get();
		return std::make_pair(std::move(element), ptr);
	}

	static bool same(const std::unique_ptr<Element> &element, const Element *ptr)
	{
		return element.get() == ptr;
	}
};

template <class T>
struct OrderLOCALLess {
	bool operator()(const Element *lhs, const Element *rhs) const
	{
		return lhs -> name() < rhs -> name();
	}
};

template <class DICT>
struct OrderLOCAL;

template <class DELEM>
struct OrderLOCAL<L<DELEM>> : public std::set<
		const Element *, OrderLOCALLess<DELEM>>
{
	using Super = std::set<const Element *, OrderLOCALLess<DELEM>>;
	void add(const Element *e) {Super::insert(e);}
	void replace(const Element *ne)
	{
		auto nh = Super::extract(ne);
		nh.value() = ne;
		auto [iter, inserted, node] = Super::insert(std::move(nh));
		ASSERT_TRUE(inserted);
	}
};

template <class DELEM>
struct OrderLOCAL<F<DELEM>> : public std::vector<const Element *>
{
	using Super = std::vector<const Element *>;
	void add(const Element *e) {Super::push_back(e);}
	void replace(const Element *ne)
	{
		auto i = Super::begin();

		for (; i != Super::end(); ++i) {
			if ((*i) -> name() == ne -> name())
				break;
		}

		ASSERT_NE(i, Super::end());

		*i = ne;
	}
};

template <class DICT>
struct TesterEmpty {
	static void exec() {
		DICT d;
		auto &const_d = const_cast<const DICT &>(d);

		ASSERT_EQ(d.size(), 0);
		EXPECT_EQ(const_d.c_strings().size(), 1);
		EXPECT_EQ(const_d.c_strings().at(0), nullptr);
	}
};

template <class DICT>
struct TesterOne {
	static void exec() {
		DICT d;
		auto &const_d = const_cast<const DICT &>(d);
		Maker<DICT> m;
		auto [e, ptr] = m.make("lol");

		d.add(std::move(e));

		ASSERT_EQ(d.size(), 1);
		EXPECT_EQ(const_d.c_strings().size(), 2);
		EXPECT_TRUE(m.same(d.at("lol"), ptr));
		EXPECT_STREQ(const_d.c_strings().at(0), "lol");
		EXPECT_EQ(const_d.c_strings().at(0), ptr -> name().c_str());
		EXPECT_EQ(const_d.c_strings().at(1), nullptr);
	}
};

template <class DICT>
struct TesterMany {
	static void exec() {
		DICT d;
		auto &const_d = const_cast<const DICT &>(d);
		Maker<DICT> m;
		OrderLOCAL<DICT> OrderLOCAL;

		for (auto name : {"a", "d", "c", "b"}) {
			auto [element, ptr] = m.make(name);
			d.add(std::move(element));
			OrderLOCAL.add(ptr);

			ASSERT_EQ(d.size(), OrderLOCAL.size());
			EXPECT_EQ(const_d.c_strings().size(), OrderLOCAL.size() + 1);

			for (auto ptr : OrderLOCAL) {
				EXPECT_TRUE(m.same(d.at(ptr -> name()), ptr));
			}

			unsigned int i = 0;
			for (auto ptr : OrderLOCAL) {
				EXPECT_EQ(const_d.c_strings().at(i), ptr -> name().c_str());
				++i;
			}

			EXPECT_EQ(const_d.c_strings().at(i), nullptr);
		}
	}
};

template <class DICT>
struct TesterReplace {
	static void exec() {
		DICT d;
		auto &const_d = const_cast<const DICT &>(d);
		Maker<DICT> m;
		OrderLOCAL<DICT> OrderLOCAL;

		for (auto name : {"a", "d", "e", "c", "b"}) {
			auto [element, ptr] = m.make(name);
			d.add(std::move(element));
			OrderLOCAL.add(ptr);
		}

		auto [newE, newEptr] = m.make("e");
		auto [newD, newDptr] = m.make("d");

		OrderLOCAL.replace(newEptr);
		d.replace(std::move(newE));
		OrderLOCAL.replace(newDptr);
		d.replace(std::move(newD));

		unsigned int i = 0;
		for (auto ptr : OrderLOCAL) {
			EXPECT_TRUE(m.same(d.at(ptr -> name()), ptr));
			EXPECT_EQ(const_d.c_strings().at(i), ptr -> name().c_str());
			++i;
		}

	}
};

TEST(DictionaryTest, empty)
{
	exec<TesterEmpty>();
}

TEST(DictionaryTest, one)
{
	exec<TesterOne>();
}

TEST(DictionaryTest, many)
{
	exec<TesterMany>();
}

TEST(DictionaryTest, replace)
{
	exec<TesterReplace>();
}
