# Platform

The images in this article with the suffix "*.drawio.png" can be edited with "Draw.io".
https://marketplace.visualstudio.com/items?itemName=hediet.vscode-drawio

## Workflow

- ESPHome connects directly to HA via WiFi.
- A 12V lead-acid battery is charged by connecting a relay to a 220V to 12V charger. When the 12V bus comes out, current and voltage sensors are first connected to determine whether the battery is in a charging or discharging state.
- A 12V to 5V power module supplies power to ESPHome and liquefied gas sensors.
- A 12V lead-acid battery can be connected to a discharge relay, a 12V to 220V inverter, and a 220V load for external discharge.
- Set up an automation scenario on the HA server to control ESPHome for timed charge and discharge control. ESPHome collects battery voltage and current locally and uploads them to ESPHome, and determines if there are any abnormalities such as overvoltage or undervoltage. If any abnormalities occur, the local system automatically cuts off charge and discharge.

## How to realize

1. Open HA and goto Setings - Automation and scene.
2. Click "creat new automation".
3. Refer to these yaml documents to realize two charge and two discharge:
    - [ores_charge1.yaml](/Reference_Implementation/Platform/ores_charge1.yaml)
    - [ores_discharge1.yaml](/Reference_Implementation/Platform/ores_discharge1.yaml)
    - [ores_charge2.yaml](/Reference_Implementation/Platform/ores_charge2.yaml)
    - [ores_discharge2.yaml](/Reference_Implementation/Platform/ores_discharge2.yaml)

## Physical Drawing

![Physical_Drawing](/Reference_Implementation/Platform/assets/Physical_Drawing.drawio.png)
