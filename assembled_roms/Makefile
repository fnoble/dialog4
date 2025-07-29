# -----------------------------------------------------------
#  Requirements:  srec_cat  (part of the “srecord” package)
# -----------------------------------------------------------

# ---- Raw ROM dumps ----
# ---- NPP55 ----
NPP55_A := ../raw_roms/353.21\ NPP55\ IC21\ A\ 2.14.bin   # low‑byte EPROM
NPP55_B := ../raw_roms/353.21\ NPP55\ IC22\ B\ 2.14.bin   # high‑byte EPROM
# ---- NEP52 ----
NEP52_A := ../raw_roms/353.32\ NEP52\ IC11\ A\ 2.16.bin   # low  byte
NEP52_B := ../raw_roms/353.32\ NEP52\ IC10\ B\ 2.16.bin   # high byte
NEP52_C := ../raw_roms/353.32\ NEP52\ IC13\ C\ 2.16.bin
NEP52_D := ../raw_roms/353.32\ NEP52\ IC12\ D\ 2.16.bin
NEP52_E := ../raw_roms/353.32\ NEP52\ IC15\ E\ 2.16.bin
NEP52_F := ../raw_roms/353.32\ NEP52\ IC14\ F\ 2.16.bin
NEP52_G := ../raw_roms/353.32\ NEP52\ IC17\ G\ 2.16.bin
NEP52_H := ../raw_roms/353.32\ NEP52\ IC16\ H\ 2.16.bin
NEP52_J := ../raw_roms/353.32\ NEP52\ IC19\ J\ 2.16.bin
NEP52_K := ../raw_roms/353.32\ NEP52\ IC18\ K\ 2.16.bin

# ---- output file names ------------------------------------
NPP55_OUTBIN := 353.21_NPP55_2.14.bin
NPP55_OUTHEX := $(NPP55_OUTBIN:.bin=.hex)
NEP52_OUTBIN := 353.32_NEP52_2.16.bin
NEP52_OUTHEX := $(NEP52_OUTBIN:.bin=.hex)

# ---- rule: convert to Intel‑HEX ---------------------------
%.hex: %.bin
	@echo "  [IHEX]   $@"
	srec_cat "$<" -Binary -o "$@" -Intel 

# ---- default target ---------------------------------------
all: $(NPP55_OUTBIN) $(NPP55_OUTBIN:.bin=.hex) $(NEP52_OUTBIN) $(NEP52_OUTBIN:.bin=.hex)

# ─────────── Merge the five pairs into one 16‑bit image ────
$(NEP52_OUTBIN): $(NEP52_A) $(NEP52_B) $(NEP52_C) $(NEP52_D) $(NEP52_E) $(NEP52_F) \
           $(NEP52_G) $(NEP52_H) $(NEP52_J) $(NEP52_K)
	@echo "  [MERGE] $@"
	@srec_cat \
	    $(NEP52_B) -binary -unsplit 2 0 -offset 0x00000 \
	    $(NEP52_A) -binary -unsplit 2 1 -offset 0x00000 \
	    $(NEP52_D) -binary -unsplit 2 0 -offset 0x20000 \
	    $(NEP52_C) -binary -unsplit 2 1 -offset 0x20000 \
	    $(NEP52_F) -binary -unsplit 2 0 -offset 0x40000 \
	    $(NEP52_E) -binary -unsplit 2 1 -offset 0x40000 \
	    $(NEP52_H) -binary -unsplit 2 0 -offset 0x60000 \
	    $(NEP52_G) -binary -unsplit 2 1 -offset 0x60000 \
	    $(NEP52_K) -binary -unsplit 2 0 -offset 0x80000 \
	    $(NEP52_J) -binary -unsplit 2 1 -offset 0x80000 \
	    -o "$@" -binary

# ---- rule: build merged 16‑bit binary ---------------------
$(NPP55_OUTBIN): $(NPP55_B) $(NPP55_A)
	@echo "  [MERGE]  $@"
	srec_cat -o "$@" -binary $(NPP55_A) -binary -unsplit 2 1 $(NPP55_B) -binary -unsplit 2 0

# ---- housekeeping ----------------------------------------
.PHONY: clean
clean:
	@rm -f $(NPP55_OUTBIN) $(NPP55_OUTHEX)

# -----------------------------------------------------------
# Usage:
#   $ make          # builds merged .bin and .hex
#   $ make clean    # removes the generated outputs
# -----------------------------------------------------------