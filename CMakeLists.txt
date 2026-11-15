# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          CMakeLists.txt
# @id            build.root.orchestrator
# @layer         Layer 0 Infrastructure (Build Orchestration)
# @archetype     operational | stateful
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Master Build Graph Orchestrator
# @role          Root CMake Master Orchestrator: Coordinates modular subsystems
#                under cmake/ across toolchain standards, ROM vault generation,
#                Layer 0-3 OBJECT/executable targets, 4-tier CTest suites,
#                and workspace maintenance targets.
# @maturity      Rev 5 Baseline | Modular CMake Orchestrator Architecture.
# @chassis       zero-runtime-deps | Delegated subsystem build orchestration.
# @scale         O(1) declarative orchestration graph over 6 modular cmake/ subsystems.
# @seams         in: cmake/*.cmake, engine/, game/, tests/ | out: build/{bin,lib,tests,generated,logs,scratch}
# @invariants    1. Progressive ISO C23 target with graceful C17/C11 floor.
#                2. Strict ISO C warning purity (-Wall -Wextra -Wpedantic / /W4).
#                3. Structural Layer 0 isolation: tenebris_engine_util excludes game/include.
#                4. Compiled-in .rodata Config ROM headers generated out-of-source (.h and .hpp).
#                5. Four-tier Layer 4 verification suite + CLI contracts registered in CTest (65+ tests).
#                6. Structured build topology: bin/, lib/, tests/, scratch/, generated/, logs/.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Master CMake build orchestrator for Tenebris and Lux (powered by the
#   Cornerstone Game Engine). Decouples build specifications into single-purpose
#   subsystems under cmake/, keeping the root build graph declarative, readable,
#   and maintainable across human and agentic workflows.
#
#   ### For Game Designers & Non-Programmers:
#   Coordinates the compilation of the playable game (`tenebris-lux`), emergency
#   ROM assets, and all 65 automated health checks without manual build scripting.
#
#   ### For Systems Programmers & Engine Architects:
#   Orchestrates the 5 Universal Blocks:
#   - Block 1 (Metadata): Identity, provenance, and toolchain invariants.
#   - Block 2 (Opening): Standards, options, include boundaries & ROM generator.
#   - Block 3 (Body): Layer 0-3 compilation units (Engine, Viewport, Sim, App).
#   - Block 4 (Audit): Four-tier CTest battery & C++20/C++17 companion probe.
#   - Block 5 (Closing): Maintenance targets & workspace sanitation.
# ==============================================================================

cmake_minimum_required(VERSION 3.24)
project(TenebrisLux VERSION 0.1.2 LANGUAGES C)

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Toolchain Standards, Diagnostic Options & Config ROM Vaults )
# ==============================================================================
# Load compiler standards ladder, warning flags, X11, and output directory topology
include(cmake/CornerstoneOptions.cmake)

# Load Config ROM generator function and register telemetry, debug, and io ROM targets
include(cmake/CornerstoneRom.cmake)

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Layer 0 Engine, Layer 1 Simulation, Layer 2 Viewport & Layer 3 App Binary )
# ==============================================================================
# Build Layer 0 (tenebris_engine_util) and Layer 2 (tenebris_engine_viewport)
include(cmake/CornerstoneEngine.cmake)

# Build Layer 1 (tenebris_game_sim) and Layer 3 primary binary (tenebris-lux)
include(cmake/CornerstoneGame.cmake)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Four-Tier Verification Suite, C++ Probe & CLI Interface Contracts )
# ==============================================================================
# Register all 65 Functional, Unit, Integrated, and System CTest suites
include(cmake/CornerstoneTesting.cmake)

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Workspace Maintenance Targets & Lifecycle Handshake )
# ==============================================================================
# Register clean-scratch, clean-logs, clean-tests, and clean-all-artifacts
include(cmake/CornerstoneMaintenance.cmake)
