#include "monte_carlo.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
  MonteCarlo mc;

  // Function to estimate
  auto f = [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; };

  const long int nPts = 1e4;
  auto res = mc.sim( f, nPts );

  std::cout << res.Mean << " +/- " << res.StDev/res.Mean/nPts << std::endl;

  return 0;
}
