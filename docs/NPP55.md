RAM -> 
 - one image shows 6x SONY CXK5864PN-15L (8k x 8)
 - another image shows 6x HM6264LP-15 (32k x 8)
(perhaps later version used larger parts due to availability but unless the address decoding changed it probably wasn't a memory expansion)

3x MC68230 PIAs (parallel interfaces).
One seems to go to K9 which is not populated in some board images.
One goes to what looks like an 8-bit bus on VG1 - I'm guessing this is the link to the NPP54 bus.
One goes partially to K9 and partially to the other ribbon connector that is not K9 or K10?
From FW, two of the PIAs seem to be mapped at 0x12000 and 0x12400.

The address decoding logic is at the bottom.

Blocks of XORs w. open collector outputs OR'd together select A23-A18 == 0. This block is repeated twice.

There are 4 3-to-8 decoders.
I'm thinking the rightmost one drives the ROM /CS lines as it has 4 outputs
Two of them have 3 output lines - probably ones is the RAMs and one is the parallel controllers.

The leftmost chip of the block looks like another 3-to-8 but wired up as an OR? It could also be a counter or something to count the first few clocks?? Its driving a line that might be the boot ROM overlay signal. If so its connected to the enable of the first 3-to-8 with 3 outputs so could be gating the RAM enable and it also goes via a couple gates to the 3-to-8 with 4 outputs that might drive the boot ROMs.

This would indicate some kind of auto-overlay mechanism to let the CPU grab the initial PC vector on the first cycle.


