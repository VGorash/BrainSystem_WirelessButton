#include "HalImpl.h"
#include "src/Framework/colors.h"

using namespace vgs;

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

Color convertColor(Color color)
{
  Color result{};
  result.r = (unsigned char)((float)color.r * 1.0);
  result.g = (unsigned char)((float)color.g * 0.6);
  result.b = (unsigned char)((float)color.b * 0.5);
  return result;
}

void HalImpl::blinkLed(int player)
{
  Color color = convertColor(colorFromPlayerNumber(player));
  m_rgb.setRGB(color.r, color.g, color.b);
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

  Color color = convertColor(colorFromPlayerNumber(player));
  m_rgb.setRGB(color.r, color.g, color.b);
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
