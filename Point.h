#pragma once

#include "pch.h"
#include "LightGroupRotateObject.h"

extern CCamera* camera;

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

enum class eSirenDirection {
	FRONT,
	BOTH,
	BACK,
	SIDES
};

class Point {
public:
	CVector customOffset = { 0, 0, 0 };

	bool useCustomColor = false;
	CRGBA customColor = { 255, 255, 0 };

	// Cor individual da Small White Corona
	bool useCustomWhiteCoronaColor = false;
	CRGBA customWhiteCoronaColor = { 255, 255, 255 };

	// Quantidade de Small White Coronas
	int whiteCoronaCount = 1;

	// Deslocamento da Small White Corona
	std::vector<CVector> whiteCoronaOffsets;

	bool useCustomLedColor = false;
	CRGBA customLedColor_on = { 255, 255, 255 };
	CRGBA customLedColor_off = { 0, 0, 0 };
	eSirenDirection customDirection = eSirenDirection::BOTH;

	LightGroupRotateObject rotateObject;

	Json::Value ToJSON()
	{
		Json::Value value = Json::objectValue;

		value["customOffset"] = CVectorToJSON(customOffset);
		value["useCustomColor"] = useCustomColor;
		value["customColor"] = ColorToJSON(customColor);

		value["useCustomWhiteCoronaColor"] = useCustomWhiteCoronaColor;
    value["customWhiteCoronaColor"] = ColorToJSON(customWhiteCoronaColor);

Json::Value offsets = Json::arrayValue;

for (auto& offset : whiteCoronaOffsets)
{
    offsets.append(CVectorToJSON(offset));
}

value["whiteCoronaOffsets"] = offsets;
value["whiteCoronaCount"] = whiteCoronaCount;

    value["useCustomLedColor"] = useCustomLedColor;
		value["customLedColor_on"] = ColorToJSON(customLedColor_on);
		value["customLedColor_off"] = ColorToJSON(customLedColor_off);
		value["customDirection"] = (int)customDirection;

		//rotate object
		Json::Value rotateObjectValue = Json::objectValue;
		rotateObjectValue["speed"] = rotateObject.speed;
		rotateObjectValue["object"] = rotateObject.object;
		rotateObjectValue["axis"] = (int)rotateObject.axis;
		rotateObjectValue["rotateAlways"] = rotateObject.rotateAlways;
		rotateObjectValue["directionFix"] = rotateObject.directionFix;
		rotateObjectValue["mode"] = (int)rotateObject.mode;
		value["rotateObject"] = rotateObjectValue;

		return value;
	}

	void FromJSON(Json::Value value)
	{
		customOffset = ValidateCVector(value["customOffset"], customOffset);

		useCustomColor = ValidateValue(value["useCustomColor"], useCustomColor).asBool();
		customColor = ValidateColor(value["customColor"], customColor);

		useCustomWhiteCoronaColor = ValidateValue(value["useCustomWhiteCoronaColor"], useCustomWhiteCoronaColor).asBool();
		customWhiteCoronaColor = ValidateColor(value["customWhiteCoronaColor"], customWhiteCoronaColor);
		
		whiteCoronaCount =
    ValidateValue(
        value["whiteCoronaCount"],
        whiteCoronaCount
    ).asInt();
    
    whiteCoronaOffsets.clear();

Json::Value offsets = value["whiteCoronaOffsets"];

if (offsets.isArray())
{
    for (auto& offset : offsets)
    {
        whiteCoronaOffsets.push_back(
            ValidateCVector(offset, CVector(0.0f, 0.0f, 0.0f))
        );
    }
}

useCustomLedColor =
    ValidateValue(
        value["useCustomLedColor"],
        useCustomLedColor
    ).asBool();
    
		customLedColor_on = ValidateColor(value["customLedColor_on"], customLedColor_on);
		customLedColor_off = ValidateColor(value["customLedColor_off"], customLedColor_off);
		customDirection = (eSirenDirection)ValidateValue(value["customDirection"], (int)customDirection).asInt();

		//rotate object
		Json::Value rotateObjectValue = value["rotateObject"];
		if(!rotateObjectValue.isNull())
		{
			rotateObject.speed = ValidateValue(rotateObjectValue["speed"], rotateObject.speed).asFloat();
			rotateObject.object = ValidateValue(rotateObjectValue["object"], rotateObject.object).asString();
			rotateObject.axis = (eRotateObjectAxis)ValidateValue(rotateObjectValue["axis"], (int)rotateObject.axis).asInt();
			rotateObject.rotateAlways = ValidateValue(rotateObjectValue["rotateAlways"], rotateObject.rotateAlways).asBool();
			rotateObject.directionFix = ValidateValue(rotateObjectValue["directionFix"], rotateObject.directionFix).asFloat();
			
			rotateObject.mode =
    (eRotateMode)ValidateValue(
        rotateObjectValue["mode"],
        (int)rotateObject.mode
    ).asInt();
			
		}
		
		while (whiteCoronaOffsets.size() < (size_t)whiteCoronaCount)
{
    whiteCoronaOffsets.push_back(CVector(0.0f, 0.0f, 0.0f));
}

while (whiteCoronaOffsets.size() > (size_t)whiteCoronaCount)
{
    whiteCoronaOffsets.pop_back();
}
		
	}

	static double GetAngle(CVehicle* vehicle, CVector position)
	{
		CVector cameraPos = camera->m_matrix->pos;

		auto vec1 = TransformFromObjectSpace(vehicle, CVector(position.x, 0.0001f, position.z));
		auto vec2 = TransformFromObjectSpace(vehicle, position);
		auto vec3 = cameraPos;

		double angle = GetAngleBetweenVectors(vec1, vec2, vec3);

		if (isnan(angle)) {
			angle = 0.0001;
		}
		return angle;
	}
}; 