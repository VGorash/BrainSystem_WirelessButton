#include "MainApp.h"
#include "HalImpl.h"

using namespace vgs;

MainApp::MainApp()
{

}

void MainApp::init(IHal& hal)
{
  hal.clearSignals();
}

void MainApp::tick(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;
  WirelessLink& link = halImpl->getLink();

  if(hal.getButtonState().player >= 0)
  {
    link.send(link::Command::ButtonPressed);
  }

  if(link.getCommand() != link::Command::None)
  {
    int player = link.getData();

    switch(link.getCommand())
    {
      case link::Command::CorrectPressSignal:
        hal.correctPressSignal(player);
        break;
      case link::Command::FalstartPressSignal:
        hal.falstartPressSignal(player);
        break;
      case link::Command::PendingPressSignal:
        hal.pendingPressSignal(player);
        break;
      case link::Command::Clear:
        hal.clearSignals();
        break;
    }
  }
}

AppChangeType MainApp::appChangeNeeded()
{
  return AppChangeType::None;
}

IApp* MainApp::createCustomApp()
{
  return nullptr;
}
