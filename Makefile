CC = gcc
CFLAGS = -std=c23 -O2 -g -Wall -Werror -MMD -MP

# Libraries
LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lfreetype -lm 

# Include paths for internal includes and external includes
INCLUDE_PATHS = -Iinclude -Ilibs/include $(shell pkg-config --cflags freetype2 | awk '{print $1}')

# Directories
SRC_DIR = src
LIBS_SRC_DIR = libs/src
BUILD_DIR = build

# Source files
SRCS := $(shell find $(SRC_DIR) -name "*.c")
LIB_SRCS := $(shell find $(LIBS_SRC_DIR) -name "*.c")

# Object files (internal)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
# Object files (external)
OBJS += $(LIB_SRCS:$(LIBS_SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Final executable
EXEC = $(BUILD_DIR)/equafun

# Default target (compiling the executable)
all: $(EXEC)

	# remove the current data there
	rm -fr $(BUILD_DIR)/data

	# copy the data folder to the build folder
	cp -r data/ $(BUILD_DIR)/

# Linking the object files into the final executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LIBS)

# Compilation rule for each internal source file (src/)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Compilation rule for each external source file (lib/src/)
$(BUILD_DIR)/%.o: $(LIBS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

-include $(OBJS:.o=.d)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
