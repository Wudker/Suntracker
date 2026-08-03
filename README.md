# Suntracker – Solar-Tracking Powerbank with MPPT

<img src="Hardware/Model/Parts/main_zlozon.png" alt="Suntracker CAD model" width="700"/>

Suntracker is a design-stage solar-tracking powerbank developed as a mechatronic student project. It combines dual-axis panel positioning, Li-Ion battery charging, powerbank functionality and a simplified P&O-based MPPT concept. Photoresistors guide the automatic search for the optimal panel position, while the INA219 monitors photovoltaic voltage and current. An STM32 microcontroller controls the positioning mechanism, power rails and charging current through a digital potentiometer. The repository contains firmware, PCB and mechanical files, simulations, a BOM, documentation and validation calculations.

## Technologies

`STM32L051C8` `C/C++` `Photovoltaics` `P&O MPPT` `INA219` `MCP40xx` `TP4056` `XL4015` `DRV8833` `Li-Ion battery` `KiCad` `LTspice` `3D printing`

## Usage and development status

After assembling the device, program the microcontroller using an ST-Link and verify the battery polarity, charging current and mechanical movement limits before operation. The controller searches for the strongest light direction, positions the panel and adjusts the charging current according to the measured photovoltaic power. The project is currently a complete design and documentation package supported by approximate mechanical and energy calculations. The next step is to build a physical prototype and verify tracking efficiency, energy balance and long-term outdoor reliability.
