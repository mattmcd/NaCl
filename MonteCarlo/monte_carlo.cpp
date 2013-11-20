#include "mc_instance.hpp"
#include "ppapi/cpp/var_dictionary.h"

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto N = var_message.AsInt();
  // Run the simulation 
  auto res = mc.sim(N);
  pp::VarDictionary reply;
  reply.Set( "Mean", res.Mean );
  reply.Set( "StdError", res.StDev/sqrt(N));
  PostMessage( reply );
}
