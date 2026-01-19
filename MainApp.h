#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "src/Framework/Core/App.h"
#include "src/Framework/Timer.h"

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
  bool checkBattery(IHal& hal);

private:
  bool m_idleState = true;
  bool m_needPairing = false;

  Timer m_batteryCheckTimer;
  Timer m_batterySignalTimer;
  float m_batteryVoltageSum;
  int m_numBatteryMeasurments;
};

} // namespace vgs

#endif