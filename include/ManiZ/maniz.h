#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <format>
#include <source_location>

namespace ManiZ
{
	class ISerializable
	{
	public:
		virtual size_t memberCount() const = 0;
	};

	struct Any
	{
		template<typename T>
		operator T();
	};

	struct Serializer
	{
		template<typename T>
		inline constexpr static size_t memberCount()
		{
			if		constexpr (requires { requires std::is_empty_v<T> && sizeof(T); }) { return 0; }
			else if constexpr (requires { T{ Any{}, Any{}, Any{} }; })	return 3;
			else if constexpr (requires { T{ Any{}, Any{} }; })			return 2;
			else if constexpr (requires { T{ Any{} }; })				return 1;
			else { static_assert(std::is_void_v<T>, "maximum member count reached"); }
		}

		inline constexpr static std::string serializeMany(const auto& first, const auto& ...others);
		inline constexpr static std::string serializeMany() { return ""; }
		inline constexpr static std::string serialize(const auto& data);
		inline constexpr static std::string visitMembers(const auto& object, auto&& f);
	};

	constexpr std::string Serializer::serializeMany(const auto& first, const auto& ...others)
	{
		return Serializer::serialize(first) + Serializer::serializeMany(others...);
	}

	constexpr std::string Serializer::serialize(const auto& data)
	{
		using type = std::remove_cvref_t<decltype(data)>;
		static_assert(!std::is_pointer_v<type>);

		if constexpr (std::is_floating_point_v<type>)
		{
			return std::format("\"{}\": {},\n", "name", data);
		}
		else
		{
			return Serializer::visitMembers(data, [](const auto& ...members) { return Serializer::serializeMany(members...); });
		}
	}

	constexpr std::string Serializer::visitMembers(const auto& object, auto&& f)
	{

		constexpr size_t count = Serializer::memberCount<decltype(object)>();
		std::string value = "{\n";
		if		constexpr (count == 0)	{									return ""; }
		else if constexpr (count == 1)	{ const auto& [a1]			= object; value += f(a1); }
		else if constexpr (count == 2)	{ const auto& [a1, a2]		= object; value += f(a1, a2); }
		else if constexpr (count == 3)	{ const auto& [a1, a2, a3]	= object; value += f(a1, a2, a3); }
		value += "},\n";
		return value;
	}

	std::string toJson(const auto& ...data)
	{
		return Serializer::serializeMany(data...);
	}

	/*template<typename T>
	T deserialize(const std::string& data)
	{

	}*/
}