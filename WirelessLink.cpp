#include "WirelessLink.h"

using namespace vgs::link;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

WirelessLink::WirelessLink()
{

}

void WirelessLink::init()
{
  m_interface = EspNowInterface::getInstance();
  m_interface->setHandler(this);
  WiFi.setSleep(false);
  WiFi.setTxPower(WIFI_POWER_17dBm);
}

void WirelessLink::tick()
{
  if(!m_dirty)
  {
    m_command = Command::None;
    m_data = 0;
  }
}

Command WirelessLink::getCommand()
{
  m_dirty = false;
  return m_command;
}

unsigned int WirelessLink::getData()
{
  return m_data;
}

void WirelessLink::send(Command command, unsigned int data)
{
  if(!m_paired)
  {
    return;
  }

  unsigned char code;

  if(commandToCode(command, data, code))
  {
    m_interface->send(m_serverAddress, LINK_WIRELESS_HEADER_COMMAND_V2, code);
  }
}

bool WirelessLink::isPaired()
{
  return m_paired;
}

void WirelessLink::resetPairing()
{
  m_paired = false;
}

void WirelessLink::tryPairing()
{
  m_interface->send(broadcastAddress, LINK_WIRELESS_HEADER_PAIRING_REQUEST, LINK_WIRELESS_DEVICE_BUTTON);
}

void WirelessLink::setServerAddress(const uint8_t* address)
{
  memcpy(m_serverAddress, address, 6);
  m_paired = true;
}

const uint8_t* WirelessLink::getServerAddress()
{
  return m_serverAddress;
}

void WirelessLink::onPairingResponse(const uint8_t* address, uint8_t data)
{
  memcpy(m_serverAddress, address, 6);
  m_paired = true;
}

bool checkAddressEqual(const uint8_t* a, const uint8_t* b)
{
  bool equal = true;
  for(int i=0; i<6; i++)
  {
    if(a[i] != b[i])
    {
      equal = false;
      break;
    }
  }

  return equal;
}

void WirelessLink::onCommandV2(const uint8_t* address, uint8_t data)
{
  if(!m_paired || !checkAddressEqual(address, m_serverAddress))
  {
    return;
  }

  m_command = Command::None;
  m_data = 0;

  m_dirty = codeToCommand(data, m_command, m_data);
}
