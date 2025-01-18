# Initiative and Planning for 2025

Continue to design and develop the Open Renewable Energy System standard architecture, extensible Application Programming Interfaces (API), and Protocol standards with the reference implementation. The planned deliverables include an ORES brand white box AC Battery ESS, disaggregated and plug-and-play renewable energy demonstration system with third part devices, architecture and design documents. Including specifically:

- **MVP - Minimum Viable Product**
  - **White Box ESS** (Energy Storage System): **ORES brand AC-Battery**. In addition to the general features, its North Bound (AC IN) can easily plug into 110V - 240V single or splite phase power socket or connect directly to micro inverter with solar panels. Its South Bound (AC OUT) should have 110V socket for loads, like refrigerators, to plug in. In the future the white box could have a different model, different modular configuration from different vendors, and upgrade revisions.
  - **Protocol Adapter Dangle**: with (ESPHome) MCU hardware and firmware for protocol adaption. One side with RS-485 Modbus (and BMS, dry contact, etc.) and the other side with WiFi for the device to communicate with the controller.
  - Controller: Raspberry PI (with Home Assistant), **Software Defined EMS**  

- Disaggregated and Plug-and-Play **Demonstration System** Trials
  - Plug-and-play ORES compliant devices and systems that work in North American and European regions.
  - The demonstration system: includes solar panel, battery, inverter, charger, and control component (BMS+ wifi communication protocol adaption), with third party devices, operation with monitoring app (Home Assistant).
  - Exhibits plug and play, accommodate on/off grid use cases, and **multi-vendor support** (open source version of the Balcony system). 
  - Support battery backup appliances, water heat pump, etc.
  - Support various **application scenario**: solar power storage, tariff.
  - The system capacity should be scalable in the number of panels and battery storage.
  - The system should be meeting each region’s **safety** standards. 

- Standardization and Ecosystem Development:
  - Software Defined ATS, UL lab certificate. 
  - **Electrical Rack Cabinet** standard (Open19), structural, cable, batteries, telemetry information, environmental, safety, design certifcate, etc.
  - Multiple hardware vendor support of hardware and software.

- Architecture discussion and white composition effort for vision promotion and explore VPP implementation with potential utility company’s involvement.
  - Smart Home Grid initiative: distributed devices, smart centralized controller, **container like home grid electrical cabinet**, **drop and play**, no installation needed, better DIY potential.
  - **Software Defined EMS**; Software-defined power (SDP) systems: Protection, Automation, and Control (PAC) .
