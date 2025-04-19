/*
 * lexicalpointingdictionary.h
 *
 *  Created on: Sep 30, 2024
 *      Author: ovenror
 */

#ifndef SRC_UTIL_LEXICALPOINTINGDICTIONARY_H_
#define SRC_UTIL_LEXICALPOINTINGDICTIONARY_H_

#include <util/lexicaldictionary.h>

namespace rhdl {

template<class T, bool OWNING = true>
class LexicalPointingDictionary : public Dictionary<T> {
	using PT = typename std::conditional<OWNING, std::unique_ptr<const T>, const T*>::type;
	using Dict = LexicalDictionary<PT>;

public:
	LexicalPointingDictionary() {}
	LexicalPointingDictionary(LexicalPointingDictionary &&moved)
			: impl_(std::move(moved.impl_)) {}
	virtual ~LexicalPointingDictionary() {}

	bool contains(const char *name) const override {return impl_.contains(name);}
	bool contains(const std::string &name) const override {return impl_.contains(name);}

	const T& at(const char *name) const override {return *impl_.at(name);}
	const T& at(const std::string &name) const override {return *impl_.at(name);}

	const T* add(PT element) {return &*impl_.add(std::move(element));}
	const T* replace(PT element) {return &*impl_.replace(std::move(element));}

	std::size_t size() const override {return impl_.size();}

	const std::vector<const char*> &c_strings() const override {return impl_.c_strings();}

	void clear() {impl_.clear();}

private:
	Dict impl_;
};

} /* namespace rhdl */

#endif /* SRC_UTIL_LEXICALPOINTINGDICTIONARY_H_ */
