#include "mc_instance.hpp"
#include <vector>
#include <thread>
#include <functional>

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::Simulate( int32_t /*result*/,  unsigned int N) {
  pp::VarArray reply;

  // Run the simulation in 10 parts
  const len_t nParts = 10;
  auto step = N/nParts;
  step = step > 0 ? step : 1;
  len_t runningTotal = 0;
  unsigned int count=0;
  for( len_t i=step; i<=N; i += step) {
    auto res = mc.sim(step);
    runningTotal += res.Total;
    auto outData = PostResponse( runningTotal, i);
    reply.Set( count, outData );
    count++;
  }
  PostMessage( reply );
}

pp::VarDictionary MonteCarloInstance::PostResponse( len_t runningTotal, len_t i){
  pp::VarDictionary outData;
  outData.Set( "Samples", static_cast<double>(i) );
  outData.Set( "Total", static_cast<double>(runningTotal) );
  auto runningMean = 1.0*runningTotal/i;
  outData.Set( "Mean", runningMean );
  outData.Set( "StdError", sqrt( runningMean*(1.0-runningMean)/i));
  PostMessage( outData ); // For progress measurement
  return outData;
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto N = var_message.AsInt();
  
  //std::thread t(&MonteCarloInstance::Simulate, this, N);
  //t.detach();
  // Simulate(N);
  sim_thread_.message_loop().PostWork( 
    callback_factory_.NewCallback( &MonteCarloInstance::Simulate, N));
}
