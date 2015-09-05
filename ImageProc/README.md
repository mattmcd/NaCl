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
- [x] Javascript puts echoed image to __processed__ canvas.
- [x] Start playing with OpenCV image processing.
- [x] Use strategy pattern on the C++ side to register new image
  processors.
- [x] Modify NaCl getVersion interface to return list of available processors.
- [x] Use returned list of processors to populate js list selector.
- [ ] Get the [Pulse Detection](http://people.csail.mit.edu/mrub/vidmag/) filter working. 
- [x] Get the [Detecting
  Barcodes](http://www.pyimagesearch.com/2014/11/24/detecting-barcodes-images-python-opencv) demo working.
- [x] Simple cartoon filter.
- [x] Face detection.

[Try it out!](https://www.matt-mcdonnell.com/code/NaCl/ImageProc/index.html) - you may need 
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
2. ~~Follow the [Install guide for SDL](https://code.google.com/p/naclports/wiki/InstallingSDL),
   replacing __sdl__ with __opencv__ and choosing pnacl as the architecture to
   build.~~
   Seems like the original instructions have been deleted, and didn't cover
   which branch to use in any event.  cd to the dir where you've checked
   out nacl_ports (for me ~/Work/ExternCode/nacl_ports/src ) with depotools
   then change to the branch that matches NACL_SDK_ROOT

    git checkout pepper_40 <br>
    NACL_ARCH=pnacl make opencv

   The build itself took about 15 minutes on my Intel i3 dual core laptop.

3. Add the opencv libraries and zlib to the build arguments:

    LDFLAGS := -L$(NACL_SDK_ROOT)/lib/pnacl/Release -lppapi_cpp -lppapi -lopencv_core -lz

Updating NaCl and NaCl Ports
----------------------------
NaCl updates on around the same release cycle as the Chrome browser.
Chrome itself is backwards compatible but it's still a good idea to keep
roughly up to date.

Here we assume that the NaCl SDK has been installed in NACL_ROOT
(NACL_SDK_ROOT will be under this), and NaCl ports has been installed in
PORTS_ROOT.  On my machine these are ~/Work/ExternCode/nacl_sdk and
~/Work/ExternCode/nacl_ports respectively.

1. Update NaCl SDK
   - cd NACL_ROOT
   - ./naclsdk update
2. Update NACL_SDK_ROOT environment variable e.g. NACL_ROOT/pepper_41
3. Update ports
   - cd PORTS_ROOT
   - cd src
   - git checkout -b pepper_41 origin/pepper_41
   - gclient sync
   - (If that fails, cd .. vim .gclient and set managed=False)

LICENSE
-------
BSD
