#include "counter.hpp"

pp::Module* pp::CreateModule() {
  return new InstanceFactory<CounterInstance>();
}

void CounterInstance::HandleMessage( const pp::Var& var_message ) {
  if ( !var_message.is_number() )
    return; //Early exit
  auto x = var_message.AsInt();
  count += x;
  const pp::Var reply( count );
  PostMessage( reply );
}
