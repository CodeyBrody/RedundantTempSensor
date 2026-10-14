const { SerialPort } = require("serialport");

const port = new SerialPort({
  path: "COM3",
  baudRate: 115200,
});

port.on("open", () => {
  console.log("Port opened successfully.");
});

port.on("error", (err) => {
  console.error("Serial error:", err.message);
});
