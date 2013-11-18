#include "monte_carlo.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
  const long int nPts = 1e4;
  MonteCarlo mc(nPts);

  // Function to estimate
  auto f = [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; };

  auto res = mc.sim( f );

  std::cout << res.Mean << std::endl;

  return 0;
}