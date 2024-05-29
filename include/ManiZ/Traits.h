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

	template<typename T, typename _ = void>
	struct is_container : std::false_type {};

	template<typename... Ts>
	struct is_container_helper {};

	template<typename T>
	struct is_container<
		T,
		std::conditional_t<
			false,
			is_container_helper<
				typename T::value_type,
				typename T::size_type,
				typename T::allocator_type,
				typename T::iterator,
				typename T::const_iterator,
				decltype(std::declval<T>().size()),
				decltype(std::declval<T>().begin()),
				decltype(std::declval<T>().end()),
				decltype(std::declval<T>().cbegin()),
				decltype(std::declval<T>().cend())
			>,
			void
		>
	> : public std::true_type
	{};
}