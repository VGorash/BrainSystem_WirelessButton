#include "PairingApp.h"
#include "MainApp.h"

using namespace vgs;

PairingApp::PairingApp(bool resetPairing) : m_resetPairing(resetPairing)
{

}

void PairingApp::init(IHal& hal)
{
  if(m_resetPairing)
  {
    m_resetPairing = false;
    clearServerAddress(hal);
  }

  m_needExit = false;
  hal.clearSignals();

  if(loadServerAddress(hal))
  {
    m_needExit = true;
    return;
  }

  m_timer.setTime(500);
  m_timer.setPeriodMode(true);
  m_timer.start(hal);
}

void PairingApp::tick(IHal& hal)
{
  if(m_needExit)
  {
    return;
  }

  HalImpl* halImpl = (HalImpl*) &hal;
  WirelessLink& link = halImpl->getLink();

  if(link.isPaired())
  {
    m_needExit = true;
    saveServerAddress(hal);
    return;
  }

  if(m_timer.tick(hal))
  {
    m_blinkState = !m_blinkState;
    link.tryPairing();

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

constexpr const char* namespaceKey = "wireless_link";
constexpr const char* serverAddressKey = "serverAddress";

bool PairingApp::loadServerAddress(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  Preferences& preferences = halImpl->getPreferences();
  preferences.begin(namespaceKey, true);

  if(!preferences.isKey(serverAddressKey) || preferences.getBytesLength(serverAddressKey) != 6)
  {
    preferences.end();
    return false;
  }

  uint8_t serverAddress[6];
  preferences.getBytes(serverAddressKey, serverAddress, 6);
  halImpl->getLink().setServerAddress(serverAddress);

  preferences.end();
  return true;
}

void PairingApp::saveServerAddress(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  Preferences& preferences = halImpl->getPreferences();
  preferences.begin(namespaceKey, false);
  preferences.putBytes(serverAddressKey, halImpl->getLink().getServerAddress(), 6);
  preferences.end();
}

void PairingApp::clearServerAddress(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  Preferences& preferences = halImpl->getPreferences();
  preferences.begin(namespaceKey, false);
  preferences.remove(serverAddressKey);
  halImpl->getLink().resetPairing();
  preferences.end();
}

AppChangeType PairingApp::appChangeNeeded()
{
  if(m_needExit)
  {
    return AppChangeType::Custom;
  }

  return AppChangeType::None;
}

IApp* PairingApp::createCustomApp()
{
  return new MainApp();
}