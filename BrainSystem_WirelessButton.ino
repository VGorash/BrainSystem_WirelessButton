#include <EncButton.h>
#include <RGBLED.h>

#include "WirelessLink.h"

#define BUTTON_PIN 5
#define LED_R_PIN 6
#define LED_G_PIN 7
#define LED_B_PIN 10

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

WirelessLink wirelessLink;

void processIdle()
{
  if(currentColor != RGB::Color::Black)
  {
    currentColor = RGB::Color::Black;
    ledDirty = true;
  }
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
  if(wirelessLink.isInitialized())
  {
    toIdle();
    return;
  }

  if(millis() - timeMarker >= 500)
  {
    timeMarker = millis();
    currentBrightness = 128 - currentBrightness;
    currentColor = RGB::Color::White;
    ledDirty = true;
    wirelessLink.sendPairingRequest();
  }
}

void processCommand(vgs::link::Command command, uint8_t data)
{
  switch(command)
  {
    case vgs::link::Command::CorrectPressSignal:
      toCorrect(data);
      break;
    case vgs::link::Command::FalstartPressSignal:
      toFalstart(data);
      break;
    case vgs::link::Command::PendingPressSignal:
      toPending(data);
      break;
    case vgs::link::Command::Clear:
      toIdle();
      break;
  }
}

void toIdle()
{
  ledDirty = true;
  currentColor = RGB::Color::Black;
  currentBrightness = 0;
  state = State::Idle;
}

void toCorrect(uint8_t player)
{
  state = State::Correct;
  currentColor = colors[player % 4];
  currentBrightness = 128;
  ledDirty = true;
}

void toFalstart(uint8_t player)
{
  state = State::Falstart;
  currentColor = colors[player % 4];
  currentBrightness = 128;
  ledDirty = true;
  timeMarker = millis();
}

void toPending(uint8_t player)
{
  state = State::Pending;
  currentColor = colors[player % 4];
  currentBrightness = 128;
  ledDirty = true;
  timeMarker = millis();
}

void toPairing()
{
  state = State::Pairing;
  currentColor = RGB::Color::White;
  currentBrightness = 128;
  ledDirty = true;
  timeMarker = millis();
}

void setup() {
  Serial.begin(9600);
  button.init(BUTTON_PIN, INPUT);
  wirelessLink.init();
}

void loop() 
{
  button.tick();
  wirelessLink.tick();

  if(ledDirty)
  {
    rgb.setColor(currentColor, currentBrightness);
    ledDirty = false;
  }

  if(!wirelessLink.isInitialized() && state != State::Pairing)
  {
    toPairing();
  }

  if(state == State::Pairing)
  {
    processPairing();
    return;
  }

  if(button.press())
  {
    wirelessLink.send(vgs::link::Command::ButtonPressed);
  }

  if(wirelessLink.getCommand() != vgs::link::Command::None)
  {
    processCommand(wirelessLink.getCommand(), wirelessLink.getData());
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