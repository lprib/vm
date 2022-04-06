TARGET = $(SUBMAKE_OUT)/main
VM_HEADERS = $(wildcard $(VM_SRC_DIR)/*.h)

VM_SRC_FILES = $(wildcard $(VM_SRC_DIR)/*.c)
MY_SRC_FILES = $(wildcard *.c)

VM_OUT_FILES = $(VM_SRC_FILES:$(VM_SRC_DIR)/%.c=$(SUBMAKE_OUT)/%.o)
MY_OUT_FILES = $(MY_SRC_FILES:%.c=$(SUBMAKE_OUT)/%.o)

ALL_OUT_FILES = $(VM_OUT_FILES) $(MY_OUT_FILES)

all: $(TARGET)

# Make VM sources
$(SUBMAKE_OUT)/%.o: $(VM_SRC_DIR)/%.c $(VM_HEADERS)
	gcc $(VM_CFLAGS) -I$(VM_SRC_DIR) -I. -c $< -o $@

# Make local sources
$(SUBMAKE_OUT)/%.o: %.c $(VM_HEADERS)
	gcc $(VM_CFLAGS) -I$(VM_SRC_DIR) -c $< -o $@

$(TARGET): $(ALL_OUT_FILES)
	gcc $(SUBMAKE_OUT)/main.o $(filter-out %/main.o,$^) -o $@
