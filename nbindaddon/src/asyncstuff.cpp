#include <node.h>
#include <iostream>
#include <string>
#include <cstddef>
#include <uv.h> 
#include <windows.h>

#include "nbind/api.h"

using v8::Isolate;
using v8::HandleScope;


void sayHello(std::string name) {
  std::cout << "Sanity Check!! Hello, " << name << "!\n\n";
}


struct Worker {
  Worker( nbind::cbFunction cb ): callback( cb ) {}
  
  uv_work_t worker;
  nbind::cbFunction  callback;

  std::string genericinput;
  int msdelay;
  
  bool error;
  std::string errormsg;
  int errorcode;

  std::string genericresult;
};

std::string testDelay( int time ) {
  // std::cout << "Delay Start " << time << "!\n";
  Sleep( time );
  return "Delay Complete";
} 

void longRunFunctionDone( uv_work_t * order, int status ) {
  // std::cout << "longRunFunctionDone()\n" << std::endl; 
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope( isolate );

  Worker * work = static_cast< Worker * >( order -> data );

  if ( work -> error ) {
    work -> callback.call<void>( work -> errormsg.c_str(), NULL );
  }
  else {
    work -> callback.call<void>(  NULL, work -> genericresult.c_str() );
  }

  // Memory cleanup
  work -> callback.reset();
  delete work;
}

void longRunFunctionRunner(  uv_work_t * order ) {
  // std::cout << "longRunFunctionRunner()\n" << std::endl; 
  Worker * work = static_cast< Worker * >( order -> data );
  try {
    work -> genericresult = testDelay( work -> msdelay );
  }
  catch ( std::exception & error ) {
    work -> errormsg = error.what();
    work -> error = true;
  }
}


void longRunFunction( int delay, nbind::cbFunction & callback ) {
  // std::cout << "longRunFunction()\n" << std::endl; 
  Worker * work = new Worker( callback );
  
  work -> worker.data = work;
  work -> msdelay = delay;
  work -> error = false;
   
  uv_queue_work( uv_default_loop(), & work -> worker, longRunFunctionRunner, longRunFunctionDone );
}


#include "nbind/nbind.h"

NBIND_GLOBAL() {
  function ( sayHello );
  function( longRunFunction );
}