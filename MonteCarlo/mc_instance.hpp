#ifndef MC_INSTANCE
#define MC_INSTANCE

#include "instance_factory.hpp"
#include "monte_carlo.hpp"

// The MonteCarloInstance that stores 
class MonteCarloInstance : public pp::Instance {
  public:
    explicit MonteCarloInstance( PP_Instance instance ) 
      : pp::Instance(instance), N(100) { };
    virtual ~MonteCarloInstance( ){};
    virtual void HandleMessage( const pp::Var& );
  private:
    len_t N;
};

#endif
