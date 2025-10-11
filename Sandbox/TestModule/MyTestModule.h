#pragma once

namespace TestModule
{
	struct MyTestStruct
	{
		float a = 0.f;
		float b = 0.f;
		float c = 0.f;
	};

	template<typename T>
	struct MyTestTemplate
	{
		T a = T();
	};
}