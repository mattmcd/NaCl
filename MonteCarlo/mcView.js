// Global handle to module
var MonteCarloModule = null;

// Global status message
var statusText = 'NO-STATUS';

var lastClick = null;

var nPtsSim = null;

var results = new Array();

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
  var modelList = document.getElementById( "model" );
  go.onclick = function() {
    nPtsSim = Number( nPts.value );
    console.log( "Sending " + nPtsSim);
    updateStatus( "Sending " + nPtsSim);
    lastClick = Date.now();
    // clear results
    results.length = 0;
    var model = modelList[ modelList.selectedIndex ].text;
    var cmd = { cmd: "sim", nPts: nPtsSim, model: model };
    MonteCarloModule.postMessage( cmd ); 
    updateControls_SimRunning();
    }; 
  // Stop button
  var stop = document.getElementById( "stop" );
  stop.onclick = function() {
    console.log( "Stop sent" );
    MonteCarloModule.postMessage( { cmd: "stop" } );
  }
  updateControls_SimStopped();
}

function handleMessage(message_event) {
  var res = message_event.data;
  if ( res.Type == "version" ) {
    updateStatus( res.Version );
    updateModels( res.Models );
  } else if ( res.Type == "partial" ) {
    var pctComplete = 100*res.Samples/nPtsSim;
    if ( Math.floor( pctComplete/10 ) > results.length ) {
      results[results.length] = res; // Add result to list of results
    }
    updateStatus( "Received: " + res.Mean.toFixed(7) 
        + " +/- " + res.StdError.toFixed(7) 
        + " after " + pctComplete.toFixed(1) + "% completion" );
  }
  if ( res.Type == "completed" ) {
    if ( results.length > 0 ) {
      // Show the full table and plot
      console.log( "Received " + results.length + " results" );
      var tDiff = Date.now() - lastClick;
      updateStatus( "Received: " + results[results.length-1].Mean.toFixed(7) 
          + " +/- " + results[results.length-1].StdError.toFixed(7) 
          +  " after " + tDiff + "ms" + " for " + results[results.length-1].Samples + " points" );
      updateTable( results );
      updatePlot( results );
    } else {
      // Simulation stopped before any data received
      updateStatus( "Stopped" );
    }
    updateControls_SimStopped();
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

function updateModels( models ) {
  // Add model names to listbox
  var modelList = d3.select("#model");
  var options = modelList.selectAll("option")
    .data( models ).enter().append("option")
    .text( function (d) {return d; });
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

function updatePlot( res ){
  d3.select("#plot svg").remove();
  var w=320, h=200;
  var x = d3.scale.linear().domain([0, res[res.length-1].Samples]).range( [0, w]);
  var y = d3.scale.linear().domain([0, 1]).range([0, h]);
  var chart = d3.select("#plot").append("svg").attr("width", w).attr("height", h);
  var rect = chart.selectAll("rect").data( res );
  var barWidth = 10;
  rect.enter()
    .append("rect")
    .attr("x", function (d,i) { return x(d.Samples) - barWidth;})
    .attr("y", function (d,i) { return h - y(d.Mean + d.StdError/2); })
    .attr("width", function (d,i) { return barWidth; })
    .attr("height", function (d,i) { return Math.max( y( d.StdError ), 1); })
    .style("fill", "steelblue");
  chart.append("line").attr("x1", 0).attr("x2",w).attr("y1",h).attr("y2",h).style("stroke","#000000");
  chart.append("line").attr("x1", 0).attr("x2",0).attr("y1",h).attr("y2",0).style("stroke","#000000");
}

function updateControls_SimRunning() {
  d3.select("#results table").remove(); // Remove old data
  d3.select("#plot svg").remove(); // Remove old data
  var go = document.getElementById( "go" );
  var nPts = document.getElementById( "nPts" );
  var model = document.getElementById( "model" );
  var stop = document.getElementById( "stop" );
  go.disabled = true;
  nPts.disabled = true;
  model.disabled = true;
  stop.disabled = false;
}

function updateControls_SimStopped() {
  var go = document.getElementById( "go" );
  var nPts = document.getElementById( "nPts" );
  var model = document.getElementById( "model" );
  var stop = document.getElementById( "stop" );
  go.disabled = false;
  nPts.disabled = false;
  model.disabled = false;
  stop.disabled = true;
}
