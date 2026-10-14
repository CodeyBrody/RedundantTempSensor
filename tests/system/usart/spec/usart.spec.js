const { SerialPort } = require("serialport");

describe("USART system tests", () => {
  let sent_string = "";
  let expected_string = "";
  let port;

  beforeEach(async () => {
    port = new SerialPort({
      path: process.env.RTS_PORT || "COM3",
      baudRate: 115200,
      autoOpen: false,
    });

    await new Promise((resolve, reject) => {
      port.open((err) => {
        if (err) reject(err);
        else resolve();
      });
    });

    // Establish a known RTC state before each test.
    const responsePromise = waitForText(port, "SEND_TIME\r\n", 3000);

    await writeAndDrain(port, "SET_TIME\n");
    await responsePromise;

    // Complete the date/time exchange.
    const dataPromise = waitForText(port, "D: 10/12/2026 09:30:", 10000);

    await writeAndDrain(port, "2026/10/12 09:30:00\n");
    await dataPromise;
  });

  afterEach(async () => {
    if (port?.isOpen) {
      await new Promise((resolve) => {
        port.close(() => resolve());
      });
    }
  });

  it("accepts valid SET_TIME command terminated by \\r\\n", async () => {
    sent_string = "SET_TIME\r\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const received = await responsePromise;

    expect(received).toContain(expected_string);
  });

  it("accepts valid SET_TIME command terminated by \\n", async () => {
    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const secondResponsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const received = await secondResponsePromise;

    expect(received).toContain(expected_string);
  });

  for (const terminator of ["\n", "\r\n"]) {
    it(`accepts valid date/time terminated by ${JSON.stringify(terminator)}`, async () => {
      sent_string = "SET_TIME" + terminator;
      expected_string = "SEND_TIME\r\n";

      const responsePromise = waitForText(port, expected_string, 3000);

      await writeAndDrain(port, sent_string);
      await responsePromise;

      sent_string = "2026/10/13 14:22:00" + terminator;
      expected_string = "D: 10/13/2026 14:22:";

      const logPromise = waitForText(port, expected_string, 10000);
      await writeAndDrain(port, sent_string);
      const received = await logPromise;

      expect(received).toContain(expected_string);

      // Send another command after the RTC was updated.
      sent_string = "SET_TIME\n";
      expected_string = "SEND_TIME\r\n";

      const nextResponsePromise = waitForText(port, expected_string, 3000);

      await writeAndDrain(port, sent_string);

      const nextReceived = await nextResponsePromise;

      // Verify the command was recognized.
      expect(nextReceived).toContain(expected_string);

      // Verify it wasn't mistakenly processed as invalid RTC data.
      expect(nextReceived).not.toContain("Expected data to set RTC");
    });
  }

  it("rejects malformed command and accepts the next valid command", async () => {
    // Send a malformed command...
    sent_string = "BAD_COMMAND\n";
    expected_string = "Received: BAD_COMMAND";

    const errorPromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const receivedError = await errorPromise;

    expect(receivedError).toContain("Unrecognized command received via USART.");
    expect(receivedError).toContain(expected_string);

    // Send a valid command next...
    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const receivedResponse = await responsePromise;

    expect(receivedResponse).toContain(expected_string);
  });

  it("rejects malformed RTC data and accepts corrected data", async () => {
    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);
    await responsePromise;

    sent_string = "2026/10/13 14-22-00\n";

    const formattingError =
      "Expected data to set RTC, but received data incorrectly formatted to do so.";

    const retryPromise = waitForAllText(
      port,
      [formattingError, "Received: 2026/10/13 14-22-00", "SEND_TIME\r\n"],
      3000,
    );

    await writeAndDrain(port, sent_string);

    const errorOutput = await retryPromise;

    expect(errorOutput).toContain(formattingError);
    expect(errorOutput).toContain("SEND_TIME");

    sent_string = "2026/10/14 11:40:00\n";
    expected_string = "D: 10/14/2026 11:40:";

    const logPromise = waitForText(port, expected_string, 10000);

    await writeAndDrain(port, sent_string);

    const received = await logPromise;

    expect(received).toContain(expected_string);
  });

  it("rejects invalid RTC data and accepts corrected data", async () => {
    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);
    await responsePromise;

    sent_string = "2026/10/13 24:22:00\n";

    const invalidDateTimeError =
      "An invalid date/time was received as input to set the RTC.";

    const retryPromise = waitForAllText(
      port,
      [invalidDateTimeError, "Received: 2026/10/13 24:22:00", "SEND_TIME\r\n"],
      3000,
    );

    await writeAndDrain(port, sent_string);

    const errorOutput = await retryPromise;

    expect(errorOutput).toContain(invalidDateTimeError);
    expect(errorOutput).toContain("SEND_TIME");

    sent_string = "2026/10/14 11:40:00\n";
    expected_string = "D: 10/14/2026 11:40:";

    const logPromise = waitForText(port, expected_string, 10000);

    await writeAndDrain(port, sent_string);

    const received = await logPromise;

    expect(received).toContain(expected_string);
  });

  it("rejects other commands when expecting RTC-data and accepts corrected data", async () => {
    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);
    await responsePromise;

    sent_string = "SET_TIME\n";

    const formattingError =
      "Expected data to set RTC, but received data incorrectly formatted to do so.";

    const retryPromise = waitForAllText(
      port,
      [formattingError, "Received: SET_TIME", "SEND_TIME\r\n"],
      3000,
    );

    await writeAndDrain(port, sent_string);

    const errorOutput = await retryPromise;

    expect(errorOutput).toContain(formattingError);
    expect(errorOutput).toContain("SEND_TIME");

    sent_string = "2026/10/14 11:40:00\n";
    expected_string = "D: 10/14/2026 11:40:";

    const logPromise = waitForText(port, expected_string, 10000);

    await writeAndDrain(port, sent_string);

    const received = await logPromise;

    expect(received).toContain(expected_string);
  });

  it("recovers after rapidly received commands", async () => {
    expected_string = "Unrecognized command received via USART.";
    sent_string = "BAD_A\nBAD_B\nBAD_C\n";

    const errorPromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const receivedError = await errorPromise;

    expect(receivedError).toContain(expected_string);

    // Allow the firmware to finish processing
    await new Promise((resolve) => setTimeout(resolve, 250));

    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const responsePromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const receivedResponse = await responsePromise;

    expect(receivedResponse).toContain(expected_string);
  });

  it("does not mix contents of consecutive commands", async () => {
    for (const command of ["BAD_A", "BAD_B"]) {
      sent_string = command + "\n";
      expected_string = `Received: ${command}\r\n`;

      const responsePromise = waitForText(port, expected_string, 3000);

      await writeAndDrain(port, sent_string);

      const received = await responsePromise;

      expect(received).toContain(expected_string);
    }
  });

  it("preserves a queued command when another message arrives", async () => {
    sent_string = "ORIGINAL_COMMAND\nOVERWRITE_ATTEMPT\n";
    expected_string = "Received: ORIGINAL_COMMAND";

    const bufferFullMessage =
      "The buffer is currently full of a command that has yet to be processed.";

    const outputPromise = waitForAllText(
      port,
      [expected_string, bufferFullMessage],
      3000,
    );

    await writeAndDrain(port, sent_string);
    const received = await outputPromise;

    expect(
      received.split(/\r?\n/).some((line) => line.endsWith(expected_string)),
    ).toBeTrue();

    expect(received).toContain(bufferFullMessage);

    // Allow the firmware to finish processing
    await new Promise((resolve) => setTimeout(resolve, 250));

    sent_string = "SET_TIME\n";
    expected_string = "SEND_TIME\r\n";

    const recoveryPromise = waitForText(port, expected_string, 3000);

    await writeAndDrain(port, sent_string);

    const recoveryResponse = await recoveryPromise;

    expect(recoveryResponse).toContain(expected_string);
    expect(recoveryResponse).not.toContain(
      "Unrecognized command received via USART.",
    );
  });

  it("preserves a queued command when a second message arrives that exceeds the buffer size", async () => {
    sent_string = "ORIGINAL_COMMAND\n" + "X".repeat(120) + "\n";
    expected_string = "Received: ORIGINAL_COMMAND";

    let capturedOutput = "";

    const onData = (data) => {
      capturedOutput += data.toString();
    };

    port.on("data", onData);

    try {
      await writeAndDrain(port, sent_string);

      await new Promise((resolve) => setTimeout(resolve, 1000));
    } finally {
      port.off("data", onData);
    }

    expect(
      capturedOutput
        .split(/\r?\n/)
        .some((line) => line.trimEnd().endsWith(expected_string)),
    ).toBeTrue();

    expect(capturedOutput).toMatch(
      /too many characters|currently full of a command/,
    );
  });

  for (const payloadLength of [100, 120]) {
    it(`reports overflow and recovers after ${payloadLength} payload characters`, async () => {
      sent_string = "X".repeat(payloadLength) + "\n";

      expect(sent_string.length).toBe(payloadLength + 1);

      expected_string =
        "The rxBuf received a message or command with too many characters.";

      const overflowPromise = waitForText(port, expected_string, 3000);

      await writeAndDrain(port, sent_string);

      const received = await overflowPromise;

      expect(received).toContain(expected_string);

      let recovered = false;
      for (let attempt = 0; attempt < 2; attempt++) {
        try {
          const responsePromise = waitForText(port, "SEND_TIME\r\n", 3000);

          await writeAndDrain(port, "SET_TIME\n");
          await responsePromise;

          const minute = 40 + attempt;
          const dateTime = `2026/10/16 11:${minute}:00`;
          const expectedLog = `D: 10/16/2026 11:${minute}:`;

          const logPromise = waitForText(port, expectedLog, 10000);

          await writeAndDrain(port, dateTime + "\n");

          const receivedLog = await logPromise;

          expect(receivedLog).toContain(expectedLog);

          recovered = true;
          break;
        } catch (error) {
          //An attempt may fail during recovery. Retry up to the defined limit. (Here currently 3)
        }
      }

      expect(recovered).toBeTrue();
    }, 45000);
  }

  it("handles messages correctly after overflow remainder", async () => {
    const overflowError =
      "The rxBuf received a message or command with too many characters.";

    const payload = 120;
    const remainingPayload = "X".repeat(payload % 100);

    const errorPromise = waitForAllText(
      port,
      [overflowError, `Received: ${remainingPayload}\r\n`],
      5000,
    );

    await writeAndDrain(port, "X".repeat(payload) + "\n");

    const errorOutput = await errorPromise;

    expect(
      errorOutput
        .split(/\r?\n/)
        .some((line) => line.endsWith(`Received: ${remainingPayload}`)),
    ).toBeTrue();

    for (const command of ["AFTER_OVERFLOW_1", "AFTER_OVERFLOW_2"]) {
      const expected = `Received: ${command}\r\n`;

      const responsePromise = waitForText(port, expected, 3000);

      await writeAndDrain(port, command + "\n");

      const received = await responsePromise;

      expect(received).toContain(expected);
    }
  }, 15000);

  it("accepts 100 characters including LF without overflow", async () => {
    const payload = "A".repeat(99);

    sent_string = payload + "\n";
    expected_string = `Received: ${payload}\r\n`;

    expect(sent_string.length).toBe(100);

    let capturedOutput = "";

    const onData = (data) => {
      capturedOutput += data.toString();
    };

    port.on("data", onData);

    try {
      const responsePromise = waitForText(port, expected_string, 3000);

      await writeAndDrain(port, sent_string);
      await responsePromise;

      // Capture any subsequent error reporting.
      await new Promise((resolve) => setTimeout(resolve, 250));
    } finally {
      port.off("data", onData);
    }

    // Complete message was received unchanged.
    expect(capturedOutput).toContain(expected_string);

    // No receive-buffer overflow occurred.
    expect(capturedOutput).not.toContain("too many characters");
  });

  it("accepts 101 characters including CRLF without overflow", async () => {
    const payload = "A".repeat(99);

    sent_string = payload + "\r\n";
    expected_string = `Received: ${payload}\r\n`;

    expect(sent_string.length).toBe(101);

    let capturedOutput = "";

    const onData = (data) => {
      capturedOutput += data.toString();
    };

    port.on("data", onData);

    try {
      const responsePromise = waitForText(port, expected_string, 3000);

      await writeAndDrain(port, sent_string);
      await responsePromise;

      // Capture any subsequent error reporting.
      await new Promise((resolve) => setTimeout(resolve, 250));
    } finally {
      port.off("data", onData);
    }

    // Complete message was received unchanged.
    expect(capturedOutput).toContain(expected_string);

    // No receive-buffer overflow occurred.
    expect(capturedOutput).not.toContain("too many characters");
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

async function writeAndDrain(port, message) {
  await new Promise((resolve, reject) => {
    port.write(message, (err) => {
      if (err) {
        reject(err);
        return;
      }

      port.drain((err) => {
        if (err) reject(err);
        else resolve();
      });
    });
  });
}

function waitForAllText(port, expectedTexts, timeoutMs) {
  return new Promise((resolve, reject) => {
    let received = "";

    const timeout = setTimeout(() => {
      port.off("data", onData);
      reject(new Error(`Timed out. Received: ${received}`));
    }, timeoutMs);

    function onData(data) {
      received += data.toString();

      if (expectedTexts.every((text) => received.includes(text))) {
        clearTimeout(timeout);
        port.off("data", onData);
        resolve(received);
      }
    }

    port.on("data", onData);
  });
}
