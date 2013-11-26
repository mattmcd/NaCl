#include "mc_instance.hpp"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array.h"
#include <vector>
#include <future>

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void monteCarloSim( MonteCarloInstance& instance, unsigned int N, unsigned int nParts ) {
  pp::VarArray reply;

  auto step = N/nParts;
  step = step > 0 ? step : 1;
  len_t runningTotal = 0;
  unsigned int count=0;
  for( len_t i=step; i<=N; i += step) {
    auto res = instance.sim(step);
    pp::VarDictionary outData;
    runningTotal += res.Total;
    outData.Set( "Samples", static_cast<double>(i) );
    outData.Set( "Total", static_cast<double>(runningTotal) );
    auto runningMean = 1.0*runningTotal/i;
    outData.Set( "Mean", runningMean );
    outData.Set( "StdError", sqrt( runningMean*(1.0-runningMean)/i));
    reply.Set( count, outData );
    instance.PostMessage( outData ); // For progress measurement
    count++;
  }
  instance.PostMessage( reply );
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto N = var_message.AsInt();
  
  // Run the simulation in 10 parts
  const len_t nParts = 10;

  monteCarloSim( *this, N, nParts );
}
