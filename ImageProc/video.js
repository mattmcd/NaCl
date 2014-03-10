function pageDidLoad() {
  var video = document.getElementById("live");
  var display = document.getElementById("display");
  var context = display.getContext("2d");

  navigator.webkitGetUserMedia(
    {video:true, audio:false}, 
    function (stream) {
      video.src = window.URL.createObjectURL(stream);
      draw( video, context);
    },
    function (err) {
      console.log("Unable to get media stream:" + err.Code );
    });
}

function draw(v,c) {
  c.drawImage(v, 0, 0);
  setTimeout( draw, 20, v, c); // Redraw every 20ms
}
