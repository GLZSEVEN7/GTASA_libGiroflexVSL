#pragma once

#include "pch.h"

#include "LightGroup.h"
#include "Pattern.h"
#include "PatternLoop.h"

class LightGroupData {
public:
	std::vector<Pattern*> patterns;

	PatternLoop* patternLoop = new PatternLoop();
	PatternLoop* stepLoop = new PatternLoop();
	LightGroup* lightGroup;
	int hVehicle;
	bool lightsOn = false;
	
	// Fade LED
bool fadeActive = false;
std::vector<bool> smoothAnimating;

float fadeIntensityA = 1.0f;
float fadeIntensityB = 0.0f;

std::vector<float> smoothIntensity;
bool lastLightsOn = false;

int fadeTimer = 0;
int fadeSpeed = 500;

bool fadeDirection = false;
	
	//add    bool lightsPaused = false;

	LightGroupData(LightGroup* lightGroup, int hVehicle)
	{
		this->lightGroup = lightGroup;
		this->hVehicle = hVehicle;
	}

	Pattern* GetCurrentPattern()
	{
		if(patterns.empty())
{
    std::cout << "No patterns available" << std::endl;
    return nullptr;
}

if(patternLoop->StepIndex >= patterns.size())
{
    std::cout << "Pattern index " << patternLoop->StepIndex << " not found" << std::endl;
    return nullptr;
}

		return patterns[patternLoop->StepIndex];
	}

	PatternStep* GetCurrenetStep()
{
    Pattern* pattern = GetCurrentPattern();

    if(pattern == nullptr)
        return nullptr;

    if(stepLoop->StepIndex >= pattern->steps.size())
        return nullptr;

    return pattern->steps[stepLoop->StepIndex];
}

	bool GetPointIsEnabled(Point* point, int index)
	{
		//auto pattern = GetCurrentPattern();
		auto step = GetCurrenetStep();

		//point->pointPosition == ePointPosition::BY_INDEX

		if (index > step->data.size() - 1) return 0;

		return step->data[index] == 1;
	}
	
	void UpdateFade(int dt)
{
    if(!fadeActive)
    {
        fadeIntensityA = 1.0f;
        fadeIntensityB = 0.0f;
        return;
    }

    float step = (float)dt / (float)fadeSpeed;

    if(!fadeDirection)
    {
        fadeIntensityA -= step;
        fadeIntensityB += step;
    }
    else
    {
        fadeIntensityA += step;
        fadeIntensityB -= step;
    }

    if(fadeIntensityA <= 0.0f)
    {
        fadeIntensityA = 0.0f;
        fadeIntensityB = 1.0f;
        fadeDirection = true;
    }

    if(fadeIntensityA >= 1.0f)
    {
        fadeIntensityA = 1.0f;
        fadeIntensityB = 0.0f;
        fadeDirection = false;
    }
}

float GetSmoothIntensity(int index, bool enabled, int dt)
{
    if(!lightGroup->smoothLedFade)
        return enabled ? 1.0f : 0.0f;

    if(index >= smoothIntensity.size())
{
    smoothIntensity.resize(index + 1, 0.0f);
    smoothAnimating.resize(index + 1, true);
}

float current = smoothIntensity[index];

if(enabled)
{
    if(current >= 1.0f)
        smoothAnimating[index] = false;
    else
        smoothAnimating[index] = true;
}
else
{
    if(current <= 0.0f)
        smoothAnimating[index] = false;
    else
        smoothAnimating[index] = true;
}

    float speed = (float)dt / (float)lightGroup->smoothLedFadeSpeed;

    if(smoothAnimating[index])
{
    if(enabled)
        smoothIntensity[index] = std::min(1.0f, smoothIntensity[index] + speed);
    else
        smoothIntensity[index] = std::max(0.0f, smoothIntensity[index] - speed);
}

    return smoothIntensity[index];
}

};