#include <ManiTests/ManiTests.h>
#include <ManiZ/reflection.h>

MANI_SECTION_BEGIN(Reflection, "reflection")
{
	MANI_TEST(ShouldReflectNames, "reflect names")
	{
		struct Vector
		{
			float x;
			float y;
			float z;
		};

		struct Transform
		{
			Vector position;
			Vector rotation;
			Vector scale;
		};

		const std::string fieldName0 = ManiZ::RFL::getFieldName<Transform, 0>();
		const std::string fieldName1 = ManiZ::RFL::getFieldName<Transform, 1>();
		const std::string fieldName2 = ManiZ::RFL::getFieldName<Transform, 2>();

		MANI_ASSERT(fieldName0 == "position", "field name should be correct");
		MANI_ASSERT(fieldName1 == "rotation", "field name should be correct");
		MANI_ASSERT(fieldName2 == "scale", "field name should be correct");
	}

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

	MANI_TEST(ShouldForeachMember, "Should iterate other the members")
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

		Transform t;
		const bool result = ManiZ::RFL::decompose(t, [](const auto& ...m) -> bool
		{
			return sizeof...(m) == 3;
		});
		
		MANI_ASSERT(result == true, "members should be properly decomposed");
	}
}
MANI_SECTION_END(Reflection)