#!/usr/bin/env python3
"""Fail-closed checker for M268-E002 runnable async/await closeout."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m268-e002-runnable-async-and-await-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-part7-runnable-async-and-await-matrix/m268-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m268" / "M268-E002" / "runnable_async_and_await_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m268_runnable_async_and_await_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m268" / "m268_e002_runnable_async_and_await_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m268_e002_lane_e_readiness.py"
NEXT_ISSUE = "M269-A001"
UPSTREAM_HANDOFF_ISSUE = "M268-E001"
EVIDENCE_MODEL = "a002-through-e001-summary-chain-runnable-part7-closeout"
FAILURE_MODEL = "fail-closed-on-runnable-part7-closeout-drift"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M268-A002", ROOT / "tmp" / "reports" / "m268" / "M268-A002" / "async_semantic_packet_summary.json", "objc3c-part7-async-source-closure/m268-a002-v1"),
    ("M268-B003", ROOT / "tmp" / "reports" / "m268" / "M268-B003" / "async_diagnostics_compatibility_completion_summary.json", "objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1"),
    ("M268-C003", ROOT / "tmp" / "reports" / "m268" / "M268-C003" / "async_cleanup_integration_summary.json", "objc3c-part7-suspension-autorelease-cleanup-integration/m268-c003-v1"),
    ("M268-D002", ROOT / "tmp" / "reports" / "m268" / "M268-D002" / "live_continuation_runtime_integration_summary.json", "objc3c-part7-live-continuation-runtime-integration/m268-d002-v1"),
    ("M268-E001", ROOT / "tmp" / "reports" / "m268" / "M268-E001" / "async_executable_conformance_gate_summary.json", "objc3c-async-executable-conformance-gate/m268-e001-v1"),
)

MATRIX_ROWS = (
    {
        "row_id": "async_function_entry",
        "source_issue": "M268-A002",
        "status": "supported",
        "evidence": "tmp/reports/m268/M268-A002/async_semantic_packet_summary.json",
        "model": "parser-owned async function entry admitted and published in semantic packets",
    },
    {
        "row_id": "async_method_entry",
        "source_issue": "M268-A002",
        "status": "supported",
        "evidence": "tmp/reports/m268/M268-A002/async_semantic_packet_summary.json",
        "model": "parser-owned Objective-C async method entry admitted and published in semantic packets",
    },
    {
        "row_id": "direct_call_await_lowering",
        "source_issue": "M268-B003",
        "status": "supported",
        "evidence": "tmp/reports/m268/M268-B003/async_diagnostics_compatibility_completion_summary.json",
        "model": "await remains fail-closed outside supported callables and runnable on the direct-call non-suspending slice",
    },
    {
        "row_id": "cleanup_integration",
        "source_issue": "M268-C003",
        "status": "supported",
        "evidence": "tmp/reports/m268/M268-C003/async_cleanup_integration_summary.json",
        "model": "supported async lowering composes with existing autoreleasepool and defer cleanup hooks",
    },
    {
        "row_id": "live_continuation_helper_execution",
        "source_issue": "M268-D002",
        "status": "supported",
        "evidence": "tmp/reports/m268/M268-D002/live_continuation_runtime_integration_summary.json",
        "model": "supported direct-call await sites allocate, handoff, and resume logical continuations through private runtime helpers",
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
        SnippetCheck("M268-E002-DOC-EXP-01", "# M268 Runnable Async And Await Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M268-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M268-E002-DOC-EXP-03", "M268-E001"),
        SnippetCheck("M268-E002-DOC-EXP-04", "M269-A001"),
        SnippetCheck("M268-E002-DOC-EXP-05", "tmp/reports/m268/M268-E002/runnable_async_and_await_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M268-E002-DOC-PKT-01", "# M268-E002 Runnable Async And Await Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M268-E002-DOC-PKT-02", "Issue: `#7293`"),
        SnippetCheck("M268-E002-DOC-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M268-E002-DOC-PKT-04", "- `M268-E001`"),
        SnippetCheck("M268-E002-DOC-PKT-05", "Next issue: `M269-A001`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M268-E002-SRC-01", "## M268 runnable async and await matrix closeout (M268-E002)"),
        SnippetCheck("M268-E002-SRC-02", "live continuation helper allocation, handoff, and resume traffic"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M268-E002-NDOC-01", "## M268 runnable async and await matrix closeout (M268-E002)"),
        SnippetCheck("M268-E002-NDOC-02", "live continuation helper allocation, handoff, and resume traffic"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M268-E002-ABS-01", "M268-E002 runnable async closeout note:"),
        SnippetCheck("M268-E002-ABS-02", "`M268-A002` through `M268-E001` proof chain"),
        SnippetCheck("M268-E002-ABS-03", "M269-A001"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M268-E002-ATTR-01", "Current implementation status (`M268-E002`):"),
        SnippetCheck("M268-E002-ATTR-02", "M268-E001"),
        SnippetCheck("M268-E002-ATTR-03", "direct-call, non-suspending async"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M268-E002-DRV-01", "M268-E002 runnable async closeout matrix anchor"),
        SnippetCheck("M268-E002-DRV-02", "matrix-only publication path"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M268-E002-MAN-01", "M268-E002 runnable async closeout matrix anchor"),
        SnippetCheck("M268-E002-MAN-02", "matrix-only reporting surface"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M268-E002-FAPI-01", "M268-E002 runnable async closeout matrix anchor"),
        SnippetCheck("M268-E002-FAPI-02", "frontend matrix-only reporting path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M268-E002-RUN-01", "run_m268_a002_lane_a_readiness.py"),
        SnippetCheck("M268-E002-RUN-02", "run_m268_b003_lane_b_readiness.py"),
        SnippetCheck("M268-E002-RUN-03", "run_m268_c003_lane_c_readiness.py"),
        SnippetCheck("M268-E002-RUN-04", "run_m268_d002_lane_d_readiness.py"),
        SnippetCheck("M268-E002-RUN-05", "run_m268_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M268-E002-PKG-01", '"check:objc3c:m268-e002-runnable-async-and-await-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M268-E002-PKG-02", '"test:tooling:m268-e002-runnable-async-and-await-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M268-E002-PKG-03", '"check:objc3c:m268-e002-lane-e-readiness"'),
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
        failures.append(Finding(display_path(path), "M268-E002-MISSING", f"missing required artifact: {display_path(path)}"))
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
        chain_passed += require(upstream_results[0]["issue"] == "M268-A002", "M268-E002", "M268-E002-UPSTREAM-FIRST", "upstream proof chain must start at M268-A002", failures)
        chain_checks += 1
        chain_passed += require(upstream_results[-1]["issue"] == "M268-E001", "M268-E002", "M268-E002-UPSTREAM-LAST", "upstream proof chain must end at M268-E001", failures)
    else:
        chain_checks += 1
        chain_passed += require(False, "M268-E002", "M268-E002-UPSTREAM-CHAIN", "upstream proof chain must not be empty", failures)

    chain_checks += 1
    chain_passed += require(UPSTREAM_HANDOFF_ISSUE == "M268-E001", "M268-E002", "M268-E002-UPSTREAM-HANDOFF", "the closeout should preserve the E001 handoff marker", failures)

    matrix_checks = 0
    matrix_passed = 0
    matrix_rows = []
    upstream_lookup = {entry["issue"]: entry for entry in upstream_results}
    for row in MATRIX_ROWS:
        matrix_checks += 1
        matrix_passed += require(row["source_issue"] in upstream_lookup, row["evidence"], f"M268-E002-ROW-{row['row_id']}", f"missing upstream proof for matrix row {row['row_id']}", failures)
        matrix_rows.append(dict(row))

    return {
        "issue": "M268-E002",
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
        "closeout_model": "summary-chain-closeout-for-runnable-part7-async-slice",
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
