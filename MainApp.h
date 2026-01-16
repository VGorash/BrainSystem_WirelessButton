#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "src/Framework/Core/App.h"

namespace vgs
{

class MainApp : public IApp
{
public:
  MainApp();

  void init(IHal& hal) override;
  void tick(IHal& hal) override;

  AppChangeType appChangeNeeded() override;
  IApp* createCustomApp() override;

private:
  bool m_needPairing = false;
};

} // namespace vgs

#endif