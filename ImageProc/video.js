// Global handle to module
var ImageProcModule = null;

// Global status message
var statusText = 'NO-STATUS';
var lastClick = null;

var isRunning = false;
var isReadyToReceive = false;

var startTime;
var endTime;
var averageFramePeriod;
var ewmaSmooth = 0.3;

var videoSourceId;
var selectedSource;

var imageData;
var cachedSmiley;

var view;

// Parameters for current processor
var parameters;

function pageDidLoad() {
  var src = make_image_source();
  view = make_image_source_view(src);
  view.init("live", "display", "camera"); 
  var camera = document.getElementById("camera");
  camera.hidden = true;
  var listener = document.getElementById("listener");
  listener.addEventListener("load", moduleDidLoad, true );
  listener.addEventListener("message", handleMessage, true );
  if ( ImageProcModule == null ) {
    updateStatus( 'LOADING...' );
  } else {
    updateStatus();
  }

}

function loadResource() {
  var cmd = { cmd: "load",  
    url: "smiley_col.bmp" };
  ImageProcModule.postMessage( cmd );
}

function sendImage() {
  if ( isRunning && isReadyToReceive ) {
    // Get the current frame from canvas and send to NaCl
    // drawImage( pixels );
    var imData = view.getImageData();

    var theCommand = "process"; //"echo"; // test, process
    var selectedProcessor = getSelectedProcessor();
    var cmd = { cmd: theCommand,  
      width: imData.width, 
      height: imData.height, 
      data: imData.data, 
      processor: selectedProcessor };
    if ( selectedProcessor === "Smiley!" ) {
      cmd.args = cachedSmiley;
    } else if ( parameters ) {
      cmd.args = JSON.stringify( parameters );
    }
    startTime = performance.now();
    ImageProcModule.postMessage( cmd );
    isReadyToReceive = false; // Don't send any more frames until ready
  } else if (isRunning ) {
    // Do nothing
  } else { 
    updateStatus( 'Stopped' );
  }
  setTimeout( sendImage, view.getSamplePeriod() );
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
  // Load image resource
  // loadResource();
  var cv = document.getElementById( "smiley_canvas" );
  var ctx = cv.getContext( "2d" );
  var smiley = document.getElementById( "smiley" );
  ctx.drawImage(smiley, 0, 0);
  imageData = ctx.getImageData( 0, 0, smiley.width, smiley.height);
  cachedSmiley = view.getImageData("smiley_canvas");

  var run = document.getElementById( "run" );
  run.onclick = toggleSending;

  var stream = document.getElementById("stream");
  stream.onclick = toggleStream;
  stream.value = "On";
  stream.textContent = stream.value;
  stream.hidden = false;

  var camera = document.getElementById("camera")
  camera.hidden = false;
  run.hidden = false;
}

function toggleStream() {
  // Toggle input video stream on/off
  var stream = document.getElementById("stream");
  if (stream.value === 'On') {
    // Stop camera
    stream.value = 'Off';
    view.stop();
  } else {
    // Start camera
    stream.value = 'On';
    view.start();
  }
  stream.textContent = stream.value;
}

function toggleSending() {
  var run = document.getElementById( "run" );
  if (isRunning) {
    // Stop processing
    isRunning = false;
    isReadyToReceive = false;
    run.value = "Go";
  } else {
    // Start processing
    isRunning = true;
    isReadyToReceive = true;
    run.value = "Stop";
  }
  run.textContent = run.value;
  sendImage();
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
      if (typeof averageFramePeriod === 'undefined' ) {
        averageFramePeriod = view.getSamplePeriod();
      }
      averageFramePeriod = (1-ewmaSmooth)*averageFramePeriod + ewmaSmooth*(endTime-startTime);
      updateStatus( 'Frame rate is ' + (averageFramePeriod).toFixed(1) + 'ms per frame' );
      drawImage( res.Data );
      isReadyToReceive = true;
    } else {
      updateStatus( "Received something unexpected");
    }
    if ( res.Parameters ) {
      parameters = JSON.parse( res.Parameters );
    } else {
      parameters = "";
    }

    // Display processed image    
    //drawImage( res.Data );
  }
  if ( res.Type == "status" ) {
    // updateStatus( res.Message ); 
  }
  if ( res.Type == "resource_loaded" ) {
    // Load image from server
    updateStatus( res.Message ); 
    imageData = res.Data;
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
