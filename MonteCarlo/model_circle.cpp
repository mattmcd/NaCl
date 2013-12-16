#include "singleton_factory.hpp"

namespace {
  auto circleReg = ObjectRegister<std::function<int(double,double)> >("Circle", 
    [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; });
}

