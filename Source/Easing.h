#pragma once
#include <math.h>

float AttackEasing(float x)
{
	const float c1 = 1.70158;
	const float c2 = c1 * 1.525;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

float PushEasing(float x)
{
	const float c1 = 1.70158;
	const float c2 = c1 * 1.525;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}
