#pragma once

#include "pch.h"
#include "Log.h"

#include <vector>
#include <string>
#include <algorithm>

#include <json/json.h>


class LedColor {
public:

    CRGBA coronaColor;
    CRGBA ledColor;
    CRGBA whiteCoronaColor;


    LedColor()
        : coronaColor(0,0,0,255),
          ledColor(0,0,0,255),
          whiteCoronaColor(0,0,0,0)
    {
    }
};



class PatternStep {
public:

    std::vector<int> data;
    int duration;

    std::vector<LedColor> ledColors;


    // Compatibilidade antiga
    bool useCustomColor = false;
    CRGBA customColor = CRGBA(255,255,255,255);


    bool useCustomLedColor = false;
    CRGBA customLedColor = CRGBA(255,255,255,255);


    // Começa desligado
    bool useCustomWhiteCoronaColor = false;
    CRGBA customWhiteCoronaColor = CRGBA(0,0,0,0);



    PatternStep()
    {
        duration = 0;
    }
};




class Pattern {

public:

    std::string id = "";

    std::vector<PatternStep*> steps;



    Pattern(std::string id)
    {
        this->id = id;
    }




    PatternStep* AddStep(std::vector<int> data, int duration)
    {
        PatternStep* step = new PatternStep();

        step->data = data;
        step->duration = duration;



        for(size_t i = 0; i < data.size(); i++)
        {
            LedColor led;


            led.coronaColor =
                CRGBA(0,0,255,255);


            led.ledColor =
                CRGBA(255,255,255,255);


            // White corona desligado por padrão
            led.whiteCoronaColor =
                CRGBA(0,0,0,0);



            step->ledColors.push_back(led);
        }



        steps.push_back(step);


        return step;
    }





    void RemoveStep(PatternStep* step)
    {
        auto it = std::find(
            steps.begin(),
            steps.end(),
            step
        );


        if(it == steps.end())
            return;


        steps.erase(it);

        delete step;
    }





    void RemoveAllSteps()
    {
        while(steps.size() > 0)
            RemoveStep(steps[0]);
    }





    Json::Value ToJSON()
    {
        Json::Value value = Json::objectValue;

        value["steps"] = Json::arrayValue;



        for(auto step : steps)
        {

            Json::Value stepValue =
                Json::objectValue;


            stepValue["duration"] =
                step->duration;


            stepValue["data"] =
                Json::arrayValue;



            for(auto d : step->data)
            {
                stepValue["data"].append(d);
            }



            stepValue["useCustomColor"] =
                step->useCustomColor;


            stepValue["customColor"] =
                ColorToJSON(step->customColor);



            stepValue["useCustomLedColor"] =
                step->useCustomLedColor;


            stepValue["customLedColor"] =
                ColorToJSON(step->customLedColor);



            stepValue["useCustomWhiteCoronaColor"] =
                step->useCustomWhiteCoronaColor;


            stepValue["customWhiteCoronaColor"] =
                ColorToJSON(step->customWhiteCoronaColor);




            stepValue["ledColors"] =
                Json::arrayValue;



            for(auto led : step->ledColors)
            {
                Json::Value ledValue =
                    Json::objectValue;


                ledValue["coronaColor"] =
                    ColorToJSON(
                        led.coronaColor
                    );


                ledValue["ledColor"] =
                    ColorToJSON(
                        led.ledColor
                    );


                ledValue["whiteCoronaColor"] =
                    ColorToJSON(
                        led.whiteCoronaColor
                    );


                stepValue["ledColors"]
                    .append(ledValue);
            }



            value["steps"].append(stepValue);
        }



        return value;
    }






    void FromJSON(Json::Value value)
{
    for(int step_i = 0;
        step_i < (int)value["steps"].size();
        step_i++)
    {

        Json::Value stepValue =
            value["steps"][step_i];


        int duration =
            stepValue["duration"].asInt();


        if(duration <= 0)
            continue;



        std::vector<int> data;


        for(int val_i = 0;
            val_i < (int)stepValue["data"].size();
            val_i++)
        {
            data.push_back(
                stepValue["data"][val_i].asInt()
            );
        }



        auto step =
            AddStep(data, duration);




        // ==================================
        // Compatibilidade antiga
        // ==================================

        step->useCustomColor =
            ValidateValue(
                stepValue["useCustomColor"],
                false
            ).asBool();


        step->customColor =
            ValidateColor(
                stepValue["customColor"],
                CRGBA(255,255,255,255)
            );



        step->useCustomLedColor =
            ValidateValue(
                stepValue["useCustomLedColor"],
                false
            ).asBool();


        step->customLedColor =
            ValidateColor(
                stepValue["customLedColor"],
                CRGBA(255,255,255,255)
            );



        step->useCustomWhiteCoronaColor =
            ValidateValue(
                stepValue["useCustomWhiteCoronaColor"],
                false
            ).asBool();


        step->customWhiteCoronaColor =
            ValidateColor(
                stepValue["customWhiteCoronaColor"],
                CRGBA(0,0,0,0)
            );




        // ==================================
        // Pattern NOVO
        // Tem ledColors individual
        // ==================================

        if(stepValue["ledColors"].isArray())
        {

            step->ledColors.clear();


            for(auto ledValue : stepValue["ledColors"])
            {

                LedColor led;


                led.coronaColor =
                    ValidateColor(
                        ledValue["coronaColor"],
                        CRGBA(0,0,255,255)
                    );


                led.ledColor =
                    ValidateColor(
                        ledValue["ledColor"],
                        CRGBA(255,255,255,255)
                    );


                led.whiteCoronaColor =
                    ValidateColor(
                        ledValue["whiteCoronaColor"],
                        CRGBA(0,0,0,0)
                    );


                step->ledColors.push_back(led);

            }

        }



        // ==================================
        // Pattern ANTIGO
        // Converte para ledColors
        // somente se não existir novo
        // ==================================

        else
{
    // Compatibilidade com patterns antigos:
    // copia as cores globais para todas as cores individuais.

    for(auto& led : step->ledColors)
    {
        // Corona principal
        if(step->useCustomColor)
            led.coronaColor = step->customColor;

        // LED
        if(step->useCustomLedColor)
            led.ledColor = step->customLedColor;

        // Ponto branco
        if(step->useCustomWhiteCoronaColor)
            led.whiteCoronaColor = step->customWhiteCoronaColor;
    }

    // Faz o renderer entrar no modo de cores individuais
    if(step->useCustomColor ||
       step->useCustomLedColor ||
       step->useCustomWhiteCoronaColor)
    {
        step->useCustomLedColor = true;
    }
}

}
}





~Pattern()
{
    RemoveAllSteps();
}

};