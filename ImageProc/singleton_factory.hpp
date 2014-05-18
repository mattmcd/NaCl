#ifndef SINGLETON_FACTORY_HPP
#define SINGLETON_FACTORY_HPP

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <vector>
#include "processor.hpp"

template <typename T>
class SingletonFactory {
  public:
    T getObject( std::string name ) const;
    std::vector<std::string> getNames() const;
    static SingletonFactory& getInstance();
    void registerObject( std::string name, T fcn) ;
    ~SingletonFactory(){};
  private:
    std::map<std::string, T> SingletonCreatorFcns;
    SingletonFactory(){};
};

template <typename T>
class ObjectRegister {
  // Helper class to allow registration of scenes by construction of global
  // variables in a hidden namespace.  
  // See p164 of Joshi "C++ design patterns for derivatives pricing"
  // Note that the C++11 language features make this pattern a lot simpler.

  public:
    ObjectRegister( std::string name, T fcn ) {
      auto& theFactory = SingletonFactory<T>::getInstance();
      theFactory.registerObject( name, fcn );
    }
};

// Singleton factory for registering derived Processor classes 
template <typename T>
class ProcessorRegister {
  public:
    ProcessorRegister( std::string name ) {
      auto& theFactory = 
        SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
      theFactory.registerObject( name, []() { return
        std::unique_ptr<T>(new T());} );
    }
};


template <typename T>
void SingletonFactory<T>::registerObject( std::string name, T fcn) {
  SingletonCreatorFcns.insert( 
    std::pair<std::string, T>(name, fcn));
}

template <typename T>
T SingletonFactory<T>::getObject( std::string name) const {
  auto it = SingletonCreatorFcns.find(name);
  if ( it == SingletonCreatorFcns.end() ) {
    return NULL;
  }
  return (it->second);
}

template <typename T>
std::vector<std::string> SingletonFactory<T>::getNames( ) const {
  std::vector<std::string> names;
  for (auto entry : SingletonCreatorFcns ) {
    names.push_back( entry.first );
  }
  return names;
}

template <typename T>
SingletonFactory<T>& SingletonFactory<T>::getInstance(){
  static SingletonFactory<T> theFactory;
  return theFactory;
}

#endif
