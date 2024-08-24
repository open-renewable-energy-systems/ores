# Vendors' Kits Evaluation

## SunGoldPower, 2024-08-23

1. **The kit**
   - Off-Grid Solar Kit 8000W 48VDC 120V/240V LifePo4 10.24KWH Lithium Battery 8 X 415 Watts Solar Panels
   - https://sungoldpower.com/products/off-grid-solar-kit-8000w-48vdc-120v-240v-lifepo4-10-24kwh-lithium-battery-8-x-415-watts-solar-panels-sgr-8k10e?gad_source=1&gclid=Cj0KCQjwsuSzBhCLARIsAIcdLm6sxmwi9g3GgTIcpTyADAkzXtL3KBy4b-EfUvwdEKpPcPJ3DVqQ4wYaAqNPEALw_wcB

2. **Installation**
   - ![System Diagram](./kit_evaluation_sungoldpower.png)

   - Follow exactly the youtube video: https://www.youtube.com/watch?v=EO_ngtckK24
     - 08 L16   # Battery type: 16 series of LFP batteries (USER, SLD, FLD, GEL, N13/N14, NOb)
     - 32 485   # communication function: 485 (SLA, CAN)
     - 33 PYL   # BMS communication: Pylontech (PACE, Ruida, Aoguan, Oliter, Sunwoda, Daqin, SRNE, Vilion)

   - The only additional setting change to fix the "Inadequate outlet charging” issue: 
     - 63 ENA # Auto N-PE connection switch function: ENA Enable (Disable)

   - For information
     - 68 AC output phase mode # 180 (default) represents the split-phase mode
   
3. **EV Charging**
   - It is mostly regulated by the residential 12A current max for Tesla Model 3.
     - For one phase 120V, it is about 1kW charging power.

4. **Brief Cost** (USD)
   - The kit: 6000$
   - Accessories: 1000$
   - Installation: 2000$
   - License and Permit application if use grid AC input for charge: 1000$ (takes time)  
   - Total: 7900$ if consider tax return 30% hardware purchase

5. **Initial Value Proposition Analysis**
   - Solar power generation 5 sun hour average, winter 0-3, summer 6-8, so average energy is about 3x5=15kWH per day.For ESS, 2 batteries, 48vdc 100AH each, total 48 x 200 = 10kwh, active capacity is 8Kwh. So it could be full charged and provide extra home usage supply during summer. But may not be able to charge the ESS full during winter and/or raining.
   - Energy generation per day： 3kW x 5Hours = 15 kWH
   - The value generated per day: 15kWH x 0.51$/kWH = 7.6$
   - The value generated per year: 7.6$ x 365 = 2800$
   - So it shoud take about **3 years** ROI (Return Of Investment) .

## Enphase， todo 

- https://enphase.com/homeowners
