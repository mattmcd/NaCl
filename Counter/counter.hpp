#ifndef COUNTER_HPP
#define COUNTER_HPP

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

// Factory method called by the browser when first loaded
namespace pp {
  Module* CreateModule();
};

// Module used to create instance of NaCl module on webpage
class CounterModule : public pp::Module {
  public:
    CounterModule( ){};
    virtual ~CounterModule(){};
    virtual pp::Instance* CreateInstance( PP_Instance );
};

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
