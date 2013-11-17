// Global handle to module
CounterModule = null;

// Global status message
statusText = 'NO-STATUS';

function pageDidLoad() {
  if ( CounterModule == null ) {
    updateStatus( 'LOADING...' );
  } else {
    updateStatus();
  }
}

function moduleDidLoad() {
  CounterModule = document.getElementById( 'counter' );
  updateStatus( 'OK' );
  var inc = document.getElementById( 'inc' );
  inc.onclick = function() { CounterModule.postMessage(1); }; 
  inc.disabled = false;
}

function callNaCl() {
  console.log( "Calling NaCl" );
  CounterModule.postMessage(1); 
}

function handleMessage(message_event) {
  console.log( "Received message from NaCl");
  updateStatus(message_event.data);
}

function updateStatus( optMessage ) {
  if (optMessage)
    statusText = optMessage;
  var statusField = document.getElementById('statusField');
  if (statusField) {
    statusField.innerHTML = statusText;
  }
}
