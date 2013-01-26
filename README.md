# sorrow.js

A v8-based CommonJS implementation, no longer under heavy development.  The original goals were:

  * **performance** - Be fast, whenever possible.
  * **minimalism** - Provide a full platform for JavaScript development, but don't go overboard.
  * **flexibility** - Don't write C++ when you can write JavaScript.  Don't be wastefully slow, but try to stay outside of the interpreter itself

## Building
sorrow depends on [v8](http://code.google.com/p/v8/) and [gyp](http://code.google.com/p/gyp/) to build, which require svn and Python.  These can both be pulled into your local copy by running `make dependencies`.  After that, sorrow can be built by running `make`.

## Legal
sorrow is licensed under the BSD license.  This is available in the LICENSE file.

## Current Status

The original goals of this project were to provided an non-async competitor of node.js.
The best way to appreciate a package is to rewrite it, and it turns out node is pretty swell :)
