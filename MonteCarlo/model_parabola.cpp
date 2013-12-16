#include "singleton_factory.hpp"

namespace {
  auto parabolaReg = ObjectRegister<std::function<int(double,double)> >("Parabola", 
    [](double x, double y){ return y < x*x  ? 1 : 0; });
}

