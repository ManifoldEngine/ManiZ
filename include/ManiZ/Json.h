#pragma once

#include <ManiZ/Reflection.h>
#include <ManiZ/Traits.h>
#include <vector>
#include <string>
#include <format>
#include <algorithm>

namespace ManiZ
{
	namespace to
	{
		namespace json_impl
		{
			struct JsonSerializationState
			{
				uint32_t indent = 0;
				std::vector<std::vector<std::string>> namestack;
				std::vector<size_t> offsetStack;

				std::string safeGetBackName()
				{
					if (namestack.size() > 0)
					{
						return namestack.back()[offsetStack.back()];
					}
					else
					{
						return "";
					}
				};

				void safeIncrementBackOffset()
				{
					if (offsetStack.size() > 0)
					{
						offsetStack.back()++;
					}
				};
			};

			inline std::string serializeMany(JsonSerializationState& state, const auto& first, const auto& ...others);
			inline std::string serializeMany(JsonSerializationState& state);
			inline std::string serialize(JsonSerializationState& state, const auto& data);
			inline void addIndent(std::string& s, uint32_t indent);

			inline std::string serializeMany(JsonSerializationState& state) { return ""; }

			inline std::string serializeMany(JsonSerializationState& state, const auto& first, const auto& ...others)
			{
				return serialize(state, first) + serializeMany(state, others...);
			}

			inline std::string serialize(JsonSerializationState& state, const auto& data)
			{
				std::string s;
				addIndent(s, state.indent);

				using type = std::remove_cvref_t<decltype(data)>;
				static_assert(!std::is_pointer_v<type>);

				const std::string name = state.safeGetBackName();

				if constexpr (std::is_enum_v<type> || std::is_fundamental_v<type>)
				{
					s += std::format("\"{}\": {},\n", name, data);
				}
				else if constexpr (ManiZ::is_string<type>::value)
				{
					s += std::format("\"{}\": \"{}\",\n", name, data);
				}
				else
				{
					auto memberNames = RFL::getMemberNames<type>();
					std::reverse(memberNames.begin(), memberNames.end());
					state.namestack.push_back(memberNames);
					state.offsetStack.push_back(0);

					if (name.empty())
					{
						s += "{\n";
					}
					else
					{
						s += std::format("\"{}\":", name) + " {\n";
					}
					
					state.indent++;
					
					RFL::visitMembers(data, [&](auto& ...members)
					{
						s += serializeMany(state, members...);
					});
					
					state.indent--;
					addIndent(s, state.indent);
					
					s += "},\n";

					state.namestack.pop_back();
					state.offsetStack.pop_back();
				}

				state.safeIncrementBackOffset();

				return s;
			}

			inline void addIndent(std::string& s, uint32_t indent)
			{
				for (uint32_t i = 0; i < indent; i++)
				{
					s += "\t";
				}
			}
		}

		inline std::string json(const auto& ...data)
		{
			json_impl::JsonSerializationState state;
			std::string s = json_impl::serializeMany(state, data...);
			s.pop_back();
			s.pop_back();
			return s;
		}
	}
	
	namespace from
	{
		template<class T>
		T json(const std::string_view& jsonString)
		{
			return T();
		}
	}
}