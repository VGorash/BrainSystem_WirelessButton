#include "src/Framework/Core/Engine.h"

#include "HalImpl.h"
#include "PairingApp.h"

using namespace vgs;
 
Engine engine(new HalImpl(), new PairingApp(), nullptr);

void setup()
{
  engine.init();
}

void loop() 
{
  engine.tick();
}