// Global handle to module
MonteCarloModule = null;

// Global status message
statusText = 'NO-STATUS';

lastClick = null;

nPtsSim = null;

function pageDidLoad() {
  var listener = document.getElementById("listener");
  listener.addEventListener('load', moduleDidLoad, true );
  listener.addEventListener('message', handleMessage, true );
  if ( MonteCarloModule == null ) {
    updateStatus( 'LOADING...' );
  } else {
    updateStatus();
  }
}

function moduleDidLoad() {
  MonteCarloModule = document.getElementById( 'monte_carlo' );
  updateStatus( 'OK' );
  var go = document.getElementById( 'go' );
  var nPts = document.getElementById('nPts');
  go.onclick = function() {
    go.disabled = true;
    nPtsSim = Number( nPts.value );
    console.log( "Sending " + nPtsSim);
    lastClick = Date.now();
    MonteCarloModule.postMessage( nPtsSim ); }; 
  go.disabled = false;
  nPts.disabled = false;
}

function handleMessage(message_event) {
  console.log( "Received " + message_event.data );
  var tDiff = Date.now() - lastClick;
  var res = message_event.data;
  updateStatus( "Received: " + res.Mean.toFixed(7) 
    + " +/- " + res.StdError.toFixed(7) 
    +  " after " + tDiff + "ms" + " for " + nPtsSim + " points" );
  console.log( res.TotalConverge );
  console.log( res.SamplesConverge );
  console.log( res.MeanConverge );
  console.log( res.StdErrConverge );
  // Write convergence table
  var results = document.getElementById( "results" );
  var tableStr = '<tr><th>Samples</th><th>Total</th><th>Mean</th><th>Std Error</th>';
  for ( var iRow = 0; iRow < res.SamplesConverge.length; iRow++ ){
    tableStr += "<tr>";
    tableStr += "<td>" + res.SamplesConverge[iRow] + "</td>";
    tableStr += "<td>" + res.TotalConverge[iRow] + "</td>";
    tableStr += "<td>" + res.MeanConverge[iRow].toFixed(7) + "</td>";
    tableStr += "<td>" + res.StdErrConverge[iRow].toFixed(7) + "</td>";
    tableStr += "</tr>";
  }
  results.innerHTML = tableStr;
  var go = document.getElementById( 'go' );
  go.disabled = false;
}

function updateStatus( optMessage ) {
  if (optMessage)
    statusText = optMessage;
  var statusField = document.getElementById('statusField');
  if (statusField) {
    statusField.innerHTML = statusText;
  }
}
