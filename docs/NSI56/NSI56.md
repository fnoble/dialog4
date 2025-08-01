# NSI56 - Serial Interface

## Parts

| Designator    | Part #                        | Description (device type)           |
| ------------- | ----------------------------- | ----------------------------------- |
| **IC1**       |                               |                                     |
| **IC2**       |                               |                                     |
| **IC3**       |                               |                                     |
| **IC4**       |                               |                                     |
| **IC5**       |                               |                                     |
| **IC6**       |                               |                                     |
| **IC7**       |                               |                                     |
| **IC8**       |                               |                                     |
| **IC9**       |                               |                                     |
| **IC10**      |                               |                                     |
| **IC11**      |                               |                                     |
| **IC12**      |                               |                                     |
| **IC13**      |                               |                                     |
| **IC14**      |                               |                                     |
| **IC15**      |                               |                                     |
| **IC16**      |                               |                                     |
| **IC17**      |                               |                                     |
| **IC18**      |                               |                                     |
| **IC19**      |                               |                                     |
| **IC20**      |                               |                                     |
| **IC21**      | MC68681P                      |                                     |
| **IC22**      |                               |                                     |
| **IC23**      |                               |                                     |
| **IC24**      | SN74LS266N                    | Quad XNOR, open‑collector output    |
| **IC25**      | SN74LS266N                    | Quad XNOR, open‑collector output    |
| **IC26**      | SN74LS266N                    | Quad XNOR, open‑collector output    |
| **IC27**      | SN74LS266N                    | Quad XNOR, open‑collector output    |
| **IC28**      | MC74F138 N                    | 3‑to‑8‑line decoder / demultiplexer |
| **IC29**      |                               |                                     |
| **IC30**      |                               |                                     |
| **IC31**      | MC68681P                      |                                     |
| **IC32**      |                               |                                     |
| **Q1**        | Metal‑can XO – “3.686??0 MHz” |                                     |
| **VG1 / VG2** | 32‑pin edge connectors        |                                     |

## Solder Jumpers

| Designator | State     | Description |
| ---------- | --------- | ----------- |
| **BR1**    | Short     |             |
| **BR2**    | Open      |             |
| **BR3**    | Open (?)  |             |
| **BR4**    | Open      |             |
| **BR5**    | Short     |             |
| **BR6**    | Open      |             |
| **BR7**    |           |             |
| **BR8**    | Short (?) | A8          |
| **BR9**    | Short (?) | A9          |
| **BR10**   | Short (?) | A10         |
| **BR11**   | Short (?) | A11         |
| **BR12**   | Short (?) | A12         |
| **BR13**   | Short (?) | A13         |
| **BR14**   | Short (?) | A14         |
| **BR15**   | Short (?) | A15         |
| **BR16**   | Short     | A16         |
| **BR17**   | Short     | A17         |
| **BR18**   | Short     | A18         |
| **BR19**   | Short     | A19         |
| **BR20**   | Short     | A20         |
| **BR21**   | Open      | A21         |
| **BR22**   | Open      | A22         |
| **BR23**   | Open      | A23         |
| **BR24**   | Short     |             |
| **BR25**   | Open (?)  |             |
| **BR26**   | Short (?) |             |
| BR27       | Open (?)  |             |
| BR28       | Open      |             |

## Addressing

| A23-A8                | A5-7    | A1-4         | A0   |
| --------------------- | ------- | ------------ | ---- |
| Solder Jumpers        | IC28 Y0 | -> UART A1-4 |      |
| 0b1110 0000 0000 0000 | 0b000   | 0bXXXX       | 0b0? |
| 0xE000                | 0x0     | 0xX          | 0x0? |

Address is 0xE000XX
Range is 0xE00000 - 0xE0001F
Confirmed in the firmware