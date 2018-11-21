#include <iostream>
#include <string>
#include <cstddef>
#include <node.h>
#include <uv.h> 
#include <chrono>
#include <thread>

#include "nbind/api.h"

using v8::Isolate;
using v8::HandleScope;


std::string sayHello(std::string name) {
  std::cout << "Sanity Check!! Hello, " << name << "!\n\n";
  return "Hello from synchronous function";
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
  std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds( time ));
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
  catch( ... ) {
    work -> error = true;
    work -> errormsg = "Error executing delay";
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




struct ProgressWorker {
  ProgressWorker( nbind::cbFunction cb, nbind::cbFunction prgrs ): callback( cb ), progress( prgrs ) {}
  ~ProgressWorker() {
    callback.reset();
    progress.reset();
  }
  
  uv_work_t worker;
  nbind::cbFunction callback;

  uv_async_t progworker;
  nbind::cbFunction progress;
  float progpercent;

  std::string genericinput;
  int msdelay;
  
  bool error;
  std::string errormsg;
  int errorcode;

  std::string genericresult;
};

void callProgress( uv_async_t * order ) {
  ProgressWorker * work = static_cast< ProgressWorker * >( order -> data );
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope( isolate );
  work -> progress.call<void>( work -> progpercent );
}

void longRunFuncWithProgressDone( uv_work_t * order, int status ) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope( isolate );

  ProgressWorker * work = static_cast< ProgressWorker * >( order -> data );

  if ( work -> error ) {
    work -> callback.call<void>( work -> errormsg.c_str(), NULL );
  }
  else {
    work -> callback.call<void>(  NULL, work -> genericresult.c_str() );
  }

  uv_close( (uv_handle_t*) & work -> progworker, NULL );

  delete work;
}

void longRunFuncWithProgressRunner(  uv_work_t * order ) {
  ProgressWorker * work = static_cast< ProgressWorker * >( order -> data );
  try {
    testDelay( work -> msdelay );
    work -> progpercent = 1.0/3;
    uv_async_send( &work -> progworker );

    testDelay( work -> msdelay );
    work -> progpercent = 2.0/3;
    uv_async_send( &work -> progworker );

    testDelay( work -> msdelay );
    work -> progpercent = 3.0/3;
    uv_async_send( &work -> progworker );
    work -> genericresult = "Success";
  }
  catch( ... ) {
    work -> error = true;
    work -> errormsg = "Error executing delay";
  }
}

void longRunFuncWithProgress( int delay, nbind::cbFunction & callback, nbind::cbFunction & progress ) {
  ProgressWorker * work = new ProgressWorker( callback, progress );
  work -> worker.data = work;
  work -> msdelay = delay;
  work -> error = false;
  work -> progpercent = 0.0;
  work -> progworker.data = work;
  uv_async_init( uv_default_loop(), & work -> progworker, callProgress );

  uv_queue_work( uv_default_loop(), & work -> worker, longRunFuncWithProgressRunner, longRunFuncWithProgressDone );
}




#include "nbind/nbind.h"

NBIND_GLOBAL() {
  function ( sayHello );
  function( longRunFunction );

  function( longRunFuncWithProgress );
}