#pragma once

#include "Reflection.h"

#include <unordered_map>
#include <functional>
#include <string>
#include <format>

namespace ManiZ
{
	class Serializer
	{
	public:
		inline constexpr static std::string serializeMany(const auto& first, const auto& ...others);
		inline constexpr static std::string serializeMany() { return ""; }
		inline constexpr static std::string serialize(const auto& data);
		inline constexpr static auto visitMembers(const auto& object, auto&& f);

	private:
		inline constexpr static auto visitMembers_impl(const auto& object, auto&& f);
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
			return RFL::decompose(data, [](const auto& ...members) -> std::string 
			{ 
				return Serializer::serializeMany(members...); 
			});
		}
	}

	std::string toJson(const auto& ...data)
	{
		return Serializer::serializeMany(data...);
	}
}

