# NCR53 - CRT Controller

## Parts

| Designator    | Part #                                  | Description (device type)                         |
| ------------- | --------------------------------------- | ------------------------------------------------- |
| **IC1**       | SN74LS86 N                              | Quad 2‑input XOR gate                             |
| **IC2**       | 27C256‑15 – label “44206‑352C‑00 V1.04” | 32 k × 8 UV‑EPROM                                 |
| **IC3**       | SN74LS166 N                             | 8‑bit parallel‑load/serial‑out shift register     |
| **IC4**       | SN74LS161 AN                            | 4‑bit synchronous binary counter                  |
| **IC5**       | SN74LS04 N                              | Hex inverting buffer (hex inverter)               |
| **IC6**       | SN74LS74 AN                             | Dual D‑type flip‑flop                             |
| **IC7**       | SN74LS86 N                              | Quad 2‑input XOR gate                             |
| **IC8**       | — (socket empty)                        | —                                                 |
| **IC9**       | SN74LS02 N                              | Quad 2‑input NOR gate                             |
| **IC10**      | MC6845 P  “JRS 8610”                    | CRT controller (video timing generator)           |
| **IC11**      | ETL2128N‑4                              | 2 k × 8 static RAM                                |
| **IC12**      | ETL2128N‑4                              | 2 k × 8 static RAM                                |
| **IC13**      | SN74LS14 N                              | Hex Schmitt‑trigger inverter                      |
| **IC14**      | SN74LS74 AN                             | Dual D‑type flip‑flop                             |
| **IC15**      | SN74LS74 AN                             | Dual D‑type flip‑flop                             |
| **IC16**      | SN74LS645 N                             | Octal bidirectional bus transceiver (3‑state)     |
| **IC17**      | SN74LS157 N                             | Quad 2‑to‑1 data multiplexer                      |
| **IC18**      | SN74LS157 N                             | Quad 2‑to‑1 data multiplexer                      |
| **IC19**      | SN74LS157 N                             | Quad 2‑to‑1 data multiplexer                      |
| **IC20**      | SN74LS00 N                              | Quad 2‑input NAND gate                            |
| **IC21**      | SN74LS138 N                             | 3‑line‑to‑8‑line decoder / demultiplexer          |
| **IC22**      | SN74LS645 N                             | Octal bidirectional bus transceiver (3‑state)     |
| **IC23**      | SN74LS645 N                             | Octal bidirectional bus transceiver (3‑state)     |
| **Q1**        | Metal‑can XO – “8 MHz” (?)              | Crystal‑controlled clock oscillator module        |
| **NW1 … NW5** | 8‑pin SIP resistor packs (blue)         | Resistor network arrays (pull‑ups / pull‑downs)   |
| **K5/K6**     | 20‑pin dual‑row header pair             | Ribbon‑cable connector to NDM51 ("Monitorplatte") |
| **K7**        | 20‑pin dual‑row header                  | Ribbon‑cable connector                            |
| **VG1 / VG2** | 32‑pin edge connectors                  | Back‑plane edge connectors                        |
| **MP1**       | Test loop (plated hole)                 | Test / measurement point                          |