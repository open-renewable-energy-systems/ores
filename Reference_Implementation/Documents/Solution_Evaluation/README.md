# Vendors' Kits Evaluation

## SunGoldPower, 2024-08-23

1. **The Kit**
   - Off-Grid Solar Kit 8000W 48VDC 120V/240V LifePo4 10.24KWH Lithium Battery 8 X 415 Watts Solar Panels
   - https://sungoldpower.com/products/off-grid-solar-kit-8000w-48vdc-120v-240v-lifepo4-10-24kwh-lithium-battery-8-x-415-watts-solar-panels-sgr-8k10e?gad_source=1&gclid=Cj0KCQjwsuSzBhCLARIsAIcdLm6sxmwi9g3GgTIcpTyADAkzXtL3KBy4b-EfUvwdEKpPcPJ3DVqQ4wYaAqNPEALw_wcB

2. **Installation**
   - ![System Diagram](./kit_evaluation_sungoldpower.png)

   - Follow the YouTube video: https://www.youtube.com/watch?v=EO_ngtckK24
     - 08 L16   # Battery Type: 16 series of LFP batteries (USER, SLD, FLD, GEL, N13/N14, NOb)
     - 32 485   # Communication Function: 485 (SLA, CAN)
     - 33 PYL   # BMS Communication: Pylontech (PACE, Ruida, Aoguan, Oliter, Sunwoda, Daqin, SRNE, Vilion)

   - In addition, Setting Change to Fix the issue of "Inadequate outlet charging”: 
     - 63 ENA # Auto N-PE Connection Switch Function: ENA Enable (Disable)

   - For Information:
     - 68 AC output phase mode # 180 (default) represents the split-phase mode
   
3. **EV Charging**
   - Mostly regulated by the residential 12A/32A current max for Tesla Model 3.
     - One phase 120V is about 12A/1kW charging power.
     - Split phase 240V is about charging 32A current / 7kW power limit.

4. **Brief Cost** (USD)
   - Kit: $6000
   - Accessories: $1000
   - Installation: $2000
   - License and Permit application if using grid AC input for charge: $1000 (and the procedure takes time)  
   - Total: $7900 if considering 30% tax return for hardware purchase

5. **Initial Value Proposition Analysis**
   - For ESS (Energy Storage System), two batteries (48VDC 100AH each) can hold up to 10kWH energy with an active capacity of 8kWH.
   - Solar power generates an average at 5 hours per day, varying from 0-3 hours during winter to 6-8 hours during summer，for a total of about 15kWH per day. Therefore, solar power can fully charge the ESS and provide an extra supply for home usage during summer, but may not be enough to charge the ESS during winter or rainy seasons.
   - Energy Generation per Day：3kW x 5Hours = 15kWH
   - Value Generated per Day: 15kWH x $0.51/kWH = $7.6
   - Value Generated per Year: $7.6 x 365 = $2800
   - About **3 years** ROI (Return Of Investment).


## SunGoldPower #2, 2025-01-30

1. **The Kit**
   - Off-Grid Solar Kit 10kW 48VDC 120V/240V LifePo4 25KWH Lithium Battery 2 x 6 x 415 Watts Solar Panels

2. **The Cabinet**
   - https://www.currentconnected.com/product/sok-12-slot-outdoor-battery-rack/?srsltid=AfmBOop1aOMCDlqQGGd4roljEBWLIODLcKApVv-zM0UxMrz0fTLQPt6Q

3. **Cooling Fan & Controller**
   - PWM FAN https://www.amazon.com/FFC1248DE-12038-Double-Blower-Cooling/dp/B0BGSGP6HB/ref=sr_1_8?crid=3DGMD1AYR5XL1&dib=eyJ2IjoiMSJ9.E6cl0r3dylObChQQBbFvRx2wEC7tBt_bRMtuSoV-cTgITWyABrWRdpQInyl3MHfnKn_vgJC5ahuvqRdV4Wq3n3y6WECMioOV94T9EO8RcZuNwRmV40IbuOH1LG6RSrejhvKEx2n8_kJaa-s7peufm5e3aDoFsvDv6bAdJKpJlaSlq2kATKBocVUqIxnrI1NdwWIBgkYP0xkPelZQvRA5s7PZxEbghyghn1qV2slaDsvTKMtI7crJjDnq5m34H9PeBaZ_IcHiTzM1er-rA1tP0hnKnlT6owGTCUwy0kTWWPbJu-4Z--UKzUVjpxtiDlqW4hBc1kNvMchqCaa6cZPCsVG3rt6V486BPFBnlRTq0l8.QNBnKppg_vp9t_X9FoUIOqVaXA8wzE6tyhqeAtKIIUM&dib_tag=se&keywords=%EF%BC%94%EF%BC%98%EF%BD%96+%EF%BD%90%EF%BD%97%EF%BD%8D+%EF%BD%86%EF%BD%81%EF%BD%8E&qid=1738703372&s=electronics&sprefix=%EF%BC%94%EF%BC%98%EF%BD%96+%EF%BD%90%EF%BD%97%EF%BD%8D+%EF%BD%86%EF%BD%81%EF%BD%8E%2Celectronics%2C125&sr=1-8
   - CONTROLLER https://www.amazon.com/Channel-Temperature-Controller-Governor-Display/dp/B083R9FY8S/ref=sr_1_5?crid=1EI9J6L2E57UI&dib=eyJ2IjoiMSJ9.H4jbk5JferNmCZgnK7iEV_vpw_SToYeXcH9Q5_19qVGLuFOEp-vVaJZXj6MPqRxKycCO9UtmSEcn7-XtZxGxkwP77Id4hPgrwwYWktHz9FTskGl3WmQzJVkpsCmad8Nac2BBB7y9a6C4OiCv4YrFvGxL-_cA233oXjlKbflsrigMcoj9IYUUhRBFWJ5edZsoQ2JC1wWLxasNGOLuZwvanEL75Ohbqi1o-PSX3jbgmk8.o5RPRXZ7eMHs2xBlnZB-QNYsVlKj045xh43qEhWB-L0&dib_tag=se&keywords=dc+48v+temperature+controller&qid=1738214322&sprefix=dc+48v+temperature+controller%2Caps%2C197&sr=8-5
   
## Enphase，(To Do)  

- https://enphase.com/homeowners
