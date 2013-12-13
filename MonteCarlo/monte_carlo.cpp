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
    if ( run_simulation_ ) {
      auto res = mc.sim(step);
      runningTotal += res.Total;
      auto outData = PostResponse( runningTotal, i);
      reply.Set( count, outData );
      count++;
    }
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
  // Simple interface:  
  // - receive a number = start simulation with that number of runs
  // - receive anything else = stop the simulation
  if ( var_message.is_number() ) {
    // Message is number of simulations to run
    auto N = var_message.AsInt();
    // Enable simulation
    run_simulation_ = true;
    // Start simulation on background thread
    sim_thread_.message_loop().PostWork( 
        callback_factory_.NewCallback( &MonteCarloInstance::Simulate, N));
  } else {
    // Disable simulation - background thread will see this at start of
    // next iteration and terminate early
    run_simulation_ = false;
  }
}
