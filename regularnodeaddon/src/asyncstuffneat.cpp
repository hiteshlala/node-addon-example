#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <node.h>
#include <uv.h> 


using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::CopyablePersistentTraits;
using v8::Isolate;
using v8::Local;
using v8::Handle;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Boolean;
using v8::Persistent;
using v8::HandleScope;
using v8::Array;

void sayHello( const FunctionCallbackInfo< Value >& args ) {
  Isolate * isolate = Isolate::GetCurrent();
  if ( args.Length() < 1 ) {
    isolate -> ThrowException( Exception::TypeError(
      String::NewFromUtf8( isolate, "Wrong number of arguments, expects: name" )
    ));
    return;
  }
  else if ( !args[0] -> IsString() ) {
    isolate -> ThrowException( Exception::TypeError(
      String::NewFromUtf8( isolate, "First argument must be a string" )
    ));
    return;
  }
  String::Utf8Value namein( args[0] );
  std::string name( *namein );
  std::cout << "Sanity Check!! Hello, " << name << "!\n\n";
}

struct Worker {
  uv_work_t worker;
  Persistent< Function, CopyablePersistentTraits< Function > > callback;

  std::string genericinput;
  int msdelay;
  
  bool error;
  std::string errormsg;
  int errorcode;

  std::string genericresult;
};

std::string testDelay( int time ) {
  std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds( time ));
  return "Delay Complete";
}

void longRunFunctionDone( uv_work_t * order, int status ) {
  // std::cout << "longRunFunctionDone()\n" << std::endl; 
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope( isolate );
  auto global = isolate -> GetCurrentContext() -> Global();
  Worker * work = static_cast< Worker * >( order -> data );
  Local<Function> cb = Local<Function>::New( isolate, work -> callback );

  Local<Value> argv[2];
  if ( work -> error ) {
    argv[0] = Exception::Error( String::NewFromUtf8( isolate,  work -> errormsg.c_str() ) );
    argv[1] = Null(isolate);
  }
  else {
    argv[0] =  Null(isolate);
    argv[1] = String::NewFromUtf8( isolate, work -> genericresult.c_str() );
  }
  cb -> Call( global, 2, argv );

  // Memory cleanup
  work -> callback.Reset();
  delete work;
}

void longRunFunctionRunner(  uv_work_t * order ) {
  // std::cout << "longRunFunctionRunner()\n" << std::endl; 
  Worker * work = static_cast< Worker * >( order -> data );
  try {
    work -> genericresult = testDelay( work -> msdelay );
  }
  catch ( ... ) {
    work -> errormsg = "Error executing delay";
    work -> error = true;
  }
}


void longRunFunction( const FunctionCallbackInfo< Value >& args ) {
  // std::cout << "longRunFunction()\n" << std::endl; 
  Isolate* isolate = args.GetIsolate();
  Worker * work = new Worker();

  if ( args.Length() < 2 ) {
    isolate -> ThrowException( Exception::TypeError(
      String::NewFromUtf8( isolate, "Wrong number of arguments, expects: delay, callback" )
    ));
    return;
  }
  else if ( !args[0] -> IsNumber() ) {
    isolate -> ThrowException( Exception::TypeError(
      String::NewFromUtf8( isolate, "First argument must be a number ms delay" )
    ));
    return;
  }
  else if ( !args[1] -> IsFunction() ) {
    isolate -> ThrowException( Exception::TypeError(
      String::NewFromUtf8( isolate, "Second argument must be a callback( error, data ) function" )
    ));
    return;
  }

  int delay = args[0] -> Int32Value();
  Local<Function> callback = Local<Function>::Cast( args[1] );

  work -> error = false;
  work -> callback.Reset( isolate, callback );
  work -> worker.data = work;
  work -> msdelay = delay;
   
  uv_queue_work( uv_default_loop(), & work -> worker, longRunFunctionRunner, longRunFunctionDone );
}


void init( Local< Object > exports, Local< Object > module ) {
  NODE_SET_METHOD( exports, "longRunFunction", longRunFunction );
  NODE_SET_METHOD( exports, "sayHello", sayHello );
}

NODE_MODULE( NODE_GYP_MODULE_NAME, init )