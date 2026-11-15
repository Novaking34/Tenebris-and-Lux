"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/probes/cpu_probe.py
@id            omnicode.tools.probes.cpu
@layer         Layer 1: Hardware & System Probes
@version       0.7.2-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // CPU Topology & Threading Calibrator
@role          Discovers P-Core / E-Core Topologies, AVX2, and Dynamic Max N Thread Capacity
@invariants    1. Zero hardcoded thread limits (Max is dynamic physical N).
               2. Direct sysfs inspection of core frequencies and topology.
               3. Separates P-core burst pool (5.4 GHz) from E-core pool (4.0 GHz).
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import os
import re
import glob
from typing import Dict, Any, List

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
def probe_cpu_topology() -> Dict[str, Any]:
    """
    Directly inspects Linux sysfs and /proc/cpuinfo to determine the true hardware
    topology: total threads N, P-core vs E-core layout, and frequency tiers.
    """
    model_name = "Intel Core Processor"
    cores = 0
    total_threads = os.cpu_count() or 1
    has_avx2 = False

    # 1. Parse /proc/cpuinfo
    if os.path.isfile("/proc/cpuinfo"):
        try:
            with open("/proc/cpuinfo", "r", encoding="utf-8") as f:
                content = f.read()

            for line in content.splitlines():
                if "model name" in line and model_name == "Intel Core Processor":
                    model_name = line.split(":", 1)[1].strip()
                if "cpu cores" in line and cores == 0:
                    try:
                        cores = int(line.split(":", 1)[1].strip())
                    except ValueError:
                        pass
                if "flags" in line and not has_avx2:
                    if "avx2" in line.lower():
                        has_avx2 = True
        except Exception:
            pass

    # 2. Direct sysfs frequency probe for P-core vs E-core classification
    cpu_freqs: Dict[int, int] = {}
    for p in glob.glob("/sys/devices/system/cpu/cpu[0-9]*/cpufreq/cpuinfo_max_freq"):
        m = re.search(r"cpu(\d+)/cpufreq", p)
        if m:
            cpu_num = int(m.group(1))
            try:
                with open(p, "r", encoding="utf-8") as f:
                    cpu_freqs[cpu_num] = int(f.read().strip())
            except Exception:
                pass

    if cpu_freqs:
        max_f = max(cpu_freqs.values())
        min_f = min(cpu_freqs.values())
        p_threads = [c for c in sorted(cpu_freqs.keys()) if cpu_freqs[c] == max_f]
        e_threads = [c for c in sorted(cpu_freqs.keys()) if cpu_freqs[c] < max_f]
        p_freq_ghz = round(max_f / 1000000.0, 1)
        e_freq_ghz = round(min_f / 1000000.0, 1)
    else:
        # Fallback if sysfs cpufreq is masked
        p_threads = list(range(min(4, total_threads)))
        e_threads = list(range(len(p_threads), total_threads))
        p_freq_ghz = 5.4
        e_freq_ghz = 4.0

    if cores == 0:
        cores = len(set(p_threads)) // 2 + len(e_threads) if total_threads >= 10 else total_threads

    # Dynamic Capacity Calculations (Max = N)
    max_capacity_n = total_threads
    min_capacity = 1
    p_burst_threads = len(p_threads) if p_threads else 4
    hybrid_balanced_threads = min(max_capacity_n, p_burst_threads + (len(e_threads) // 2))

    return {
        "model_name": model_name,
        "physical_cores": cores,
        "logical_threads": total_threads,
        "max_capacity_n": max_capacity_n,
        "min_threads": min_capacity,
        "p_threads": p_threads,
        "e_threads": e_threads,
        "p_thread_count": len(p_threads),
        "e_thread_count": len(e_threads),
        "p_freq_ghz": p_freq_ghz,
        "e_freq_ghz": e_freq_ghz,
        "has_avx2": has_avx2,
        # Optimal tiers based on physical topology:
        "optimal_p_burst": p_burst_threads,
        "optimal_hybrid": hybrid_balanced_threads,
        "optimal_full_n": max_capacity_n,
        "optimal_threads": hybrid_balanced_threads  # Dynamic default
    }

def get_cpu_load() -> float:
    """Estimates CPU utilization percentage from /proc/loadavg."""
    if os.path.isfile("/proc/loadavg"):
        try:
            with open("/proc/loadavg", "r", encoding="utf-8") as f:
                load_1m = float(f.read().split()[0])
            threads = os.cpu_count() or 1
            return min(100.0, (load_1m / threads) * 100.0)
        except Exception:
            pass
    return 0.0

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    topo = probe_cpu_topology()
    assert topo["logical_threads"] > 0
    assert topo["max_capacity_n"] >= topo["min_threads"]
    assert topo["p_thread_count"] > 0
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    t = probe_cpu_topology()
    print("=== LIVE CPU HARDWARE TOPOLOGY ===")
    print(f"Model:           {t['model_name']}")
    print(f"Physical Cores:  {t['physical_cores']} | Max Capacity N: {t['max_capacity_n']} threads")
    print(f"P-Threads (Fast): {t['p_threads']} ({t['p_freq_ghz']} GHz)")
    print(f"E-Threads (Eco):  {t['e_threads']} ({t['e_freq_ghz']} GHz)")
    print(f"Allocations:     P-Burst={t['optimal_p_burst']}, Hybrid={t['optimal_hybrid']}, Max N={t['optimal_full_n']}")
