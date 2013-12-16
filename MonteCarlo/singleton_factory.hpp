#ifndef SINGLETON_FACTORY_HPP
#define SINGLETON_FACTORY_HPP

#include <string>
#include <map>
#include <functional>
#include <vector>

template <typename T>
class SingletonFactory {
  public:
    T getObject( std::string name );
    std::vector<std::string> getNames();
    static SingletonFactory& getInstance();
    void registerSingleton( std::string name, T fcn) ;
    ~SingletonFactory(){};
  private:
    std::map<std::string, T> SingletonCreatorFcns;
    SingletonFactory(){};
};

template <typename T>
class SingletonRegister {
  // Helper class to allow registration of scenes by construction of global
  // variables in a hidden namespace.  
  // See p164 of Joshi "C++ design patterns for derivatives pricing"
  // Note that the C++11 language features make this pattern a lot simpler.

  public:
    SingletonRegister( std::string name, T fcn ) {
      SingletonFactory<T>& theFactory = SingletonFactory<T>::getInstance();
      theFactory.registerSingleton( name, fcn );
    }
};

template <typename T>
void SingletonFactory<T>::registerSingleton( std::string name, T fcn) {
  SingletonCreatorFcns.insert( 
    std::pair<std::string, T>(name, fcn));
}

template <typename T>
T SingletonFactory<T>::getObject( std::string name) {
  auto it = SingletonCreatorFcns.find(name);
  if ( it == SingletonCreatorFcns.end() ) {
    return NULL;
  }
  return (it->second);
}

template <typename T>
std::vector<std::string> SingletonFactory<T>::getNames( ) {
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