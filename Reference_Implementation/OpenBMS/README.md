2026-01-17

For active load balancing, refer to https://github.com/DoganM95/CN3302-ETA3000-2S-Charger-Balancer

## Brief Analysis of Libre BMS Firmware *
- main: the main thread mainly responsible for core Battery Management System functionality.
- data_objects: ThingSet implementation for handling external communication and data mapping.
- bms.h: BMS high-level API with the context information struct bms_context.
- bms_ic.h: API for BMS front-end ICs with struct bms_ic_driver_api.
- button: this device/driver implements 3-second long-press detection using interrupts.
- leds: this device/driver manages charge/discharge status indicators via a dedicated thread.
- oled: this device/driver handles real-time data visualization via a dedicated thread.
