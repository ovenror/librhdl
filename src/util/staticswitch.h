/*
 * typeswitch.h
 *
 *  Created on: Apr 12, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_STATICSWITCH_H_
#define SRC_UTIL_STATICSWITCH_H_

#include <type_traits>

namespace rhdl {
namespace staticswitch {

struct Class;

template<class SWITCHCLASS = Class, class RESULTCLASS = Class>
struct Setup {
	template <SWITCHCLASS CASE, RESULTCLASS RESULT> struct Case;

private:
	template <SWITCHCLASS SWITCH, RESULTCLASS DEFAULT, class... CASES>
	struct DoSwitch {
		constexpr static RESULTCLASS result = DEFAULT;
	};

	template <
			SWITCHCLASS SWITCH, RESULTCLASS DEFAULT,
			SWITCHCLASS CASE, RESULTCLASS RESULT,
			class... CASES>
	struct DoSwitch<SWITCH, DEFAULT, Case<CASE, RESULT>, CASES...> {
		constexpr static RESULTCLASS result = CASE == SWITCH ?
				RESULT : DoSwitch<SWITCH, DEFAULT, CASES...>::result;
	};

public:
	template <SWITCHCLASS SWITCH, RESULTCLASS DEFAULT, class... CASES>
	using Switch = typename DoSwitch<SWITCH, DEFAULT, CASES...>::result;
};

template<class SWITCHCLASS>
struct Setup<SWITCHCLASS, Class> {
	template <SWITCHCLASS CASE, class RESULT> struct Case;

private:
	template <SWITCHCLASS SWITCH, class DEFAULT, class... CASES>
	struct DoSwitch {
		using result = DEFAULT;
	};

	template <
			SWITCHCLASS SWITCH, class DEFAULT,
			SWITCHCLASS CASE, class RESULT,
			class... CASES>
	struct DoSwitch<SWITCH, DEFAULT, Case<CASE, RESULT>, CASES...> {
		using result = typename std::conditional<CASE == SWITCH,
				RESULT,
				typename DoSwitch<SWITCH, DEFAULT, CASES...>::result>::type;
	};

public:
	template <SWITCHCLASS SWITCH, class DEFAULT, class... CASES>
	using Switch = typename DoSwitch<SWITCH, DEFAULT, CASES...>::result;
};

}}

#endif /* SRC_UTIL_STATICSWITCH_H_ */
