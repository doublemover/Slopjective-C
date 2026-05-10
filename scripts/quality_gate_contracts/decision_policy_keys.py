from __future__ import annotations

EVIDENCE_ITEM_KEYS = ("evidence_id", "status", "summary", "blocking_refs")
EV_ARTIFACT_MAPPING_KEYS = ("evidence_id", "artifact_path")
GATE_RESULT_KEYS = ("gate_id", "status", "rationale")
CONSUMER_HANDOFF_KEYS = (
    "consumer_seed",
    "required_inputs",
    "handoff_state",
    "handoff_note",
)
VALID_EVIDENCE_STATUSES = {"pass", "fail"}
VALID_GATE_STATUSES = {"pass", "conditional-pass", "fail", "blocked"}

__all__ = [
    "CONSUMER_HANDOFF_KEYS",
    "EVIDENCE_ITEM_KEYS",
    "EV_ARTIFACT_MAPPING_KEYS",
    "GATE_RESULT_KEYS",
    "VALID_EVIDENCE_STATUSES",
    "VALID_GATE_STATUSES",
]
