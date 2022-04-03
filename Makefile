SRC_DIR = src
TEST_DIR = test
OUT_DIR = out
IMPL_DIR = impl
SCHEMA_DIR = schema

# TODO add separate INC folder for public headers

TEST_EXE_DIR = $(OUT_DIR)/test_exe
IMPL_EXE_DIR = $(OUT_DIR)/impl_exe

# include path when building source
SRC_INCLUDE_FLAGS = -I$(SRC_DIR)
# include path when building tests (SRC_INCLUDE_FLAGS is also required)
TEST_INCLUDE_FLAGS = -I$(TEST_DIR)

TEST_SOURCES = $(wildcard $(TEST_DIR)/test_*.c)
TEST_EXES = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_EXE_DIR)/%)

SRC_INCLUDE_FILES = $(wildcard $(SRC_DIR)/*.h)
TEST_INCLUDE_FILES = $(wildcard $(TEST_DIR)/*.h)

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
SRC_OUT_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)

CC = gcc
# excessive pedanticism
CFLAGS = -Wall -Wextra -Werror -flto -O3

COVERAGE_CFLAGS = --coverage

.DEFAULT_GOAL := silent_test

# quiet flag. conditionally set to "@" to supress output
Q = 

# Generate folders on demand

$(OUT_DIR):
	$(Q) mkdir -p $(OUT_DIR)

$(TEST_EXE_DIR): | $(OUT_DIR)
	$(Q) mkdir -p $(TEST_EXE_DIR)

$(IMPL_EXE_DIR): | $(OUT_DIR)
	$(Q) mkdir -p $(IMPL_EXE_DIR)

# Generate out objects from main sources
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_INCLUDE_FILES) | $(OUT_DIR)
	$(Q) $(CC) $(CFLAGS) $(SRC_INCLUDE_FLAGS) -c $< -o $@

# Generate out objects from test sources
$(OUT_DIR)/%.o: $(TEST_DIR)/%.c $(SRC_INCLUDE_FILES) $(TEST_INCLUDE_FILES)| $(OUT_DIR)
	$(Q) $(CC) $(CFLAGS) $(SRC_INCLUDE_FLAGS) $(TEST_INCLUDE_FLAGS) -c $< -o $@


# Generate test executables.

# Test exes depend on two sources: test source and UUT module source.
# Everything else should be mocked.
$(TEST_EXE_DIR)/test_%: $(OUT_DIR)/test_%.o $(OUT_DIR)/%.o $(SRC_INCLUDE_FILES) $(TEST_INCLUDE_FILES) | $(TEST_EXE_DIR)
	$(Q) $(CC) $(CFLAGS) $< $(word 2,$^) -o $@

# Generate a phony rule for running each test suite
RUN_TESTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=%)
.PHONY: $(RUN_TESTS)

# This rule substitues the executable basename (no path) with the path to
# executable to actually execute
$(RUN_TESTS):%:$(TEST_EXE_DIR)/%
	$(Q) ./$<

test: $(RUN_TESTS)

silent_test: Q=@
silent_test: test

# Coverage

coverage_test: CFLAGS=$(COVERAGE_CFLAGS)
coverage_test: test

$(OUT_DIR)/coverage.html: coverage_test
	gcovr --html-details $@ -e "test/*"

coverage: $(OUT_DIR)/coverage.html
	xdg-open $<


# Generate opcode schema

$(OUT_DIR)/opcode_schema.csv: $(SCHEMA_DIR)/vm_OpcodeSchema.h $(SRC_DIR)/vm_Opcodes.h | $(OUT_DIR)
	gcc $(SRC_INCLUDE_FLAGS) -fsyntax-only -imacros vm_Opcodes.h -P -E $< | tr -d "[:space:]" | sed "s/;/\n/g" > $@

schema: $(OUT_DIR)/opcode_schema.csv


# Generate intgration test executable
# Assumed to be a single main.c file

INTEGRATION_SRC_DIR = $(IMPL_DIR)/integration_test
$(IMPL_EXE_DIR)/integration_test: $(INTEGRATION_SRC_DIR)/main.c $(SRC_OUT_FILES) | $(IMPL_EXE_DIR)
	$(Q) $(CC) $(CFLAGS) $(SRC_INCLUDE_FLAGS) $^ -o $@

integration: $(IMPL_EXE_DIR)/integration_test
	$(Q) ./$<

clean:
	$(Q) rm -rf $(OUT_DIR)

debug: 
	@echo TEST_SOURCES
	@echo $(TEST_SOURCES)
	@echo TEST_EXES
	@echo $(TEST_EXES)
	@echo SRC_INCLUDE_FILES
	@echo $(SRC_INCLUDE_FILES)
	@echo TEST_INCLUDE_FLAGS
	@echo $(TEST_INCLUDE_FLAGS)
	@echo TEST_INCLUDE_FILES
	@echo $(TEST_INCLUDE_FILES)
	@echo SRC_FILES
	@echo $(SRC_FILES)
	@echo SRC_OUT_FILES
	@echo $(SRC_OUT_FILES)
	@echo RUN_TESTS
	@echo $(RUN_TESTS)

.PHONY: debug integration test silen_test coverage_test coverage schema clean
