OUTDIR = out
SRCS = $(wildcard src/*.vhd) $(wildcard test/*.vhd)

# TODO: shame we need to silence these warns to link in uvvm utils
GHDLFLAGS = --std=08 --workdir=$(OUTDIR) -P$(OUTDIR) -frelaxed -Wno-shared -Wno-hide

UVVM_BUILT_CANARY = $(OUTDIR)/i_have_built_uvvm
UVVM_DIR = uvvm

# This order is specific
UVVM_SRCS = $(UVVM_DIR)/src_util/types_pkg.vhd \
$(UVVM_DIR)/src_util/adaptations_pkg.vhd \
$(UVVM_DIR)/src_util/string_methods_pkg.vhd \
$(UVVM_DIR)/src_util/protected_types_pkg.vhd \
$(UVVM_DIR)/src_util/global_signals_and_shared_variables_pkg.vhd \
$(UVVM_DIR)/src_util/hierarchy_linked_list_pkg.vhd \
$(UVVM_DIR)/src_util/alert_hierarchy_pkg.vhd \
$(UVVM_DIR)/src_util/license_pkg.vhd \
$(UVVM_DIR)/src_util/methods_pkg.vhd \
$(UVVM_DIR)/src_util/bfm_common_pkg.vhd \
$(UVVM_DIR)/src_util/generic_queue_pkg.vhd \
$(UVVM_DIR)/src_util/data_queue_pkg.vhd \
$(UVVM_DIR)/src_util/data_fifo_pkg.vhd \
$(UVVM_DIR)/src_util/data_stack_pkg.vhd \
$(UVVM_DIR)/src_util/rand_pkg.vhd \
$(UVVM_DIR)/src_util/func_cov_pkg.vhd \
$(UVVM_DIR)/src_util/uvvm_util_context.vhd

all: uvvm | $(OUTDIR)
	ghdl -m $(GHDLFLAGS) test_decoder
	ghdl -r $(GHDLFLAGS) test_decoder --wave=test_decoder.ghw

$(OUTDIR):
	mkdir -p $@

$(UVVM_BUILT_CANARY): | $(OUTDIR)
	ghdl -a $(GHDLFLAGS) -frelaxed -Wno-shared -Wno-hide --work=uvvm_util $(UVVM_SRCS)
	touch $(UVVM_BUILT_CANARY)

import: | $(OUTDIR) $(UVVM_BUILT_CANARY)
	ghdl -i $(GHDLFLAGS) $(SRCS)

clean:
	rm -rf $(OUTDIR)


.PHONY: all clean uvvm