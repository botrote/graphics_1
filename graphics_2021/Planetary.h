#pragma once
#include "default.h"

class Planetary
{
public:
	Planetary(Pos starPos, float starRad, float planetRad, float satelliteRad);
	void update();
	Pos starPosition;
	Color starColor;
	Color planetColor;
	Color satelliteColor;
	float starRadius;
	float planetRadius;
	float satelliteRadius;
	float planetRevoRadius;
	float satelliteRevoRadius;
	float planetAngle;
	float satelliteAngle;
};

