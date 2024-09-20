const express = require("express");
const http = require("http");
const { SerialPort, ReadlineParser } = require("serialport");
const socketIo = require("socket.io");

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

const serialPort = new SerialPort({
  path: "COM1",
  baudRate: 9600,
});

const parser = serialPort.pipe(new ReadlineParser({ delimiter: "\n" }));

app.use(express.static("public"));

io.on("connection", (socket) => {
  console.log("Usuario conectado");

  socket.on("sendKey", (key) => {
    serialPort.write(`${key}\n`);
  });

  socket.on("setThreshold", (threshold) => {
    serialPort.write(`THRESHOLD:${threshold}\n`);
  });

  socket.on("disconnect", () => {
    console.log("Usuario desconectado");
  });
});

parser.on("data", (data) => {
  console.log("Datos recibidos desde Arduino:", data);

  io.emit("arduinoData", data);
});

server.listen(3000, () => {
  console.log("Servidor escuchando en http://localhost:3000");
});
