#include <EncButton.h>
#include <RGBLED.h>

#define USE_ESP_NOW_LINK
#include "src/Link/Codes.h"
#include "src/Link/EspNowInterface.h"

#define BUTTON_PIN 5
#define LED_R_PIN 6
#define LED_G_PIN 7
#define LED_B_PIN 10

uint8_t serverMac[6];
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

Button button;
RGBLED rgb(LED_R_PIN, LED_G_PIN, LED_B_PIN);

constexpr RGB::Color colors[4] = {RGB::Color::Orange, RGB::Color::Blue, RGB::Color::Green, RGB::Color::Red};

enum class State
{
  Idle,
  Correct,
  Falstart,
  Pending,
  Pairing,
  Connecting
};

State state = State::Connecting;
bool ledDirty = true;
RGB::Color currentColor = RGB::Color::Black;
uint8_t currentBrightness = 128;

unsigned long timeMarker;

vgs::link::EspNowInterface* interface;

void processCommand(const uint8_t* address, uint8_t data)
{
  if (state == State::Pairing)
  {
    return;
  }

  unsigned char command = data & 0xF0;
  unsigned char payload = data & 0x0F;

  if(command == LINK_CORRECT_PRESS_SIGNAL)
  {
    currentColor = colors[payload % 4];
    currentBrightness = 128;
    ledDirty = true;
    state = State::Correct;
    return;
  }
  if(command == LINK_FALSTART_PRESS_SIGNAL)
  {
    currentColor = colors[payload % 4];
    currentBrightness = 128;
    ledDirty = true;
    state = State::Falstart;
    timeMarker = millis();
    return;
  }
  if(command == LINK_PENDING_PRESS_SIGNAL)
  {
    currentColor = colors[payload % 4];
    currentBrightness = 128;
    ledDirty = true;
    state = State::Pending;
    timeMarker = millis();
    return;
  }
  if(command = LINK_CLEAR)
  {
    currentColor = RGB::Color::Black;
    ledDirty = true;
    state = State::Idle;
    return;
  }
}

void processPingResponse(const uint8_t* address, uint8_t data)
{
  if(state == State::Connecting)
  {
    currentColor = RGB::Color::Black;
    currentBrightness = 128;
    ledDirty = true;
    memcpy(serverMac, address, 6);
    state = State::Idle;
  }
}

void processPairingResponse(const uint8_t* address, uint8_t data)
{
  
}

class EspNowHandler : public vgs::link::EspNowHandler
{
  void handleEspNowMessage(const uint8_t* address, uint8_t header, uint8_t data) override
  {
    switch(header)
    {
      case LINK_WIRELESS_HEADER_COMMAND_V2:
        processCommand(address, data);
        break;
      case LINK_WIRELESS_HEADER_PING_RESPONSE:
        processPingResponse(address, data);
        break;
      case LINK_WIRELESS_HEADER_PAIRING_RESPONSE:
        processPairingResponse(address, data);
        break;
    }
  }
};

EspNowHandler handler;

void processIdle()
{

}

void processCorrect()
{

}

void processFalstart()
{
  if(millis() - timeMarker >= 500)
  {
    timeMarker = millis();
    currentBrightness = 128 - currentBrightness;
    ledDirty = true;
  }
}

void processPending()
{
  if(millis() - timeMarker >= 500)
  {
    timeMarker = millis();
    currentBrightness = 128 - currentBrightness;
    ledDirty = true;
  }
}

void processPairing()
{

}

void processConnecting()
{
  if(millis() - timeMarker >= 500)
  {
    timeMarker = millis();
    currentBrightness = 128 - currentBrightness;
    currentColor = RGB::Color::White;
    ledDirty = true;

    interface->send(broadcastMac, LINK_WIRELESS_HEADER_PING_REQUEST, 0);
  }
}

void setup() {
    Serial.begin(9600);

    button.init(BUTTON_PIN, INPUT);

    interface = vgs::link::EspNowInterface::getInstance();
    interface->setHandler(&handler);
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_17dBm);
}

void loop() 
{
  button.tick();

  if(ledDirty)
  {
    rgb.setColor(currentColor, currentBrightness);
    ledDirty = false;
  }

  if(state == State::Pairing)
  {
    processPairing();
    return;
  }
  if(state == State::Connecting)
  {
    processConnecting();
    return;
  }

  if(button.press())
  {
    interface->send(serverMac, LINK_WIRELESS_HEADER_COMMAND_V2, LINK_BUTTON_PRESSED);
  }

  switch(state)
  {
    case State::Idle:
      processIdle();
      break;
    case State::Correct:
      processCorrect();
      break;
    case State::Falstart:
      processFalstart();
      break;
    case State::Pending:
      processPending();
      break;
  }
}