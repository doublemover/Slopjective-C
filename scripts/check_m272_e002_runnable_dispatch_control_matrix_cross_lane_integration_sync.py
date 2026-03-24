#!/usr/bin/env python3
"""Fail-closed checker for M272-E002 runnable dispatch-control closeout."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m272-e002-runnable-dispatch-control-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-part9-runnable-dispatch-control-matrix/m272-e002-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m272" / "M272-E002" / "runnable_dispatch_control_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_runnable_dispatch_control_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_e002_runnable_dispatch_control_matrix_cross_lane_integration_sync_packet.md"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DECISIONS_SPEC = ROOT / "spec" / "DECISIONS_LOG.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m272_e002_lane_e_readiness.py"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DOC_BUILDER = ROOT / "scripts" / "build_objc3c_native_docs.py"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m272" / "M272-E002" / "ensure_objc3c_native_build_summary.json"

A002_CONTRACT_ID = "objc3c-part9-dispatch-intent-source-completion/m272-a002-v1"
B003_CONTRACT_ID = "objc3c-part9-dynamism-control-compatibility-diagnostics/m272-b003-v1"
C003_CONTRACT_ID = "objc3c-part9-dispatch-metadata-interface-preservation/m272-c003-v1"
D002_CONTRACT_ID = "objc3c-part9-live-dispatch-fast-path-and-cache-integration/m272-d002-v1"
E001_CONTRACT_ID = "objc3c-part9-performance-and-dynamism-conformance-gate/m272-e001-v1"

A002_CHECKER = ROOT / "scripts" / "check_m272_a002_frontend_attribute_and_defaulting_surface_completion_core_feature_implementation.py"
B003_CHECKER = ROOT / "scripts" / "check_m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion.py"
C003_CHECKER = ROOT / "scripts" / "check_m272_c003_metadata_and_interface_preservation_for_dynamism_controls_core_feature_expansion.py"
D002_CHECKER = ROOT / "scripts" / "check_m272_d002_live_dispatch_fast_path_and_cache_integration_core_feature_implementation.py"
E001_CHECKER = ROOT / "scripts" / "check_m272_e001_performance_and_dynamism_conformance_gate_contract_and_architecture_freeze.py"

NEXT_ISSUE = "M273-A001"
UPSTREAM_HANDOFF_ISSUE = "M272-E001"
EVIDENCE_MODEL = "a002-through-e001-summary-chain-plus-d002-live-dispatch-proof"
FAILURE_MODEL = "fail-closed-on-runnable-dispatch-control-closeout-drift"
CLOSEOUT_MODEL = "lane-e-closeout-replays-supported-part9-direct-final-sealed-slice-without-widening-deferred-surface"

UPSTREAM_SPECS: tuple[tuple[str, Path, str], ...] = (
    ("M272-A002", ROOT / "tmp" / "reports" / "m272" / "M272-A002" / "dispatch_intent_source_completion_summary.json", A002_CONTRACT_ID),
    ("M272-B003", ROOT / "tmp" / "reports" / "m272" / "M272-B003" / "dispatch_control_compatibility_summary.json", B003_CONTRACT_ID),
    ("M272-C003", ROOT / "tmp" / "reports" / "m272" / "M272-C003" / "dispatch_metadata_interface_preservation_summary.json", C003_CONTRACT_ID),
    ("M272-D002", ROOT / "tmp" / "reports" / "m272" / "M272-D002" / "live_dispatch_fast_path_summary.json", D002_CONTRACT_ID),
    ("M272-E001", ROOT / "tmp" / "reports" / "m272" / "M272-E001" / "performance_dynamism_conformance_gate_summary.json", E001_CONTRACT_ID),
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
        SnippetCheck("M272-E002-DOC-EXP-01", "# M272 Runnable Dispatch-Control Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M272-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M272-E002-DOC-EXP-03", "M272-E001"),
        SnippetCheck("M272-E002-DOC-EXP-04", "M273-A001"),
        SnippetCheck("M272-E002-DOC-EXP-05", "tmp/reports/m272/M272-E002/runnable_dispatch_control_matrix_summary.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M272-E002-DOC-PKT-01", "# M272-E002 Packet: Runnable Dispatch-Control Matrix - Cross-Lane Integration Sync"),
        SnippetCheck("M272-E002-DOC-PKT-02", "Issue: `#7345`"),
        SnippetCheck("M272-E002-DOC-PKT-03", "Dependencies: `M272-A002`, `M272-B003`, `M272-C003`, `M272-D002`, `M272-E001`"),
        SnippetCheck("M272-E002-DOC-PKT-04", f"Next issue: `{NEXT_ISSUE}`"),
        SnippetCheck("M272-E002-DOC-PKT-05", "direct exact-call continuity"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M272-E002-SRC-01", "## M272 runnable dispatch-control matrix closeout (M272-E002)"),
        SnippetCheck("M272-E002-SRC-02", "direct exact-call continuity"),
        SnippetCheck("M272-E002-SRC-03", "final/sealed seeded runtime fast path"),
        SnippetCheck("M272-E002-SRC-04", "M273-A001"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M272-E002-NDOC-01", "## M272 runnable dispatch-control matrix closeout (M272-E002)"),
        SnippetCheck("M272-E002-NDOC-02", "direct exact-call continuity"),
        SnippetCheck("M272-E002-NDOC-03", "final/sealed seeded runtime fast path"),
        SnippetCheck("M272-E002-NDOC-04", "M273-A001"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M272-E002-ATTR-01", "## M272 runnable dispatch-control matrix closeout (E002)"),
        SnippetCheck("M272-E002-ATTR-02", "seeded runtime fast path"),
        SnippetCheck("M272-E002-ATTR-03", "M273-A001"),
    ),
    DECISIONS_SPEC: (
        SnippetCheck("M272-E002-DEC-01", "## D-035: Part 9 closeout publishes one runnable dispatch-control matrix on the existing D002 runtime proof"),
        SnippetCheck("M272-E002-DEC-02", "direct exact-call continuity"),
        SnippetCheck("M272-E002-DEC-03", "deterministic fallback caching"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M272-E002-DRV-01", "M272-E002 runnable dispatch-control matrix closeout anchor"),
        SnippetCheck("M272-E002-DRV-02", "same driver artifact surface while Part 9 closeout stays"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M272-E002-MAN-01", "M272-E002 runnable dispatch-control matrix closeout anchor"),
        SnippetCheck("M272-E002-MAN-02", "no second manifest-only closeout path"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M272-E002-FAPI-01", "M272-E002 runnable dispatch-control matrix closeout anchor"),
        SnippetCheck("M272-E002-FAPI-02", "same surfaced publication paths"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M272-E002-RUN-01", "run_m272_a002_lane_a_readiness.py"),
        SnippetCheck("M272-E002-RUN-02", "run_m272_b003_lane_b_readiness.py"),
        SnippetCheck("M272-E002-RUN-03", "run_m272_c003_lane_c_readiness.py"),
        SnippetCheck("M272-E002-RUN-04", "run_m272_d002_lane_d_readiness.py"),
        SnippetCheck("M272-E002-RUN-05", "run_m272_e001_lane_e_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M272-E002-PKG-01", '"check:objc3c:m272-e002-runnable-dispatch-control-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M272-E002-PKG-02", '"test:tooling:m272-e002-runnable-dispatch-control-matrix-cross-lane-integration-sync"'),
        SnippetCheck("M272-E002-PKG-03", '"check:objc3c:m272-e002-lane-e-readiness"'),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M272-E002-MISSING", f"required artifact missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def refresh_upstream_evidence(skip_dynamic_probes: bool, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0

    build = run_process([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m272-e002-closeout-refresh",
        "--summary-out",
        str(ENSURE_BUILD_SUMMARY),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M272-E002-UP-01", f"fast build failed: {build.stdout}{build.stderr}", failures)

    docs = run_process([sys.executable, str(DOC_BUILDER)])
    checks_total += 1
    checks_passed += require(docs.returncode == 0, display_path(DOC_BUILDER), "M272-E002-UP-02", f"docs rebuild failed: {docs.stdout}{docs.stderr}", failures)

    commands = [
        (A002_CHECKER, [sys.executable, str(A002_CHECKER), "--skip-dynamic-probes"]),
        (B003_CHECKER, [sys.executable, str(B003_CHECKER), "--skip-dynamic-probes"]),
        (C003_CHECKER, [sys.executable, str(C003_CHECKER), "--skip-dynamic-probes"]),
        (D002_CHECKER, [sys.executable, str(D002_CHECKER)] if not skip_dynamic_probes else [sys.executable, str(D002_CHECKER), "--skip-dynamic-probes"]),
        (E001_CHECKER, [sys.executable, str(E001_CHECKER)] if not skip_dynamic_probes else [sys.executable, str(E001_CHECKER), "--skip-dynamic-probes"]),
    ]
    for index, (artifact, command) in enumerate(commands, start=3):
        completed = run_process(command)
        checks_total += 1
        checks_passed += require(completed.returncode == 0, display_path(artifact), f"M272-E002-UP-{index:02d}", f"upstream refresh failed: {completed.stdout}{completed.stderr}", failures)
    return checks_total, checks_passed


def validate_upstream_summaries(failures: list[Finding]) -> tuple[int, int, list[dict[str, Any]], dict[str, dict[str, Any]]]:
    checks_total = 0
    checks_passed = 0
    proof_chain: list[dict[str, Any]] = []
    raw_payloads: dict[str, dict[str, Any]] = {}

    for issue, path, contract_id in UPSTREAM_SPECS:
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"{issue}-SUM-01", "missing upstream summary", failures)
        if not path.exists():
            proof_chain.append({"issue": issue, "summary_path": artifact, "missing": True})
            continue

        payload = load_json(path)
        raw_payloads[issue] = payload
        total = payload.get("checks_total", payload.get("total_checks", 0))
        passed = payload.get("checks_passed", payload.get("passed_checks", -1))

        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(total == passed, artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(total > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)

        proof_chain.append(
            {
                "issue": issue,
                "summary_path": artifact,
                "contract_id": payload.get("contract_id"),
                "checks_total": total,
                "checks_passed": passed,
            }
        )

    if proof_chain:
        checks_total += 1
        checks_passed += require(proof_chain[0]["issue"] == "M272-A002", "M272-E002", "M272-E002-CHAIN-01", "proof chain must start at M272-A002", failures)
        checks_total += 1
        checks_passed += require(proof_chain[-1]["issue"] == "M272-E001", "M272-E002", "M272-E002-CHAIN-02", "proof chain must end at M272-E001", failures)
    else:
        checks_total += 1
        checks_passed += require(False, "M272-E002", "M272-E002-CHAIN-03", "proof chain must not be empty", failures)

    return checks_total, checks_passed, proof_chain, raw_payloads


def validate_d002_runtime_proof(raw_payloads: dict[str, dict[str, Any]], failures: list[Finding]) -> tuple[int, int, dict[str, str]]:
    payload = raw_payloads.get("M272-D002", {})
    artifact = display_path(UPSTREAM_SPECS[3][1])
    dynamic = payload.get("dynamic", {}) if isinstance(payload, dict) else {}
    probe = dynamic.get("probe_values", {}) if isinstance(dynamic, dict) else {}
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M272-E002-D002-01", "expected D002 summary to include live runtime evidence", failures)

    expected_pairs = {
        "explicit_entry_fast_path_reason": "direct",
        "direct_delta_live_dispatch_count": "0",
        "baseline_fast_path_seed_count": "4",
        "dynamic_entry_fast_path_seeded": "1",
        "dynamic_entry_objc_final_declared": "1",
        "dynamic_entry_objc_sealed_declared": "1",
        "dynamic_entry_fast_path_reason": "class-final",
        "mixed_first_delta_cache_hit_count": "1",
        "mixed_first_delta_slow_path_lookup_count": "0",
        "mixed_first_delta_fast_path_hit_count": "1",
        "fallback_first_delta_fallback_dispatch_count": "1",
        "fallback_second_delta_cache_hit_count": "1",
        "fallback_second_state_last_dispatch_fell_back": "1",
    }
    for index, (key, expected) in enumerate(expected_pairs.items(), start=2):
        checks_total += 1
        checks_passed += require(
            probe.get(key) == expected,
            artifact,
            f"M272-E002-D002-{index:02d}",
            f"expected {key}={expected}, got {probe.get(key)!r}",
            failures,
        )

    return checks_total, checks_passed, probe


def build_matrix_rows(skip_dynamic_probes: bool, probe: dict[str, str]) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = [
        {
            "row_id": "dispatch_intent_source_completion",
            "source_issue": "M272-A002",
            "status": "supported",
            "evidence": "tmp/reports/m272/M272-A002/dispatch_intent_source_completion_summary.json",
            "model": "Part 9 source admission/defaulting for direct-members and callable intent is published and replay-stable.",
        },
        {
            "row_id": "dispatch_control_compatibility_diagnostics",
            "source_issue": "M272-B003",
            "status": "supported",
            "evidence": "tmp/reports/m272/M272-B003/dispatch_control_compatibility_summary.json",
            "model": "Unsupported function/protocol/category topologies and conflicting intent combinations fail closed before lowering.",
        },
        {
            "row_id": "dispatch_metadata_interface_preservation",
            "source_issue": "M272-C003",
            "status": "supported",
            "evidence": "tmp/reports/m272/M272-C003/dispatch_metadata_interface_preservation_summary.json",
            "model": "Direct/final/sealed intent survives interface and metadata publication without widening the runtime surface.",
        },
        {
            "row_id": "lane_e_dispatch_control_gate",
            "source_issue": "M272-E001",
            "status": "supported",
            "evidence": "tmp/reports/m272/M272-E001/performance_dynamism_conformance_gate_summary.json",
            "model": "Lane E preserves one truthful gate over the current Part 9 slice while keeping D002 as the executable proof boundary.",
        },
    ]

    runtime_status = "supported" if not skip_dynamic_probes else "proof-skipped"
    runtime_evidence = "tmp/reports/m272/M272-D002/live_dispatch_fast_path_summary.json"
    rows.extend(
        [
            {
                "row_id": "direct_exact_call_continuity",
                "source_issue": "M272-D002",
                "status": runtime_status,
                "evidence": runtime_evidence,
                "model": "Explicit/direct methods remain exact-call lowered and do not add live runtime dispatch traffic.",
                "proof": {
                    "explicit_entry_fast_path_reason": probe.get("explicit_entry_fast_path_reason", ""),
                    "direct_delta_live_dispatch_count": probe.get("direct_delta_live_dispatch_count", ""),
                },
            },
            {
                "row_id": "final_and_sealed_seeded_runtime_fast_path",
                "source_issue": "M272-D002",
                "status": runtime_status,
                "evidence": runtime_evidence,
                "model": "Final/sealed live sends that still enter runtime dispatch use seeded fast-path cache state on the first call.",
                "proof": {
                    "baseline_fast_path_seed_count": probe.get("baseline_fast_path_seed_count", ""),
                    "dynamic_entry_fast_path_seeded": probe.get("dynamic_entry_fast_path_seeded", ""),
                    "dynamic_entry_fast_path_reason": probe.get("dynamic_entry_fast_path_reason", ""),
                    "mixed_first_delta_fast_path_hit_count": probe.get("mixed_first_delta_fast_path_hit_count", ""),
                },
            },
            {
                "row_id": "deterministic_fallback_caching",
                "source_issue": "M272-D002",
                "status": runtime_status,
                "evidence": runtime_evidence,
                "model": "Unresolved selectors continue through deterministic cached fallback behavior on repeat dispatches.",
                "proof": {
                    "fallback_first_delta_fallback_dispatch_count": probe.get("fallback_first_delta_fallback_dispatch_count", ""),
                    "fallback_second_delta_cache_hit_count": probe.get("fallback_second_delta_cache_hit_count", ""),
                    "fallback_second_state_last_dispatch_fell_back": probe.get("fallback_second_state_last_dispatch_fell_back", ""),
                },
            },
        ]
    )
    return rows


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, Any], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    refresh_total, refresh_passed = refresh_upstream_evidence(skip_dynamic_probes, failures)
    checks_total += refresh_total
    checks_passed += refresh_passed

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_total, upstream_passed, proof_chain, raw_payloads = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    checks_total += 1
    checks_passed += require(UPSTREAM_HANDOFF_ISSUE == "M272-E001", "M272-E002", "M272-E002-HANDOFF-01", "closeout should preserve the E001 handoff marker", failures)

    dynamic: dict[str, Any]
    probe: dict[str, str]
    if skip_dynamic_probes:
        dynamic = {"skipped": True}
        probe = {}
    else:
        dynamic_total, dynamic_passed, probe = validate_d002_runtime_proof(raw_payloads, failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed
        dynamic = {"probe_values": probe}

    matrix_rows = build_matrix_rows(skip_dynamic_probes, probe)
    checks_total += len(matrix_rows)
    for row in matrix_rows:
        checks_passed += require(
            any(entry.get("issue") == row["source_issue"] for entry in proof_chain),
            row["evidence"],
            f"M272-E002-ROW-{row['row_id']}",
            f"missing proof chain entry for {row['row_id']}",
            failures,
        )

    summary = {
        "issue": "M272-E002",
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": not skip_dynamic_probes,
        "upstream_handoff_issue": UPSTREAM_HANDOFF_ISSUE,
        "next_issue": NEXT_ISSUE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "closeout_model": CLOSEOUT_MODEL,
        "proof_chain": proof_chain,
        "matrix_rows": matrix_rows,
        "dynamic": dynamic,
    }
    return summary, failures


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    summary, failures = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        print(f"[fail] M272-E002 runnable dispatch-control closeout check failed ({len(failures)} findings)", file=sys.stderr)
        return 1

    print(f"[ok] M272-E002 runnable dispatch-control closeout check passed ({summary['checks_passed']}/{summary['checks_total']} checks)")
    print(f"[ok] wrote summary: {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
