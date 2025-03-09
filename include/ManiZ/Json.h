#pragma once

#include <ManiZ/Reflection.h>
#include <ManiZ/Traits.h>
#include <vector>
#include <map>
#include <string>
#include <format>
#include <algorithm>
#include <assert.h>
#include <ranges>

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
				else if constexpr (std::ranges::range<type>)
				{
					if (!name.empty())
					{
						s += std::format("\"{}\": ", name);
					}
					else
					{
						addIndent(s, 1); // special formatting for nested arrays.
						state.indent++;
					}

					// hard iterate over the container
					s += std::format("[\n");
					state.indent++;
					for (const auto& v : data)
					{
						s += serialize(state, v, true);
					}
					state.indent--;
					
					addIndent(s, state.indent);

					if (name.empty())
					{
						state.indent--;
					}
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

			template<typename T>
			inline std::string format(const T& data)
			{
				return std::format("{}", data);
			}

			template<>
			inline std::string format<float>(const float& data)
			{
				return std::format("{:f}", data);
			}

			template<>
			inline std::string format<double>(const double& data)
			{
				return std::format("{:f}", data);
			}

			template<>
			inline std::string format(const std::string& data)
			{
				return std::format("\"{}\"", data);
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
		using json_variant = std::variant<long, unsigned long, double, std::string, bool>;
	}

	class JsonObject
	{
	public:
		JsonObject() = default;

		JsonObject(const auto& in)
		{
			m_isValid = true;
			m_value = in;
		}

		JsonObject(const std::vector<JsonObject>& in)
		{
			m_isValid = true;
			m_array = in;
		}

		template<typename T> 
		T get() const;

		template<typename T>
		requires std::is_integral_v<T> && std::is_unsigned_v<T>
		T get() const
		{
			return static_cast<T>(std::get<unsigned long>(m_value));
		}

		template<typename T>
		requires std::is_integral_v<T> && !std::is_unsigned_v<T>
		T get() const
		{
			// we deserialize to long only if the value is negative, otherwise it is set to the unsigned long slot of the variant.
			if (const long* value = std::get_if<long>(&m_value))
			{
				return static_cast<T>(*value);
			}
			return static_cast<T>(std::get<unsigned long>(m_value));
		}

		template<typename T>
		requires std::is_floating_point_v<T>
		T get() const
		{
			return static_cast<T>(std::get<double>(m_value));
		}

		template<>
		bool get<bool>() const
		{
			return std::get<bool>(m_value);
		}

		template<>
		std::string get<std::string>() const
		{
			return std::get<std::string>(m_value);
		}

		const std::vector<JsonObject>& getArray() const
		{
			return m_array;
		}

		const JsonObject& getAt(size_t index) const
		{
			assert(index >= 0 && index < m_keysInOrder.size());
			return m_members.at(m_keysInOrder[index]);
		}

		JsonObject& operator[](const std::string& key) 
		{
			if (!m_members.contains(key))
			{
				m_keysInOrder.push_back(key);
			}
			return m_members[key]; 
		}

		const JsonObject& operator[](const std::string& key) const { return m_members.at(key); }
		size_t size() const { return m_members.size(); }
		bool isValid() const { return m_isValid || m_members.size() > 0 || m_members.size() > 0; }
	private:
		_impl::json_variant m_value;
		std::vector<std::string> m_keysInOrder;
		std::vector<JsonObject> m_array;
		std::map<std::string, JsonObject> m_members;
		bool m_isValid = false;
	};

	namespace from
	{
		// json parser
		namespace _impl
		{
			struct JsonParser
			{
				std::string::const_iterator it;
				size_t line = 0;
				size_t column = 0;

				void inc()
				{
					if (*it == '\n')
					{
						column = 0;
						line++;
					}
					else
					{
						column++;
					}
					it++;
				}

				char get() const { return *it; }
			};

			inline JsonObject parseMany(JsonParser& parser, const std::string& text);
			inline JsonObject parse(JsonParser& parser, const std::string& text);
			inline void skipWhitespaces(JsonParser& parser, const std::string& text);
			inline JsonObject error(const JsonParser& parser);

			inline JsonObject parseMany(JsonParser& parser, const std::string& text)
			{
				if (parser.get() != '{')
				{
					return error(parser);
				}

				parser.inc();

				JsonObject obj;
				while (parser.it != text.end() && parser.get() != '}')
				{
					skipWhitespaces(parser, text);

					if (parser.get() != '"')
					{
						// we expect a key
						return error(parser);
					}

					// skip "
					parser.inc();

					const std::string::const_iterator start = parser.it;
					while (parser.get() != '"' && parser.it != text.end())
					{
						parser.inc();
					}
					
					std::string key = text.substr(start - text.begin(), parser.it - start);
					
					parser.inc();
					skipWhitespaces(parser, text);
					if (parser.get() != ':')
					{
						// we expect a : between keys and values
						return error(parser);
					}

					parser.inc();
					skipWhitespaces(parser, text);
					
					obj[key] = parse(parser, text);
					if (!obj[key].isValid())
					{
						return error(parser);
					}

					parser.inc();
					skipWhitespaces(parser, text);
					if (parser.get() != ',')
					{
						break;
					}
					parser.inc();
					skipWhitespaces(parser, text);
				}
				return obj;
			}

			inline JsonObject parse(JsonParser& parser, const std::string& text)
			{
				if (parser.get() == '{')
				{
					return parseMany(parser, text);
				} 
				else if (parser.get() == '[')
				{
					// array
					parser.inc();
					std::vector<JsonObject> vec;
					while (parser.get() != ']' && parser.it != text.end())
					{
						skipWhitespaces(parser, text);
						vec.push_back(parse(parser, text));
						parser.inc();
						if (parser.get() != ',')
						{
							break;
						}
						parser.inc();
						skipWhitespaces(parser, text);
					}
					return JsonObject(vec);
				}
				else if (parser.get() == '"')
				{
					// string
					parser.inc();
					const std::string::const_iterator start = parser.it;
					while (parser.get() != '"' && parser.it != text.end())
					{
						parser.inc();
					}

					std::string value = text.substr(start - text.begin(), parser.it - start);
					return JsonObject(value);
				}
				else
				{
					// primitive
					const std::string::const_iterator start = parser.it;
					while (parser.get() != ',' && parser.it != text.end())
					{
						parser.inc();
					}
					std::string value = text.substr(start - text.begin(), parser.it - start);
					parser.it--; // we go back one step because that's the behavior expected of this function.
					if (value == "true")
					{
						return JsonObject(true);
					}
					else if (value == "false")
					{
						return JsonObject(false);
					}
					else if (value.find(".") != std::string::npos)
					{
						return JsonObject(std::stod(value));
					}
					else if (value.find("-") != std::string::npos)
					{
						return JsonObject(std::stol(value));
					}
					else
					{
						return JsonObject(std::stoul(value));
					}
				}
			}

			inline void skipWhitespaces(JsonParser& parser, const std::string& text)
			{
				// skip white space
				while (parser.get() == ' ' || parser.get() == '\n' || parser.get() == '\t' && parser.it != text.end())
				{
					parser.inc();
				}
			}

			inline JsonObject error(const JsonParser& parser)
			{
				std::cout << std::format("[ManiZ::json]: failed to parse, error at line {}:{}", parser.line, parser.column) << std::endl;
				return JsonObject();
			}
		}

		// object builder
		namespace _impl
		{
			inline void deserializeMany(size_t index, const JsonObject& json, auto& first, auto& ...others);
			inline void deserializeMany(size_t index, const JsonObject& json);
			inline void deserialize(size_t index, const JsonObject& json, auto& data, bool isLeaf = false);

			inline void deserializeMany(size_t index, const JsonObject& json) {}

			inline void deserializeMany(size_t index, const JsonObject& json, auto& first, auto& ...others)
			{
				deserialize(index, json, first);
				deserializeMany(index + 1, json, others...);
			}
			
			inline void deserialize(size_t index, const JsonObject& json, auto& data, bool isLeaf)
			{
				using type = std::remove_cvref_t<decltype(data)>;
				static_assert(!std::is_pointer_v<type>);

				if (!isLeaf)
				{
					if constexpr (!ManiZ::is_aggregate_struct<type>)
					{
						deserialize(0, json.getAt(index), data, true);
						return;
					}
				}

				if constexpr (std::is_enum_v<type> || std::is_fundamental_v<type> || ManiZ::is_string<type>::value)
				{
					if (isLeaf)
					{
						data = json.get<type>();
					}
				}
				else if constexpr (std::ranges::range<type>)
				{
					using value_type = typename type::value_type;
					
					// hard iterate over the container
					const std::vector<JsonObject>& jsonArray = json.getArray();
					const size_t size = jsonArray.size();

					if constexpr (requires { data.resize(size); })
					{
						data.resize(size);
					}
					
					for (size_t index = 0; index < size; index++)
					{
						const JsonObject& jsonObject = jsonArray[index];
						constexpr bool isLeaf = true;
						deserialize(0, jsonObject, data[index], isLeaf);
					}
				}
				else
				{
					// we're in an aggregate type
					RFL::visitMembers(data, [&](auto& ...members)
					{
						// recursively serialize the members.
						if (isLeaf)
						{
							// we're in a nested structure
							deserializeMany(0, json, members...);
						}
						else
						{
							deserializeMany(0, json.getAt(index), members...);
						}
					});
				}
			}
		}

		inline JsonObject parse(const std::string& jsonString)
		{
			if (jsonString.empty())
			{
				return JsonObject();
			}
			_impl::JsonParser parser;
			parser.it = jsonString.begin();
			return _impl::parseMany(parser, jsonString);
		}

		template<class T>
		inline T json(const std::string& jsonString)
		{
			T obj;
			JsonObject json = parse(jsonString);
			// we wrap the json object to kickstart the deserialization recursion
			JsonObject wrapper;
			wrapper["value"] = std::move(json);
			_impl::deserializeMany(0, wrapper, obj);
			return obj;
		}
	}
}