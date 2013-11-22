#include "mc_instance.hpp"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array.h"
#include <vector>

pp::Module* pp::CreateModule() {
  return new InstanceFactory<MonteCarloInstance>();
}

void MonteCarloInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto N = var_message.AsInt();
  
  // Run the simulation in 10 parts
  const len_t nParts = 10;
  std::vector<len_t> total;
  std::vector<len_t> samples;
  std::vector<double> mean;
  std::vector<double> stdError;
  auto step = N/nParts;
  step = step > 0 ? step : 1;
  len_t runningTotal = 0;
  for( len_t i=step; i<=N; i += step) {
    auto res = mc.sim(step);
    runningTotal += res.Total;
    total.push_back(runningTotal);
    samples.push_back(i);
    auto runningMean = 1.0*runningTotal/i;
    mean.push_back(runningMean);
    stdError.push_back(sqrt( runningMean*(1.0-runningMean) / i));
  }

  pp::VarDictionary reply;
  auto finalMean =  mean[ mean.size() -1];
  auto finalStdError =  stdError[ stdError.size() - 1];
  pp::VarArray MeanConverge;
  MeanConverge.SetLength( mean.size());
  for ( unsigned int i=0; i<mean.size(); ++i ) {
    MeanConverge.Set( i, mean[i]);
  }
  pp::VarArray StdErrConverge;
  StdErrConverge.SetLength( stdError.size());
  for ( unsigned int i=0; i<stdError.size(); ++i ) {
    StdErrConverge.Set( i, stdError[i]);
  }
  pp::VarArray TotalConverge;
  TotalConverge.SetLength( total.size());
  for ( unsigned int i=0; i<stdError.size(); ++i ) {
    TotalConverge.Set( i, static_cast<double>(total[i]));
  }
  pp::VarArray SamplesConverge;
  SamplesConverge.SetLength( samples.size());
  for ( unsigned int i=0; i<stdError.size(); ++i ) {
    SamplesConverge.Set( i, static_cast<double>(samples[i]));
  }

  reply.Set( "Mean", finalMean );
  reply.Set( "StdError", finalStdError );
  reply.Set( "MeanConverge", MeanConverge );
  reply.Set( "StdErrConverge", StdErrConverge );
  reply.Set( "TotalConverge", TotalConverge );
  reply.Set( "SamplesConverge", SamplesConverge );
  PostMessage( reply );
}
