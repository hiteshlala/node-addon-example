# Node Async Addon

Some experiments accessing C++ code asynchronously.

Note: I primarily code using Javascript and  not C++ so I am sure there might be bugs or poor implementation of my C++ code.

Both modules uses Python 2.7 underneath and I had to include a path to that version of Python in the build scripts.

There are two servers in this demo one of which ( Koa ) demonstrates a delay in response from the node addons.


## Building

Have to install and build each of the projects in the subdirectories:
  nanaddon
  nbindaddon
  regularnodeaddon

Then have to install and build in the root directory.
  node serveKoa.js will start a server listening on 7003 that has a web inteface.
  node serveExpress.js will start a server listening on 7004 that has a web inteface.


## Problem

There seems to be a delay in the handoff between the c++ to the awaiter in js.

This issue exists in Regular and Nbind version.  Nan version does not display this behaviour.

The issue exists both in Koa and Express servers.

In Express if I remove the Promise call and use a callback the delay is gone. This demos Express server uses callback methods.

I found the same behaviour using Node 10.13 and Node 8.12.

Wrapping the callback function bodies in setTimeout() seems to solve this issue.

Originally I had wrapped the addon async functions like this:

```
  function asyncAddOnFuncCall( time ) {
    return new Promise(( resolve, reject ) => {
      addon.longRunFunction( time, ( err, res ) => {
        if( err ) {
          reject( err );
        }
        else {
          resolve( res );
        }
      });
    });
  }
```

Which displays delays using nbind and regular addon functions. Nan addon function responds in timely manner.

I discovered that the delay happens after the resolve is called in the else block above.  I discovered this by logging the time elapsed after the resovle ( short ) and the elapsed time in the calling code after the await ( long ).

By wrapping the callback body in a setTimeout then all addon behave the same without any delays.

```
  function asyncAddOnFuncCall( time ) {
    return new Promise(( resolve, reject ) => {
      addon.longRunFunction( time, ( err, res ) => {
        setTimeout( () => {
          if( err ) {
            reject( err );
          }
          else {
            resolve( res );
          }
        }, 0 );
      });
    });
  }
```






