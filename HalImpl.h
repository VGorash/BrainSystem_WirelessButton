#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#include <EncButton.h>
#include <RGBLED.h>
#include <Preferences.h>

#include "src/Framework/Core/Hal.h"
#include "src/Framework/Timer.h"

#include "WirelessLink.h"

class HalImpl : public vgs::IHal
{
public:
  HalImpl();
  ~HalImpl();

  void init() override;
  void tick() override;

  //buttons
  vgs::ButtonState getButtonState() override;

  //leds
  void correctPressSignal(int player) override;
  void falstartPressSignal(int player) override;
  void pendingPressSignal(int player) override;
  void gameStartSignal() override;
  void clearSignals() override;

  //sound
  void sound(vgs::HalSound soundType) override {};  // no impl
  void sound(unsigned int frequency, unsigned int duration) override {};  // no impl

  //display
  void updateDisplay(const vgs::GameDisplayInfo& info) override {}; // no impl
  void updateDisplay(const vgs::CustomDisplayInfo& info) override {};  // no impl

  //time
  unsigned long getTimeMillis() override;

  // link
  WirelessLink& getLink();

  //preferences
  Preferences& getPreferences();

  //battery
  float getBatteryVoltage();

private:
  void blinkLed(int player);

  void loadBatteryCalibrationData();

private:
  WirelessLink m_link;
  RGBLED m_rgb;
  Button m_button;

  vgs::Timer m_blinkTimer;
  bool m_blinkState = 0;

  Preferences m_preferences;

  float m_batteryCalibrationSlope = 2.5f / 4095.0f;
  float m_batteryCalibrationOffset = 0.0f;
};

#endif