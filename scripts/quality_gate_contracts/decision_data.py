from __future__ import annotations

from .reports import QualityGateReportInputs

CONTRACT_ID = "V013-CONF-02-QUALITY-GATE-v2"
SEED_ID = "V013-CONF-02"
ACCEPTANCE_GATE_ID = "AC-V013-CONF-02"
TASK_ID = "D-05"
RELEASE_LABEL = "v0.11"
RELEASE_ID = "20260223-issue713-lanea-012"
SOURCE_REVISION = "50c106ed1e0392d5b7820672ce7c3f96f1f0f9c8"
VALIDATED_BY = "worker-lane-b"

ACTIVE_EXCEPTION_IDS: list[str] = []

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

EV_ARTIFACT_MAPPING: tuple[dict[str, str], ...] = (
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

EVIDENCE_ITEMS = [
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
]

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

BASE_GATE_RESULTS = [
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
]

THRESHOLD_RESULTS = [
    {
        "threshold_id": "CT-04",
        "gate_id": "QG-01",
        "observed_metric": "high_or_critical_open_blockers=3",
        "pass_threshold": "==0",
        "status": "fail",
        "evidence_refs": ["reports/conformance/dashboard_v011.status.json#summary.blocker_counts"],
    },
    {
        "threshold_id": "FR-01",
        "gate_id": "QG-01",
        "observed_metric": "dashboard_age_hours=2.25",
        "pass_threshold": "<=24",
        "status": "pass",
        "evidence_refs": ["reports/conformance/dashboard_v011.status.json#generated_at"],
    },
    {
        "threshold_id": "FR-02",
        "gate_id": "QG-01",
        "observed_metric": "seeded_conformance_age_hours=4.17",
        "pass_threshold": "<=24",
        "status": "pass",
        "evidence_refs": ["reports/conformance/dashboard_v011.status.json#dependencies"],
    },
    {
        "threshold_id": "FR-03",
        "gate_id": "QG-03",
        "observed_metric": "rerun_digest_age_hours=2.25",
        "pass_threshold": "<=72",
        "status": "pass",
        "evidence_refs": ["reports/conformance/reproducibility/v011_rerun_digest_report.md"],
    },
    {
        "threshold_id": "FR-04",
        "gate_id": "QG-04",
        "observed_metric": "exception_ledger_age_hours=0.00",
        "pass_threshold": "<=24",
        "status": "pass",
        "evidence_refs": ["reports/releases/v011_quality_gate_exceptions.md"],
    },
    {
        "threshold_id": "RT-05",
        "gate_id": "QG-03",
        "observed_metric": "cross_run_verdict_consistency=100%",
        "pass_threshold": "==100%",
        "status": "pass",
        "evidence_refs": ["reports/conformance/reproducibility/v011_rerun_digest_report.md#2-rerun-matrix"],
    },
]

UNRESOLVED_BLOCKERS = [
    {
        "blocker_id": "BLK-189-01",
        "status": "OPEN",
        "owner": "D-LEAD",
        "due_date_utc": "2026-02-24",
        "due_path": "reports/releases/v011_readiness_dossier.md#scope-and-baseline",
        "summary": "Final dependency evidence links from D-06, D-08, D-10, and D-11 are pending.",
    },
    {
        "blocker_id": "BLK-189-02",
        "status": "OPEN",
        "owner": "RELEASE-LIAISON",
        "due_date_utc": "2026-02-24",
        "due_path": "reports/releases/v011_readiness_dossier.md#gate-decision-package",
        "summary": "Gate-decision approver signatures are not yet recorded for final recommendation publication.",
    },
    {
        "blocker_id": "BLK-189-03",
        "status": "OPEN",
        "owner": "D-OPS",
        "due_date_utc": "2026-02-23",
        "due_path": "docs/reference/legacy_spec_anchor_index.md#planning-issue-189-readiness-dossier-package",
        "summary": "Final commit-SHA slot for readiness closeout is not yet populated.",
    },
]

DOWNSTREAM_HANDOFFS = [
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
]

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

ACCEPTANCE_ROLLUP = [
    {
        "acceptance_id": "AC-V013-CONF-02-01",
        "status": "pass",
        "summary": "EV-06 exception ledger exists with required D-05 fields.",
    },
    {
        "acceptance_id": "AC-V013-CONF-02-02",
        "status": "pass",
        "summary": "EV-07 and EV-08 are regenerated deterministically from tooling output.",
    },
    {
        "acceptance_id": "AC-V013-CONF-02-03",
        "status": "pass",
        "summary": "Decision state, blocker posture, and active exception set are internally consistent.",
    },
    {
        "acceptance_id": "AC-V013-CONF-02-04",
        "status": "pass",
        "summary": "Downstream handoff notes for V013-CONF-03 and V013-REL-01 are explicit.",
    },
    {
        "acceptance_id": "AC-V013-CONF-02-05",
        "status": "pass",
        "summary": "AC-V013-CONF-02 rollup and unresolved blocker posture are recorded.",
    },
]

VALIDATION_COMMAND_REFS = [
    "python scripts/spec_lint.py",
    "python scripts/generate_quality_gate_decision.py",
    (
        "node -e \"const fs=require('fs'); "
        "JSON.parse(fs.readFileSync('reports/releases/v011_quality_gate_decision.status.json','utf8')); "
        "console.log('status-json: OK');\""
    ),
    (
        "rg -n \"EV-06|EV-07|EV-08|QG-04|recommendation_signal\" "
        "reports/releases/v011_quality_gate_decision.md "
        "reports/releases/v011_quality_gate_decision.status.json "
        "reports/releases/v011_quality_gate_exceptions.md"
    ),
]

REPORT_INPUTS = QualityGateReportInputs(
    contract_id=CONTRACT_ID,
    seed_id=SEED_ID,
    acceptance_gate_id=ACCEPTANCE_GATE_ID,
    task_id=TASK_ID,
    release_label=RELEASE_LABEL,
    release_id=RELEASE_ID,
    source_revision=SOURCE_REVISION,
    validated_by=VALIDATED_BY,
    validation_command_refs=VALIDATION_COMMAND_REFS,
    ev_artifact_mapping=EV_ARTIFACT_MAPPING,
    evidence_items=EVIDENCE_ITEMS,
    threshold_results=THRESHOLD_RESULTS,
    active_exception_ids=ACTIVE_EXCEPTION_IDS,
    unresolved_blockers=UNRESOLVED_BLOCKERS,
    downstream_handoffs=DOWNSTREAM_HANDOFFS,
    acceptance_rollup=ACCEPTANCE_ROLLUP,
)

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
BASE_GATE_SEQUENCE = tuple(str(row["gate_id"]) for row in BASELINE_GATE_CONTRACT)


