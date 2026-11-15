# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/CornerstoneOptions.cmake
# @id            build.cmake.options
# @layer         Layer 0 Infrastructure (Build Orchestration)
# @archetype     operational | stateless
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Toolchain & Standards Configurator
# @role          Modular CMake Subsystem: Configures ISO C23/C17/C11 and C++20/17
#                standards ladders, warning flags, X11 discovery, include boundaries,
#                and structured build output directory topology.
# @maturity      Rev 5 Baseline | Progressive standard probes + structured topology.
# @chassis       zero-runtime-deps | Configures compile-time standards and include paths.
# @scale         O(1) configuration pass over toolchain features and build directories.
# @seams         in: CMake toolchain | out: ENGINE_PUBLIC_INCLUDES, GAME_WARNING_FLAGS
# @invariants    1. Progressive ISO C23 target with graceful C17/C11 floor.
#                2. Strict ISO C warning purity (-Wall -Wextra -Wpedantic / /W4).
#                3. Layer 0 public include roots must NEVER include game/include.
#                4. Structured build output topology (bin/, lib/, tests/, scratch/).
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Modular CMake configuration provider establishing toolchain standards,
#   compiler warning flags, native X11 package discovery, layered include
#   boundaries, and structured directory outputs.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Compiler Standards, Feature Detection & Diagnostic Options )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.options
# Mission:       Establish toolchain standards, feature probes, and diagnostic flags.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Progressive ISO C23->C17->C11 Conformance Options
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Cornerstone employs an "Engine-as-a-Java" Fall-Down Stairs model:
#   1. Forward-First Target: When compiled with modern toolchains (GCC 15+, Clang 18+),
#      Cornerstone leverages ISO C23 native keywords (bool, true, false, nullptr,
#      alignas, static_assert, nodiscard, maybe_unused) natively.
#   2. Graceful Fallback Floor: If older compilers or embedded toolchains are used,
#      the engine automatically steps down to ISO C17 or ISO C11 without code changes
#      or macro spaghetti, anchored by fallback definitions in core/manifest.h.
#   3. Strict Standards Conformance: CMAKE_C_EXTENSIONS is set to OFF to strictly
#      disallow proprietary compiler extensions (e.g., GNU extensions) and guarantee
#      pure ISO C portability across Linux, BSD, and Windows/MSVC.
# ------------------------------------------------------------------------------
set(CORNERSTONE_C_STANDARD "AUTO" CACHE STRING
    "ISO C standard tier: AUTO (probes C23 -> C17 -> C11), 23, 17, or 11"
)
set_property(CACHE CORNERSTONE_C_STANDARD PROPERTY STRINGS "AUTO" "23" "17" "11")

set(CORNERSTONE_CXX_STANDARD "AUTO" CACHE STRING
    "ISO C++ header verification tier: AUTO (probes C++20 -> C++17), 20, or 17"
)
set_property(CACHE CORNERSTONE_CXX_STANDARD PROPERTY STRINGS "AUTO" "20" "17")

if(CORNERSTONE_C_STANDARD STREQUAL "AUTO")
    if("c_std_23" IN_LIST CMAKE_C_COMPILE_FEATURES)
        set(_cs_c_tier 23)
    elseif("c_std_17" IN_LIST CMAKE_C_COMPILE_FEATURES)
        set(_cs_c_tier 17)
    else()
        set(_cs_c_tier 11)
    endif()
elseif(CORNERSTONE_C_STANDARD MATCHES "^(11|17|23)$")
    set(_cs_c_tier "${CORNERSTONE_C_STANDARD}")
else()
    message(FATAL_ERROR
        "Invalid CORNERSTONE_C_STANDARD='${CORNERSTONE_C_STANDARD}'. Expected AUTO, 23, 17, or 11."
    )
endif()

set(CMAKE_C_STANDARD ${_cs_c_tier})
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
message(STATUS
    "[Cornerstone] ISO C tier: C${_cs_c_tier} "
    "(requested: ${CORNERSTONE_C_STANDARD}, floor: C11, extensions: OFF)"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 2.2: Diagnostic and Header Verification Switches
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   - CORNERSTONE_ENABLE_WERROR: Ensures zero-warning tolerance during CI runs
#     and focused refactoring sweeps, treating every warning as a hard failure.
#   - CORNERSTONE_VERIFY_CPP_HEADERS: Automatically compiles a synthetic C++
#     probe against all public Cornerstone headers to verify that 'extern "C"'
#     guards, type visibility, and modern C++ .hpp companions are 100% valid.
#   - CMAKE_EXPORT_COMPILE_COMMANDS: Emits compile_commands.json in the build
#     directory so Language Server Protocols (clangd, rust-analyzer, ccls) have
#     accurate AST flag bindings out-of-the-box.
# ------------------------------------------------------------------------------
option(CORNERSTONE_ENABLE_WERROR
    "Treat compiler warnings as hard build errors across Cornerstone & game targets"
    OFF
)
option(CORNERSTONE_VERIFY_CPP_HEADERS
    "Compile-check Rev 5 Cornerstone public headers under C++20/C++17 via CheckLanguage(CXX)"
    OFF
)

# Export compilation database for LSP servers (clangd, rust-analyzer, ccls)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Include Strata, Package Discovery & Compiler Flags )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.options
# Mission:       Define include boundaries, discover host dependencies, and configure flags.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Layered Include Boundary Definitions
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Strict downward dependency isolation is the central pillar of Cornerstone.
#   - ENGINE_PUBLIC_INCLUDES: Dedicated exclusively to Layer 0 (tenebris_engine_util).
#     Under Invariant 11 (Standalone Engine Boundary Covenant), engine cogs must
#     NEVER be able to resolve headers under game/include/. Placing game/ in the
#     include search path for Layer 0 would allow accidental domain bleed.
#   - COMMON_INCLUDES: Used by Layer 1 (Simulation), Layer 2 (Viewport), and
#     Layer 3 (Application Plug), which legally consume both engine/ and game/.
#   - TEST_INCLUDES: Includes tests/ for test harness utilities and shared mocks.
# ------------------------------------------------------------------------------
set(ENGINE_PUBLIC_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(COMMON_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/game/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(TEST_INCLUDES
    ${COMMON_INCLUDES}
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.2: Native X11 Discovery & Strict Warning Flags
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   - X11: Layer 2 rasterization and event loops target native X11 windowing on
#     Linux/Unix for high-performance direct framebuffer blitting without SDL2/GLFW
#     heavyweight third-party dependencies.
#   - Compiler Flags: -Wall -Wextra -Wpedantic (or /W4 on MSVC) enforces strict
#     adherence to ISO C syntax, preventing undefined behavior, implicit casts,
#     padding bugs, and missing return values across all build targets.
# ------------------------------------------------------------------------------
find_package(X11 REQUIRED)

if(MSVC)
    set(GAME_WARNING_FLAGS /W4)
    if(CORNERSTONE_ENABLE_WERROR)
        list(APPEND GAME_WARNING_FLAGS /WX)
    endif()
else()
    set(GAME_WARNING_FLAGS -Wall -Wextra -Wpedantic)
    if(CORNERSTONE_ENABLE_WERROR)
        list(APPEND GAME_WARNING_FLAGS -Werror)
    endif()
endif()

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Output Directory Topology & Workspace Sandbox Verification )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Build Orchestration)
# Module:        build.cmake.options
# Mission:       Partition build outputs into designated role directories.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 4.1: Structured Build Output Topology (bin/, lib/, tests/, scratch/)
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Prevents build output clutter where executables, libraries, test harnesses,
#   and temporary filesystem mounts are commingled in the root build directory.
#   - build/bin/: Final customer-facing executables (tenebris-lux).
#   - build/lib/: Intermediate static/shared archives and OBJECT library outputs.
#   - build/tests/: All compiled Layer 4 verification test binaries.
#   - build/scratch/: Sandboxed workspace for file-system and VFS tests to create
#     temporary files without polluting or damaging real source or config files.
# ------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Ensure core output and scratch hierarchy exists
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/scratch")

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Configuration Affirmation & Handshake )
# ==============================================================================
# Invariant Affirmation: Toolchain standards, warning purity, include boundaries,
# and structured output directories are affirmed and bound into the build graph.
