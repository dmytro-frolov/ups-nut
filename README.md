# ESP32-C3 GPIO NUT server

This project turns a simple "dumb" UPS into a network UPS source for NAS and
homelab servers.

Many cheap UPS units have no USB data interface, but they still expose useful
state through front-panel LEDs. An ESP32-C3 reads those LED states through GPIO
inputs and publishes the result as a small Network UPS Tools-compatible TCP
server over Wi-Fi.

Tested with a QNAP NAS in `Network UPS slave` mode and an EnerGenie
EG-UPS-B650-style LED mapping.

## How it works

```text
                AC mains
                   |
                  UPS
                   |
        +----------+----------+
        |          |          |
      router      NAS     ESP32-C3
                              ^
                              |
UPS LED signals -> optocouplers -> GPIO pins
                              |
                              v
                    Wi-Fi NUT server
                    tcp/3493, UPS: qnapups
                              |
                              v
                    NAS or another NUT client
```

The ESP32 does not control the UPS. It only observes the UPS state and reports
it over the network:

- green LED means line power is available: `OL`
- red LED means the UPS is on battery: `OB DISCHRG`
- green + yellow means line power is available and charging: `OL CHRG`

When QNAP sees `OB DISCHRG`, it can enter auto-protection or shut down after the
configured delay.

## Safety

Do not connect ESP32 GPIO directly to unknown UPS circuitry.

Use optocouplers such as `PC817`, or read the LEDs optically, so the ESP32 side
stays isolated from the UPS electronics.

Typical ESP32-side input:

```text
ESP32 3.3V --- 10k --- GPIO
                  |
             PC817 transistor
                  |
                 GND
```

The firmware uses `INPUT_PULLUP`, so an active optocoupler pulls the GPIO to
`GND`.

## Firmware

- a NUT-like TCP server on port `3493`
- a simple HTTP JSON endpoint on `/status`

## Pins

Pins use `INPUT_PULLUP`. Short a pin to `GND` to activate it.

| GPIO | Simulated UPS LED | NUT status effect |
| --- | --- | --- |
| `GPIO4` | Green | `OL` |
| `GPIO5` | Yellow | `OFF`, or `OL CHRG` together with green |
| `GPIO6` | Red | `OB DISCHRG` |

Observed EnerGenie EG-UPS-B650 mapping:

| UPS LEDs | Meaning | NUT status |
| --- | --- | --- |
| Green on | Input power is present, battery not charging | `OL` |
| Green on, yellow blinking/on | Input power is present, battery charging | `OL CHRG` |
| Red on, beeping | Input power is lost, running on battery | `OB DISCHRG` |
| Yellow on, green off after pressing UPS power button | UPS output appears off | `OFF` |

For a minimum useful setup, connect only the red LED state. That is enough to
tell clients that AC power has failed. Green and yellow improve the status
display but are not required for timed shutdown.

## QNAP setup

In QNAP Control Panel, choose:

```text
External Device -> UPS -> Network UPS slave
```

Use the ESP32 IP address as the network UPS server.

The UPS name used by this firmware is:

```text
qnapups
```

QNAP does not ask for the name in all firmware versions, but internally it
queries `qnapups`.

Because this UPS has no real battery telemetry, shutdown should be based on a
time delay after AC failure, not on a true low-battery signal.

## Configure Wi-Fi

```sh
cp src/secrets.h.example src/secrets.h
```

Edit `src/secrets.h`:

```cpp
#define WIFI_SSID "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"
```

## Build and upload

Install PlatformIO first. On macOS the easiest options are:

- VS Code + PlatformIO IDE extension
- or CLI via Homebrew/Python, then use `pio` in this folder

If you use Homebrew:

```sh
brew install platformio
```

If Homebrew does not have it on your machine:

```sh
python3 -m pip install --user platformio
```

Connect the ESP32-C3 by USB, then check the serial port:

```sh
pio device list
```

Upload:

```sh
pio run -t upload
```

Open serial monitor:

```sh
pio device monitor
```

The serial monitor prints the ESP32 IP address.

## Test from a Mac

HTTP status, usable from Postman:

```text
GET http://ESP32_IP/status
```

NUT protocol smoke test:

```sh
nc ESP32_IP 3493
```

Then type:

```text
LIST UPS
GET VAR qnapups ups.status
LIST VAR qnapups
LOGOUT
```

If NUT tools are installed on the Mac:

```sh
upsc qnapups@ESP32_IP ups.status
upsc qnapups@ESP32_IP
```
