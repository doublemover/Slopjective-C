#!/usr/bin/env python3
"""Fail-closed checker for M271-E002 runnable system-extension closeout."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-e002-runnable-system-extension-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-part8-runnable-system-extension-matrix/m271-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m271" / "M271-E002" / "runnable_system_extension_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_runnable_system_extension_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_e002_runnable_system_extension_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
CONFORMANCE_SPEC = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m271_e002_lane_e_readiness.py"
NEXT_ISSUE = "M272-A001"
UPSTREAM_HANDOFF_ISSUE = "M271-E001"
EVIDENCE_MODEL = "a003-through-e001-summary-chain-runnable-system-extension-closeout"
FAILURE_MODEL = "fail-closed-on-runnable-system-extension-closeout-drift"
CLOSEOUT_MODEL = "lane-e-closeout-replays-implemented-part8-slice-without-surface-widening"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M271-A003", ROOT / "tmp" / "reports" / "m271" / "M271-A003" / "retainable_c_family_source_completion_summary.json", "objc3c-part8-retainable-c-family-source-completion/m271-a003-v1"),
    ("M271-B004", ROOT / "tmp" / "reports" / "m271" / "M271-B004" / "capture_list_and_retainable_family_legality_completion_summary.json", "objc3c-part8-capture-list-retainable-family-legality/m271-b004-v1"),
    ("M271-C003", ROOT / "tmp" / "reports" / "m271" / "M271-C003" / "borrowed_retainable_abi_completion_summary.json", "objc3c-part8-borrowed-retainable-family-abi-completion/m271-c003-v1"),
    ("M271-D002", ROOT / "tmp" / "reports" / "m271" / "M271-D002" / "live_cleanup_retainable_runtime_integration_summary.json", "objc3c-part8-live-cleanup-retainable-runtime-integration/m271-d002-v1"),
    ("M271-E001", ROOT / "tmp" / "reports" / "m271" / "M271-E001" / "strict_system_conformance_gate_summary.json", "objc3c-strict-system-conformance-gate/m271-e001-v1"),
)

MATRIX_ROWS = (
    {
        "row_id": "retainable_c_family_source_completion",
        "source_issue": "M271-A003",
        "status": "supported",
        "evidence": "tmp/reports/m271/M271-A003/retainable_c_family_source_completion_summary.json",
        "model": "retainable C-family declaration source ownership is published and replay-stable",
    },
    {
        "row_id": "capture_list_and_retainable_family_legality_completion",
        "source_issue": "M271-B004",
        "status": "supported",
        "evidence": "tmp/reports/m271/M271-B004/capture_list_and_retainable_family_legality_completion_summary.json",
        "model": "capture-list edge cases and retainable-family legality diagnostics fail closed before lowering",
    },
    {
        "row_id": "borrowed_pointer_and_retainable_family_abi_completion",
        "source_issue": "M271-C003",
        "status": "supported",
        "evidence": "tmp/reports/m271/M271-C003/borrowed_retainable_abi_completion_summary.json",
        "model": "borrowed-pointer and retainable-family ABI publication is stable on the current Part 8 slice",
    },
    {
        "row_id": "live_cleanup_helpers_and_retainable_family_integration",
        "source_issue": "M271-D002",
        "status": "supported",
        "evidence": "tmp/reports/m271/M271-D002/live_cleanup_retainable_runtime_integration_summary.json",
        "model": "the supported cleanup/resource and retainable-family helper slice executes through the packaged runtime helper cluster",
    },
    {
        "row_id": "lane_e_strict_system_conformance_gate",
        "source_issue": "M271-E001",
        "status": "supported",
        "evidence": "tmp/reports/m271/M271-E001/strict_system_conformance_gate_summary.json",
        "model": "lane-E gate freezes the truthful runnable Part 8 slice without widening deferred borrowed-lifetime/runtime claims",
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
        SnippetCheck("M271-E002-DOC-EXP-01", "# M271 Runnable System-Extension Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M271-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M271-E002-DOC-EXP-03", "M271-E001"),
        SnippetCheck("M271-E002-DOC-EXP-04", "M272-A001"),
        SnippetCheck("M271-E002-DOC-EXP-05", "tmp/reports/m271/M271-E002/runnable_system_extension_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M271-E002-DOC-PKT-01", "# M271-E002 Runnable System-Extension Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M271-E002-DOC-PKT-02", "Issue: `#7333`"),
        SnippetCheck("M271-E002-DOC-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M271-E002-DOC-PKT-04", "- `M271-E001`"),
        SnippetCheck("M271-E002-DOC-PKT-05", "Next issue: `M272-A001`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M271-E002-SRC-01", "## M271 runnable system-extension matrix closeout (M271-E002)"),
        SnippetCheck("M271-E002-SRC-02", "retainable C-family declaration source completion"),
        SnippetCheck("M271-E002-SRC-03", "M272-A001"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M271-E002-NDOC-01", "## M271 runnable system-extension matrix closeout (M271-E002)"),
        SnippetCheck("M271-E002-NDOC-02", "retainable C-family declaration source completion"),
        SnippetCheck("M271-E002-NDOC-03", "M272-A001"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M271-E002-ABS-01", "M271-E002 runnable system-extension closeout note:"),
        SnippetCheck("M271-E002-ABS-02", "`M271-A003` through"),
        SnippetCheck("M271-E002-ABS-03", "M272-A001"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M271-E002-ATTR-01", "Current implementation status (`M271-E002`):"),
        SnippetCheck("M271-E002-ATTR-02", "retainable-family source completion"),
        SnippetCheck("M271-E002-ATTR-03", "M272-A001"),
    ),
    CONFORMANCE_SPEC: (
        SnippetCheck("M271-E002-CONF-01", "M271-E002 runnable system-extension closeout note:"),
        SnippetCheck("M271-E002-CONF-02", "retainable-family source completion"),
        SnippetCheck("M271-E002-CONF-03", "M272-A001"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M271-E002-DRV-01", "M271-E002 runnable system-extension closeout matrix anchor"),
        SnippetCheck("M271-E002-DRV-02", "matrix-only publication path"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M271-E002-MAN-01", "M271-E002 runnable system-extension closeout matrix anchor"),
        SnippetCheck("M271-E002-MAN-02", "matrix-only reporting surface"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M271-E002-FAPI-01", "M271-E002 runnable system-extension closeout matrix anchor"),
        SnippetCheck("M271-E002-FAPI-02", "matrix-only reporting path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M271-E002-RUN-01", "run_m271_a003_lane_a_readiness.py"),
        SnippetCheck("M271-E002-RUN-02", "run_m271_b004_lane_b_readiness.py"),
        SnippetCheck("M271-E002-RUN-03", "run_m271_c003_lane_c_readiness.py"),
        SnippetCheck("M271-E002-RUN-04", "run_m271_d002_lane_d_readiness.py"),
        SnippetCheck("M271-E002-RUN-05", "run_m271_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M271-E002-PKG-01", '"check:objc3c:m271-e002-runnable-system-extension-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M271-E002-PKG-02", '"test:tooling:m271-e002-runnable-system-extension-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M271-E002-PKG-03", '"check:objc3c:m271-e002-lane-e-readiness"'),
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
        failures.append(Finding(display_path(path), "M271-E002-MISSING", f"missing required artifact: {display_path(path)}"))
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
        return 1, 0, {"issue": issue, "summary_path": artifact, "missing": True}

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
    elif "passed_checks" in payload and "total_checks" in payload:
        checks_total += 1
        checks_passed += require(payload.get("passed_checks") == payload.get("total_checks"), artifact, f"{issue}-COVERAGE", "upstream summary must report full check coverage", failures)
    elif "ok" not in payload:
        checks_total += 1
        checks_passed += require(False, artifact, f"{issue}-STATUS", "upstream summary must report either ok=true or full coverage counters", failures)

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
        "summary_path": artifact,
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
        chain_passed += require(upstream_results[0]["issue"] == "M271-A003", "M271-E002", "M271-E002-UPSTREAM-FIRST", "upstream proof chain must start at M271-A003", failures)
        chain_checks += 1
        chain_passed += require(upstream_results[-1]["issue"] == "M271-E001", "M271-E002", "M271-E002-UPSTREAM-LAST", "upstream proof chain must end at M271-E001", failures)
    else:
        chain_checks += 1
        chain_passed += require(False, "M271-E002", "M271-E002-UPSTREAM-CHAIN", "upstream proof chain must not be empty", failures)

    chain_checks += 1
    chain_passed += require(UPSTREAM_HANDOFF_ISSUE == "M271-E001", "M271-E002", "M271-E002-UPSTREAM-HANDOFF", "the closeout should preserve the E001 handoff marker", failures)

    matrix_checks = 0
    matrix_passed = 0
    matrix_rows = []
    upstream_lookup = {entry["issue"]: entry for entry in upstream_results}
    for row in MATRIX_ROWS:
        matrix_checks += 1
        matrix_passed += require(row["source_issue"] in upstream_lookup, row["evidence"], f"M271-E002-ROW-{row['row_id']}", f"missing upstream proof for matrix row {row['row_id']}", failures)
        matrix_rows.append(dict(row))

    return {
        "issue": "M271-E002",
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
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_checks_total, upstream_checks_passed, upstream_results = load_upstream_chain(failures)
    checks_total += upstream_checks_total
    checks_passed += upstream_checks_passed

    summary = build_summary(upstream_results, failures, checks_total, checks_passed)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        print(f"[fail] M271-E002 runnable system-extension closeout check failed ({len(failures)} findings)", file=sys.stderr)
        return 1

    print(f"[ok] M271-E002 runnable system-extension closeout check passed ({summary['checks_passed']}/{summary['checks_total']} checks)")
    print(f"[ok] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
