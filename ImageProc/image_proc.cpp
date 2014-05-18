#include "improc_instance.hpp"
#include "singleton_factory.hpp"
#include <vector>
#include <thread>
#include <functional>

pp::Module* pp::CreateModule() 
{
  return new InstanceFactory<ImageProcInstance>();
}

void ImageProcInstance::Process( cv::Mat im) 
{
  auto result = (*processor)( im );
  auto nBytes = result.elemSize() * result.total();
  pp::VarDictionary msg;
  pp::VarArrayBuffer data(nBytes);
  uint8_t* copy = static_cast<uint8_t*>( data.Map());
  memcpy( copy, result.data, nBytes );

  msg.Set( "Type", "completed" );
  msg.Set( "Data", data );
  PostMessage( msg );
}

void ImageProcInstance::PostTest() 
{
  pp::VarDictionary msg;
  msg.Set( "Type", "completed" );
  msg.Set( "Data", "Processed ok" );
  PostMessage( msg );
}

void ImageProcInstance::SendStatus(const std::string& status) 
{
  pp::VarDictionary msg;
  msg.Set( "Type", "status" );
  msg.Set( "Message", status );
  PostMessage( msg );
}


void ImageProcInstance::HandleMessage( const pp::Var& var_message ) 
{
  // Interface: receive a { cmd: ..., args... } dictionary  
  pp::VarDictionary var_dict( var_message );
  auto cmd = var_dict.Get( "cmd" ).AsString();
  if ( cmd == "process" ) {

    // Message is number of simulations to run
    auto width  = var_dict.Get("width").AsInt();
    auto height = var_dict.Get("height").AsInt();
    auto data   = pp::VarArrayBuffer( var_dict.Get("data") );
    auto selectedProcessor = var_dict.Get("processor").AsString();
    if ( selectedProcessor != processorName ) {
      SendStatus("Creating processor factory");
      auto processorFactory = SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
      SendStatus("Creating processor");
      processor = processorFactory.getObject( selectedProcessor )();
      processorName = selectedProcessor;
    } else {
      SendStatus("Reusing processor");
    }
    // Convert data to CMat
    // SendStatus("Casting to byte array");
    uint8_t* byteData = static_cast<uint8_t*>(data.Map());
    // SendStatus("Creating cv::Mat");
    auto Img = cv::Mat(height, width, CV_8UC4, byteData );
    // SendStatus("Calling processing");
    Process( Img );
  } else if ( cmd == "test" ) {
    PostTest();
  } else if ( cmd == "echo" ) {
      auto data = pp::VarArrayBuffer( var_dict.Get("data") );
      // auto result = data.is_array_buffer();
      pp::VarDictionary msg;
      msg.Set( "Type", "completed" );
      msg.Set( "Data", data );
      PostMessage( msg );
  } else {
    // Disable simulation - background thread will see this at start of
    // next iteration and terminate early
    run_simulation_ = false;
  }
}
