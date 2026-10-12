const { SerialPort } = require("serialport");

describe("USART system tests", () => {
  let sent_string = "";
  let expected_string = "";
  let port;

  beforeEach(async () => {
    port = new SerialPort({
      path: "COM3",
      baudRate: 115200,
      autoOpen: false,
    });

    await new Promise((resolve, reject) => {
      port.open((err) => {
        if (err) reject(err);
        else resolve();
      });
    });
  });

  afterEach(async () => {
    if (port?.isOpen) {
      await new Promise((resolve) => {
        port.close(() => resolve());
      });
    }
  });

  it("returns SEND_TIME", async () => {
    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    port.write(sent_string);

    const received = await responsePromise;

    expect(received).toContain(expected_string);
  });

  it("accepts valid commands terminated by \\n and \\r\\n", async () => {
    sent_string = "SET_TIME\r\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    port.write(sent_string);

    const received = await responsePromise;

    expect(received).toContain(expected_string);

    sent_string = "SET_TIME\n";

    const secondResponsePromise = waitForText(port, expected_string, 3000);

    port.write(sent_string);

    const secondReceived = await secondResponsePromise;

    expect(secondReceived).toContain(expected_string);
  });
});

function waitForText(port, expectedText, timeoutMs) {
  return new Promise((resolve, reject) => {
    let received = "";

    const timeout = setTimeout(() => {
      port.off("data", onData);

      reject(
        new Error(
          `Timed out waiting for "${expectedText}". Received: ${received}`,
        ),
      );
    }, timeoutMs);

    function onData(data) {
      received += data.toString();

      if (received.includes(expectedText)) {
        clearTimeout(timeout);
        port.off("data", onData);
        resolve(received);
      }
    }
    port.on("data", onData);
  });
}
