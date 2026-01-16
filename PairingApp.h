#ifndef PAIRING_APP_H
#define PAIRING_APP_H

#include "src/Framework/Core/App.h"
#include "HalImpl.h"

namespace vgs
{

class PairingApp : public IApp
{
public:
  PairingApp();

  void init(IHal& hal) override;
  void tick(IHal& hal) override;

  AppChangeType appChangeNeeded() override;
  IApp* createCustomApp() override;

private:
  bool m_paired = false;
  bool m_blinkState = false;
  Timer m_timer;
};

} // namespace vgs

#endif