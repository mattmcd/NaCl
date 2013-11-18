#include "mc_instance.hpp"

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto n = var_message.AsInt();
  N = n;
  // Create the simulation engine
  MonteCarlo mc(N);
  auto res = mc.sim();
  const pp::Var reply( res.Mean );
  PostMessage( reply );
}
