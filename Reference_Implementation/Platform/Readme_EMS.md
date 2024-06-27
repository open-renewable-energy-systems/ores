# Platform

## Workflow

- ESPHome connects directly to HA via WiFi
- A 12V lead-acid battery is charged by connecting a relay to a 220V to 12V charger. When the 12V bus comes out, current and voltage sensors are first connected to determine whether the battery is in a charging or discharging state
- A 12V to 5V power module supplies power to ESPHome and liquefied gas sensors
- A 12V lead-acid battery can be connected to a discharge relay, a 12V to 220V inverter, and a 220V load for external discharge
- Set up an automation scenario on the HA server to control ESPHome for timed charge and discharge control. ESPHome collects battery voltage and current locally and uploads them to ESPHome, and determines if there are any abnormalities such as overvoltage or undervoltage. If any abnormalities occur, the local system automatically cuts off charge and discharge

## Physical Drawing

![Physical_Drawing](/Reference_Implementation/Platform/assets/Physical_Drawing.drawio.png)
