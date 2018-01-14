#include "stdafx.h"
#include "StaticSettings.h"

/* Linking our static settings */
bool Settings::bDebug = false;

const float Settings::PING_RATE = 0.1f;
const float Settings::NET_RATE = 1.0f / 15.0f;
const float Settings::TIMEOUT_VAL = 2.0f;

sf::Font* Settings::m_DefaultFont = nullptr;