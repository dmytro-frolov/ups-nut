# ESP32-C3 GPIO NUT server

Minimal Wi-Fi test firmware for an ESP32-C3 that exposes GPIO state as:

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
