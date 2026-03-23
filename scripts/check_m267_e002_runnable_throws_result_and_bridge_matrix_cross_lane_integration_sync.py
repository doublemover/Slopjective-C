#!/usr/bin/env python3
"""Fail-closed checker for M267-E002 runnable throws/result/bridge closeout."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m267-e002-runnable-throws-result-and-bridge-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-part6-runnable-throws-result-and-bridge-matrix/m267-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m267" / "M267-E002" / "runnable_throws_result_and_bridge_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m267_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m267" / "m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
MODULE_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
PART6_SPEC = ROOT / "spec" / "PART_6_ERRORS_RESULTS_THROWS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
READINESS_RUNNER = ROOT / "scripts" / "run_m267_e002_lane_e_readiness.py"
NEXT_ISSUE = "M268-A001"
UPSTREAM_HANDOFF_ISSUE = "M267-E001"
EVIDENCE_MODEL = "a001-through-e001-summary-chain-runnable-part6-closeout"
FAILURE_MODEL = "fail-closed-on-runnable-part6-closeout-drift"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M267-A001", ROOT / "tmp" / "reports" / "m267" / "M267-A001" / "error_source_closure_summary.json", "objc3c-part6-error-source-closure/m267-a001-v1"),
    ("M267-A002", ROOT / "tmp" / "reports" / "m267" / "M267-A002" / "error_bridge_marker_surface_summary.json", "objc3c-part6-error-bridge-markers/m267-a002-v1"),
    ("M267-B001", ROOT / "tmp" / "reports" / "m267" / "M267-B001" / "error_semantic_model_summary.json", "objc3c-part6-error-semantic-model/m267-b001-v1"),
    ("M267-B002", ROOT / "tmp" / "reports" / "m267" / "M267-B002" / "try_do_catch_semantics_summary.json", "objc3c-part6-try-throw-do-catch-semantics/m267-b002-v1"),
    ("M267-B003", ROOT / "tmp" / "reports" / "m267" / "M267-B003" / "bridging_legality_summary.json", "objc3c-part6-error-bridge-legality/m267-b003-v1"),
    ("M267-C001", ROOT / "tmp" / "reports" / "m267" / "M267-C001" / "throws_abi_and_propagation_lowering_summary.json", "objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1"),
    ("M267-C002", ROOT / "tmp" / "reports" / "m267" / "M267-C002" / "error_out_abi_and_propagation_lowering_summary.json", "objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1"),
    ("M267-C003", ROOT / "tmp" / "reports" / "m267" / "M267-C003" / "result_and_bridging_artifact_replay_completion_summary.json", "objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1"),
    ("M267-D001", ROOT / "tmp" / "reports" / "m267" / "M267-D001" / "error_runtime_bridge_helper_contract_summary.json", "objc3c-part6-error-runtime-and-bridge-helper-api/m267-d001-v1"),
    ("M267-D002", ROOT / "tmp" / "reports" / "m267" / "M267-D002" / "live_error_runtime_integration_summary.json", "objc3c-part6-live-error-runtime-integration/m267-d002-v1"),
    ("M267-D003", ROOT / "tmp" / "reports" / "m267" / "M267-D003" / "cross_module_error_surface_preservation_summary.json", "objc3c-cross-module-error-surface-preservation-hardening/m267-d003-v1"),
    ("M267-E001", ROOT / "tmp" / "reports" / "m267" / "M267-E001" / "error_model_conformance_gate_summary.json", "objc3c-error-model-conformance-gate/m267-e001-v1"),
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
        SnippetCheck("M267-E002-DOC-EXP-01", "# M267 Runnable Throws, Result, And Bridge Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M267-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-E002-DOC-EXP-03", "M267-E001"),
        SnippetCheck("M267-E002-DOC-EXP-04", "M268-A001"),
        SnippetCheck("M267-E002-DOC-EXP-05", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M267-E002-DOC-PKT-01", "# M267-E002 Runnable Throws, Result, And Bridge Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M267-E002-DOC-PKT-02", "Issue: `#7281`"),
        SnippetCheck("M267-E002-DOC-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M267-E002-DOC-PKT-04", "- `M267-E001`"),
        SnippetCheck("M267-E002-DOC-PKT-05", "Next issue: `M268-A001`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M267-E002-SRC-01", "## M267 runnable throws, Result, and bridge matrix closeout (M267-E002)"),
        SnippetCheck("M267-E002-SRC-02", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
        SnippetCheck("M267-E002-SRC-03", "`M268-A001` is the next issue"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M267-E002-NDOC-01", "## M267 runnable throws, Result, and bridge matrix closeout (M267-E002)"),
        SnippetCheck("M267-E002-NDOC-02", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
        SnippetCheck("M267-E002-NDOC-03", "`M268-A001` is the next issue"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M267-E002-ABS-01", "M267-E002 runnable closeout note:"),
        SnippetCheck("M267-E002-ABS-02", "`M267-A001` through `M267-E001` proof chain"),
        SnippetCheck("M267-E002-ABS-03", "`M268-A001`"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M267-E002-ATTR-01", "## M267 runnable throws, Result, and bridge matrix closeout (E002)"),
        SnippetCheck("M267-E002-ATTR-02", "Current implementation status (`M267-E002`):"),
        SnippetCheck("M267-E002-ATTR-03", "`M268-A001`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M267-E002-LOW-01", "## M267 runnable throws, result, and bridge matrix closeout (E002)"),
        SnippetCheck("M267-E002-LOW-02", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
    ),
    MODULE_SPEC: (
        SnippetCheck("M267-E002-MOD-01", "## M267 runnable throws, result, and bridge matrix closeout metadata anchors (E002)"),
        SnippetCheck("M267-E002-MOD-02", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
    ),
    PART6_SPEC: (
        SnippetCheck("M267-E002-P6-01", "## M267 current implementation closeout note"),
        SnippetCheck("M267-E002-P6-02", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
        SnippetCheck("M267-E002-P6-03", "next issue: `M268-A001`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M267-E002-ARCH-01", "## M267 Part 6 Runnable Throws, Result, And Bridge Matrix Closeout (E002)"),
        SnippetCheck("M267-E002-ARCH-02", "tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json"),
        SnippetCheck("M267-E002-ARCH-03", "`M268-A001`"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M267-E002-RUN-01", "check_m267_a001_throws_try_do_catch_result_and_bridging_source_closure_contract_and_architecture_freeze.py"),
        SnippetCheck("M267-E002-RUN-02", "run_m267_a002_lane_a_readiness.py"),
        SnippetCheck("M267-E002-RUN-03", "run_m267_c003_lane_c_readiness.py"),
        SnippetCheck("M267-E002-RUN-04", "run_m267_d003_lane_d_readiness.py"),
        SnippetCheck("M267-E002-RUN-05", "run_m267_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M267-E002-PKG-01", '"check:objc3c:m267-e002-runnable-throws-result-and-bridge-matrix-cross-lane-integration-sync": "python scripts/check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py"'),
        SnippetCheck("M267-E002-PKG-02", '"test:tooling:m267-e002-runnable-throws-result-and-bridge-matrix-cross-lane-integration-sync": "python -m pytest tests/tooling/test_check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py -q"'),
        SnippetCheck("M267-E002-PKG-03", '"check:objc3c:m267-e002-lane-e-readiness": "python scripts/run_m267_e002_lane_e_readiness.py"'),
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
        failures.append(Finding(display_path(path), "M267-E002-MISSING", f"missing required artifact: {display_path(path)}"))
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
        chain_passed += require(upstream_results[0]["issue"] == "M267-A001", "M267-E002", "M267-E002-UPSTREAM-FIRST", "upstream proof chain must start at M267-A001", failures)
        chain_checks += 1
        chain_passed += require(upstream_results[-1]["issue"] == "M267-E001", "M267-E002", "M267-E002-UPSTREAM-LAST", "upstream proof chain must end at M267-E001", failures)
    else:
        chain_checks += 1
        chain_passed += require(False, "M267-E002", "M267-E002-UPSTREAM-CHAIN", "upstream proof chain must not be empty", failures)

    chain_checks += 1
    chain_passed += require(UPSTREAM_HANDOFF_ISSUE == "M267-E001", "M267-E002", "M267-E002-UPSTREAM-HANDOFF", "the closeout should preserve the E001 handoff marker", failures)

    return {
        "issue": "M267-E002",
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total + chain_checks,
        "checks_passed": checks_passed + chain_passed,
        "checks_failed": max((checks_total + chain_checks) - (checks_passed + chain_passed), 0),
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": False,
        "upstream_handoff_issue": UPSTREAM_HANDOFF_ISSUE,
        "next_issue": NEXT_ISSUE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "closeout_model": "summary-chain-closeout-for-throws-result-and-nserror-bridge-proof",
        "proof_chain": upstream_results,
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
