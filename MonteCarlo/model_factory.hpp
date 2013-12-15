#ifndef MODEL_FACTORY_HPP
#define MODEL_FACTORY_HPP

#include <string>
#include <map>
#include <functional>
#include <vector>

class ModelFactory {
  public:
    std::function<int(double,double)> getModel( std::string name );
    std::vector<std::string> getModelNames();
    static ModelFactory& getInstance();
    void registerModel( std::string name, std::function<int(double, double)> fcn) ;
    ~ModelFactory(){};
  private:
    std::map<std::string, std::function<int(double,double)>> ModelCreatorFcns;
    ModelFactory(){};
};

class ModelRegister {
  // Helper class to allow registration of scenes by construction of global
  // variables in a hidden namespace.  
  // See p164 of Joshi "C++ design patterns for derivatives pricing"
  // Note that the C++11 language features make this pattern a lot simpler.

  public:
    ModelRegister( std::string name, std::function<int(double,double)> fcn ) {
      ModelFactory& theFactory = ModelFactory::getInstance();
      theFactory.registerModel( name, fcn );
    }
};

#endif
