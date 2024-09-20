const socket = io();

function sendKey() {
  const key = document.getElementById("keyInput").value;
  socket.emit("sendKey", key);
}

function setThreshold() {
  const threshold = document.getElementById("thresholdInput").value;
  socket.emit("setThreshold", threshold);
}

socket.on("arduinoData", (data) => {
  document.getElementById("data").textContent = "Datos de Arduino: " + data;
});
