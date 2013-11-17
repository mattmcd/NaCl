#ifndef INSTANCE_FACTORY_HPP
#define INSTANCE_FACTORY_HPP

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

// Factory method called by the browser when first loaded
namespace pp {
  Module* CreateModule();
};

// Module used to create instance of NaCl module on webpage
template <class T>
class InstanceFactory : public pp::Module {
  public:
    InstanceFactory( ){};
    virtual ~InstanceFactory(){};
    virtual pp::Instance* CreateInstance( PP_Instance instance ) { 
      return new T(instance);
    };
};

#endif
