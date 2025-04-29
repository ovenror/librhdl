/*
 * fcfsdictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_FCFSDICTIONARY_H_
#define SRC_UTIL_DICTIONARY_FCFSDICTIONARY_H_

#include <util/dictionary/dictionaryimpl.h>
#include <util/dictionary/fcfs.h>

namespace rhdl::dictionary {

template <class T>
using FCFSDictionary = DictionaryImpl<T, order::FCFS>;

} /* namespace rhdl */


#endif /* SRC_UTIL_DICTIONARY_FCFSDICTIONARY_H_ */
