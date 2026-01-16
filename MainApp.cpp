#include "MainApp.h"
#include "PairingApp.h"
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
