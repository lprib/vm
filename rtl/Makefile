PIN_DEF = iceblink40/iceblink.pcf
DEVICE = lp1k
PACKAGE = qn84

ICE40_SRCS = iceblink40/platform.vhd src/toplevel.vhd src/negate.vhd

TOPLEVEL = platform

out.json: $(ICE40_SRCS)
	ghdl -a $(ICE40_SRCS)
	yosys -m ghdl -p "ghdl $(TOPLEVEL); synth_ice40 -json out.json"
	
%.asc: %.json $(PIN_DEF)
	nextpnr-ice40 --$(DEVICE) --json $< --pcf $(PIN_DEF) --asc $@ --package $(PACKAGE)

%.bin: %.asc
	icepack $< $@

prog: out.bin
	iCEburn  -e -v -w  $<