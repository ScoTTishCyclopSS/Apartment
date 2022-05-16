#include "object.h"

struct CoffeeMake
{
	Object* cup;
	Object* fill;
	Object* coffeeMachine;
	Object* coffeeFlow;

	bool filling;
	bool pickebleCup;
	bool cupPicked;

	float fillYpos_old = 0.0;
	float waveStrength = 0.2;
	float layer = 0.4;
};

struct Window
{
	Object* window;
	Object* glass;
	bool opened;
	bool opening;
	float oldPosY = 0.0;
};

struct Lamp
{
	Object* lamp;
	bool active = false;
};

struct Robot {
	MyCurve* coons;
	float t2 = 0.0;
	bool backtrack = false;
};