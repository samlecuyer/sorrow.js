# sorrow.js

A v8-based CommonJS implementation, currently under heavy development.  The current goals are:

  * **performance** - Be fast, whenever possible.
  * **minimalism** - Provide a full platform for JavaScript development, but don't go overboard.
  * **flexibility** - Don't write C++ when you can write JavaScript.  Don't be wastefully slow, but try to stay outside of the interpreter itself

## Platforms
Currently, sorrow is supported on POSIX systems.  It's only been built on a Mac, but should work on any POSIX system.

## Installation
If you have a C++ compiler, svn and Python installed, it should be as simple as `make`.  It'll take outrageously long to build the first time, because you'll need to download and compile v8 the first time, but should be faster after that.

