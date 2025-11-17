# Makefile for CMSC 240 Final Project - Death Run Game
# Team: Claire Wu, Tyeon Ford, Andy Quach

# Compiler and build system
SCONS = scons
PLATFORM = linux
TARGET = template_debug

# Detect platform
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM = linux
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
endif

# Default target
.PHONY: all
all: build

# Build the C++ extension
.PHONY: build
build:
	@echo "Building Death Run C++ extension..."
	$(SCONS) platform=$(PLATFORM) target=$(TARGET)
	@echo "Build complete!"

# Build release version
.PHONY: release
release:
	@echo "Building release version..."
	$(SCONS) platform=$(PLATFORM) target=template_release
	@echo "Release build complete!"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	$(SCONS) --clean
	rm -rf project/bin/
	@echo "Clean complete!"

# Run the game
.PHONY: run
run: build
	@echo "Launching game..."
	godot --path project/

# Run in editor mode
.PHONY: editor
editor: build
	@echo "Opening Godot editor..."
	godot -e --path project/

# Generate documentation (if Doxygen is installed)
.PHONY: docs
docs:
	@if command -v doxygen > /dev/null; then \
		doxygen Doxyfile; \
		echo "Documentation generated in docs/"; \
	else \
		echo "Doxygen not installed. Skipping documentation generation."; \
	fi

# Setup godot-cpp bindings (first time only)
.PHONY: setup
setup:
	@echo "Setting up godot-cpp bindings..."
	@if [ ! -d "godot-cpp" ]; then \
		git clone -b 4.3 https://github.com/godotengine/godot-cpp.git; \
		cd godot-cpp && $(SCONS) platform=$(PLATFORM); \
	else \
		echo "godot-cpp already exists"; \
	fi

# Help target
.PHONY: help
help:
	@echo "Death Run Game - Makefile Commands"
	@echo "==================================="
	@echo "make         - Build the C++ extension (default)"
	@echo "make build   - Build debug version"
	@echo "make release - Build release version"
	@echo "make clean   - Remove build artifacts"
	@echo "make run     - Build and run the game"
	@echo "make editor  - Open in Godot editor"
	@echo "make setup   - Setup godot-cpp (first time only)"
	@echo "make docs    - Generate documentation"
	@echo "make help    - Show this help message"

# Set default goal
.DEFAULT_GOAL := all