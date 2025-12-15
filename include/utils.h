#ifndef UWRAPPER_H
#define UWRAPPER_H

#include <iterator>
#include <concepts>
#include <type_traits>
#include <array>
#include "token.h"

template <class Container>
concept Iterable = requires (Container &&c) {
	std::begin(c);
	std::end(c);
};

template <class T>
struct UWrapper {
	T value;
	
	template <Iterable TT>
	inline bool from(const TT &list) const {
		if (value == '\0')
			return false;
		for (const auto &elem : list) {
			if (value == elem)
				return true;
		};
		return false;
	};
	
	template <class U = T>
	std::enable_if_t<std::is_same_v<U, char>, bool>
	inline symStart() const {
		return
			(value >= 'A' && value <= 'Z') ||
			(value >= 'a' && value <= 'z') ||
			 value == '_' || static_cast<unsigned char>(value) >= 0x80;
	};
	
	template <class U = T>
	std::enable_if_t<std::is_same_v<U, char>, bool>
	inline symCont() const {
		return
			symStart() ||
			(value >= '0' && value <= '9') ||
			 value == '\'';
	};
};

template <class T>
inline UWrapper<T> is(T value) {
	return UWrapper<T>{value};
};

/* ==== Static arrays ==== */
static constexpr
std::array<Token::Type, 5> valueTypes = {
	Token::Symbol,
	Token::Integer,
	Token::Float,
	Token::Char,
	Token::String,
};


#endif // UWRAPPER_H