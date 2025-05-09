#pragma once
#include <ManiTests/ManiTests.h>
#include <ManiZ/ManiZ.h>

MANI_SECTION_BEGIN(Reflection, "reflection")
{
	MANI_TEST(ShouldCountMembers, "Should count members")
	{
		struct Vector
		{
			float x;
			float y;
		};

		struct Transform
		{
			Vector position;
			Vector rotation;
			Vector scale;
		};

		const size_t vectorCount = ManiZ::RFL::memberCount<Vector>();
		const size_t transformCount = ManiZ::RFL::memberCount<Transform>();
		
		MANI_ASSERT(vectorCount == 2, "member count should match");
		MANI_ASSERT(transformCount == 3, "member count should match");
	}

	MANI_TEST(ShouldGetMembersName, "Should get members name")
	{
		struct Vector
		{
			float x;
			float y;
		};

		struct Transform
		{
			Vector position;
			Vector rotation;
			Vector scale;
		};

		const std::string vector0Name = ManiZ::RFL::getMemberName<Vector, 0>();
		const std::string transform1Name = ManiZ::RFL::getMemberName<Transform, 1>();

		MANI_ASSERT(vector0Name == "x", "member name should match");
		MANI_ASSERT(transform1Name == "rotation", "member name should match");
	}

	MANI_TEST(ShouldGetAllMembersNames, "Should get all members names")
	{
		struct Vector
		{
			float x;
			float y;
		};

		struct Transform
		{
			Vector position;
			Vector rotation;
			Vector scale;
		};

		const std::vector<std::string> vectorNames = ManiZ::RFL::getMemberNames<Vector>();
		const std::vector<std::string> transformNames = ManiZ::RFL::getMemberNames<Transform>();

		std::vector<std::string> vectorNamesRef = { "x", "y" };
		std::vector<std::string> transformNamesRef = { "position", "rotation", "scale"};

		MANI_ASSERT(vectorNames.size() == vectorNamesRef.size(), "member count should match");
		MANI_ASSERT(transformNames.size() == transformNamesRef.size(), "member count should match");

		for (size_t i = 0; i < vectorNames.size(); i++)
		{
			MANI_ASSERT(vectorNames[i] == vectorNamesRef[i], "member name should match");
		}

		for (size_t i = 0; i < transformNames.size(); i++)
		{
			MANI_ASSERT(transformNames[i] == transformNamesRef[i], "member name should match");
		}
	}
}
MANI_SECTION_END(Reflection)

MANI_SECTION_BEGIN(Json, "Json")
{
	MANI_TEST(ShouldSerializeAndDeserializeAStruct, "Should serialize then deserialize a struct to json")
	{
		struct Vector
		{
			float x;
			float y;
		};

		struct Transform
		{
			Vector position;
			Vector rotation;
			Vector scale;

			std::vector<int> vector;
			std::vector<std::string> strings;

			bool boolean;
		};
		
		Transform t{ { 1.5f, 2.5f }, { 3.f, 4.f }, { 5.f, 6.f }, { 1, 2, 3, 4 }, { "un", "deux", "trois", "quatre" }, true };

		std::string json = ManiZ::to::json(t);
		Transform t2 = ManiZ::from::json<Transform>(json);

		MANI_ASSERT(std::abs(t.position.x - t2.position.x) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.position.y - t2.position.y) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(std::abs(t.rotation.x - t2.rotation.x) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.rotation.y - t2.rotation.y) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(std::abs(t.scale.x - t2.scale.x) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.scale.y - t2.scale.y) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(t.vector.size() == t2.vector.size(), "vectors should be of the same size");
		MANI_ASSERT(t.strings.size() == t2.strings.size(), "vectors should be of the same size");

		for (size_t i = 0; i < t.vector.size(); ++i)
		{
			MANI_ASSERT(t.vector[i] == t2.vector[i], "before and after should be equal");
		}

		for (size_t i = 0; i < t.strings.size(); ++i)
		{
			MANI_ASSERT(t.strings[i] == t2.strings[i], "before and after should be equal");
		}
	}

	MANI_TEST(JsonObject, "Should properly allocate and free a Json Object")
	{
		{
			ManiZ::JsonObject obj(5);
			MANI_ASSERT(obj.get<short>() == 5, "should match value");
			MANI_ASSERT(obj.get<int>() == 5, "should match value");
			MANI_ASSERT(obj.get<long>() == 5, "should match value");
		}

		{
			ManiZ::JsonObject obj(std::numeric_limits<unsigned long>::max());			
			MANI_ASSERT(obj.get<unsigned long>() == std::numeric_limits<unsigned long>::max(), "should match value");
			MANI_ASSERT(obj.get<uint64_t>() == std::numeric_limits<unsigned long>::max(), "should match value");
		}

		{
			ManiZ::JsonObject obj(5.0);
			MANI_ASSERT(std::abs(obj.get<float>() - 5.0f) < FLT_EPSILON, "should match value");
			MANI_ASSERT(std::abs(obj.get<double>() - 5.0) < FLT_EPSILON, "should match value");
		}

		{
			ManiZ::JsonObject obj("Hello");
			MANI_ASSERT(obj.get<std::string>() == "Hello", "should match value");
		}

		{
			ManiZ::JsonObject obj(true);
			MANI_ASSERT(obj.get<bool>() == true, "should match value");
		}

		{
			ManiZ::JsonObject obj(std::vector { ManiZ::JsonObject(1), ManiZ::JsonObject(2), ManiZ::JsonObject(3), ManiZ::JsonObject(4) });
			MANI_ASSERT(obj.getArray().size() == 4, "should match value");
		}
	}

	MANI_TEST(ShouldSerializeAndParse, "Should serialize then parse the json string")
	{
		struct Vector
		{
			float x;
			float y;
		};

		struct Transform
		{
			Vector position;
			Vector rotation;
			Vector scale;

			std::vector<int> vector;
			std::vector<std::string> strings;

			bool boolean;
		};

		Transform t{ { 1.5f, 2.5f }, { 3.0f, 4.f }, { 5.f, 6.f }, { 1, 2, 3, 4 }, { "un", "deux", "trois", "quatre" }, true };

		std::string json = ManiZ::to::json(t);
		ManiZ::JsonObject jsonObject = ManiZ::from::parse(json);

		MANI_ASSERT(jsonObject.isValid(), "Json object should be valid");

		MANI_ASSERT(std::abs(t.position.x - jsonObject["position"]["x"].get<float>()) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.position.y - jsonObject["position"]["y"].get<float>()) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(std::abs(t.rotation.x - jsonObject["rotation"]["x"].get<float>()) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.rotation.y - jsonObject["rotation"]["y"].get<float>()) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(std::abs(t.scale.x - jsonObject["scale"]["x"].get<float>()) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.scale.y - jsonObject["scale"]["y"].get<float>()) < FLT_EPSILON, "before and after should be equal");
	}

	MANI_TEST(ShouldSerializeAndParseStdArray, "Should serialize then parse an std::array")
	{
		struct Test
		{
			std::array<int, 3> values;
		};

		Test t = { .values = { 1, 2, 3 } };
		std::string json = ManiZ::to::json(t);
		Test t2 = ManiZ::from::json<Test>(json);

		bool result = true;
		for (int i = 0; i < 3; i++)
		{
			result &= t.values[i] == t2.values[i];
		}

		MANI_ASSERT(result, "Both Test objects should be equal");
	}

	MANI_TEST(ShouldSerializeAndParseUnsignedIntegers, "Should serialize then parse an unsigned integer")
	{
		struct Test
		{
			unsigned int value = 0;
		};

		Test t = { .value = 5 };
		std::string json = ManiZ::to::json(t);
		Test t2 = ManiZ::from::json<Test>(json);

		MANI_ASSERT(t.value == t2.value, "Both Test integers should be equal");
	}

	MANI_TEST(ShouldSerializeAndParseDoubleArrays, "Should serialize double arrays")
	{
		struct Test
		{
			std::array<std::array<int, 3>, 3> data;
		};

		Test t = { 
			.data = {{
				{ 0, 1, 2 },
				{ 3, 4, 5 },
				{ 6, 7, 8 }
			}}
		};
		std::string json = ManiZ::to::json(t);
		Test t2 = ManiZ::from::json<Test>(json);
		MANI_ASSERT(t.data == t2.data, "Both Test integers should be equal");
	}

	MANI_TEST(ShouldSerializeAndParseArrayWithNestedStruct, "Should serialize double arrays")
	{
		struct Vector
		{
			int x;
			int y;
		};

		struct Test
		{
			std::array<Vector, 3> data;
		};

		Test t = {
			.data = {{
				{ 0, 1 },
				{ 3, 4 },
				{ 6, 7 }
			}}
		};
		std::string json = ManiZ::to::json(t);
		Test t2 = ManiZ::from::json<Test>(json);

		MANI_ASSERT(t.data.size() == t2.data.size(), "both arrays should be the same size");
		for (size_t index = 0; index < std::min(t.data.size(), t2.data.size()); index++)
		{
			MANI_ASSERT(t.data[index].x == t2.data[index].x, "data should be the same.");
			MANI_ASSERT(t.data[index].y == t2.data[index].y, "data should be the same.");
		}
	}

	MANI_TEST(ShouldSkipWindowsSpecificReturnCaseCharacters, "Should skip \\r characters")
	{
		struct Test
		{
			std::string a;
			std::string b;
		};

		Test t = {
			"Coucou",
			"salut"
		};
		
		std::string jsonString = "{\r\t\"a\": \"Coucou\",\r\t\"b\": \"salut\"\r}";
		Test t2 = ManiZ::from::json<Test>(jsonString);

		MANI_ASSERT(t.a == t2.a, "should have deserialized properly");
		MANI_ASSERT(t.b == t2.b, "should have deserialized properly");
	}

	MANI_TEST(ShouldHandlePrimitivesAtTheEndOfObjectStructures, "ShouldHandlePrimitivesAtTheEndOfObjectStructures")
	{
		struct Test
		{
			std::string a;
			std::string b;
			float value;
		};

		Test t = {
			"Coucou",
			"salut",
			1.f
		};

		{
			std::string jsonString = "{\r\t\"a\": \"Coucou\",\r\t\"b\": \"salut\",\r\t\"value\": 1.0\r}";
			Test t2 = ManiZ::from::json<Test>(jsonString);
			MANI_ASSERT(t.a == t2.a, "should have deserialized properly");
			MANI_ASSERT(t.b == t2.b, "should have deserialized properly");
			MANI_ASSERT(std::abs(t.value - t2.value) < FLT_EPSILON, "should have deserialized properly");
		}
		{
			std::string jsonString = "{\"a\":\"Coucou\",\"b\":\"salut\",\"value\":1.0}";
			Test t2 = ManiZ::from::json<Test>(jsonString);
			MANI_ASSERT(t.a == t2.a, "should have deserialized properly");
			MANI_ASSERT(t.b == t2.b, "should have deserialized properly");
			MANI_ASSERT(std::abs(t.value - t2.value) < FLT_EPSILON, "should have deserialized properly");
		}
	}

	MANI_TEST(ShouldHandleEnums, "Should Handle Enums")
	{
		enum class ESomeEnum : int
		{
			Value_1 = -1,
			Value0 = 0,
			Value1,
			Value2,
		};

		struct Test
		{
			ESomeEnum value;
		};

		{
			Test t = { .value = ESomeEnum::Value2 };
			const std::string jsonString = ManiZ::to::json(t);
			Test t2 = ManiZ::from::json<Test>(jsonString);
			MANI_ASSERT(t.value == t2.value, "should have deserialized properly");
		}

		{
			Test t = { .value = ESomeEnum::Value_1 };
			const std::string jsonString = ManiZ::to::json(t);
			Test t2 = ManiZ::from::json<Test>(jsonString);
			MANI_ASSERT(t.value == t2.value, "should have deserialized properly");
		}
	}

	MANI_TEST(HandleStandardizedJson, "should handle standard json coming from outside sources")
	{
		struct Vector
		{
			int x;
			int y;
		};

		struct Test
		{
			std::vector<Vector> data;
		};

		std::string jsonString = "{\n\t\"data\": [\n\t\t{\n\t\t\t\"x\": 0,\n\t\t\t\"y\" : 1\n\t\t},\n\t\t{\n\t\t\t\"x\": 3,\n\t\t\t\"y\" : 4\n\t\t},\n\t\t{\n\t\t\t\"x\": 6,\n\t\t\t\"y\" : 7\n\t\t}\n\t]\n}";
		Test t = ManiZ::from::json<Test>(jsonString);
		MANI_ASSERT(t.data.size() == 3, "should have deserialized 3 vectors");
		std::vector<Vector> expected = { {0, 1}, {3, 4}, {6, 7} };
		for (size_t i = 0; i < 3; i++)
		{
			MANI_ASSERT(t.data[i].x == expected[i].x, "Vectors should be equal");
			MANI_ASSERT(t.data[i].y == expected[i].y, "Vectors should be equal");
		}
	}

	MANI_TEST(ShouldParseImcompleteJsonString, "Should be able to parse an json object that is a subset of a struct")
	{
		struct Test
		{
			int a = 0;
			int b = 0;
			int c = 0;
		};

		std::string jsonString =	"{\n"
									"\t\"a\": 4,\n"
									"\t\"c\": 2\n"
									"}";
		Test t = ManiZ::from::json<Test>(jsonString);
		MANI_ASSERT(t.a == 4 && t.b == 0 && t.c == 2, "t should be equal to the json values");
	}
}
MANI_SECTION_END(Json)
