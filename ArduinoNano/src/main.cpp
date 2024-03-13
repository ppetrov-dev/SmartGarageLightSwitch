#pragma once
#include <Arduino.h>
#include "Configuration.h"

#include <BroButton.h>

BroButton _sensorButtons[SECTION_COUNT];

bool _relayStates[SECTION_COUNT];

void ChangeRelayState(byte buttonPin);
void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial OK");

    for (auto i = 0; i < SECTION_COUNT; i++)
    {
        _relayStates[i] = RELAY_STATE_DEFAULT;
        pinMode(RELAY_START_PIN + i, OUTPUT);

        _sensorButtons[i].ChangePin(BUTTON_START_PIN + i);
        _sensorButtons[i].AttachOnClick(&ChangeRelayState);
        _sensorButtons[i].Init();
    }
}

void loop()
{
    for (auto i = 0; i < SECTION_COUNT; i++)
        _sensorButtons[i].Tick();
}

byte GetIndex(byte buttonPin)
{
    return buttonPin - BUTTON_START_PIN;
}
void ChangeRelayState(byte buttonPin)
{
    auto index = GetIndex(buttonPin);

    auto newState = !_relayStates[index];
    digitalWrite(RELAY_START_PIN + index, newState);
    _relayStates[index] = newState;
}
