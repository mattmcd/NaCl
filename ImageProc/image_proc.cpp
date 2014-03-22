#include "improc_instance.hpp"
#include "singleton_factory.hpp"
#include <vector>
#include <thread>
#include <functional>

pp::Module* pp::CreateModule() {
  return new InstanceFactory<ImageProcInstance>();
}

void ImageProcInstance::Process(  
        std::function<cv::Mat(cv::Mat)> f, cv::Mat im) {
  auto result = processor.process( f, im );
  pp::VarDictionary msg;
  pp::VarArrayBuffer data;
  msg.Set( "Type", "completed" );
  msg.Set( "Date", data );
  PostMessage( msg );
}


void ImageProcInstance::HandleMessage( const pp::Var& var_message ) {
  // Interface: receive a { cmd: ..., args... } dictionary  
  pp::VarDictionary var_dict( var_message );
  auto cmd = var_dict.Get( "cmd" ).AsString();
  if ( cmd == "process" ) {
    // Message is number of simulations to run
    auto width  = var_dict.Get("width").AsInt();
    auto height = var_dict.Get("height").AsInt();
    auto data   = static_cast<pp::VarArrayBuffer>(var_dict.Get("data"));
    auto processorFactory = SingletonFactory<std::function<cv::Mat(cv::Mat)>>::getInstance();
    auto processor = processorFactory.getObject( var_dict.Get("processor").AsString() );
    // Convert data to CMat
    uint8_t* byteData = static_cast<uint8_t*>(data.Map());
    auto Img = cv::Mat(height, width, CV_8UC4, byteData );
    Process( processor, Img );
    //// Enable simulation
    //run_simulation_ = true;
    //// Start simulation on background thread
    //proc_thread_.message_loop().PostWork( 
    //    callback_factory_.NewCallback( &ImageProcInstance::Process, processor));
  } else {
    // Disable simulation - background thread will see this at start of
    // next iteration and terminate early
    run_simulation_ = false;
  }
}
