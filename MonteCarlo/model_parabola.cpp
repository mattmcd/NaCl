#include "model_factory.hpp"

namespace {
  auto parabolaReg = ModelRegister("Parabola", 
    [](double x, double y){ return y < x*x  ? 1 : 0; });
}

