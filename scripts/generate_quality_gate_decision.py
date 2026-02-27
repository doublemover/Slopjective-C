#!/usr/bin/env python3
"""Generate deterministic EV-06..EV-08 quality-gate decision artifacts."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence

DEFAULT_MD = Path("reports/releases/v011_quality_gate_decision.md")
DEFAULT_STATUS = Path("reports/releases/v011_quality_gate_decision.status.json")
DEFAULT_GENERATED_AT = "2026-02-23T22:00:00Z"

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
        "due_path": "spec/planning/issue_189_readiness_dossier_package.md#issue-189-closeout-comment-template",
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


class ContractDriftError(RuntimeError):
    """Raised when baseline contract rows are structurally valid but semantically drifted."""


class ContractHardFailError(RuntimeError):
    """Raised when baseline contract rows are malformed and not safely consumable."""


def require_mapping(value: object, *, context: str) -> dict[str, object]:
    if not isinstance(value, dict):
        raise ContractHardFailError(f"{context}: expected object")
    return value


def require_exact_keys(
    payload: dict[str, object],
    *,
    expected_keys: tuple[str, ...],
    context: str,
) -> None:
    missing = [key for key in expected_keys if key not in payload]
    unexpected = [key for key in payload if key not in expected_keys]
    if missing or unexpected:
        detail_parts: list[str] = []
        if missing:
            detail_parts.append(f"missing keys {missing}")
        if unexpected:
            detail_parts.append(f"unexpected keys {unexpected}")
        details = "; ".join(detail_parts)
        raise ContractHardFailError(f"{context}: schema mismatch ({details})")


def require_non_empty_string(
    payload: dict[str, object],
    *,
    key: str,
    context: str,
) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value.strip():
        raise ContractHardFailError(f"{context}.{key}: expected non-empty string")
    return value.strip()


def require_string_list(
    payload: dict[str, object],
    *,
    key: str,
    context: str,
) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise ContractHardFailError(f"{context}.{key}: expected array of strings")
    normalized: list[str] = []
    for index, item in enumerate(value):
        if not isinstance(item, str) or not item.strip():
            raise ContractHardFailError(
                f"{context}.{key}[{index}]: expected non-empty string"
            )
        normalized.append(item.strip())
    return normalized


def validate_evidence_contract(evidence_items: list[dict[str, object]]) -> None:
    expected_by_id = {
        str(row["evidence_id"]): row for row in BASELINE_EVIDENCE_CONTRACT
    }
    expected_order = tuple(str(row["evidence_id"]) for row in BASELINE_EVIDENCE_CONTRACT)
    seen_ids: set[str] = set()
    actual_order: list[str] = []

    if len(evidence_items) != len(expected_order):
        raise ContractDriftError(
            "evidence_items row-count drift: "
            f"expected {len(expected_order)}, found {len(evidence_items)}"
        )

    for index, raw_item in enumerate(evidence_items):
        context = f"evidence_items[{index}]"
        item = require_mapping(raw_item, context=context)
        require_exact_keys(item, expected_keys=EVIDENCE_ITEM_KEYS, context=context)
        evidence_id = require_non_empty_string(item, key="evidence_id", context=context)
        status = require_non_empty_string(item, key="status", context=context)
        summary = require_non_empty_string(item, key="summary", context=context)
        blocking_refs = require_string_list(item, key="blocking_refs", context=context)

        if status not in VALID_EVIDENCE_STATUSES:
            raise ContractHardFailError(
                f"{context}.status: unsupported evidence status '{status}'"
            )

        if evidence_id in seen_ids:
            raise ContractDriftError(f"duplicate evidence_id detected: {evidence_id}")
        seen_ids.add(evidence_id)
        actual_order.append(evidence_id)

        expected = expected_by_id.get(evidence_id)
        if expected is None:
            raise ContractDriftError(f"unexpected evidence_id: {evidence_id}")
        if status != expected["status"]:
            raise ContractDriftError(
                f"{evidence_id} status drift: expected {expected['status']}, found {status}"
            )
        expected_summary = str(expected["summary"])
        if summary != expected_summary:
            raise ContractDriftError(
                f"{evidence_id} summary drift: expected {expected_summary!r}, found {summary!r}"
            )
        expected_blocking_refs = list(expected["blocking_refs"])
        if blocking_refs != expected_blocking_refs:
            raise ContractDriftError(
                f"{evidence_id} blocking_refs drift: expected {expected_blocking_refs}, "
                f"found {blocking_refs}"
            )

    if tuple(actual_order) != expected_order:
        raise ContractDriftError(
            f"evidence_id ordering drift: expected {list(expected_order)}, found {actual_order}"
        )


def validate_ev_artifact_mapping_contract(
    ev_contract_mapping: Sequence[dict[str, str]],
) -> None:
    expected_order = tuple(
        str(row["evidence_id"]) for row in BASELINE_EV_ARTIFACT_CONTRACT
    )
    expected_by_id = {
        str(row["evidence_id"]): row for row in BASELINE_EV_ARTIFACT_CONTRACT
    }

    if len(ev_contract_mapping) != len(expected_order):
        raise ContractDriftError(
            "ev_contract_mapping row-count drift: "
            f"expected {len(expected_order)}, found {len(ev_contract_mapping)}"
        )

    seen_ids: set[str] = set()
    actual_order: list[str] = []
    for index, raw_row in enumerate(ev_contract_mapping):
        context = f"ev_contract_mapping[{index}]"
        row = require_mapping(raw_row, context=context)
        require_exact_keys(row, expected_keys=EV_ARTIFACT_MAPPING_KEYS, context=context)
        evidence_id = require_non_empty_string(row, key="evidence_id", context=context)
        artifact_path = require_non_empty_string(row, key="artifact_path", context=context)

        if evidence_id in seen_ids:
            raise ContractDriftError(f"duplicate evidence_id detected: {evidence_id}")
        seen_ids.add(evidence_id)
        actual_order.append(evidence_id)

        expected = expected_by_id.get(evidence_id)
        if expected is None:
            raise ContractDriftError(f"unexpected evidence_id in ev_contract_mapping: {evidence_id}")
        expected_artifact_path = str(expected["artifact_path"])
        if artifact_path != expected_artifact_path:
            raise ContractDriftError(
                f"{evidence_id} artifact_path drift: expected {expected_artifact_path!r}, "
                f"found {artifact_path!r}"
            )

    if tuple(actual_order) != expected_order:
        raise ContractDriftError(
            "ev_contract_mapping ordering drift: "
            f"expected {list(expected_order)}, found {actual_order}"
        )


def validate_gate_results_contract(gate_results: list[dict[str, str]]) -> None:
    if len(gate_results) != len(BASE_GATE_SEQUENCE):
        raise ContractDriftError(
            "gate_results row-count drift: "
            f"expected {len(BASE_GATE_SEQUENCE)}, found {len(gate_results)}"
        )

    actual_sequence: list[str] = []
    for index, raw_gate in enumerate(gate_results):
        context = f"gate_results[{index}]"
        gate = require_mapping(raw_gate, context=context)
        require_exact_keys(gate, expected_keys=GATE_RESULT_KEYS, context=context)
        gate_id = require_non_empty_string(gate, key="gate_id", context=context)
        status = require_non_empty_string(gate, key="status", context=context)
        rationale = require_non_empty_string(gate, key="rationale", context=context)
        actual_sequence.append(gate_id)

        if status not in VALID_GATE_STATUSES:
            raise ContractHardFailError(
                f"{context}.status: unsupported gate status '{status}'"
            )

        expected = next(
            (row for row in BASELINE_GATE_CONTRACT if row["gate_id"] == gate_id),
            None,
        )
        if expected is None:
            raise ContractDriftError(f"unexpected gate_id: {gate_id}")
        expected_status = str(expected["status"])
        if status != expected_status:
            raise ContractDriftError(
                f"{gate_id} status drift: expected {expected_status}, found {status}"
            )
        expected_rationale = str(expected["rationale"])
        if rationale != expected_rationale:
            raise ContractDriftError(
                f"{gate_id} rationale drift: expected {expected_rationale!r}, found {rationale!r}"
            )

    if tuple(actual_sequence) != BASE_GATE_SEQUENCE:
        raise ContractDriftError(
            f"gate ordering drift: expected {list(BASE_GATE_SEQUENCE)}, found {actual_sequence}"
        )


def validate_consumer_contract(downstream_handoffs: list[dict[str, object]]) -> None:
    expected_order = tuple(
        str(row["consumer_seed"]) for row in BASELINE_CONSUMER_CONTRACT
    )
    expected_by_seed = {
        str(row["consumer_seed"]): row for row in BASELINE_CONSUMER_CONTRACT
    }

    if len(downstream_handoffs) != len(expected_order):
        raise ContractDriftError(
            "downstream_handoffs row-count drift: "
            f"expected {len(expected_order)}, found {len(downstream_handoffs)}"
        )

    seen: set[str] = set()
    actual_order: list[str] = []
    for index, raw_handoff in enumerate(downstream_handoffs):
        context = f"downstream_handoffs[{index}]"
        handoff = require_mapping(raw_handoff, context=context)
        require_exact_keys(handoff, expected_keys=CONSUMER_HANDOFF_KEYS, context=context)
        consumer_seed = require_non_empty_string(handoff, key="consumer_seed", context=context)
        required_inputs = require_string_list(handoff, key="required_inputs", context=context)
        handoff_state = require_non_empty_string(handoff, key="handoff_state", context=context)
        handoff_note = require_non_empty_string(handoff, key="handoff_note", context=context)

        if consumer_seed in seen:
            raise ContractDriftError(f"duplicate consumer_seed detected: {consumer_seed}")
        seen.add(consumer_seed)
        actual_order.append(consumer_seed)

        expected = expected_by_seed.get(consumer_seed)
        if expected is None:
            raise ContractDriftError(f"unexpected consumer_seed: {consumer_seed}")

        expected_required_inputs = list(expected["required_inputs"])
        if required_inputs != expected_required_inputs:
            raise ContractDriftError(
                f"{consumer_seed} required_inputs drift: expected {expected_required_inputs}, "
                f"found {required_inputs}"
            )

        expected_handoff_state = str(expected["handoff_state"])
        if handoff_state != expected_handoff_state:
            raise ContractDriftError(
                f"{consumer_seed} handoff_state drift: expected {expected_handoff_state}, "
                f"found {handoff_state}"
            )
        expected_handoff_note = str(expected["handoff_note"])
        if handoff_note != expected_handoff_note:
            raise ContractDriftError(
                f"{consumer_seed} handoff_note drift: expected {expected_handoff_note!r}, "
                f"found {handoff_note!r}"
            )

    if tuple(actual_order) != expected_order:
        raise ContractDriftError(
            "downstream consumer ordering drift: "
            f"expected {list(expected_order)}, found {actual_order}"
        )


def validate_decision_semantics(
    *,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    evidence_items: list[dict[str, object]],
) -> None:
    if qg04_result not in VALID_GATE_STATUSES:
        raise ContractHardFailError(f"qg04_result is unsupported: {qg04_result}")

    expected_recommendation = recommendation_signal_for(qg04_result)
    if recommendation_signal != expected_recommendation:
        raise ContractDriftError(
            "recommendation_signal drift: "
            f"expected {expected_recommendation}, found {recommendation_signal}"
        )

    expected_decision = determine_decision(qg04_result)
    if decision != expected_decision:
        raise ContractDriftError(
            f"overall_decision drift: expected {expected_decision}, found {decision}"
        )

    non_pass_evidence = [
        require_non_empty_string(row, key="evidence_id", context=f"evidence_items[{index}]")
        for index, row in enumerate(evidence_items)
        if require_non_empty_string(row, key="status", context=f"evidence_items[{index}]")
        != "pass"
    ]
    if decision == "approve" and non_pass_evidence:
        raise ContractDriftError(
            "approve/hold contract drift: decision=approve with non-pass evidence rows "
            f"{non_pass_evidence}"
        )


def validate_baseline_contract(
    *,
    generated_at: str,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    gate_results: list[dict[str, str]],
    ev_contract_mapping: Sequence[dict[str, str]],
    evidence_items: list[dict[str, object]],
    downstream_handoffs: list[dict[str, object]],
) -> None:
    if not generated_at.strip():
        raise ContractHardFailError("generated_at must be a non-empty string")
    validate_evidence_contract(evidence_items)
    validate_ev_artifact_mapping_contract(ev_contract_mapping)
    validate_gate_results_contract(gate_results)
    validate_consumer_contract(downstream_handoffs)
    validate_decision_semantics(
        decision=decision,
        qg04_result=qg04_result,
        recommendation_signal=recommendation_signal,
        evidence_items=evidence_items,
    )


def determine_qg04_result(
    gate_results: list[dict[str, str]], active_exception_ids: list[str]
) -> str:
    statuses = {item["status"] for item in gate_results}
    if "blocked" in statuses:
        return "blocked"
    if "fail" in statuses:
        return "fail"
    if statuses == {"pass"}:
        return "conditional-pass" if active_exception_ids else "pass"
    return "conditional-pass"


def recommendation_signal_for(qg04_result: str) -> str:
    return {
        "pass": "go-candidate",
        "conditional-pass": "conditional-go-candidate",
        "fail": "no-go",
        "blocked": "hold",
    }[qg04_result]


def determine_decision(qg04_result: str) -> str:
    return "approve" if qg04_result == "pass" else "hold"


def render_markdown(
    generated_at: str,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    gate_results: list[dict[str, str]],
) -> str:
    lines = [
        f"# v0.11 Quality Gate Decision (`{SEED_ID}`)",
        "",
        f"_Generated at {generated_at}_",
        "",
        "## Decision Summary",
        "",
        f"- `contract_id`: `{CONTRACT_ID}`",
        f"- `seed_id`: `{SEED_ID}`",
        f"- `acceptance_gate_id`: `{ACCEPTANCE_GATE_ID}`",
        f"- `task_id`: `{TASK_ID}`",
        f"- `release_label`: `{RELEASE_LABEL}`",
        f"- `release_id`: `{RELEASE_ID}`",
        f"- `source_revision`: `{SOURCE_REVISION}`",
        f"- `overall_decision`: `{decision}`",
        f"- `QG-04`: `{qg04_result}`",
        f"- `recommendation_signal`: `{recommendation_signal}`",
        "",
        "## EV-06..EV-08 Evidence Register",
        "",
        "| EV ID | Status | Summary | Blocking refs |",
        "| --- | --- | --- | --- |",
    ]

    for item in EVIDENCE_ITEMS:
        refs = ", ".join(item["blocking_refs"]) if item["blocking_refs"] else "none"
        lines.append(
            f"| `{item['evidence_id']}` | `{item['status']}` | {item['summary']} | {refs} |"
        )

    lines.extend(
        [
            "",
            "## Gate Results (`QG-01`..`QG-04`)",
            "",
            "| Gate ID | Result | Rationale |",
            "| --- | --- | --- |",
        ]
    )

    for gate in gate_results:
        lines.append(f"| `{gate['gate_id']}` | `{gate['status']}` | {gate['rationale']} |")

    lines.append(
        "| `QG-04` | "
        f"`{qg04_result}` | Precedence result from `QG-01`..`QG-03` and active exception set (`{len(ACTIVE_EXCEPTION_IDS)}`). |"
    )

    lines.extend(
        [
            "",
            "## EV Contract Mapping",
            "",
            "| EV ID | Artifact path | Baseline status |",
            "| --- | --- | --- |",
        ]
    )

    ev_status_by_id = {
        item["evidence_id"]: item["status"] for item in EVIDENCE_ITEMS
    }
    for row in EV_ARTIFACT_MAPPING:
        evidence_id = row["evidence_id"]
        lines.append(
            f"| `{evidence_id}` | "
            f"`{row['artifact_path']}` | "
            f"`{ev_status_by_id[evidence_id]}` |"
        )

    lines.extend(
        [
            "",
            "## Threshold Results Snapshot",
            "",
            "| Threshold ID | Gate ID | Observed metric | Pass threshold | Result |",
            "| --- | --- | --- | --- | --- |",
        ]
    )

    for item in THRESHOLD_RESULTS:
        lines.append(
            "| "
            f"`{item['threshold_id']}` | "
            f"`{item['gate_id']}` | "
            f"`{item['observed_metric']}` | "
            f"`{item['pass_threshold']}` | "
            f"`{item['status']}` |"
        )

    lines.extend(
        [
            "",
            "## Active Exception Set (`EV-06` linkage)",
            "",
            "- `exception_ledger_path`: `reports/releases/v011_quality_gate_exceptions.md`",
            f"- `active_exception_ids`: `{ACTIVE_EXCEPTION_IDS}`",
            f"- `active_exception_count`: `{len(ACTIVE_EXCEPTION_IDS)}`",
            "",
            "## Unresolved Blocker Posture",
            "",
            "| Blocker ID | Status | Owner | Due date (UTC) | Due path |",
            "| --- | --- | --- | --- | --- |",
        ]
    )

    for blocker in UNRESOLVED_BLOCKERS:
        lines.append(
            f"| `{blocker['blocker_id']}` | "
            f"`{blocker['status']}` | "
            f"`{blocker['owner']}` | "
            f"`{blocker['due_date_utc']}` | "
            f"`{blocker['due_path']}` |"
        )

    lines.extend(
        [
            "",
            "## Downstream Handoff Notes",
            "",
            "| Consumer seed | Required inputs | Handoff state | Handoff note |",
            "| --- | --- | --- | --- |",
        ]
    )

    for handoff in DOWNSTREAM_HANDOFFS:
        required_inputs = ", ".join(handoff["required_inputs"])
        lines.append(
            f"| `{handoff['consumer_seed']}` | "
            f"`{required_inputs}` | "
            f"`{handoff['handoff_state']}` | "
            f"{handoff['handoff_note']} |"
        )

    lines.extend(
        [
            "",
            "## Acceptance Rollup (`AC-V013-CONF-02`)",
            "",
            "| Acceptance ID | Status | Summary |",
            "| --- | --- | --- |",
        ]
    )

    for row in ACCEPTANCE_ROLLUP:
        lines.append(
            f"| `{row['acceptance_id']}` | `{row['status']}` | {row['summary']} |"
        )

    lines.extend(
        [
            "",
            "## Validation Command References",
            "",
        ]
    )

    for command in VALIDATION_COMMAND_REFS:
        lines.append(f"- `{command}`")

    lines.extend(
        [
            "",
            "## Deterministic Rule",
            "",
            "- If any of `QG-01`, `QG-02`, or `QG-03` is `blocked`, then `QG-04=blocked`.",
            "- Else if any of `QG-01`, `QG-02`, or `QG-03` is `fail`, then `QG-04=fail`.",
            "- Else if all are `pass`, `QG-04=pass` when no active exceptions exist; otherwise `QG-04=conditional-pass`.",
            "- `recommendation_signal` is derived from `QG-04` (`pass` -> `go-candidate`; `conditional-pass` -> `conditional-go-candidate`; `fail` -> `no-go`; `blocked` -> `hold`).",
            "- `overall_decision` is `approve` only when `QG-04=pass`; otherwise `hold`.",
            "",
        ]
    )
    return "\n".join(lines)


def render_status(
    generated_at: str,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    gate_results: list[dict[str, str]],
) -> dict[str, object]:
    gate_results_with_qg04 = gate_results + [
        {
            "gate_id": "QG-04",
            "status": qg04_result,
            "rationale": (
                "Integrated precedence output derived from QG-01..QG-03 and active exception set."
            ),
        }
    ]
    downstream_consumers = [
        handoff["consumer_seed"] for handoff in DOWNSTREAM_HANDOFFS
    ]

    return {
        "contract_id": CONTRACT_ID,
        "seed_id": SEED_ID,
        "acceptance_gate_id": ACCEPTANCE_GATE_ID,
        "task_id": TASK_ID,
        "release_label": RELEASE_LABEL,
        "release_id": RELEASE_ID,
        "source_revision": SOURCE_REVISION,
        "generated_at_utc": generated_at,
        "validated_by": VALIDATED_BY,
        "validation_command_refs": VALIDATION_COMMAND_REFS,
        "ev_contract_mapping": list(EV_ARTIFACT_MAPPING),
        "evidence_items": EVIDENCE_ITEMS,
        "gate_results": gate_results_with_qg04,
        "threshold_results": THRESHOLD_RESULTS,
        "active_exception_ids": ACTIVE_EXCEPTION_IDS,
        "exception_ledger": {
            "artifact_path": "reports/releases/v011_quality_gate_exceptions.md",
            "active_exception_count": len(ACTIVE_EXCEPTION_IDS),
            "active_exception_budget_max": 2,
            "max_per_gate_domain": 1,
        },
        "qg_04_result": qg04_result,
        "recommendation_signal": recommendation_signal,
        "overall_decision": decision,
        "unresolved_blockers": UNRESOLVED_BLOCKERS,
        "downstream_consumers": downstream_consumers,
        "downstream_handoffs": DOWNSTREAM_HANDOFFS,
        "acceptance_rollup": ACCEPTANCE_ROLLUP,
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_quality_gate_decision.py",
        description="Generate deterministic quality gate markdown and status JSON.",
    )
    parser.add_argument("--output-md", type=Path, default=DEFAULT_MD)
    parser.add_argument("--output-status", type=Path, default=DEFAULT_STATUS)
    parser.add_argument("--generated-at", default=DEFAULT_GENERATED_AT)
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    try:
        qg04_result = determine_qg04_result(BASE_GATE_RESULTS, ACTIVE_EXCEPTION_IDS)
        recommendation_signal = recommendation_signal_for(qg04_result)
        decision = determine_decision(qg04_result)

        validate_baseline_contract(
            generated_at=args.generated_at,
            decision=decision,
            qg04_result=qg04_result,
            recommendation_signal=recommendation_signal,
            gate_results=BASE_GATE_RESULTS,
            ev_contract_mapping=EV_ARTIFACT_MAPPING,
            evidence_items=EVIDENCE_ITEMS,
            downstream_handoffs=DOWNSTREAM_HANDOFFS,
        )

        md = render_markdown(
            generated_at=args.generated_at,
            decision=decision,
            qg04_result=qg04_result,
            recommendation_signal=recommendation_signal,
            gate_results=BASE_GATE_RESULTS,
        )
        status = render_status(
            generated_at=args.generated_at,
            decision=decision,
            qg04_result=qg04_result,
            recommendation_signal=recommendation_signal,
            gate_results=BASE_GATE_RESULTS,
        )

        args.output_md.parent.mkdir(parents=True, exist_ok=True)
        args.output_status.parent.mkdir(parents=True, exist_ok=True)
        args.output_md.write_text(md + "\n", encoding="utf-8", newline="\n")
        args.output_status.write_text(
            json.dumps(status, indent=2) + "\n",
            encoding="utf-8",
            newline="\n",
        )
    except ContractDriftError as exc:
        print(f"quality-gate-generator: DRIFT ({exc})", file=sys.stderr)
        return 1
    except (ContractHardFailError, OSError) as exc:
        print(f"quality-gate-generator: HARD-FAIL ({exc})", file=sys.stderr)
        return 2

    print(
        "quality-gate-generator: OK "
        f"(decision={decision}, qg04={qg04_result}, recommendation={recommendation_signal}, "
        f"ev_items={len(EVIDENCE_ITEMS)})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
