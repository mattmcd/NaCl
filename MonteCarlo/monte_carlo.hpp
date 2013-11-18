#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <functional>
#include <random>

struct result {
  double Mean;
  double StDev;
};

typedef long unsigned int len_t;

class MonteCarlo {
  public:
    explicit MonteCarlo(len_t nPts_) : N(nPts_) {
    };
    virtual ~MonteCarlo() {};
    result sim( std::function<int(double,double)> const& f) {
      // Bind the generator to the first argument of distribution so that
      // we can call with rng() instead of dist(gen).
      std::uniform_real_distribution<double> unifdist(0.0, 1.0);
      auto rng = std::bind( unifdist, generator );
      
      len_t sum = 0;
      for( len_t i=0; i<N; ++i) {
        double x = rng();
        double y = rng();
        sum += f(x,y);
      }
      result res;
      res.Mean = (1.0*sum)/N;
      return res;
    };
    result sim() {
      auto f = [](double x, double y){ return x*x + y*y < 1 ? 1 : 0; };
      return sim(f);
    }
  private:
    len_t N;
    std::mt19937 generator;

};

#endif
