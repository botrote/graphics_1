#include "random.h"
#include <ctime>
#include <random>


float Random::getRandomFloat(float start, float end)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(start, end);
	return distr(eng);
}

int Random::getRandomInt(int start, int end)
{
	srand(time(0));
	return rand() % (end - start + 1) + start;
}