#ifndef PAIRING_APP_H
#define PAIRING_APP_H

#include "src/Framework/Core/App.h"
#include "HalImpl.h"

namespace vgs
{

class PairingApp : public IApp
{
public:
  PairingApp(bool resetPairing = false);

  void init(IHal& hal) override;
  void tick(IHal& hal) override;

  AppChangeType appChangeNeeded() override;
  IApp* createCustomApp() override;

private:
  bool loadServerAddress(IHal& hal);
  void saveServerAddress(IHal& hal);
  void clearServerAddress(IHal& hal);

private:
  bool m_resetPairing;
  bool m_needExit = false;
  bool m_blinkState = false;
  Timer m_timer;
};

} // namespace vgs

#endif