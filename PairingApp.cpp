#include "PairingApp.h"
#include "MainApp.h"

using namespace vgs;

PairingApp::PairingApp()
{

}

void PairingApp::init(IHal& hal)
{
  hal.clearSignals();
  m_timer.setTime(500);
  m_timer.setPeriodMode(true);
  m_timer.start(hal);
}

void PairingApp::tick(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;
  WirelessLink& link = halImpl->getLink();

  m_paired = link.isInitialized();

  if(m_paired)
  {
    return;
  }

  if(m_timer.tick(hal))
  {
    m_blinkState = !m_blinkState;
    link.sendPairingRequest();

    if(m_blinkState)
    {
      hal.gameStartSignal();
    }
    else
    {
      hal.clearSignals();
    }
  }
}

AppChangeType PairingApp::appChangeNeeded()
{
  if(m_paired)
  {
    return AppChangeType::Custom;
  }

  return AppChangeType::None;
}

IApp* PairingApp::createCustomApp()
{
  return new MainApp();
}