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
		namespace _impl
		{
			// this is used to keep track of where we are in the serialization process.
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
			inline std::string serialize(JsonSerializationState& state, const auto& data, bool isInContainer = false);
			inline void addIndent(std::string& s, uint32_t indent);
			template<typename T>
			inline std::string format(const T& data);

			inline std::string serializeMany(JsonSerializationState& state) { return ""; }

			inline std::string serializeMany(JsonSerializationState& state, const auto& first, const auto& ...others)
			{
				return serialize(state, first) + serializeMany(state, others...);
			}

			inline std::string serialize(JsonSerializationState& state, const auto& data, bool isInContainer)
			{
				std::string s;
				addIndent(s, state.indent);

				using type = std::remove_cvref_t<decltype(data)>;
				static_assert(!std::is_pointer_v<type>);

				const std::string name = isInContainer ? "" : state.safeGetBackName();

				const auto write = [&](const std::string& key, const std::string& value)
				{
					if (isInContainer)
					{
						// we're in an array-like container, no key needed
						s += std::format("{},\n", value);
					}
					else
					{
						s += std::format("\"{}\": {},\n", key, value);
					}
				};

				if constexpr (std::is_enum_v<type> || std::is_fundamental_v<type>)
				{
					write(name, format(data));
				}
				else if constexpr (ManiZ::is_string<type>::value)
				{
					write(name, format(data));
				}
				else if constexpr (ManiZ::is_container<type>::value)
				{
					// hard iterate over the container
					s += std::format("\"{}\": [\n", name);
					state.indent++;
					for (const auto& v : data)
					{
						s += serialize(state, v, true);
					}
					state.indent--;
					addIndent(s, state.indent);
					s += std::format("],\n");
				}
				else
				{
					// we're in an aggregate type
					// push the member names in the stack
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
						// if we're already in an aggregate type, we want to output the key
						s += std::format("\"{}\":", name) + " {\n";
					}
					
					state.indent++;
					
					RFL::visitMembers(data, [&](auto& ...members)
					{
						// recursively serialize the members.
						s += serializeMany(state, members...);
					});
					
					state.indent--;
					addIndent(s, state.indent);
					
					s += "},\n";

					// pop the name stack
					state.namestack.pop_back();
					state.offsetStack.pop_back();
				}

				if (!isInContainer)
				{
					// if we're in a container we don't need to increment the offset as we're hard iterating
					state.safeIncrementBackOffset();
				}

				return s;
			}

			inline void addIndent(std::string& s, uint32_t indent)
			{
				for (uint32_t i = 0; i < indent; i++)
				{
					s += "\t";
				}
			}

			template<>
			inline std::string format(const std::string& data)
			{
				return std::format("\"{}\"", data);
			}

			template<typename T>
			inline std::string format(const T& data)
			{
				return std::format("{}", data);
			}
		}

		inline std::string json(const auto& ...data)
		{
			_impl::JsonSerializationState state;
			std::string s = _impl::serializeMany(state, data...);
			s.pop_back();
			s.pop_back();
			return s;
		}
	}

	namespace _impl
	{
		template<typename T>
		using json_variant = std::variant<long, unsigned long, double, std::string, bool, std::vector<T>>;
	}

	class JsonObject
	{
	public:
		JsonObject(const auto& in)
		{
			value = in;
		}

		template<typename T> 
		T get() const;

		template<typename T>
		requires std::is_integral_v<T> && std::is_unsigned_v<T>
		T get() const
		{
			return static_cast<T>(std::get<unsigned long>(value));
		}

		template<typename T>
		requires std::is_integral_v<T> && !std::is_unsigned_v<T>
		T get() const
		{
			return static_cast<T>(std::get<long>(value));
		}

		template<typename T>
		requires std::is_floating_point_v<T>
		T get() const
		{
			return static_cast<T>(std::get<double>(value));
		}

		template<>
		std::string get<std::string>() const
		{
			return std::get<std::string>(value);
		}

		template<>
		std::vector<JsonObject> get<std::vector<JsonObject>>() const
		{
			return std::get<std::vector<JsonObject>>(value);
		}

	private:
		_impl::json_variant<JsonObject> value;
		std::unordered_map<std::string, JsonObject> members;
	};

	namespace from
	{
		template<class T>
		T json(const std::string_view& jsonString);
		JsonObject parse(const std::string_view& jsonString);

		namespace _impl
		{
			inline void deserializeMany(const JsonObject& json, auto& first, auto& ...others);
			inline void deserializeMany(const JsonObject& json);
			inline void deserialize(const JsonObject& json, auto& data, bool isInContainer = false);


			inline void deserializeMany(const JsonObject & json) {}

			inline void deserializeMany(const JsonObject& json, auto& first, auto& ...others)
			{
			}
			
			inline void deserialize(const JsonObject & json, auto & data, bool isInContainer)
			{
			}
		}

		template<class T>
		T json(const std::string_view& jsonString)
		{
			T obj;
			JsonObject json = parse(jsonString);
			_impl::deserializeMany(json, obj);
			return ;
		}

		JsonObject parse(const std::string_view& jsonString)
		{
			return JsonObject(0);
		}
	}
}