// Global handle to module
MonteCarloModule = null;

// Global status message
statusText = 'NO-STATUS';

lastClick = null;

nPtsSim = null;

function pageDidLoad() {
  var listener = document.getElementById("listener");
  listener.addEventListener("load", moduleDidLoad, true );
  listener.addEventListener("message", handleMessage, true );
  if ( MonteCarloModule == null ) {
    updateStatus( 'LOADING...' );
  } else {
    updateStatus();
  }
}

function moduleDidLoad() {
  MonteCarloModule = document.getElementById( "monte_carlo" );
  updateStatus( "OK" );
  var go = document.getElementById( "go" );
  var nPts = document.getElementById( "nPts" );
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
  console.log( "Received " + message_event.data.length + " results" );
  var tDiff = Date.now() - lastClick;
  var res = message_event.data;
  updateStatus( "Received: " + res[res.length-1].Mean.toFixed(7) 
    + " +/- " + res[res.length-1].StdError.toFixed(7) 
    +  " after " + tDiff + "ms" + " for " + nPtsSim + " points" );
  updateTable( res );
  var go = document.getElementById( "go" );
  go.disabled = false;
}

function updateStatus( optMessage ) {
  if (optMessage)
    statusText = optMessage;
  var statusField = document.getElementById("statusField");
  if (statusField) {
    statusField.innerHTML = statusText;
  }
}

function updateTable( res ) {
  // D3 visualization
  d3.select("#results table").remove(); // Remove old data
  var table = d3.select("#results").append("table");
  var th = table.selectAll("th")
    .data(["Samples","Total","Mean","Std Error"])
    .enter().append("th").text( function (d) { return d; });
  var tr = table.selectAll("tr")
    .data( res )
    .enter().append("tr");
  var td = tr.selectAll("td")
    .data( function (d) { 
      return [d.Samples, d.Total, d.Mean.toFixed(7), d.StdError.toFixed(7)]; })
    .enter().append("td");
  td.text( function (d) { return d; });
}
