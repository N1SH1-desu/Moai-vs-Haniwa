#pragma once
#include <math.h>

float Easing(float x)
{
	return 1 - powf(1 - x, 4);
}

float AttackEasing(float x)
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

float PushEasing(float x)
{
	float c1 = 10.0f;
	float c2 = c1 * 1.525f;

	if (x < 0.5)
	{
		c1 = 10.0f;
		c2 = c1 * 1.525f;
		return (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2;
	}
	else
	{
		c1 = 1.70158f;
		c2 = c1 * 1.525f;
		return (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
	}
}
