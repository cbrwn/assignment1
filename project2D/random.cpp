#include <cmath>

int randBetween(int min, int max)
{
	return min + (rand() % max - min);
}

float randBetween(float min, float max)
{
	float percentage = (rand() % 10000) / 10000.0f;
	return min + ((max - min) * percentage);
}