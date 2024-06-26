# Platform

## Workflow

- Liquefied gas sensors are directly connected to HA via WiFi
- ESPHome also connects directly to HA via WiFi
- A 12V lead-acid battery is charged by connecting a relay to a 220V to 12V charger. When the 12V bus comes out, current and voltage sensors are first connected to determine whether the battery is in a charging or discharging state
- A 12V to 5V power module supplies power to ESPHome and liquefied gas sensors
- A 12V lead-acid battery can be connected to a discharge relay, a 12V to 220V inverter, and a 220V load for external discharge
- Set up an automation scenario on the HA server to control ESPHome for timed charge and discharge control. ESPHome collects battery voltage and current locally and uploads them to ESPHome, and determines if there are any abnormalities such as overvoltage or undervoltage. If any abnormalities occur, the local system automatically cuts off charge and discharge
- If the liquefied gas sensor detects gas leakage exceeding the threshold, it will automatically turn off the battery charging and discharging relay through HA, achieving self-protection

## Protection logic

- When the battery voltage exceeds 15V, it is judged as the end of charging. Regardless of whether the current time period is in the charging time period, it stops and charging is prohibited
- When the battery voltage is below 10.5V, stop and prohibit discharge. Only when the voltage rises to 11.0V can discharge be enabled
- When liquefied gas sensor alarm and smoke sensor alarm are detected, stop and prohibit charging and discharging. Wait for manual reset before enabling charging and discharging
- When low battery voltage is detected, automatic charging is turned on to replenish power, preventing the battery from over discharging

## Protect Framework

Protect Framework schematic diagram:

![Protect_Framework](/Reference_Implementation/Platform/assets/Protect_Framework.drawio.png)

## Physical Drawing

![Physical_Drawing](/Reference_Implementation/Platform/assets/Physical_Drawing.drawio.png)

