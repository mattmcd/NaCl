#include "mc_instance.hpp"
#include "singleton_factory.hpp"
#include <vector>
#include <thread>
#include <functional>

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::Simulate( int32_t /*result*/,  
        std::function<int(double,double)> model, unsigned int N) {
  // Run the simulation in 10 parts or max 1e6 points at a time
  const len_t nParts = 10;
  auto step = N/nParts;
  step = step > 0 ? step : 1;
  step = step < 1e6 ? step : 1e6;
  len_t runningTotal = 0;
  unsigned int count=0;
  for( len_t i=step; i<=N; i += step) {
    if ( run_simulation_ ) {
      auto res = mc.sim(model, step);
      runningTotal += res.Total;
      auto outData = PostResponse( runningTotal, i);
      count++;
    }
  }
  pp::VarDictionary msg;
  msg.Set( "Type", "completed" );
  PostMessage( msg );
}

pp::VarDictionary MonteCarloInstance::PostResponse( len_t runningTotal, len_t i){
  pp::VarDictionary outData;
  outData.Set( "Type", "partial" );
  outData.Set( "Samples", static_cast<double>(i) );
  outData.Set( "Total", static_cast<double>(runningTotal) );
  auto runningMean = 1.0*runningTotal/i;
  outData.Set( "Mean", runningMean );
  outData.Set( "StdError", sqrt( runningMean*(1.0-runningMean)/i));
  PostMessage( outData ); // For progress measurement
  return outData;
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  // Interface: receive a { cmd: ..., args... } dictionary  
  // - sim, nPts = start simulation with that number of runs
  // - receive anything else = stop the simulation
  pp::VarDictionary var_dict( var_message );
  auto cmd = var_dict.Get( "cmd" ).AsString();
  if ( cmd == "sim" ) {
    // Message is number of simulations to run
    auto N = var_dict.Get("nPts").AsInt();
    auto modelFactory = SingletonFactory<std::function<int(double,double)>>::getInstance();
    auto model = modelFactory.getObject( var_dict.Get("model").AsString() );
    // Enable simulation
    run_simulation_ = true;
    // Start simulation on background thread
    sim_thread_.message_loop().PostWork( 
        callback_factory_.NewCallback( &MonteCarloInstance::Simulate, model, N));
  } else {
    // Disable simulation - background thread will see this at start of
    // next iteration and terminate early
    run_simulation_ = false;
  }
}
