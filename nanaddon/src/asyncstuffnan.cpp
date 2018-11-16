#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <node.h>
#include <uv.h> 
#include <nan.h>


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


NAN_METHOD( sayHello ) {
  if ( info.Length() < 1 ) {
    Nan::ThrowTypeError( "Wrong number of arguments, expects: name" );
    return;
  }
  else if ( !info[0] -> IsString() ) {
    Nan::ThrowTypeError( "First argument must be a string" );
    return;
  }
  String::Utf8Value namein( info[0] );
  std::string name( *namein );
  std::cout << "Sanity Check!! Hello, " << name << "!\n\n";
  info.GetReturnValue().Set( Nan::New("Hello from synchronous function").ToLocalChecked() );
}


std::string testDelay( int time ) {
  std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds( time ));
  return "Delay Complete";
}

class LongRunFunction: public Nan::AsyncWorker {
  public:
    LongRunFunction( int w, Nan::Callback * callback ) : Nan::AsyncWorker( callback, "LongRunFunction" ), wait( w ), result("") {}
    ~LongRunFunction() {}

    void Execute() {
      result = testDelay( wait );
    }

    void HandleOKCallback () {
      Nan::HandleScope scope;

      v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        Nan::New< v8::String >( result.c_str() ).ToLocalChecked()
      };

      callback->Call( 2, argv );
    }

  private:
    std::string result;
    int wait;
};

NAN_METHOD( longRunFunction ) {
  // std::cout << "longRunFunction()\n" << std::endl; 
  if ( info.Length() < 2 ) {
    Nan::ThrowTypeError( "Wrong number of arguments, expects: delay, callback" );
    return;
  }
  else if ( !info[0] -> IsNumber() ) {
    Nan::ThrowTypeError( "First argument must be a number ms delay" );
    return;
  }
  else if ( !info[1] -> IsFunction() ) {
    Nan::ThrowTypeError( "Second argument must be a callback( error, data ) function" );
    return;
  }

  int delay = info[0] -> Int32Value();
  Nan::Callback *callback = new Nan::Callback( Nan::To<v8::Function>( info[1] ).ToLocalChecked() );
  Nan::AsyncQueueWorker( new LongRunFunction( delay, callback) );
}

NAN_MODULE_INIT( InitAll ) {

  Nan::Set( target, 
    Nan::New< v8::String >( "longRunFunction" ).ToLocalChecked(), 
    Nan::GetFunction( Nan::New<v8::FunctionTemplate>( longRunFunction ) ).ToLocalChecked() 
  );

  Nan::Set(  target,
    Nan::New< v8::String >( "sayHello" ).ToLocalChecked(), 
    Nan::GetFunction (Nan::New<v8::FunctionTemplate>( sayHello )).ToLocalChecked()
  );

}

NODE_MODULE( NODE_GYP_MODULE_NAME, InitAll )