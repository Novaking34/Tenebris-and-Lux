"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/cloud/colab_runner.py
@id            omnicode.tools.cloud.colab_runner
@layer         Layer 2: Cloud Nexus Automation
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Free Cloud Nexus Setup Script (Google Colab / Kaggle)
@role          Launches Remote Ollama with Free T4/A100 GPU and Cloudflare Tunnel
@invariants    1. Zero cost: runs entirely within Google Colab free compute tiers.
               2. Exposes secure public tunnel without opening firewall ports.
               3. Pulls Qwen 2.5-Coder 14B or 32B for online peak stress testing.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
# INSTRUCTIONS FOR RUNNING IN GOOGLE COLAB / KAGGLE:
# 1. Open Google Colab (https://colab.research.google.com/)
# 2. Select: Runtime -> Change runtime type -> T4 GPU (or A100 if available)
# 3. Paste and run this script in a code cell.
# 4. Copy the printed Cloudflare URL and paste into tools/orchestrator/config/cloud_nexus.json

COLAB_SHELL_RECIPE = r"""
# 1. Install Ollama and Cloudflare Tunnel
!curl -fsSL https://ollama.com/install.sh | sh
!wget -q -nc https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64.deb
!dpkg -i cloudflared-linux-amd64.deb

# 2. Launch Ollama Server in Background
import subprocess, time
ollama_proc = subprocess.Popen(["ollama", "serve"])
time.sleep(4)

# 3. Pull Peak Model (14B or 32B)
print("=== Pulling Peak Model (Qwen2.5-Coder 14B) on Free Cloud GPU ===")
!ollama pull qwen2.5-coder:14b

# 4. Start Cloudflare Tunnel and Print Remote URL
print("=== Starting Cloudflare Tunnel ===")
tunnel_proc = subprocess.Popen(
    ["cloudflared", "tunnel", "--url", "http://localhost:11434"],
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True
)

for line in tunnel_proc.stdout:
    if "trycloudflare.com" in line:
        for word in line.split():
            if "trycloudflare.com" in word:
                print("\n=======================================================")
                print(f"SUCCESS! YOUR FREE CLOUD NEXUS ENDPOINT IS:")
                print(f"  {word.strip()}")
                print("Paste this URL into tools/orchestrator/config/cloud_nexus.json")
                print("under: providers.free_colab.remote_tunnel_url")
                print("=======================================================\n")
                break
        break
"""

if __name__ == "__main__":
    print("OmniCode Free Cloud Nexus Recipe:")
    print(COLAB_SHELL_RECIPE)

