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
  // updatePlot( res );
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
  // Write convergence table
  var results = document.getElementById( "results" );
  var tableStr = "<tr><th>Samples</th><th>Total</th><th>Mean</th><th>Std Error</th>";
  for ( var iRow = 0; iRow < res.length; iRow++ ){
    tableStr += "<tr>";
    tableStr += "<td>" + res[iRow].Samples+ "</td>";
    tableStr += "<td>" + res[iRow].Total + "</td>";
    tableStr += "<td>" + res[iRow].Mean.toFixed(7) + "</td>";
    tableStr += "<td>" + res[iRow].StdError.toFixed(7) + "</td>";
    tableStr += "</tr>";
  }
  results.innerHTML = tableStr;
}

function updatePlot( res ) {
  // D3 visualization
  var table = d3.select("#plot").append("table");
  var tr = table.selectAll("tr")
    .data( [res.SamplesConverge, res.TotalConverge] )
    .enter().append("tr");
  var td = tr.selectAll("td").data( function (d) { return d; }).enter().append("td");
  td.text( function (d) { return d; });
}
