#include "mc_instance.hpp"

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto N = var_message.AsInt();
  // Run the simulation 
  auto res = mc.sim(N);
  const pp::Var reply( res.Mean );
  PostMessage( reply );
}
