const { SerialPort } = require("serialport");

let received = "";
let stringUnderTest = "SEND_TIME";

const port = new SerialPort({
  path: "COM3",
  baudRate: 115200,
});

port.on("open", () => {
  console.log("Port successfully opened.");

  port.write("SET_TIME\n");
});

port.on("data", (data) => {
  received += data.toString();
  if (received.includes(stringUnderTest)) {
    console.log(`Found expected response: ${stringUnderTest}`);
    received = "";
  }
});

port.on("error", (err) => {
  console.error("Serial error:", err.message);
});
