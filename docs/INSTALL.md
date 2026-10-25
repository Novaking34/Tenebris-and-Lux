# Installation and Build Guide

> **Project**: Tenebris and Lux  
> **Target Version**: `0.0.1`  
> **Language Standard**: ISO C11 (`-std=c11`)  
> **Build Tool**: CMake 3.24+ or native C compiler  

---

## 1. System Requirements

### Supported Operating Systems

- **Linux**: Ubuntu 20.04+, Debian 11+, Fedora 38+, Arch Linux, or any modern POSIX environment.
- **macOS**: macOS 12 (Monterey) or later with Xcode Command Line Tools.
- **Windows**: Windows 10/11 with MSVC (Visual Studio 2022) or MinGW-w64 / MSYS2.

### Required Software

1. **C11 Compiler**:
   - `gcc` (version 9.0+) OR
   - `clang` (version 10.0+) OR
   - Microsoft Visual C++ (`cl.exe`, 2019+)
2. **CMake**: Version 3.24 or higher.
3. **Make / Ninja**: Standard build generator.

On Debian/Ubuntu systems, install the core development dependencies:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake
```

On Fedora/RHEL:

```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake gcc
```

On macOS:

```bash
xcode-select --install
brew install cmake
```

---

## 2. Building with CMake (Recommended)

### Step 1: Configure the Build

From the repository root directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

For an optimized Release build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

### Step 2: Compile the Executable

```bash
cmake --build build --config Debug
```

The resulting binary will be located at:

- **Linux / macOS**: `./build/tenebris-lux`
- **Windows (MSVC)**: `.\build\Debug\tenebris-lux.exe`

---

## 3. Direct Compilation (No CMake Required)

If CMake is unavailable, you can compile the text prototype directly with `gcc` or `clang`:

```bash
# Debug build with full diagnostics and native X11 GUI
gcc -std=c11 -Wall -Wextra -Wpedantic -g src/main.c src/gui.c -Iinclude -lX11 -o tenebris-lux

# Release build with optimization
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 src/main.c src/gui.c -Iinclude -lX11 -o tenebris-lux
```

---

## 4. Running the Game

By default, launching `./build/tenebris-lux` will automatically detect your desktop environment:

- If a display is available, it opens the native graphical desktop GUI.
- If run in a headless environment, it seamlessly defaults to CLI terminal mode.

```bash
# Default (auto-detects GUI or CLI)
./build/tenebris-lux

# Force terminal CLI mode
./build/tenebris-lux --cli

# Force native desktop GUI mode
./build/tenebris-lux --gui

# Run diagnostic self-audit
./build/tenebris-lux --audit
```

### Command-Line Arguments

- `-h, --help`: Display available command-line flags.
- `-v, --version`: Display application version.
- `-a, --audit`: Execute the internal diagnostic self-audit suite.
- `-c, --cli`: Force execution in terminal CLI mode.
- `-g, --gui`: Force execution in native desktop GUI mode.

---

## 5. Automated Verification & Testing

Run all automated unit and invariant tests through `CTest`:

```bash
ctest --test-dir build -C Debug --output-on-failure
```

To run a fast syntax check without generating an executable:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -fsyntax-only -Iinclude src/main.c
```

---

## 6. Troubleshooting

- **Error: `cmake: command not found`**  
  Install CMake using your system package manager or download it directly from [cmake.org](https://cmake.org/download/). Alternatively, use the direct GCC compilation method described in Section 3.
- **Error: `unrecognized command-line option '-std=c11'`**  
  Your compiler version is outdated. Update GCC or Clang to a modern version supporting standard C11.
