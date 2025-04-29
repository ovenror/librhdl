/*
 * lexicaldictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_LEXICALDICTIONARY_H_
#define SRC_UTIL_DICTIONARY_LEXICALDICTIONARY_H_

#include <util/dictionary/dictionaryimpl.h>
#include <util/dictionary/lexical.h>

namespace rhdl::dictionary {

template <class T, bool FAST_LOOKUP = true>
using LexicalDictionary = DictionaryImpl<T, order::Lexical>;

}

#endif /* SRC_UTIL_DICTIONARY_LEXICALDICTIONARY_H_ */
