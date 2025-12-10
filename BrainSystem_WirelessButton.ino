#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <EncButton.h>
#include <RGBLED.h>

#include "src/Link/Codes.h"

#define BUTTON_PIN 5
#define LED_R_PIN 6
#define LED_G_PIN 7
#define LED_B_PIN 10

uint8_t serverMac[] = {0x5C, 0x01, 0x3B, 0x66, 0xE3, 0x7C};

Button button;
RGBLED rgb(LED_R_PIN, LED_G_PIN, LED_B_PIN);

constexpr RGB::Color colors[4] = {RGB::Color::Orange, RGB::Color::Blue, RGB::Color::Green, RGB::Color::Red};

enum class State
{
  Idle,
  Correct,
  Falstart,
  Pending,
  Pairing
};

State state = State::Idle;
bool ledDirty = true;
RGB::Color currentColor = RGB::Color::Black;
uint8_t currentBrightness = 128;

unsigned long timeMarker;

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

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *raw, int len)
{
  if (state != State::Pairing)
  {
    if(len != 1)
    {
      return;
    }

    unsigned char command = *raw & 0xF0;
    unsigned char payload = *raw & 0x0F;

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
  else
  {

  }
}

void setup() {
    Serial.begin(9600);

    button.init(BUTTON_PIN, INPUT);

    WiFi.mode(WIFI_STA);
    WiFi.setChannel(1);
    WiFi.setTxPower(WIFI_POWER_17dBm);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

    esp_now_init();
    esp_now_register_recv_cb(OnDataRecv);

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, serverMac, 6);
    peer.channel = 1;
    peer.encrypt = false;
    peer.ifidx = WIFI_IF_STA;
    esp_now_add_peer(&peer);

    esp_now_rate_config_t config = {};
    config.phymode = WIFI_PHY_MODE_LR;
    config.rate = WIFI_PHY_RATE_LORA_250K;
    config.ersu = false;  
    config.dcm = false;
    esp_now_set_peer_rate_config(serverMac, &config);
}

void loop() 
{
  button.tick();

  if(state == State::Pairing)
  {
    processPairing();
    return;
  }

  if(button.press())
  {
    uint8_t data = LINK_BUTTON_PRESSED;
    esp_now_send(serverMac, &data, 1);
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

  if(ledDirty)
  {
    rgb.setColor(currentColor, currentBrightness);
    ledDirty = false;
  }
}