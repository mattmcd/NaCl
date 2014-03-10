Image Processing
================
Image processing using Native Client.  The goal of this project is to use
the C++ OpenCV image processing library to do realtime image processing of
the webcam feed.

Implementation
--------------
Rough plan for next few commits:

- [x] Hidden HTML5 _video_ element sourced from navigator.getUserMedia (webcam).
- [x] Canvas element samples video every 20ms and draws on the canvas.
- [ ] Send canvas image as ArrayBuffer to Native Client.
- [ ] NaCl responds that image has been received.

[Try it out!](https://rawgithub.com/mattmcd/NaCl/master/ImageProc/index.html) 
