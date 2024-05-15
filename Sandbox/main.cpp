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
	std::cout << ManiZ::toJson(t) << std::endl;
	return EXIT_SUCCESS;
}