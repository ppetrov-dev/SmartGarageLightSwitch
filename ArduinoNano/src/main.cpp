#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "Configuration.h"

#include <BroButton.h>

BroButton _sensorButtons[SECTION_COUNT];

bool _relayStates[SECTION_COUNT];

void ChangeRelayState(byte buttonPin);
void RestoreRelayState(byte index);
void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial OK");

    for (byte i = 0; i < SECTION_COUNT; i++)
    {
        RestoreRelayState(i);
        byte relayPin = RELAY_START_PIN + i;
        pinMode(relayPin, OUTPUT);
        digitalWrite(relayPin, _relayStates[i]);

        _sensorButtons[i].ChangePin(BUTTON_START_PIN + i);
        _sensorButtons[i].AttachOnClick(&ChangeRelayState);
        _sensorButtons[i].Init();
    }
}

void loop()
{
    for (byte i = 0; i < SECTION_COUNT; i++)
        _sensorButtons[i].Tick();
}

byte GetIndex(byte buttonPin)
{
    return buttonPin - BUTTON_START_PIN;
}

void RestoreRelayState(byte index)
{
    byte storedState = EEPROM.read(index);

    if (storedState == 0xFF)// Проверка, было ли значение записано в EEPROM
    {                                         
        _relayStates[index] = RELAY_STATE_DEFAULT; // Установка значения по умолчанию, если EEPROM пустая
        EEPROM.write(index, _relayStates[index]);      // Запись значения в EEPROM
        return;
    }

    _relayStates[index] = storedState;
}
void ChangeRelayState(byte buttonPin)
{
    auto index = GetIndex(buttonPin);

    auto newState = !_relayStates[index];
    digitalWrite(RELAY_START_PIN + index, newState);
    _relayStates[index] = newState;

    // Сохраняем новое состояние в EEPROM
    EEPROM.write(index, newState);
}
