#include "HalImpl.h"

using namespace vgs;

#define NUM_COLORS 4
constexpr RGB::Color colors[NUM_COLORS] = {RGB::Color::Orange, RGB::Color::Blue, RGB::Color::Green, RGB::Color::Red};

HalImpl::HalImpl() : m_rgb(LED_R_PIN, LED_G_PIN, LED_B_PIN)
{
  
}

HalImpl::~HalImpl()
{

}

void HalImpl::init()
{
  Serial.begin(9600);

  m_link.init();
  m_button.init(BUTTON_PIN, INPUT);
  m_rgb.setColor(RGB::Color::Black);
  m_rgb.setBrightness(128);
  m_rgb.disable();

  m_blinkTimer.setTime(500);
  m_blinkTimer.setPeriodMode(true);
  
  m_button.setTimeout(4000);
  m_button.setHoldTimeout(3000);
}

void HalImpl::tick()
{
  if(m_blinkTimer.tick(*this))
  {
    m_blinkState = !m_blinkState;
    m_rgb.setPower(m_blinkState);
  }

  m_button.tick();
  m_link.tick();
}

ButtonState HalImpl::getButtonState()
{
  ButtonState s = ButtonState();

  if(m_button.press())
  {
    s.player = 0;
  }

  if(m_button.hold())
  {
    s.menu = true;
  }

  return s;
}

void HalImpl::blinkLed(int player)
{
  m_rgb.setColor(colors[player % NUM_COLORS]);
  m_rgb.enable();

  if(!m_blinkTimer.isStarted())
  {
    m_blinkState = 1;
    m_blinkTimer.start(*this);
  }
}

void HalImpl::correctPressSignal(int player)
{
  if (player < 0)
  {
    return;
  }

  m_rgb.setColor(colors[player % NUM_COLORS]);
  m_rgb.enable();
}

void HalImpl::falstartPressSignal(int player)
{
  if (player < 0)
  {
    return;
  }

  blinkLed(player);
}

void HalImpl::pendingPressSignal(int player)
{
  if (player < 0)
  {
    return;
  }

  blinkLed(player);
}

void HalImpl::gameStartSignal()
{
  m_rgb.setColor(RGB::Color::White);
  m_rgb.enable();
}

void HalImpl::clearSignals()
{
  m_rgb.setColor(RGB::Color::Black);
  m_rgb.disable();
  m_blinkTimer.stop();
}

unsigned long HalImpl::getTimeMillis()
{
  return millis();
}

WirelessLink& HalImpl::getLink()
{
  return m_link;
}

Preferences& HalImpl::getPreferences()
{
  return m_preferences;
}
