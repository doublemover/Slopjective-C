#!/usr/bin/env python3
"""Fail-closed checker for M269-E002 runnable task/executor closeout."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-e002-runnable-task-and-executor-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-part7-runnable-task-executor-matrix/m269-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m269" / "M269-E002" / "runnable_task_and_executor_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_runnable_task_and_executor_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_e002_runnable_task_and_executor_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m269_e002_lane_e_readiness.py"
NEXT_ISSUE = "M270-A001"
UPSTREAM_HANDOFF_ISSUE = "M269-E001"
EVIDENCE_MODEL = "a002-through-e001-summary-chain-runnable-task-runtime-closeout"
FAILURE_MODEL = "fail-closed-on-runnable-task-runtime-closeout-drift"
CLOSEOUT_MODEL = "lane-e-closeout-replays-implemented-part7-task-runtime-slice-without-surface-widening"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M269-A002", ROOT / "tmp" / "reports" / "m269" / "M269-A002" / "task_group_cancellation_source_closure_summary.json", "objc3c-part7-task-group-cancellation-source-closure/m269-a002-v1"),
    ("M269-B003", ROOT / "tmp" / "reports" / "m269" / "M269-B003" / "executor_hop_affinity_compatibility_summary.json", "objc3c-part7-executor-hop-affinity-compatibility/m269-b003-v1"),
    ("M269-C003", ROOT / "tmp" / "reports" / "m269" / "M269-C003" / "task_runtime_abi_completion_summary.json", "objc3c-part7-task-runtime-abi-completion/m269-c003-v1"),
    ("M269-D003", ROOT / "tmp" / "reports" / "m269" / "M269-D003" / "task_runtime_hardening_summary.json", "objc3c-part7-task-runtime-hardening/m269-d003-v1"),
    ("M269-E001", ROOT / "tmp" / "reports" / "m269" / "M269-E001" / "task_executor_conformance_gate_summary.json", "objc3c-task-executor-conformance-gate/m269-e001-v1"),
)

MATRIX_ROWS = (
    {
        "row_id": "task_creation_source_closure",
        "source_issue": "M269-A002",
        "status": "supported",
        "evidence": "tmp/reports/m269/M269-A002/task_group_cancellation_source_closure_summary.json",
        "model": "task creation, task-group, and cancellation source ownership is published and replay-stable",
    },
    {
        "row_id": "executor_affinity_legality",
        "source_issue": "M269-B003",
        "status": "supported",
        "evidence": "tmp/reports/m269/M269-B003/executor_hop_affinity_compatibility_summary.json",
        "model": "executor affinity and detached-task legality are enforced before runtime lowering",
    },
    {
        "row_id": "task_runtime_abi_helper_publication",
        "source_issue": "M269-C003",
        "status": "supported",
        "evidence": "tmp/reports/m269/M269-C003/task_runtime_abi_completion_summary.json",
        "model": "task helper ABI, lowering summaries, and runtime snapshot publication are stable",
    },
    {
        "row_id": "hardened_cancellation_autorelease_replay",
        "source_issue": "M269-D003",
        "status": "supported",
        "evidence": "tmp/reports/m269/M269-D003/task_runtime_hardening_summary.json",
        "model": "live task runtime remains deterministic across cancellation, autoreleasepool, and reset replay edges",
    },
    {
        "row_id": "lane_e_conformance_gate",
        "source_issue": "M269-E001",
        "status": "supported",
        "evidence": "tmp/reports/m269/M269-E001/task_executor_conformance_gate_summary.json",
        "model": "lane-E gate freezes the truthful runnable task/runtime slice without widening the front-door claim",
    },
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M269-E002-DOC-EXP-01", "# M269 Runnable Task And Executor Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M269-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M269-E002-DOC-EXP-03", "M269-E001"),
        SnippetCheck("M269-E002-DOC-EXP-04", "M270-A001"),
        SnippetCheck("M269-E002-DOC-EXP-05", "tmp/reports/m269/M269-E002/runnable_task_and_executor_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M269-E002-DOC-PKT-01", "# M269-E002 Runnable Task And Executor Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M269-E002-DOC-PKT-02", "Issue: `#7306`"),
        SnippetCheck("M269-E002-DOC-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M269-E002-DOC-PKT-04", "- `M269-E001`"),
        SnippetCheck("M269-E002-DOC-PKT-05", "Next issue: `M270-A001`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M269-E002-SRC-01", "## M269 runnable task and executor matrix closeout (M269-E002)"),
        SnippetCheck("M269-E002-SRC-02", "task creation source closure"),
        SnippetCheck("M269-E002-SRC-03", "M270-A001"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M269-E002-NDOC-01", "## M269 runnable task and executor matrix closeout (M269-E002)"),
        SnippetCheck("M269-E002-NDOC-02", "task creation source closure"),
        SnippetCheck("M269-E002-NDOC-03", "M270-A001"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M269-E002-ABS-01", "M269-E002 runnable task/executor closeout note:"),
        SnippetCheck("M269-E002-ABS-02", "`M269-A002` through `M269-E001` proof"),
        SnippetCheck("M269-E002-ABS-03", "M270-A001"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M269-E002-ATTR-01", "Current implementation status (`M269-E002`):"),
        SnippetCheck("M269-E002-ATTR-02", "task creation source closure"),
        SnippetCheck("M269-E002-ATTR-03", "M270-A001"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M269-E002-DRV-01", "M269-E002 runnable task/executor closeout matrix anchor"),
        SnippetCheck("M269-E002-DRV-02", "matrix-only publication path"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M269-E002-MAN-01", "M269-E002 runnable task/executor closeout matrix anchor"),
        SnippetCheck("M269-E002-MAN-02", "matrix-only reporting surface"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M269-E002-FAPI-01", "M269-E002 runnable task/executor closeout matrix anchor"),
        SnippetCheck("M269-E002-FAPI-02", "matrix-only reporting path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M269-E002-RUN-01", "run_m269_a002_lane_a_readiness.py"),
        SnippetCheck("M269-E002-RUN-02", "run_m269_b003_lane_b_readiness.py"),
        SnippetCheck("M269-E002-RUN-03", "run_m269_c003_lane_c_readiness.py"),
        SnippetCheck("M269-E002-RUN-04", "run_m269_d003_lane_d_readiness.py"),
        SnippetCheck("M269-E002-RUN-05", "run_m269_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M269-E002-PKG-01", '"check:objc3c:m269-e002-runnable-task-and-executor-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M269-E002-PKG-02", '"test:tooling:m269-e002-runnable-task-and-executor-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M269-E002-PKG-03", '"check:objc3c:m269-e002-lane-e-readiness"'),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M269-E002-MISSING", f"missing required artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def validate_upstream_summary(issue: str, summary_path: Path, contract_id: str, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    artifact = display_path(summary_path)
    if not summary_path.exists():
        return 1, 0, {"issue": issue, "summary_path": display_path(summary_path), "missing": True}

    payload = load_json(summary_path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-CONTRACT", "upstream contract id drifted", failures)

    if "ok" in payload:
        checks_total += 1
        checks_passed += require(payload.get("ok") is True, artifact, f"{issue}-OK", "upstream summary must report ok=true", failures)
    if "checks_failed" in payload and "checks_passed" in payload and "checks_total" in payload:
        checks_total += 1
        checks_passed += require(payload.get("checks_failed") == 0, artifact, f"{issue}-FAILED", "upstream summary must report zero failures", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{issue}-COVERAGE", "upstream summary must report full check coverage", failures)
    elif "checks_passed" in payload and "checks_total" in payload:
        checks_total += 1
        checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, f"{issue}-COVERAGE", "upstream summary must report full check coverage", failures)
    elif "ok" not in payload:
        checks_total += 1
        checks_passed += require(False, artifact, f"{issue}-STATUS", "upstream summary must report either ok=true or checks_failed=0", failures)

    dynamic_key = next((key for key in ("dynamic_probes_executed", "dynamic_probes", "dynamic_payload", "dynamic") if key in payload), None)
    checks_total += 1
    checks_passed += require(dynamic_key is not None, artifact, f"{issue}-DYN-MISSING", "upstream summary lost its dynamic proof indicator", failures)
    if dynamic_key == "dynamic_probes_executed":
        checks_total += 1
        checks_passed += require(isinstance(payload.get(dynamic_key), bool), artifact, f"{issue}-DYN-FLAG", "dynamic probe flag must stay boolean", failures)
    elif dynamic_key is not None:
        checks_total += 1
        checks_passed += require(isinstance(payload.get(dynamic_key), dict), artifact, f"{issue}-DYN-PAYLOAD", "dynamic payload must stay an object", failures)

    return checks_total, checks_passed, {
        "issue": issue,
        "summary_path": display_path(summary_path),
        "contract_id": payload.get("contract_id"),
        "ok": payload.get("ok") if "ok" in payload else None,
        "checks_failed": payload.get("checks_failed") if "checks_failed" in payload else None,
        "dynamic_key": dynamic_key,
    }


def load_upstream_chain(failures: list[Finding]) -> tuple[int, int, list[dict[str, Any]]]:
    checks_total = 0
    checks_passed = 0
    upstream_results: list[dict[str, Any]] = []

    for issue, summary_path, contract_id in UPSTREAM_SPECS:
        checks_total += 1
        if not summary_path.exists():
            failures.append(Finding(display_path(summary_path), f"{issue}-MISSING", f"missing required artifact: {display_path(summary_path)}"))
            continue
        checks_passed += 1

        summary_checks_total, summary_checks_passed, summary_payload = validate_upstream_summary(issue, summary_path, contract_id, failures)
        checks_total += summary_checks_total
        checks_passed += summary_checks_passed
        upstream_results.append(summary_payload)

    return checks_total, checks_passed, upstream_results


def build_summary(upstream_results: list[dict[str, Any]], failures: list[Finding], checks_total: int, checks_passed: int) -> dict[str, Any]:
    chain_checks = 0
    chain_passed = 0

    if upstream_results:
        chain_checks += 1
        chain_passed += require(upstream_results[0]["issue"] == "M269-A002", "M269-E002", "M269-E002-UPSTREAM-FIRST", "upstream proof chain must start at M269-A002", failures)
        chain_checks += 1
        chain_passed += require(upstream_results[-1]["issue"] == "M269-E001", "M269-E002", "M269-E002-UPSTREAM-LAST", "upstream proof chain must end at M269-E001", failures)
    else:
        chain_checks += 1
        chain_passed += require(False, "M269-E002", "M269-E002-UPSTREAM-CHAIN", "upstream proof chain must not be empty", failures)

    chain_checks += 1
    chain_passed += require(UPSTREAM_HANDOFF_ISSUE == "M269-E001", "M269-E002", "M269-E002-UPSTREAM-HANDOFF", "the closeout should preserve the E001 handoff marker", failures)

    matrix_checks = 0
    matrix_passed = 0
    matrix_rows = []
    upstream_lookup = {entry["issue"]: entry for entry in upstream_results}
    for row in MATRIX_ROWS:
        matrix_checks += 1
        matrix_passed += require(row["source_issue"] in upstream_lookup, row["evidence"], f"M269-E002-ROW-{row['row_id']}", f"missing upstream proof for matrix row {row['row_id']}", failures)
        matrix_rows.append(dict(row))

    return {
        "issue": "M269-E002",
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total + chain_checks + matrix_checks,
        "checks_passed": checks_passed + chain_passed + matrix_passed,
        "checks_failed": max((checks_total + chain_checks + matrix_checks) - (checks_passed + chain_passed + matrix_passed), 0),
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": False,
        "upstream_handoff_issue": UPSTREAM_HANDOFF_ISSUE,
        "next_issue": NEXT_ISSUE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "closeout_model": CLOSEOUT_MODEL,
        "proof_chain": upstream_results,
        "matrix_rows": matrix_rows,
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        snippet_passed = ensure_snippets(path, snippets, failures)
        checks_total += len(snippets)
        checks_passed += snippet_passed

    upstream_checks_total, upstream_checks_passed, upstream_results = load_upstream_chain(failures)
    checks_total += upstream_checks_total
    checks_passed += upstream_checks_passed

    summary = build_summary(upstream_results, failures, checks_total, checks_passed)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
