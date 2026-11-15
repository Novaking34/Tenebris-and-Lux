# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/CornerstoneMaintenance.cmake
# @id            build.cmake.maintenance
# @layer         Layer 0 Infrastructure (Workspace Maintenance)
# @archetype     operational | stateless
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Maintenance Target Coordinator
# @role          Modular CMake Subsystem: Defines ergonomic workspace housekeeping
#                targets for cleaning scratch test directories, rotated logs,
#                compiled test binaries, and combined build artifacts.
# @maturity      Rev 5 Baseline | Modular build maintenance targets.
# @chassis       zero-runtime-deps | Out-of-source artifact sanitation.
# @scale         O(1) declarative custom targets across transient directories.
# @seams         in: build/scratch/, build/logs/, build/tests/ | out: clean targets
# @invariants    1. Targets operate strictly within build/ directory bounds.
#                2. Never deletes source files, JSON configs, or git state.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Provides targeted maintenance commands allowing developers and automated
#   agents to sanitize build workspaces without deleting CMake caches.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Pre-Conditions & Target Registration )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Workspace Maintenance)
# Module:        build.cmake.maintenance
# Mission:       Audit binary directory bounds before registering sanitation targets.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 2.1: Binary Directory Safety Assertion
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Guarantees that maintenance targets can NEVER execute against the source
#   directory. If CMAKE_BINARY_DIR equals CMAKE_SOURCE_DIR (an in-source build),
#   destructive clean commands could delete source files or git state.
# ------------------------------------------------------------------------------
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR
        "CornerstoneMaintenance.cmake: In-source builds are forbidden. Run cmake in a separate build directory."
    )
endif()

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Maintenance Target Definitions )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Workspace Maintenance)
# Module:        build.cmake.maintenance
# Mission:       Define surgical custom targets for workspace sanitation.
# ==============================================================================

# ------------------------------------------------------------------------------
# Sub-Assembly 3.1: Transient Test Scratch Sanitization Target
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Layer 4 filesystem, VFS, and master loader tests create sandboxed mounts
#   and temporary test files in build/scratch and build/. This target cleans up
#   all transient artifacts without blowing away CMake cache or compiler artifacts.
# ------------------------------------------------------------------------------
add_custom_target(clean-scratch
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "${CMAKE_BINARY_DIR}/scratch"
        "${CMAKE_BINARY_DIR}/test_mount_root"
        "${CMAKE_BINARY_DIR}/test_nested_dir"
        "${CMAKE_BINARY_DIR}/system_test_patch"
        "${CMAKE_BINARY_DIR}/test_io_func_out.tmp"
        "${CMAKE_BINARY_DIR}/unit_fs_test.tmp"
        "${CMAKE_BINARY_DIR}/test_fs_atomic_dir"
        "${CMAKE_BINARY_DIR}/test_fs_ro_blocked.tmp"
        "${CMAKE_BINARY_DIR}/test_corrupt_fs_pol.json"
        "${CMAKE_BINARY_DIR}/test_fs_emergence.txt"
        "${CMAKE_BINARY_DIR}/unit_io_fb_write.tmp"
        "${CMAKE_BINARY_DIR}/unit_io_fb_ro_must_not_exist.tmp"
        "${CMAKE_BINARY_DIR}/unit_io_fb_ro_dir_must_not_exist"
        "${CMAKE_BINARY_DIR}/test_io_int_ro.tmp"
        "${CMAKE_BINARY_DIR}/test.tmp"
        "${CMAKE_BINARY_DIR}/fs_utils_ro_blocked.tmp"
        "${CMAKE_BINARY_DIR}/fs_utils_ro_dir_blocked"
        "${CMAKE_BINARY_DIR}/test_pf_sym_target.txt"
        "${CMAKE_BINARY_DIR}/test_pf_sym1.txt"
        "${CMAKE_BINARY_DIR}/test_pf_sym2.txt"
        "${CMAKE_BINARY_DIR}/test_corrupt_vfs_routes.json"
        "${CMAKE_BINARY_DIR}/ml_bad_contract_test.json"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/scratch"
    COMMENT "Purging transient test scratch directories and temporary files"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.2: Rotated Telemetry Log Sanitization Target
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Allows developers and CI to purge rotated log archives (e.g., .log.1, .log.2)
#   accumulated during extended playtests while preserving the log folder structure.
# ------------------------------------------------------------------------------
add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E rm -f
        "${CMAKE_BINARY_DIR}/logs/tenebris_lux_dev.log.*"
        "${CMAKE_BINARY_DIR}/logs/tenebris_lux_fallback.log"
    COMMENT "Purging rotated telemetry logs while preserving log directories"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.3: Compiled Test Binary Sanitization Target
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Allows purging test binaries from build/tests/ to save disk space without
#   invalidating the primary game executable (build/bin/tenebris-lux).
# ------------------------------------------------------------------------------
add_custom_target(clean-tests
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/tests"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/tests"
    COMMENT "Purging compiled test binaries from build/tests/"
)

# ------------------------------------------------------------------------------
# Sub-Assembly 3.4: Unified Transient Artifact Sanitization Target
# ------------------------------------------------------------------------------
# Architectural Rationale ("Why"):
#   Composite target running clean-scratch, clean-logs, and clean-tests together.
# ------------------------------------------------------------------------------
add_custom_target(clean-all-artifacts
    DEPENDS clean-scratch clean-logs clean-tests
    COMMENT "Purging all transient test binaries, logs, and scratch artifacts"
)

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Invariant Check: Operations Bounded to CMAKE_BINARY_DIR )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Workspace Maintenance)
# Module:        build.cmake.maintenance
# Mission:       Audit maintenance command safety and path containment.
# ==============================================================================
# Invariant Affirmation: All commands strictly operate within CMAKE_BINARY_DIR.

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Maintenance Handshake )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 0 Infrastructure (Workspace Maintenance)
# Module:        build.cmake.maintenance
# Mission:       Affirm maintenance targets registered in build graph.
# ==============================================================================
