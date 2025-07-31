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

## Addressing

### Notes

- /CS line on VG2 Pin 2 should be active
- Only A1-A15 routed
- IC21 3-to-8 decoder is primary address decoder
    - A -> A12
    - B -> A13
    - C -> A14
    - G1 -> A15
    - /G2B(?) -> /AS 
    - /G2A(?) -> /CS_VG2
    - Y0 -> RAM logic
    - Y1 -> MC6845 /CS 
- From SW it seems like the video functions are in the 0x38000 and 0x39000 regions -> /CS_VG2 gives A23-A16 = 0x03
- MC6845 /CS = Y1 -> A15-A12 = 0x9
- RAM logic = Y0 -> A15-A12 = 0x8
- UDS/LDS do not seem to be present on this board
- /M RESET on VG1 routed to /R on MC6845
- R / /W is routed from VG2
- E is routed from VG2
- E is anded with /(R / /W) to drive the VRAM /WE 
- A0 is not routed!

### Conclusions

- MC6845 is at base address 0x39000 and only supports 8-bit access
- VRAM is at base address 0x38000 and is 16-bit wide x 2k words
- A0/LDS/UDS are not present so even and odd addresses will map to the same memory
- VRAM uses 16-bit data bus but does not know about A0/LDS/UDS -> need to clarify what behaviour will result.
    - **This is ok, use WORD WRITES ONLY!**
- MC6845 is connected to D0-D7
    - **Remember to write on odd addresses due to big-endian byte lanes!**

## Firmware Initialization

| Reg                                 | Value    | Field meaning (MC6845 data-sheet)                                                           | What the number works out to                                                  |
| ----------------------------------- | -------- | ------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------- |
| 0 – Horizontal **total**            | **0x3F** | Characters/line − 1                                                                         | 0x3F + 1 = 64 character clocks per scan-line                                  |
| 1 – Horizontal **displayed**        | **0x2A** | Visible characters/line − 1                                                                 | 0x2A + 1 = 43 visible characters                                              |
| 2 – Horizontal **sync position**    | **0x32** | Character position at which HSYNC starts                                                    | 0x32 = 50 → HSYNC begins 51st character time                                  |
| 3 – **Sync widths**                 | **0x05** | bits 3-0 = HSYNC width (chars) = 5<br>bits 7-4 = VSYNC width (scan-lines) = 0 (=16 on 6845) | HSYNC = 5 char clocks → 5 × character-clock pixels<br>VSYNC = 16 raster lines |
| 4 – Vertical **total** (low 7 bits) | **0x17** | Character rows/frame − 1                                                                    | 0x17 + 1 = 24 character rows                                                  |
| 5 – Vertical total **adjust**       | **0x00** | Extra scan-lines to match CRT frame                                                         | +0 (none)                                                                     |
| 6 – Vertical **displayed**          | **0x14** | Visible rows/frame − 1                                                                      | 0x14 + 1 = 21 visible rows                                                    |
| 7 – Vertical **sync position**      | **0x15** | Row at which VSYNC starts                                                                   | 0x15 = 21 → sync during 22nd character row                                    |
| 8 – Interlace / skew control        | **0x00** | 0 = non-interlaced, no skew                                                                 | Normal progressive scan                                                       |
| 9 – Max **scan-line** address       | **0x0C** | Raster lines/row − 1                                                                        | 0x0C + 1 = 13 scan-lines per character                                        |
| 10 – Cursor **start**               | **0x20** | Bit 5 = 1 → **cursor disabled**<br>Bits 4-0 = 0 (would have started on raster 0)            | Cursor turned off                                                             |
| 11 – Cursor **end**                 | **0x00** | Raster at which cursor ends                                                                 | (Unused while cursor disabled)                                                |
| 12 – Display-start address **hi**   | **0x00** | High byte of screen memory start                                                            | Screen starts at 0x0000 in VRAM                                               |
| 13 – Display-start address **lo**   | **0x00** | Low  byte of screen memory start                                                            | ″                                                                             |
| 14 – Cursor address **hi**          | **0x00** | High byte of current cursor position                                                        | (cursor off)                                                                  |
| 15 – Cursor address **lo**          | **0x00** | Low  byte of current cursor position                                                        | (cursor off)                                                                  |

## Video RAM / ROM interface

- **Character code: **D0-D7 → ROM A4-A11
- **Font bank:** D8-D9 → ROM A12-A13
- **Reverse video:** D15 → XOR with glyph & CURSOR
- MC6845 RA0-RA3 → ROM A0-A3 (select scan-line within glyph)
- MC6845 CURSOR (pin 19) → same XOR for block cursor
- D10 is routed somewhere but label unreadable - may be N/C or ?
- D14-11 are N/C



