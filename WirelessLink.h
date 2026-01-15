#ifndef WIRELESS_LINK_H
#define WIRELESS_LINK_H

#define USE_ESP_NOW_LINK
#include "src/Link/EspNowInterface.h"
#include "src/Link/Link.h"

class WirelessLink : public vgs::link::Link, vgs::link::EspNowHandler
{
public:
  WirelessLink();

  void init();

  // Link methods
  void tick() override;
  vgs::link::Command getCommand() override;
  unsigned int getData() override;
  void send(vgs::link::Command command, unsigned int data = 0) override;

  // Custom
  bool isInitialized();
  void sendPairingRequest();

private:
  // EspNowHandler methods
  void onCommandV2(const uint8_t* address, uint8_t data) override;
  void onPingResponse(const uint8_t* address, uint8_t data) override;

private:
  vgs::link::EspNowInterface* m_interface = nullptr;
  vgs::link::Command m_command = vgs::link::Command::None;
  unsigned int m_data = 0;
  bool m_initialized;
  bool m_dirty;
  uint8_t m_serverAddress[6];
};

#endif