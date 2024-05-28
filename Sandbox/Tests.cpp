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
		};
		
		Transform t{ { 1.f, 2.f }, { 3.f, 4.f }, { 5.f, 6.f } };

		std::string json = ManiZ::to::json(t);
		std::cout << json << std::endl;
		Transform t2 = ManiZ::from::json<Transform>(json);

		MANI_ASSERT(std::abs(t.position.x - t.position.y) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.position.x - t.position.y) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(std::abs(t.rotation.x - t.rotation.y) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.rotation.x - t.rotation.y) < FLT_EPSILON, "before and after should be equal");

		MANI_ASSERT(std::abs(t.scale.x - t.scale.y) < FLT_EPSILON, "before and after should be equal");
		MANI_ASSERT(std::abs(t.scale.x - t.scale.y) < FLT_EPSILON, "before and after should be equal");
	}
}
MANI_SECTION_END(Json)
