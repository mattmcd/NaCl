#ifndef MC_INSTANCE
#define MC_INSTANCE

#include "instance_factory.hpp"
#include "monte_carlo.hpp"

// The MonteCarloInstance that stores 
class MonteCarloInstance : public pp::Instance {
  public:
    explicit MonteCarloInstance( PP_Instance instance ) 
      : pp::Instance(instance) { };
    virtual ~MonteCarloInstance( ){};
    virtual void HandleMessage( const pp::Var& );
  private:
    MonteCarlo mc;
};

#endif
