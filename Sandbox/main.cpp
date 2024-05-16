#include <iostream>

#include <ManiZ/maniz.h>

struct Vector
{
	double x;
	double y;
	double z;
};

struct Transform
{
	Vector position;
	Vector rotation;
	Vector scale;
};

int main()
{
	Vector v = { .x = 1.0, .y = 2.0, .z = 3.0 };
	Transform t = { .position = v, .rotation = v, .scale = v };

	Transform t2 =
	{
		.position = { .x = 5.0, .y = 1000.0, .z = 0.0 },
		.rotation = {.x = 0.0, .y = 0.0, .z = 0.0 },
		.scale = {.x = 1.0, .y = 1.0, .z = 1.0 },
	};

	//std::cout << ManiZ::toJson(t) << std::endl;
	std::cout << ManiZ::RFL::getFieldName<Vector, 1>() << " = " << t2.position.y << std::endl;
	return EXIT_SUCCESS;
}