TARGET = $(SUBMAKE_OUT)/generate_schema

VM_HEADERS = $(wildcard $(VM_SRC_DIR)/*.h)
VM_SRC_FILES = $(wildcard $(VM_SRC_DIR)/*.c)
VM_OUT_FILES = $(VM_SRC_FILES:$(VM_SRC_DIR)/%.c=$(SUBMAKE_OUT)/%.o)

all: $(TARGET)

# Make VM sources
$(SUBMAKE_OUT)/%.o: $(VM_SRC_DIR)/%.c $(VM_HEADERS)
	$(Q) gcc $(VM_CFLAGS) -I$(VM_SRC_DIR) -I. -c $< -o $@

# Make local sources
$(TARGET).o: generate_schema.c $(VM_HEADERS)
	$(Q) gcc $(VM_CFLAGS) -I$(VM_SRC_DIR) -c $< -o $@

$(TARGET): $(TARGET).o $(VM_OUT_FILES)
	$(Q) gcc $^ -o $@
