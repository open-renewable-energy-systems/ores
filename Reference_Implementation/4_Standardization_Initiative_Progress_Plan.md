# Standardization Reference Implementation Initiative, Progress & Plan

![Standardization](./images/HomeGrid_Standardization_Initiative.png)

1. **Power Plane / Home Grid & Smart Panel**
  - Grid tie system with **current source** type device (classic grid following) feature.
  - Off grid system with **voltage source** type device (cutting edge grid forming) feature.
  - **Switch and transition** between grid tie and off grid mode.
    - The grid charges the off grid system only.
    - Manual switch between grid tie and off grid.
 
2. **Communication Protocols & Devices Integration** 
 - Develop adapter code to integrate various new energy devices.
 
3. **Application Automation & Software Defined EMS**
  - Support various flexibility patterns and control types

4. **AC Battery Technology Based Energy Router** 
  - Hardware Interface between Power Conversion Module and Control Plane.
  - Software Interface between Cascaded PCS/SMB and PCS Control System.
  - Equalization by Cylinder, Module, and Cell.

# Progress - Run Separate Components Relatively Independent

1. **MicroInverter** 

![MicroInverter](./images/HomeGrid_Standardization_Progress_1_MicroInverter.png)
  
2. **Grid Tie**  

![GridTie](./images/HomeGrid_Standardization_Progress_2_GridTie.png)
 
3. **Off Grid Hybrid** 

![OffGrid](./images/HomeGrid_Standardization_Progress_3_OffGrid_Hybrid.png)

4. **Software Defined EMS**  

![EMS](./images/HomeGrid_Standardization_Progress_4_EMS.png)
  
# Plan - Integrate Off Grid and Grid Tie Together

5. **Start from Smart Panel** 

![Smart Panel](./images/HomeGrid_Standardization_Plan_5_SmartPanel.png)
   - Manual Switch
   - By Software Defined EMS
