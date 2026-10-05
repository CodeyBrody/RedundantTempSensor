| ID         | Requirement / Behavior           | Test Level    | Method            | Priority |
| ---------- | -------------------------------- | ------------- | ----------------- | -------- |
| BUILD-001  | Firmware Successfully Build      | System        | Attempt Build     | High     |
| BASE-001   | Successful Normal Startup        | System        | Attemp Startup    | High     |
| TEMP-001   | Average valid sensor readings    | Unit          | C test            | High     |
| TEMP-002   | Ignore faulted sensors           | Unit          | C test            | High     |
| TEMP-003   | Detect temperature out of bounds | Unit          | C test            | High     |
| TEMP-004   | Detect sensor outlier            | Unit          | C test            | High     |
| TEMP-005   | Handle all sensors invalid       | Unit/System   | C + hardware      | High     |
| SENSOR-001 | Read valid TMP102                | Hardware      | Real sensor       | High     |
| SENSOR-002 | Handle I²C failure               | Hardware      | Disconnect sensor | High     |
| SENSOR-003 | Retry failed I²C transaction     | Unit/Hardware | Mock + hardware   | Medium   |
| RTC-001    | Accept valid date/time           | Unit          | C test            | High     |
| RTC-002    | Reject malformed date/time       | Unit          | C test            | High     |
| UART-001   | Receive complete message         | Unit/System   | UART              | High     |
| UART-002   | Reject oversized message         | Unit/System   | UART              | High     |
| UART-003   | Reject second pending message    | System        | UART              | High     |
| ALERT-001  | Fault produces correct LED       | Hardware      | Visual            | Medium   |
| ALERT-002  | New fault activates buzzer       | Hardware      | Physical          | Medium   |
| WD-001     | Watchdog resets system           | Hardware      | Fault injection   | Medium   |
