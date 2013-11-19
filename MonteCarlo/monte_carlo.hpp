#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <functional>
#include <random>
#include <cmath>

struct result {
  double Mean;
  double StDev;
};

typedef long unsigned int len_t;

class MonteCarlo {
  public:
    explicit MonteCarlo() : unifdist(0.0,1.0) {
    };
    virtual ~MonteCarlo() {};
    result sim( std::function<int(double,double)> const& f, len_t N) {
      // Bind the generator to the first argument of distribution so that
      // we can call with rng() instead of dist(gen).
      auto rng = std::bind( unifdist, generator );
      
      len_t sum = 0;
      for( len_t i=0; i<N; ++i) {
        double x = rng();
        double y = rng();
        sum += f(x,y);
      }
      result res;
      res.Mean = (1.0*sum)/N;
      res.StDev = sqrt( res.Mean*(1.0 - res.Mean) );
      return res;
    };
    result sim(len_t N) {
      auto f = [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; };
      return sim(f,N);
    }
  private:
    std::mt19937 generator;
    std::uniform_real_distribution<double> unifdist;
    
};

#endif