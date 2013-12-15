#include "model_factory.hpp"

namespace {
  auto circleReg = ModelRegister("Circle", 
    [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; });
}

