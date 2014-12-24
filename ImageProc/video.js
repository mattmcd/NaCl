// Global handle to module
var ImageProcModule = null;

// Global status message
var statusText = 'NO-STATUS';
var lastClick = null;

var isRunning = false;
var isReadyToReceive = false;

var samplePeriod = 40; // ms

var startTime;
var endTime;
var averageFramePeriod = samplePeriod;
var ewmaSmooth = 0.3;

var videoSourceId;
var selectedSource;

function getVideoSources() {
  // Populate list of video sources e.g. laptop camera and USB webcam
  var videoSelect = document.querySelector("select#camera");
  MediaStreamTrack.getSources( 
    function( srcInfo ) {
      videoSourceId = srcInfo
        .filter( function(s) { return s.kind == "video"; } )
        .map( function(s, ind) { 
          // Set up list of cameras
          var option = document.createElement( "option" );
          option.text = "camera " + ind;
          option.value= s.id;
          videoSelect.appendChild( option );
          return s.id; } );
      selectedSource = videoSourceId[0];
      setVideoInput();
    } 
  );
  videoSelect.onchange = setVideoCb;
}

function setVideoCb () {
  console.log("camera changed");
  var videoSelect = document.querySelector("select#camera");
  selectedSource = videoSelect.value;
  setVideoInput();
}

function setVideoInput() {
  var video = document.getElementById("live");
  var context = display.getContext("2d");

  navigator.webkitGetUserMedia(
    {video: { optional:[{sourceId : selectedSource}] }, audio:false}, 
    function (stream) {
      video.src = window.URL.createObjectURL(stream);
      draw( video, context);
    },
    function (err) {
      console.log("Unable to get media stream:" + err.Code );
    });
}

function pageDidLoad() {
  getVideoSources();
  var listener = document.getElementById("listener");
  listener.addEventListener("load", moduleDidLoad, true );
  listener.addEventListener("message", handleMessage, true );
  if ( ImageProcModule == null ) {
    updateStatus( 'LOADING...' );
  } else {
    updateStatus();
  }

  // Set up inteface
  var go = document.getElementById("go");
  go.addEventListener("onclick", sendImage );
}

function draw(v,c) {
  c.drawImage(v, 0, 0);
  setTimeout( draw, samplePeriod, v, c);
}

function loadResource(url) {
  var cmd = { cmd: "load",  
    url: "lbpcascade_frontalface.xml" };
  ImageProcModule.postMessage( cmd );
}

function sendImage() {
  if ( isRunning && isReadyToReceive ) {
    // Get the current frame from canvas and send to NaCl
    var display = document.getElementById("display");
    var ctx = display.getContext( "2d" );
    var height = display.height;
    var width = display.width;
    var nBytes = height * width * 4; 
    var pixels = ctx.getImageData(0, 0, width, height);
    // drawImage( pixels );

    var theCommand = "process"; //"echo"; // test, process
    var selectedProcessor = getSelectedProcessor();
    var cmd = { cmd: theCommand,  
      width: width, 
      height: height, 
      data: pixels.data.buffer, 
      processor: selectedProcessor };
    startTime = performance.now();
    ImageProcModule.postMessage( cmd );
    isReadyToReceive = false; // Don't send any more frames until ready
  } else if (isRunning ) {
    // Do nothing
  } else { 
    updateStatus( 'Stopped' );
  }
  setTimeout( sendImage, samplePeriod );
}

function drawImage(pixels){
    var processed = document.getElementById("processed");
    var ctx = processed.getContext( "2d" );
    var imData = ctx.getImageData(0,0,processed.width,processed.height);
    var buf8 = new Uint8ClampedArray( pixels );
    imData.data.set( buf8 );
    ctx.putImageData( imData, 0, 0);
}

function moduleDidLoad() {
  ImageProcModule = document.getElementById( "image_proc" );
  updateStatus( "OK" );
  var go = document.getElementById( "go" );
  var videoSelect = document.querySelector( "select#camera" );
  go.onclick = startSending;
  var stop = document.getElementById( "stop" );
  stop.onclick = stopSending;
  stop.disabled = true;

  videoSelect.hidden = false;
  stop.hidden = false;
  go.hidden = false;
}

function startSending() {
  isRunning = true;
  isReadyToReceive = true;
  var go = document.getElementById( "go" );
  go.disabled = true;
  var stop = document.getElementById( "stop" );
  stop.disabled = false;
  sendImage();
}

function stopSending() {
  isRunning = false;
  isReadyToReceive = false;
  var go = document.getElementById( "go" );
  go.disabled = false;
  var stop = document.getElementById( "stop" );
  stop.disabled = true;
}

function handleMessage(message_event) {
  var res = message_event.data;
  if ( res.Type == "version" ) {
    updateStatus( res.Version );
    updateProcessors( res.Processors );
  }
  if ( res.Type == "completed" ) {
    if ( res.Data ) {
      // updateStatus( "Received array buffer");
      // Display processed image    
      endTime = performance.now();
      averageFramePeriod = (1-ewmaSmooth)*averageFramePeriod + ewmaSmooth*(endTime-startTime);
      updateStatus( 'Frame rate is ' + (averageFramePeriod).toFixed(1) + 'ms per frame' );
      drawImage( res.Data );
      isReadyToReceive = true;
    } else {
      updateStatus( "Received something unexpected");
    }

    // Display processed image    
    //drawImage( res.Data );
  }
  if ( res.Type == "status" ) {
    // updateStatus( res.Message ); 
  }
  if ( res.Type == "resource_loaded" ) {
    updateStatus( res.Message ); 
  }
}

function updateStatus( optMessage ) {
  if (optMessage)
    statusText = optMessage;
  var statusField = document.getElementById("statusField");
  if (statusField) {
    statusField.innerHTML = " : " + statusText;
  }
}

function updateProcessors( processorNames ) {
  var processors = document.getElementById( "processor" );
  for (i=0; i<processorNames.length; i++ ) {
    var option = document.createElement("option");
    option.text = processorNames[i];
    processors.add( option );
  }
  processors.disabled = false;
  processors.hidden = false;
}

function getSelectedProcessor( ) {
  var processors = document.getElementById( "processor" );
  processor = processors[ processors.selectedIndex ].text;
  if ( !processor ) {
    processor = "Id";
  }
  return processor;
}
