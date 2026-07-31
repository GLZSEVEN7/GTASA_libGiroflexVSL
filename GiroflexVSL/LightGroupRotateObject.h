#pragma once

#include "pch.h"

enum class eRotateObjectAxis {
	X,
	Y,
	Z
};

enum class eRotateMode
{
    CONTINUOUS,
    OSCILLATE
};

struct LightGroupRotateObject {
	bool rotate = true;
	bool rotateAlways = false;
	float speed = 5.0f;
	eRotateObjectAxis axis = eRotateObjectAxis::Z;
	std::string object = "[none]";
	float directionFix = 0.0f;
	
	eRotateMode mode = eRotateMode::CONTINUOUS;
	float oscillationAngle = 90.0f;
	
};