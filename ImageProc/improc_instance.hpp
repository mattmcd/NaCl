#ifndef IMPROC_INSTANCE
#define IMPROC_INSTANCE

#include "singleton_factory.hpp"
#include "instance_factory.hpp"
#include "image_proc.hpp"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array.h"
#include "ppapi/cpp/var_array_buffer.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/utility/threading/simple_thread.h"

// The ImageProcInstance that stores 
class ImageProcInstance : public pp::Instance {
  public:
    explicit ImageProcInstance( PP_Instance instance ) 
      : pp::Instance(instance), callback_factory_(this), proc_thread_(this) {};
    virtual ~ImageProcInstance( ){ proc_thread_.Join(); };
    virtual void HandleMessage( const pp::Var& );
    virtual bool Init(uint32_t /*argc*/,
        const char * /*argn*/ [],
        const char * /*argv*/ []) {
      proc_thread_.Start();
      proc_thread_.message_loop().PostWork( 
        callback_factory_.NewCallback( &ImageProcInstance::Version ));
      return true;
    }

  private:
    bool run_simulation_;
    std::string processorName; // Name of currently selected processor
    std::unique_ptr<Processor> processor;
    pp::CompletionCallbackFactory<ImageProcInstance> callback_factory_;
    pp::SimpleThread proc_thread_; // Thread for image processor 
    void Process(cv::Mat); 
    void PostTest(); 
    void SendStatus(const std::string& status); 
    pp::VarDictionary PostResponse( cv::Mat );
    void Version( int32_t ) {
      pp::VarDictionary msg;
      msg.Set( "Type", "version" );
      msg.Set( "Version", "Image Processor 0.4" );
      // Get processor
      auto processorFactory = SingletonFactory<std::function<std::unique_ptr<Processor>()>>::getInstance();
      auto processorList = processorFactory.getNames();
      pp::VarArray msgProcessorList;
      for ( size_t i=0; i < processorList.size(); i ++ ) {
        msgProcessorList.Set( i, processorList[i] );
      }
      msg.Set( "Processors", msgProcessorList );
      PostMessage( msg );
    }
};

#endif
