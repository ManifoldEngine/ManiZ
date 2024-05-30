# ManiZip
ManiZip is a simple, single header, reflection and serialization library. It doesn't require any boilerplate code or macro.

# Installation instructions
include `include/ManiZ/ManiZ.h`

# How-to:
## Serialize and Deserialize
```c++
#include <ManiZ/ManiZ.h>

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

int maint()
{
    Transform t{ { 1.5f, 2.5f }, { 3.f, 4.f }, { 5.f, 6.f }, { 1, 2, 3, 4 }, { "un", "deux", "trois", "quatre" }, true };

    std::string json = ManiZ::to::json(t);
    Transform t2 = ManiZ::from::json<Transform>(json);
    return EXIT_SUCCESS;
}
```
## Parse a Json string
```c++
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

int main()
{
    Transform t{ { 1.5f, 2.5f }, { 3.0f, 4.f }, { 5.f, 6.f }, { 1, 2, 3, 4 }, { "un", "deux", "trois", "quatre" }, true };

    std::string json = ManiZ::to::json(t);
    ManiZ::JsonObject jsonObject = ManiZ::from::parse(json);

    assert(jsonObject.isValid());

    assert(std::abs(t.position.x - jsonObject["position"]["x"].get<float>()) < FLT_EPSILON);
    assert(std::abs(t.position.y - jsonObject["position"]["y"].get<float>()) < FLT_EPSILON);
    assert(std::abs(t.rotation.x - jsonObject["rotation"]["x"].get<float>()) < FLT_EPSILON);
    assert(std::abs(t.rotation.y - jsonObject["rotation"]["y"].get<float>()) < FLT_EPSILON);
    assert(std::abs(t.scale.x - jsonObject["scale"]["x"].get<float>()) < FLT_EPSILON);
    assert(std::abs(t.scale.y - jsonObject["scale"]["y"].get<float>()) < FLT_EPSILON);
}
```