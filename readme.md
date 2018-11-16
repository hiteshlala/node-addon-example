# Node Async Addon

Some experiments accessing C++ code asynchronously.

Note: I primarily code using Javascript and  not C++ so I am sure there might be bugs or poor implementation of my C++ code.

Both modules uses Python 2.7 underneath and I had to include a path to that version of Python in the build scripts.


## Building

Have to install and build each of the projects in the subdirectories:
  nanaddon
  nbindaddon
  regularnodeaddon

Then have to install and build in the root directory.
  node serve.js will start a server listening on 7003 that has a web inteface.



## Node-gyp

Uses the node-gyp directly along with accessing V8:: classes and methods.


## Nbind

Uses the nbind with its automation - I find some of it annoying.

## Nan

Uses nan layer.


## Problem

When

