"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/server/service_adapter.py
@id            omnicode.tools.server.service_adapter
@layer         Layer 3: Service Layer & Compatibility Adapters
@version       0.5.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Service Model Adapter (OpenAI / Ollama Wire Compatibility)
@role          Exposes Fall-Down-Protected Inference to External AI Tools (Continue, VS Code, Cursor)
@invariants    1. Zero external pip dependencies (uses built-in http.server).
               2. Conforms to standard OpenAI /v1/chat/completions and Ollama /api/chat wire protocols.
               3. Seamlessly applies Fall-Down Ladder behind the scenes without breaking external tool sessions.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import json
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from typing import Dict, Any, List, Optional
import urllib.request
import urllib.error
from ..agent.reflection_loop import stream_model_response
from ..cloud.cloud_bridge import CloudNexusBridge
from ..calibration.sweet_spot import calculate_hardware_sweet_spot

SERVICE_PORT = 11435

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class ServiceModelRequestHandler(BaseHTTPRequestHandler):
    """
    HTTP Request Handler exposing OpenAI and Ollama wire compatibility.
    External services connect here; internal Fall-Down routing protects them.
    """

    cloud_bridge = CloudNexusBridge()

    def do_GET(self):
        """Health check and model tags listing."""
        if self.path in ("/api/tags", "/v1/models"):
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            models_payload = {
                "models": [
                    {"name": "omnicode-scaffold-0.5b", "details": {"family": "qwen2"}},
                    {"name": "omnicode-scaffold-1.5b", "details": {"family": "qwen2"}},
                    {"name": "omnicode-scaffold-3b", "details": {"family": "qwen2"}},
                    {"name": "omnicode-scaffold-7b", "details": {"family": "qwen2"}},
                    {"name": "omnicode-cloud-14b", "details": {"family": "qwen2"}}
                ]
            }
            self.wfile.write(json.dumps(models_payload).encode("utf-8"))
        else:
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status": "OmniCode Service Adapter Online"}')

    def do_POST(self):
        """Inference routing for /v1/chat/completions or /api/chat."""
        content_len = int(self.headers.get("Content-Length", 0))
        raw_body = self.rfile.read(content_len).decode("utf-8")

        try:
            req_data = json.loads(raw_body)
        except Exception:
            self.send_error(400, "Invalid JSON payload")
            return

        messages = req_data.get("messages", [])
        stream_requested = req_data.get("stream", False)
        model_name = req_data.get("model", "omnicode-fast-1.5b")

        # Map external alias to local calibrated model if needed
        if "0.5" in model_name:
            active_model = "qwen2.5-coder:0.5b"
        elif "3" in model_name:
            active_model = "qwen2.5-coder:3b"
        elif "7" in model_name:
            active_model = "qwen2.5-coder:7b"
        else:
            active_model = "omnicode-fast-1.5b"

        if stream_requested:
            self._handle_streaming_response(messages, active_model)
        else:
            self._handle_sync_response(messages, active_model)

    def _handle_streaming_response(self, messages: List[Dict[str, str]], active_model: str):
        """Streams Server-Sent Events (SSE) back to the client."""
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream")
        self.send_header("Cache-Control", "no-cache")
        self.send_header("Connection", "keep-alive")
        self.end_headers()

        def sse_chunk_cb(token: str, is_think: bool):
            # Only stream clean output tokens to external service tools (omit raw thinking tags if preferred)
            if not is_think and token:
                chunk_payload = {
                    "choices": [{
                        "delta": {"content": token},
                        "finish_reason": None
                    }]
                }
                msg = f"data: {json.dumps(chunk_payload)}\n\n"
                try:
                    self.wfile.write(msg.encode("utf-8"))
                    self.wfile.flush()
                except Exception:
                    pass

        # Execute with Fall-Down protection
        resp, telem = stream_model_response(
            messages=messages,
            model_name=active_model,
            chunk_callback=sse_chunk_cb
        )

        # Emit completion chunk
        final_payload = {"choices": [{"delta": {}, "finish_reason": "stop"}]}
        self.wfile.write(f"data: {json.dumps(final_payload)}\n\ndata: [DONE]\n\n".encode("utf-8"))
        self.wfile.flush()

    def _handle_sync_response(self, messages: List[Dict[str, str]], active_model: str):
        """Handles non-streaming synchronous request."""
        resp, telem = stream_model_response(
            messages=messages,
            model_name=active_model
        )
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        res_payload = {
            "choices": [{
                "message": {"role": "assistant", "content": resp or ""},
                "finish_reason": "stop"
            }],
            "usage": {
                "total_tokens": telem.get("tokens", 0)
            }
        }
        self.wfile.write(json.dumps(res_payload).encode("utf-8"))

def launch_service_adapter(port: int = SERVICE_PORT) -> HTTPServer:
    """Starts the Service Adapter HTTP daemon."""
    server_address = ("127.0.0.1", port)
    httpd = HTTPServer(server_address, ServiceModelRequestHandler)
    print(f"OmniCode Service Adapter listening on http://127.0.0.1:{port} (Wire-compatible with Continue / VS Code)")
    return httpd

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    return SERVICE_PORT == 11435

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    server = launch_service_adapter()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down Service Adapter.")
        server.server_close()

