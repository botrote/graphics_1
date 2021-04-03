#include "Planetary.h"
#include "Drawer.h"
#include "random.h"

Planetary::Planetary(Pos starPos, float starRad, float planetRad, float satelliteRad)
{
	starPosition = starPos;

	starColor = Drawer::getRandomColor();
	planetColor = Drawer::getRandomColor();
	satelliteColor = Drawer::getRandomColor();
	
	starRadius = starRad;
	planetRadius = planetRad;
	satelliteRadius = satelliteRad;
	
	planetRevoRadius = 3.5 * starRadius;
	satelliteRevoRadius = 2.5 * planetRadius;

	planetAngle = Random::getRandomFloat(0, 360);
	satelliteAngle = Random::getRandomFloat(0, 360);

}

void Planetary::update()
{
	planetAngle += starRadius;
	satelliteAngle += planetRadius * 2;
}