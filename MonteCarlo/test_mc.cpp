#include "monte_carlo.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
  MonteCarlo mc;

  // Function to estimate
  auto f = [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; };

  len_t nPts = 10;
  for (int i=0; i<6; ++i ) {
    auto res = mc.sim( f, nPts );
    std::cout << nPts << ": Pi = " 
      << 4.0*res.Mean << " +/- " 
      << 4.0*res.StDev/sqrt(nPts) << std::endl;
    nPts *= 10;
  }

  return 0;
}
