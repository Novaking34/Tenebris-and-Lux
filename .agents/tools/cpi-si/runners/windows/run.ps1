# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       OmniCode Framework / Big House Ecosystem
# @file          runners/windows/run.ps1
# @id            cog.agent.tools.cpi_si_cli.runner.windows.ps1
# @layer         Layer 4: Cross-Platform Runner (Windows PowerShell 5.1 / 7+)
# @archetype     zero_clutter_runner | windows_powershell_entrypoint
# @version       0.1.3-dev
# @interface_rev Rev 5
# @created       2026-11-20
# @updated       2026-11-20
# @author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
# @repository    https://github.com/Novaking34/OmniCode_Framework
# @system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
# @role          Windows PowerShell Runner Caching in %LOCALAPPDATA%\cpi-si-cli
# @maturity      Rev 5 Operational Baseline | Wave A Tools Alignment
# @chassis       stateless-wrapper | Zero binary files written inside .agents/
# @scale         O(1) cached pass; O(N_go) atomic rebuild on Go source edits
# @seams         builds: ../../cmd/cpi-si/main.go | wrapped_by: ./run.cmd
# @invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5).
#                2. StrictMode-clean on both Windows PowerShell 5.1 & pwsh 7+.
#                3. Supports `-AuditOnly` for safe cross-platform verification.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Windows counterpart to `runners/run.sh`, keeping compiled binaries out
#   of `.agents/` by placing `cpi-si.exe` under `%LOCALAPPDATA%\cpi-si-cli`.
#
#   ### For Architects, Operators & Non-Programmers:
#   Run `.\runners\windows\run.ps1 boot` (or via `run.cmd`) on any Windows 10/11
#   machine and get the exact same CPI-SI BIOS workflow without `.exe` clutter.
#
#   ### For Systems Programmers & Engine Architects:
#   Avoids direct `$env:VAR` failures under `Set-StrictMode -Version Latest`
#   by querying `[Environment]::GetEnvironmentVariable` on Windows & Linux pwsh.
# ==============================================================================
[CmdletBinding()]
param(
    [switch]$AuditOnly,
    [switch]$Rebuild,
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$CliArgs = @()
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( Substrate Root Resolution & External Windows Cache Directory )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (Windows PowerShell)
# Module:        cog.agent.tools.cpi_si_cli.runner.windows.ps1
# Mission:       Resolve TOOL_ROOT, AGENTS_ROOT, and external binary path.
# ==============================================================================

$ToolRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$AgentsRoot = (Resolve-Path (Join-Path $ToolRoot "..\..")).Path

if ([string]::IsNullOrWhiteSpace([Environment]::GetEnvironmentVariable("CPI_SI_AGENTS_DIR"))) {
    [Environment]::SetEnvironmentVariable("CPI_SI_AGENTS_DIR", $AgentsRoot, "Process")
}

$CacheOverride = [Environment]::GetEnvironmentVariable("CPI_SI_CACHE_DIR")
if (-not [string]::IsNullOrWhiteSpace($CacheOverride)) {
    $CacheRoot = $CacheOverride
} else {
    $LocalApp = [Environment]::GetEnvironmentVariable("LOCALAPPDATA")
    if (-not [string]::IsNullOrWhiteSpace($LocalApp)) {
        $CacheRoot = Join-Path $LocalApp "cpi-si-cli"
    } else {
        $CacheRoot = Join-Path ([System.IO.Path]::GetTempPath()) "cpi-si-cli"
    }
}

$IsWinOS = ([Environment]::GetEnvironmentVariable("OS") -eq "Windows_NT")
$BinName = if ($IsWinOS) { "cpi-si.exe" } else { "cpi-si" }
$BinPath = Join-Path $CacheRoot $BinName

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Audit Mode & Timestamp Staleness Verification )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (Windows PowerShell)
# Module:        cog.agent.tools.cpi_si_cli.runner.windows.ps1
# Mission:       Validate repository layout cleanly or rebuild stale binary.
# ==============================================================================

$GoModPath = Join-Path $ToolRoot "go.mod"
$MainGoPath = Join-Path (Join-Path (Join-Path $ToolRoot "cmd") "cpi-si") "main.go"

if ($AuditOnly) {
    if (-not (Test-Path $GoModPath) -or -not (Test-Path $MainGoPath)) {
        Write-Error "[FAIL] windows/run.ps1 audit: missing go.mod or cmd/cpi-si/main.go under $ToolRoot"
        exit 1
    }
    $GoCmd = Get-Command "go" -ErrorAction SilentlyContinue
    if ($null -eq $GoCmd) {
        Write-Error "[FAIL] windows/run.ps1 audit: 'go' compiler missing from PATH"
        exit 127
    }
    Write-Output "[PASS] runners/windows/run.ps1 audit OK (tool_root=$ToolRoot, cache_bin=$BinPath)"
    exit 0
}

$RebuildEnv = [Environment]::GetEnvironmentVariable("CPI_SI_REBUILD")
$NeedBuild = $false
if ($Rebuild -or ($RebuildEnv -eq "1") -or (-not (Test-Path $BinPath))) {
    $NeedBuild = $true
} else {
    $BinTime = (Get-Item $BinPath).LastWriteTimeUtc
    $NewerSrc = Get-ChildItem -Path $ToolRoot -Recurse -File |
        Where-Object { ($_.Extension -eq ".go" -or $_.Name -eq "go.mod") -and ($_.LastWriteTimeUtc -gt $BinTime) } |
        Select-Object -First 1
    if ($null -ne $NewerSrc) {
        $NeedBuild = $true
    }
}

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES BLOCK
# ( Go Compiler Verification & Atomic External Cache Build )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (Windows PowerShell)
# Module:        cog.agent.tools.cpi_si_cli.runner.windows.ps1
# Mission:       Compile cmd/cpi-si safely into external cache directory.
# ==============================================================================

if ($NeedBuild) {
    $GoCmd = Get-Command "go" -ErrorAction SilentlyContinue
    if ($null -eq $GoCmd) {
        Write-Error "cpi-si windows runner error: 'go' toolchain not found in PATH"
        exit 127
    }
    New-Item -ItemType Directory -Path $CacheRoot -Force | Out-Null
    $TmpBin = "$BinPath.tmp.$PID"
    Push-Location $ToolRoot
    try {
        & go build -o $TmpBin ./cmd/cpi-si
        if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
        }
    } finally {
        Pop-Location
    }
    Move-Item -Path $TmpBin -Destination $BinPath -Force
}

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( Delegate Invocation With Propagated Exit Code )
# ==============================================================================
# Engine:        CPI-SI Portable Cognitive BIOS Tooling (cpi-si-cli)
# Stratum:       Layer 4: Cross-Platform Runner (Windows PowerShell)
# Module:        cog.agent.tools.cpi_si_cli.runner.windows.ps1
# Mission:       Execute external binary and propagate exact exit code.
# ==============================================================================

& $BinPath @CliArgs
exit $LASTEXITCODE
# END OF BLOCK 5: CLOSING SEAL
