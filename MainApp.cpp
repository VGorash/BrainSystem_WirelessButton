#include "MainApp.h"
#include "PairingApp.h"
#include "HalImpl.h"

using namespace vgs;

MainApp::MainApp()
{

}

void MainApp::init(IHal& hal)
{
  m_idleState = true;
  hal.clearSignals();

  m_batteryVoltageSum = 0.0;
  m_numBatteryMeasurments = 0;

  m_batteryCheckTimer.setTime(5000);
  m_batteryCheckTimer.setPeriodMode(true);
  m_batteryCheckTimer.start(hal);

  m_batterySignalTimer.setTime(250);
  m_batterySignalTimer.setPeriodMode(false);
}

void MainApp::tick(IHal& hal)
{
  if(m_needPairing)
  {
    return;
  }

  HalImpl* halImpl = (HalImpl*) &hal;
  WirelessLink& link = halImpl->getLink();

  ButtonState buttonState = hal.getButtonState();

  if(buttonState.menu)
  {
    m_needPairing = true;
    return;
  }

  if(buttonState.player >= 0)
  {
    link.send(link::Command::ButtonPressed);
  }

  if(link.getCommand() != link::Command::None)
  {
    int player = link.getData();

    switch(link.getCommand())
    {
      case link::Command::CorrectPressSignal:
        m_idleState = false;
        hal.correctPressSignal(player);
        break;
      case link::Command::FalstartPressSignal:
        m_idleState = false;
        hal.falstartPressSignal(player);
        break;
      case link::Command::PendingPressSignal:
        m_idleState = false;
        hal.pendingPressSignal(player);
        break;
      case link::Command::Clear:
        m_idleState = true;
        hal.clearSignals();
        break;
    }
  }

  // measure battery
  m_batteryVoltageSum += halImpl->getBatteryVoltage();
  m_numBatteryMeasurments += 1;

  if(m_batteryCheckTimer.tick(hal))
  {
    if(!checkBattery(hal) && m_idleState)
    {
      m_batterySignalTimer.start(hal);
      hal.correctPressSignal(3); // enable red light in idle to show low battery
    }
  }

  if(m_batterySignalTimer.tick(hal) && m_idleState)
  {
    hal.clearSignals();  // turn off battery low signal after timer
  }
}

int mapBatteryPercentage(float voltage)
{
  constexpr float voltageEmpty = 3.3;  // 0%
  constexpr float voltageFull = 4.2;   // 100%

  int percentage = (int)((voltage - voltageEmpty) / (voltageFull - voltageEmpty) * 100);
  return constrain(percentage, 0, 100);
}

bool MainApp::checkBattery(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;
  WirelessLink& link = halImpl->getLink();

  float batteryVoltage = m_batteryVoltageSum / m_numBatteryMeasurments;
  m_batteryVoltageSum = 0.0f;
  m_numBatteryMeasurments = 0;

  int batteryPercentage = mapBatteryPercentage(batteryVoltage);
  link.sendBatteryInfo(batteryPercentage);

  return batteryPercentage > 10;
}

AppChangeType MainApp::appChangeNeeded()
{
  if(m_needPairing)
  {
    return AppChangeType::Custom;
  }

  return AppChangeType::None;
}

IApp* MainApp::createCustomApp()
{
  return new PairingApp(true);
}
