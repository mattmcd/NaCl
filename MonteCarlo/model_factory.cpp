#include "model_factory.hpp"

void ModelFactory::registerModel( std::string name, std::function<int(double,double)> fcn) {
  ModelCreatorFcns.insert( 
    std::pair<std::string, std::function<int(double,double)>>(name, fcn));
}

std::function<int(double,double)> ModelFactory::getModel( std::string name) {
  auto it = ModelCreatorFcns.find(name);
  if ( it == ModelCreatorFcns.end() ) {
    return NULL;
  }
  return (it->second);
}

std::vector<std::string> ModelFactory::getModelNames( ) {
  std::vector<std::string> names;
  for (auto entry : ModelCreatorFcns ) {
    names.push_back( entry.first );
  }
  return names;
}

ModelFactory& ModelFactory::getInstance(){
  static ModelFactory theFactory;
  return theFactory;
}
