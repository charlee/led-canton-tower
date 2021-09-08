Schematic of the LED Tower Controller
=========================================

The schematic is made with [EasyEDA](https://easyeda.com/).

The MCU (U5) is an STC89C52RC (compatible with AT89C52) IC. This is a pretty old and slow IC. A 12MHz crystal oscillator (X1) is used to provide clock.
Most instructions in the STC89C52 require 12 clock cycles, so a 12MHz oscillator can make each instruction execute in exactly 1μs.

To simplify the hardware connections, four SN74HC595 are used for controlling the LEDs. Therefore only three ports (P2.0, P2.2, P2.4) are used as output.

In order to provide enough current to the LED tower, transistors are used to provide and sink current. On the sink side, two ULN2803A are used instead of
discrete componentes to simplify hardware connections.


![Schematic](images/schematic.png)


The PCB is not really for making PCB. Instead, it is designed for soldering manually on perfboards.
The red/blue traces are only used to indicate intersecting traces.

On the actual perfboard, the intersecting traces are soldered in layers, e.g. red traces are soldered about 2-3mm away from the board.

![PCB](images/pcb.png)
