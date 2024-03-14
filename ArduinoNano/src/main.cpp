#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "Configuration.h"

#include <BroButton.h>
#include <BroTimer.h>

BroTimer _defferedSaveSateTimer;
BroButton _sensorButtons[SECTION_COUNT];
bool _relayStates[SECTION_COUNT];

const char SAVED_IN_EEPROM_CHAR = 'K';

void ChangeRelayState(byte buttonPin);
void RestoreRelayState(byte index);
void SaveRelayStates();
bool NeedsToRestoreStates();

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial OK");

    _defferedSaveSateTimer.SetInterval(SAVE_STATE_MILLISECONDS);
    _defferedSaveSateTimer.AttachOnElapsed(&SaveRelayStates);

    auto needsToRestoreStates = NeedsToRestoreStates();
    for (byte index = 0; index < SECTION_COUNT; index++)
    {
        if (needsToRestoreStates)
            RestoreRelayState(index);
        else
            _relayStates[index] = RELAY_STATE_DEFAULT;

        byte relayPin = RELAY_START_PIN + index;
        pinMode(relayPin, OUTPUT);
        digitalWrite(relayPin, _relayStates[index]);

        _sensorButtons[index].ChangePin(BUTTON_START_PIN + index);
        _sensorButtons[index].AttachOnClick(&ChangeRelayState);
        _sensorButtons[index].Init();
    }
}

void loop()
{
    for (byte i = 0; i < SECTION_COUNT; i++)
        _sensorButtons[i].Tick();

    _defferedSaveSateTimer.Tick();
}

byte GetIndex(byte buttonPin)
{
    return buttonPin - BUTTON_START_PIN;
}

bool NeedsToRestoreStates()
{
    char storedChar = EEPROM.read(SECTION_COUNT * sizeof(bool));
    return SAVED_IN_EEPROM_CHAR == storedChar;
}

void RestoreRelayState(byte index)
{
    bool storedState = EEPROM.read(index * sizeof(bool));
    _relayStates[index] = storedState;
}

void ChangeRelayState(byte buttonPin)
{
    auto index = GetIndex(buttonPin);
    auto newState = !_relayStates[index];

    digitalWrite(RELAY_START_PIN + index, newState);
    _relayStates[index] = newState;

    _defferedSaveSateTimer.Restart();
}

void SaveRelayStates()
{
    _defferedSaveSateTimer.Stop();

    for (byte index = 0; index < SECTION_COUNT; index++)
        EEPROM.write(index * sizeof(bool), _relayStates[index]);

    EEPROM.write(SECTION_COUNT * sizeof(bool), SAVED_IN_EEPROM_CHAR);
}