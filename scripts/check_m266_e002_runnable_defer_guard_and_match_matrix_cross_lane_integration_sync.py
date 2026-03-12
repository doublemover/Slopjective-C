#!/usr/bin/env python3
"""Validate M266-E002 runnable control-flow closeout matrix."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m266-e002-runnable-control-flow-matrix-closeout-v1"
CONTRACT_ID = "objc3c-part5-runnable-control-flow-matrix/m266-e002-v1"
MATRIX_MODEL = "closeout-matrix-consumes-a002-b003-c003-d002-and-e001-evidence-without-widening-the-runnable-part5-slice"
RUNNABLE_MATRIX_MODEL = "real-executable-evidence-covers-ordinary-defer-exit-guard-return-nested-unwind-and-integrated-guard-match-defer"
FAILURE_MODEL = "fail-closed-on-runnable-control-flow-matrix-drift-or-doc-mismatch"
NEXT_ISSUE = "M267-A001"

A002_CONTRACT_ID = "objc3c-part5-control-flow-source-closure/m266-a002-v1"
C003_CONTRACT_ID = "objc3c-part5-match-lowering-runtime-alignment/m266-c003-v1"
D002_CONTRACT_ID = "objc3c-runtime-cleanup-unwind-integration/m266-d002-v1"
E001_CONTRACT_ID = "objc3c-part5-control-flow-execution-gate/m266-e001-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m266_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m266" / "m266_e002_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_RULES = ROOT / "spec" / "CROSS_CUTTING_RULE_INDEX.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PACKAGE_JSON = ROOT / "package.json"
A002_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-A002" / "frontend_pattern_guard_surface_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-B003" / "defer_legality_cleanup_order_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-C003" / "match_lowering_dispatch_and_exhaustiveness_runtime_alignment_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-D002" / "runtime_cleanup_and_unwind_integration_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-E001" / "control_flow_execution_gate_summary.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m266" / "M266-E002" / "runnable_control_flow_matrix_summary.json"


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
        SnippetCheck("M266-E002-EXP-01", "# M266 Runnable Defer, Guard, And Match Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M266-E002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M266-E002-EXP-03", "tmp/reports/m266/M266-E001/control_flow_execution_gate_summary.json"),
        SnippetCheck("M266-E002-EXP-04", "`M267-A001` is the next issue after `M266` closes."),
    ),
    PACKET_DOC: (
        SnippetCheck("M266-E002-PKT-01", "# M266-E002 Runnable Defer, Guard, And Match Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M266-E002-PKT-02", "Packet: `M266-E002`"),
        SnippetCheck("M266-E002-PKT-03", "Issue: `#7268`"),
        SnippetCheck("M266-E002-PKT-04", "- `M266-E001`"),
        SnippetCheck("M266-E002-PKT-05", "integrated native `guard` + supported statement-form `match` + `defer` row"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M266-E002-SRC-01", "## M266 runnable control-flow matrix and docs (E002)"),
        SnippetCheck("M266-E002-SRC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M266-E002-SRC-03", "The milestone closeout matrix now publishes the exact runnable Part 5 evidence"),
        SnippetCheck("M266-E002-SRC-04", "`M267-A001` is the next issue"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M266-E002-NDOC-01", "## M266 runnable control-flow matrix and docs (E002)"),
        SnippetCheck("M266-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M266-E002-NDOC-03", "The milestone closeout matrix now publishes the exact runnable Part 5 evidence"),
    ),
    SPEC_AM: (
        SnippetCheck("M266-E002-AM-01", "M266-E002 closeout note:"),
        SnippetCheck("M266-E002-AM-02", "the closeout matrix is documentary and evidentiary only"),
    ),
    SPEC_RULES: (
        SnippetCheck("M266-E002-RULES-01", "Current Part 5 closeout-matrix note:"),
        SnippetCheck("M266-E002-RULES-02", "the closeout matrix consumes `M266-D002` cleanup/unwind executable evidence"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M266-E002-DRV-01", "M266-E002 runnable control-flow matrix anchor"),
        SnippetCheck("M266-E002-DRV-02", "same emitted native artifact triplet"),
    ),
    MANIFEST_ARTIFACTS_CPP: (
        SnippetCheck("M266-E002-MAN-01", "M266-E002 runnable control-flow matrix anchor"),
        SnippetCheck("M266-E002-MAN-02", "consuming this same manifest artifact rather than inventing a separate"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M266-E002-FEA-01", "M266-E002 runnable control-flow matrix anchor"),
        SnippetCheck("M266-E002-FEA-02", "same surfaced native artifact triplet instead of a second"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M266-E002-PKG-01", '"check:objc3c:m266-e002-runnable-defer-guard-and-match-matrix-cross-lane-integration-sync": "python scripts/check_m266_e002_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync.py"'),
        SnippetCheck("M266-E002-PKG-02", '"test:tooling:m266-e002-runnable-defer-guard-and-match-matrix-cross-lane-integration-sync": "python -m pytest tests/tooling/test_check_m266_e002_runnable_defer_guard_and_match_matrix_cross_lane_integration_sync.py -q"'),
        SnippetCheck("M266-E002-PKG-03", '"check:objc3c:m266-e002-lane-e-readiness": "python scripts/run_m266_e002_lane_e_readiness.py"'),
    ),
}


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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, int, list[Finding]]:
    failures: list[Finding] = []
    total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return total, 0, failures
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return total, passed, failures


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def validate_summary(name: str, path: Path, expected_contract_id: str | None, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks: list[tuple[str, bool, str]] = [
        (f"{name}-SUM-01", payload.get("checks_passed") == payload.get("checks_total"), "summary must report checks_passed == checks_total"),
        (f"{name}-SUM-02", payload.get("checks_total", 0) > 0, "summary must report at least one check"),
    ]
    if expected_contract_id is not None:
        checks.append((f"{name}-SUM-03", payload.get("contract_id") == expected_contract_id, f"expected contract_id {expected_contract_id!r}"))
    total = len(checks)
    passed = 0
    for check_id, condition, detail in checks:
        passed += require(condition, artifact, check_id, detail, failures)
    return total, passed, payload


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        total, passed, file_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += passed
        failures.extend(file_failures)

    upstream_payloads: dict[str, Any] = {}
    for name, path, contract_id in (
        ("M266-A002", A002_SUMMARY, A002_CONTRACT_ID),
        ("M266-B003", B003_SUMMARY, None),
        ("M266-C003", C003_SUMMARY, C003_CONTRACT_ID),
        ("M266-D002", D002_SUMMARY, D002_CONTRACT_ID),
        ("M266-E001", E001_SUMMARY, E001_CONTRACT_ID),
    ):
        total, passed, payload = validate_summary(name, path, contract_id, failures)
        checks_total += total
        checks_passed += passed
        upstream_payloads[name] = payload

    matrix_rows: list[dict[str, Any]] = []

    d002 = upstream_payloads["M266-D002"]
    e001 = upstream_payloads["M266-E001"]

    ordinary_exit = d002.get("evidence", {}).get("ordinary_exit", {})
    guard_return = d002.get("evidence", {}).get("guard_return", {})
    nested_return = d002.get("evidence", {}).get("nested_return", {})
    integrated_probe = e001.get("integrated_probe", {})
    replay_map = integrated_probe.get("manifest_lowering_replay_key_map", {})
    if not isinstance(replay_map, dict):
        replay_map = {}

    checks: list[tuple[str, bool, str]] = [
        ("M266-E002-MTX-01", ordinary_exit.get("run_returncode") == 21, "ordinary lexical exit row drifted"),
        ("M266-E002-MTX-02", guard_return.get("run_returncode") == 37, "guard-mediated early return row drifted"),
        ("M266-E002-MTX-03", nested_return.get("run_returncode") == 954, "nested-scope return unwind row drifted"),
        ("M266-E002-MTX-04", integrated_probe.get("run_returncode") == 195, "integrated guard/match/defer row drifted"),
        ("M266-E002-MTX-05", replay_map.get("guard_statement_sites") == "1", "integrated guard row must preserve one guard site"),
        ("M266-E002-MTX-06", replay_map.get("match_statement_sites") == "1", "integrated row must preserve one match site"),
        ("M266-E002-MTX-07", replay_map.get("defer_statement_sites") == "1", "integrated row must preserve one defer site"),
    ]
    checks_total += len(checks)
    for check_id, condition, detail in checks:
        checks_passed += require(condition, "matrix", check_id, detail, failures)

    matrix_rows.append({
        "row_id": "defer-ordinary-exit",
        "source_issue": "M266-D002",
        "run_returncode": ordinary_exit.get("run_returncode"),
        "summary_path": display_path(D002_SUMMARY),
        "claim": "ordinary lexical defer cleanup executes before function exit completes",
    })
    matrix_rows.append({
        "row_id": "guard-early-return-cleanup",
        "source_issue": "M266-D002",
        "run_returncode": guard_return.get("run_returncode"),
        "summary_path": display_path(D002_SUMMARY),
        "claim": "guard-mediated early return still executes deferred cleanup exactly once",
    })
    matrix_rows.append({
        "row_id": "nested-return-unwind-ordering",
        "source_issue": "M266-D002",
        "run_returncode": nested_return.get("run_returncode"),
        "summary_path": display_path(D002_SUMMARY),
        "claim": "nested-scope return unwind preserves inner-to-outer cleanup ordering",
    })
    matrix_rows.append({
        "row_id": "integrated-guard-match-defer",
        "source_issue": "M266-E001",
        "run_returncode": integrated_probe.get("run_returncode"),
        "summary_path": display_path(E001_SUMMARY),
        "claim": "one native executable proves guard short-circuit, supported statement-form match dispatch, and defer cleanup together",
    })

    ok = not failures
    summary = {
        "ok": ok,
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "matrix_model": MATRIX_MODEL,
        "runnable_matrix_model": RUNNABLE_MATRIX_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "matrix_rows": matrix_rows,
        "upstream_summaries": upstream_payloads,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if not ok:
        for failure in failures:
            print(f"[fail] {failure.artifact} {failure.check_id}: {failure.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print(f"[ok] M266-E002 runnable control-flow matrix validated ({checks_passed}/{checks_total})")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
