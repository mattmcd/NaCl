#include "mc_instance.hpp"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array.h"
#include <vector>
#include <thread>
#include <functional>

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::Simulate( unsigned int N ) {
  pp::VarArray reply;

  // Run the simulation in 10 parts
  const len_t nParts = 10;
  auto step = N/nParts;
  step = step > 0 ? step : 1;
  len_t runningTotal = 0;
  unsigned int count=0;
  for( len_t i=step; i<=N; i += step) {
    auto res = mc.sim(step);
    pp::VarDictionary outData;
    runningTotal += res.Total;
    outData.Set( "Samples", static_cast<double>(i) );
    outData.Set( "Total", static_cast<double>(runningTotal) );
    auto runningMean = 1.0*runningTotal/i;
    outData.Set( "Mean", runningMean );
    outData.Set( "StdError", sqrt( runningMean*(1.0-runningMean)/i));
    reply.Set( count, outData );
    PostMessage( outData ); // For progress measurement
    count++;
  }
  PostMessage( reply );
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto N = var_message.AsInt();
  
  //std::thread t(&MonteCarloInstance::Simulate, this, N);
  //t.detach();
  Simulate(N);
}
