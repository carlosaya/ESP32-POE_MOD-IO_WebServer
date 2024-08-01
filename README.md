# ESP32-POE_MOD-IO_WebServer
Simple web server for Olimex ESP32-POE to enable/disable relays on an attached MOD-IO board.

`Note:  Line 19 requires updating with the byte-encoded MAC address of your ESP32-POE.`

Originally based upon [ESP32_PoE_WebServer_MOD_IO.ino](https://github.com/OLIMEX/ESP32-POE/blob/4054ce5d80dcf66261ed52aa4d99f8d5f8ca1f5c/SOFTWARE/ARDUINO/ESP32_PoE_WebServer_MOD_IO/).

## Features
- Uses the wired LAN interface instead of WiFi like the official Olimex example linked above.
- Web interface with tick-boxes for manually enabling and disabling each relay individually from any browser
- Trigger relays via form submission for use in automation software (i.e., HomeAssistant)
    - Request Method: `POST`
    - Payload (X is any relay between 1 and 4): 
        - `EnableRelay=X` to enable a relay
        - `DisableRelay=X` to disable a relay
    - Content Type: `application/x-www-form-urlencoded`
