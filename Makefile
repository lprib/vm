SRC_DIR = src
TEST_DIR = test
OUT_DIR = out

# $(TEST_OUT_SUBDIR) is where the text executables will reside
TEST_OUT_DIR = $(OUT_DIR)/tests

# include path when building source
SRC_INCLUDES = -I$(SRC_DIR)
# include path when building tests (SRC_INCLUDES is also required)
TEST_INCLUDES = -I$(TEST_DIR)

TEST_SOURCES = $(wildcard $(TEST_DIR)/test_*.c)
TEST_EXES = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_OUT_DIR)/%)

SRC_INCLUDE_FILES = $(wildcard $(SRC_DIR)/*.h)
TEST_INCLUDE_FILES = $(wildcard $(TEST_DIR)/*.h)

CFLAGS = -Wall

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(TEST_OUT_DIR): | $(OUT_DIR)
	mkdir -p $(TEST_OUT_DIR)

$(TEST_OUT_DIR)/test_%: $(TEST_DIR)/test_%.c $(SRC_DIR)/%.c $(SRC_INCLUDE_FILES) $(TEST_INCLUDE_FILES) | $(TEST_OUT_DIR)
	gcc $(CFLAGS) $(SRC_INCLUDES) $(TEST_INCLUDES) $< $(word 2,$^) -o $@

# Generate a phony rule for running each test suite
RUN_TESTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=%)
.PHONY: $(RUN_TESTS)

# The rule substitues the executable basename (no path) with the path to
# executable to actually execute
$(RUN_TESTS):%:$(TEST_OUT_DIR)/%
	@echo Running test $<.
	@./$<
	@echo

test: $(RUN_TESTS)

clean:
	rm -r $(OUT_DIR)

debug: 
	@echo TEST_SOURCES
	@echo $(TEST_SOURCES)
	@echo TEST_EXES
	@echo $(TEST_EXES)
	@echo TEST_INCLUDES
	@echo $(TEST_INCLUDES)
	@echo SRC_INCLUDE_FILES
	@echo $(SRC_INCLUDE_FILES)
	@echo TEST_INCLUDE_FILES
	@echo $(TEST_INCLUDE_FILES)
	@echo RUN_TESTS
	@echo $(RUN_TESTS)

.PHONY: test debug
