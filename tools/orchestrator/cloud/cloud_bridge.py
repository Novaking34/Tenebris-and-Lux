"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/cloud/cloud_bridge.py
@id            omnicode.tools.cloud.cloud_bridge
@layer         Layer 2: Cloud Nexus Cogs
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Cloud Nexus Bridge & Fall-Down Router
@role          Manages Remote Inference, Health Pings, and Autonomous Step-Down
@invariants    1. Zero external pip dependencies (pure urllib).
               2. Fallback to local bedrock (1.5B/3B) is automatic upon any network fault.
               3. Paid cloud infrastructure is strictly dormant unless explicitly enabled.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import json
import time
import urllib.request
import urllib.error
from typing import Dict, Any, Optional, Tuple, List
from ..types.config_schema import load_json_block_3

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class CloudNexusBridge:
    """
    Client bridge that handles communication with a remote cloud server
    (Colab free GPU or dedicated server) and executes the Fall-Down ladder.
    """

    def __init__(self, config_path: str = "tools/orchestrator/config/cloud_nexus.json"):
        self.config_path = config_path
        self._load_config()

    def _load_config(self):
        try:
            body = load_json_block_3(self.config_path)
            self.cloud_enabled = body.get("cloud_enabled", False)
            self.active_provider_key = body.get("active_provider", "free_colab")
            self.providers = body.get("providers", {})
            self.fall_down_ladder = body.get("fall_down_ladder", [])
        except Exception:
            self.cloud_enabled = False
            self.active_provider_key = "free_colab"
            self.providers = {}
            self.fall_down_ladder = []

    def ping_cloud_server(self) -> Tuple[bool, float, str]:
        """
        Pings active cloud server. Returns (is_alive, latency_ms, status_message).
        """
        if not self.cloud_enabled:
            return False, 0.0, "Cloud compute is disabled in config."

        prov = self.providers.get(self.active_provider_key, {})
        url = prov.get("remote_tunnel_url") or prov.get("endpoint_url")
        if not url:
            return False, 0.0, "No endpoint URL configured for active cloud provider."

        start = time.time()
        try:
            ping_url = f"{url.rstrip('/')}/api/tags"
            req = urllib.request.Request(ping_url)
            with urllib.request.urlopen(req, timeout=3.0) as resp:
                elapsed_ms = (time.time() - start) * 1000.0
                if resp.status == 200:
                    return True, round(elapsed_ms, 1), f"Connected to {prov.get('name')} ({round(elapsed_ms, 1)}ms)"
                return False, round(elapsed_ms, 1), f"Unexpected HTTP status {resp.status}"
        except Exception as e:
            elapsed_ms = (time.time() - start) * 1000.0
            return False, round(elapsed_ms, 1), f"Cloud unreachable: {str(e)}"

    def execute_with_fall_down(
        self,
        messages: List[Dict[str, str]],
        local_streaming_fn: Any,
        chunk_callback: Optional[Any] = None,
        requested_tier: Optional[str] = None
    ) -> Tuple[Optional[str], Dict[str, Any]]:
        """
        Attempts execution via Cloud Nexus (Peak Form).
        Adapts cloud model based on requested_tier (14b, 32b, 72b).
        If offline or failed, steps down the ladder to local offline form.
        """
        # Step 1: Attempt Cloud Peak if enabled
        if self.cloud_enabled:
            # Select target model based on requested tier or provider default
            tier_model_map = {
                "14b": "qwen2.5-coder:14b",
                "32b": "qwen2.5-coder:32b",
                "72b": "qwen2.5:72b"
            }
            target_model = tier_model_map.get(requested_tier) if requested_tier else None

            prov = self.providers.get(self.active_provider_key, {})
            cloud_url = prov.get("remote_tunnel_url") or prov.get("endpoint_url")
            if not target_model:
                target_model = prov.get("target_model", "qwen2.5-coder:14b")

            if cloud_url:
                try:
                    payload = {
                        "model": target_model,
                        "messages": messages,
                        "stream": False,
                        "options": {"temperature": 0.3}
                    }
                    req_data = json.dumps(payload).encode("utf-8")
                    req = urllib.request.Request(
                        f"{cloud_url.rstrip('/')}/api/chat",
                        data=req_data,
                        headers={"Content-Type": "application/json"}
                    )
                    with urllib.request.urlopen(req, timeout=prov.get("timeout_seconds", 6.0)) as resp:
                        res = json.loads(resp.read().decode("utf-8"))
                        content = res.get("message", {}).get("content", "")
                        return content, {
                            "source": "CLOUD_PEAK",
                            "provider": prov.get("name"),
                            "model": target_model,
                            "tier": requested_tier or "14b",
                            "fell_down": False
                        }
                except Exception as e:
                    # Cloud failed -> tripwire triggers autonomous step-down
                    if chunk_callback:
                        chunk_callback(f"\n[TRIPWIRE: Cloud server offline ({str(e)}). Stepping down to Local Sovereign Bedrock...]\n\n", True)

        # Step 2: Fall down to Local Bedrock (1.5B or 3B)
        resp, telem = local_streaming_fn(messages=messages, chunk_callback=chunk_callback)
        telem["fell_down"] = True
        telem["source"] = "LOCAL_BEDROCK"
        return resp, telem

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    bridge = CloudNexusBridge()
    alive, _, _ = bridge.ping_cloud_server()
    assert alive is False  # Expected False when cloud is disabled by default
    return True

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    b = CloudNexusBridge()
    alive, lat, msg = b.ping_cloud_server()
    print(f"Cloud Bridge Status: alive={alive}, latency={lat}ms, msg='{msg}'")

