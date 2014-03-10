function pageDidLoad() {
  video = document.getElementById("live");
  var ctx;
  navigator.webkitGetUserMedia(
    {video:true, audio:false}, 
    function (stream) {
      video.src = window.URL.createObjectURL(stream);
    },
    function (err) {
      console.log("Unable to get media stream:" + err.Code );
    });
}
