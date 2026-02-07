#pragma once
#include <cstdlib>

struct FMinMax
{
	float min = 0.0f;
	float max = 0.0f;

	bool isSet = false;

	FMinMax() = default;

	void Set(float minValue, float maxValue)
	{
		min = minValue;
		max = maxValue;

		isSet = true;
	}

	FMinMax(float minValue, float maxValue)
	{
		Set(minValue, maxValue);
	}
};

struct IMinMax
{
	int min = 0;
	int max = 0;

	bool isSet = false;

	IMinMax() = default;

	void Set(int minValue, int maxValue)
	{
		min = minValue;
		max = maxValue;

		isSet = true;
	}

	IMinMax(int minValue, int maxValue)
	{
		Set(minValue, maxValue);
	}
};

inline float RandomFloat(float min, float max)
{
	return  (max - min) * ((((float)rand()) / (float)RAND_MAX)) + min;
}

inline float RandomFloat01()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline int RandomInt(int min, int max)
{
	int range = max - min + 1;
	return rand() % range + min;
}

inline float Distance(int x1, int y1, int x2, int y2)
{
	float dx = (float)(x1 - x2);
	float dy = (float)(y1 - y2);
	return dx * dx + dy * dy;
}

inline float HorizontalDistance(int x1, int x2)
{
	return (float)sqrt((x1 - x2) * (x1 - x2));
}