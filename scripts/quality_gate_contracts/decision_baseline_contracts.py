from __future__ import annotations

BASELINE_EVIDENCE_CONTRACT: tuple[dict[str, object], ...] = (
    {
        "evidence_id": "EV-06",
        "status": "pass",
        "summary": "Exception ledger is published with required D-05 fields and valid statuses.",
        "blocking_refs": [],
    },
    {
        "evidence_id": "EV-07",
        "status": "pass",
        "summary": "Human-readable gate decision record is published with QG-04 and handoff notes.",
        "blocking_refs": [],
    },
    {
        "evidence_id": "EV-08",
        "status": "fail",
        "summary": "Machine-readable gate decision remains no-go while BLK-189 blockers are open.",
        "blocking_refs": ["BLK-189-01", "BLK-189-02", "BLK-189-03"],
    },
)

BASELINE_EV_ARTIFACT_CONTRACT: tuple[dict[str, str], ...] = (
    {
        "evidence_id": "EV-06",
        "artifact_path": "reports/releases/v011_quality_gate_exceptions.md",
    },
    {
        "evidence_id": "EV-07",
        "artifact_path": "reports/releases/v011_quality_gate_decision.md",
    },
    {
        "evidence_id": "EV-08",
        "artifact_path": "reports/releases/v011_quality_gate_decision.status.json",
    },
)

BASELINE_GATE_CONTRACT: tuple[dict[str, str], ...] = (
    {
        "gate_id": "QG-01",
        "status": "fail",
        "rationale": "CT-04 failed: unresolved high/critical blocker count is 3 (threshold requires 0).",
    },
    {
        "gate_id": "QG-02",
        "status": "pass",
        "rationale": "Diagnostics stability evidence is present; no active diagnostics exception is required.",
    },
    {
        "gate_id": "QG-03",
        "status": "pass",
        "rationale": "Reproducibility rerun digest indicates stable replay outcomes for the locked snapshot.",
    },
)

BASELINE_CONSUMER_CONTRACT: tuple[dict[str, object], ...] = (
    {
        "consumer_seed": "V013-CONF-03",
        "required_inputs": ["EV-07", "EV-08"],
        "handoff_state": "ready-after-close",
        "handoff_note": (
            "Consume QG-04 + recommendation_signal from EV-07/EV-08 only after "
            "V013-CONF-02 is merged/closed; then finalize v0.12 dress rehearsal verdict publication."
        ),
    },
    {
        "consumer_seed": "V013-REL-01",
        "required_inputs": ["EV-07", "EV-08", "BLK-189 posture"],
        "handoff_state": "ready-after-close",
        "handoff_note": (
            "Use no-go decision state and unresolved BLK-189 posture as hard "
            "inputs to readiness dossier final recommendation gating."
        ),
    },
)

BASE_GATE_SEQUENCE = tuple(str(row["gate_id"]) for row in BASELINE_GATE_CONTRACT)

__all__ = [
    "BASELINE_CONSUMER_CONTRACT",
    "BASELINE_EV_ARTIFACT_CONTRACT",
    "BASELINE_EVIDENCE_CONTRACT",
    "BASELINE_GATE_CONTRACT",
    "BASE_GATE_SEQUENCE",
]
