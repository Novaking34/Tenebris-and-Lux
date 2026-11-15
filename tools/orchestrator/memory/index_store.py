"""
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
@project       Tenebris and Lux
@file          tools/orchestrator/memory/index_store.py
@id            omnicode.tools.memory.index_store
@layer         Layer 2: Memory & Context Cogs
@version       0.4.0-dev
@interface_rev Rev 5
@date          2026-11-13
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@system        OmniCode Orchestrator // Addressed Index Memory Store
@role          Track 4 Virtual Address Space for Episodic Turns and Memory Paging
@invariants    1. Turns stored with permanent monotonic integer addresses (#1, #2...).
               2. Pinned entries are guaranteed immune from eviction.
               3. Sparse context paging bounds working frame memory.
================================================================================
"""

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ==============================================================================
import re
import time
import os
from typing import List, Dict, Any, Tuple, Set, Optional
from ..types.state import EpisodicNode
from .symbol_indexer import extract_code_symbols

MAX_PAGED_CONTEXT_TOKENS: int = 4096

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ==============================================================================
class EpisodicIndexStore:
    def __init__(self):
        self.entries: List[EpisodicNode] = []

    def record_turn(self, user_prompt: str, assistant_response: str, files_used: List[str]) -> int:
        idx = len(self.entries) + 1
        timestamp = time.strftime("%H:%M:%S")
        first_line = user_prompt.strip().split("\n")[0][:75]

        symbols = extract_code_symbols(user_prompt + " " + assistant_response[:300])
        token_est = (len(user_prompt) + len(assistant_response)) // 4

        node = EpisodicNode(
            idx=idx,
            timestamp=timestamp,
            topic=first_line,
            symbols=symbols,
            files=files_used,
            pinned=False,
            token_est=token_est,
            user_prompt=user_prompt,
            assistant_response=assistant_response
        )
        self.entries.append(node)
        return idx

    def pin_entry(self, idx: int) -> bool:
        for e in self.entries:
            if e.idx == idx:
                e.pinned = True
                return True
        return False

    def unpin_entry(self, idx: int) -> bool:
        for e in self.entries:
            if e.idx == idx:
                e.pinned = False
                return True
        return False

    def drop_entry(self, idx: int) -> bool:
        for i, e in enumerate(self.entries):
            if e.idx == idx:
                del self.entries[i]
                return True
        return False

    def get_entries(self) -> List[EpisodicNode]:
        return list(self.entries)

    def total_tokens(self) -> int:
        return sum(e.token_est for e in self.entries)

    def pinned_count(self) -> int:
        return sum(1 for e in self.entries if e.pinned)

    def format_index_table(self) -> str:
        if not self.entries:
            return "No indexed memory nodes in this session yet."

        lines = [
            "┌─ Addressed Session Index (Track 4 Episodic Memory) ──────────────────────────────────┐",
            f"│ Total Nodes: {len(self.entries):<3} │ Total Tokens: ~{self.total_tokens():<6} │ Pinned: {self.pinned_count():<2}                            │",
            "├─────┬──────────┬──────────┬──────────────────────────────────────┬───────────────────┤",
            "│ ID  │ TIME     │ STATE    │ TOPIC / INVARIANT                    │ SYMBOLS / FILES   │",
            "├─────┼──────────┼──────────┼──────────────────────────────────────┼───────────────────┤"
        ]

        for e in self.entries:
            state = "[PINNED] " if e.pinned else "[INDEXED]"
            topic = (e.topic[:36] + "..") if len(e.topic) > 36 else e.topic.ljust(38)
            kw = ", ".join(e.symbols[:3])
            if e.files:
                kw += f" @{os.path.basename(e.files[0])}"
            kw_disp = (kw[:17] + "..") if len(kw) > 17 else kw.ljust(19)
            lines.append(f"│ #{e.idx:<2}  │ {e.timestamp} │ {state} │ {topic} │ {kw_disp} │")

        lines.append("└─────┴──────────┴──────────┴──────────────────────────────────────┴───────────────────┘")
        return "\n".join(lines)

    def assemble_sparse_payload(self, current_prompt: str) -> Tuple[str, List[int], int]:
        if not self.entries:
            est_tokens = len(current_prompt) // 4
            return current_prompt, [], est_tokens

        explicit_ids = set(int(m) for m in re.findall(r'#(\d+)', current_prompt))
        current_symbols = set(extract_code_symbols(current_prompt))

        active_ids: Set[int] = set()
        active_ids.update(explicit_ids)

        for e in self.entries:
            if e.pinned:
                active_ids.add(e.idx)

        for e in reversed(self.entries[:-1]):
            overlap = set(e.symbols).intersection(current_symbols)
            if len(overlap) >= 2:
                active_ids.add(e.idx)

        # Directory Table (TLB)
        dir_lines = ["=== SESSION INDEX DIRECTORY (Track 4 Address Space) ==="]
        for e in self.entries:
            tag = "[PINNED]" if e.pinned else "[INDEXED]"
            sym_str = ", ".join(e.symbols[:4]) if e.symbols else "none"
            dir_lines.append(f"Node #{e.idx} {tag} [{e.timestamp}]: {e.topic} (Symbols: {sym_str})")
        dir_lines.append("=== END INDEX DIRECTORY ===\n")

        paged_blocks = []
        budget_used = 0

        for e in self.entries:
            if e.idx in active_ids:
                if budget_used + e.token_est > MAX_PAGED_CONTEXT_TOKENS and not e.pinned:
                    paged_blocks.append(
                        f"/* MEMORY #{e.idx} [CONDENSED SUMMARY] */\n"
                        f"Topic: {e.topic}\n"
                        f"Symbols: {', '.join(e.symbols)}\n"
                        f"Excerpt: {e.assistant_response[:250]}...\n"
                        f"/* END MEMORY #{e.idx} */"
                    )
                else:
                    budget_used += e.token_est
                    paged_blocks.append(
                        f"/* MEMORY #{e.idx} [FULL WORKING FRAME] */\n"
                        f"User: {e.user_prompt}\n\n"
                        f"Solution:\n{e.assistant_response}\n"
                        f"/* END MEMORY #{e.idx} */"
                    )

        last_turn = self.entries[-1]
        prior_context = []
        if last_turn.idx not in active_ids:
            prior_context.append(
                f"/* IMMEDIATE PRIOR TURN (#{last_turn.idx}) */\n"
                f"User: {last_turn.topic}\n"
                f"Response: {last_turn.assistant_response[:250]}...\n"
                f"/* END PRIOR TURN */"
            )

        sections = ["\n".join(dir_lines)]
        if prior_context:
            sections.extend(prior_context)
        if paged_blocks:
            sections.extend(paged_blocks)
        sections.append(f"ACTIVE OPERATOR PROMPT:\n{current_prompt}")

        composed = "\n\n".join(sections)
        estimated_total_tokens = len(composed) // 4
        return composed, sorted(list(active_ids)), estimated_total_tokens

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ==============================================================================
def self_audit() -> bool:
    store = EpisodicIndexStore()
    idx = store.record_turn("prompt", "response", [])
    return idx == 1 and store.pin_entry(1) and store.pinned_count() == 1

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ==============================================================================
if __name__ == "__main__":
    ok = self_audit()
    print(f"Index store audit: {'PASS' if ok else 'FAIL'}")

