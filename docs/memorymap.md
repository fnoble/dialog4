# Memory Map

38000 <- Video RAM base address (Page 1?)

38800 <- Video RAM base address (Page 2?)

39001 <- MC6845 register number?

39003 <- MC6845 register value?

| Function       | Base Address | Upper Address | Length    | Chips                                               |
| -------------- | ------------ | ------------- | --------- | --------------------------------------------------- |
| NPP55 Boot ROM | 0x0000 0000  | 0x0001 FFFF   | 0x 2 0000 | 2x 27512 (64k x 8 ROM) = 64k x 16 = 128k x 8        |
|                |              |               |           |                                                     |
| VRAM Page 0    | 0x0003 8000  | 0x0003 87FF   | 0x800     | 2x ETL2128N‑4 (2k x 8 SRAM) = 2k x 16 for Page 0+1  |
| VRAM Page 1    | 0x0003 8800  | 0x0003 8FFF   | 0x800     | ^^ Ignores A0 but uses 16-bit data, WORD WRITE ONLY |
| MC6845         | 0x0003 9000  |               |           |                                                     |
| NEP52 Main ROM | 0x00A0 0000  | 0x00A9 FFFF   | A 0000    | 10x 27512 (64k x 8 ROM) = 64k x 16 = 640k x 8       |