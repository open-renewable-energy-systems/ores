# Platform

## Protection logic

- Liquefied gas sensors are directly connected to HA via WiFi
- If the liquefied gas sensor detects gas leakage exceeding the threshold, it will automatically turn off the battery charging and discharging relay through HA, achieving self-protection
- When the battery voltage exceeds 15V, it is judged as the end of charging. Regardless of whether the current time period is in the charging time period, it stops and charging is prohibited
- When the battery voltage is below 10.5V, stop and prohibit discharge. Only when the voltage rises to 11.0V can discharge be enabled
- When liquefied gas sensor alarm and smoke sensor alarm are detected, stop and prohibit charging and discharging. Wait for manual reset before enabling charging and discharging
- When low battery voltage is detected, automatic charging is turned on to replenish power, preventing the battery from over discharging

## Protect Framework

Protect Framework schematic diagram:

![Protect_Framework](/Reference_Implementation/Platform/assets/Protect_Framework.drawio.png)
