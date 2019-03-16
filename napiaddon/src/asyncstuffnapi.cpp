#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <napi.h>


Napi::Value sayHello( const Napi::CallbackInfo& args ) {
  Napi::Env env = args.Env();
  if ( args.Length() < 1 ) {
    Napi::TypeError::New( env, "Wrong number of arguments, expects: name" ).ThrowAsJavaScriptException();
    return env.Null();
  }
  else if ( !args[0].IsString() ) {
    Napi::TypeError::New( env, "First argument must be a string" ).ThrowAsJavaScriptException();
    return env.Null();
  }
  std::string name = args[0].As< Napi::String >().Utf8Value();
  std::cout << "Sanity Check!! Hello, " << name.c_str() << "!\n\n";
  return Napi::String::New( env, "Hello from synchronous function " + name );
}

class LongRun : public Napi::AsyncWorker {
  public:
    LongRun( int delay, Napi::Function& callback ) : Napi::AsyncWorker( callback ), msdelay( delay ) {}
    ~LongRun() {}

    void Execute () {
      // std::cout << "Execute()\n" << std::endl;
      try {
        result = testDelay( msdelay );
      }
      catch ( ... ) {
        errormsg = "Error executing delay";
        error = true;
      }
    }

    void OnOK() {
      // std::cout << "OnOK()\n" << std::endl;
      Napi::Env env = Env();
      Napi::HandleScope scope( env );
      if ( error ) {
        Callback().Call({ Napi::Error::New( env, errormsg  ).Value(), env.Undefined() });
      }
      else {
        Callback().Call({ env.Null(), Napi::String::New( env, result ) });
      }
    }

    std::string testDelay( int time ) {
      std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds( time ));
      return "Delay Complete";
    }

  private:
    int msdelay;
    std::string result;
  
    bool error = false;
    std::string errormsg;
    int errorcode;

};



Napi::Value longRunFunction( const Napi::CallbackInfo& args ) {
  // std::cout << "longRunFunction()\n" << std::endl;
  Napi::Env env = args.Env();
  if ( args.Length() < 2 ) {
    Napi::TypeError::New( env, "Wrong number of arguments, expects: delay, callback" ).ThrowAsJavaScriptException();
    return env.Null();
  }
  else if ( !args[0].IsNumber() ) {
    Napi::TypeError::New( env, "First argument must be a number ms delay" ).ThrowAsJavaScriptException();
    return env.Null();
  }
  else if ( !args[1].IsFunction() ) {
    Napi::TypeError::New( env, "Second argument must be a callback( error, data ) function" ).ThrowAsJavaScriptException();
    return env.Null();
  }

  int delay = args[0].As< Napi::Number >().Uint32Value();
  Napi::Function callback = args[1].As< Napi::Function >();
  LongRun* worker = new LongRun( delay, callback );
  worker -> Queue();
  return env.Undefined();
}


Napi::Object init( Napi::Env env, Napi::Object exports ) {
  exports.Set( Napi::String::New( env, "longRunFunction" ), Napi::Function::New( env, longRunFunction ) );
  exports.Set( Napi::String::New( env, "sayHello" ), Napi::Function::New( env, sayHello ) );
  return exports;
}

NODE_API_MODULE( NODE_GYP_MODULE_NAME, init )