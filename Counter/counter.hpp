#ifndef COUNTER_HPP
#define COUNTER_HPP

#include "instance_factory.hpp"

// The CounterInstance that stores count of values passed from webpage
class CounterInstance : public pp::Instance {
  public:
    explicit CounterInstance( PP_Instance instance ) 
      : pp::Instance(instance), count(0) {};
    virtual ~CounterInstance( ){};
    virtual void HandleMessage( const pp::Var& );
  private:
    void inc(int x=0);
    int count;
};

#endif
