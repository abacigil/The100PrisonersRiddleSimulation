#include "Random.h"
#include <random>

int Random::GenerateRandom(int i)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, i - 1);
	return dist(rng);
}
