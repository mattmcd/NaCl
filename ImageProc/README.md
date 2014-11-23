Image Processing
================
Image processing using Native Client.  The goal of this project is to use
the C++ OpenCV image processing library to do realtime image processing of
the webcam feed.

Implementation
--------------
Rough plan for next few commits:

- [x] Hidden HTML5 __video__ element sourced from navigator.getUserMedia (webcam).
- [x] Canvas element samples video every 20ms and draws on the canvas.
- [x] Send canvas image as ArrayBuffer to Native Client.
- [x] NaCl echoes image that has been received.
- [x] Javascript puts echoed image to __processed__ canvas
- [x] Start playing with OpenCV image processing
- [x] Use strategy pattern on the C++ side to register new image processors
- [x] Modify NaCl getVersion interface to return list of available processors
- [x] Use returned list of processors to populate js list selector
- [ ] Get the [Pulse Detection](http://people.csail.mit.edu/mrub/vidmag/) filter working 

[Try it out!](http://www.matt-mcdonnell.com/code/NaCl/ImageProc/index.html) - you may need 
to clear the cache to see any updates.  To do this open Chrome Developer Tools (Ctrl-Shift-i or F12), 
then click and hold Reload, choose "Empty Cache and Hard Reload" from the popup menu.  Popup menu may
only appear if you have a cached page i.e. have previously visited.

Build Setup
-----------
There are a few steps needed to configure in order to use OpenCV with NaCl.
Fortunately, once you become aware of the existence of  [NaCl Ports](https://code.google.com/p/naclports/) 
they are fairly straight forward.

1. Install [Depo Tools](http://dev.chromium.org/developers/how-tos/install-depot-tools).
2. [Check out the NaCl Ports](https://code.google.com/p/naclports/wiki/HowTo_Checkout).
2. Follow the [Install guide for SDL](https://code.google.com/p/naclports/wiki/InstallingSDL),
   replacing __sdl__ with __opencv__ and choosing pnacl as the architecture to
   build.  The build itself took about 15 minutes on my Intel i3 dual core laptop.
3. Add the opencv libraries and zlib to the build arguments:

    LDFLAGS := -L$(NACL_SDK_ROOT)/lib/pnacl/Release -lppapi_cpp -lppapi -lopencv_core -lz
