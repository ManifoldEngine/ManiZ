#pragma once

#include <type_traits>

namespace ManiZ
{
	template<typename T>
	struct is_string
		: public std::disjunction<
		std::is_same<char*, std::decay_t<T>>,
		std::is_same<const char*, std::decay_t<T>>,
		std::is_same<std::string, std::decay_t<T>>
		>
	{};
}