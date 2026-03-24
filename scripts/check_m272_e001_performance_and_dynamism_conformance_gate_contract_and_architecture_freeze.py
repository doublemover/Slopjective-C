#!/usr/bin/env python3
"""Fail-closed checker for M272-E001 performance and dynamism conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m272-e001-performance-and-dynamism-conformance-gate-v1"
CONTRACT_ID = "objc3c-part9-performance-and-dynamism-conformance-gate/m272-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-static-summary-chain-plus-d002-live-fast-path-proof"
NEXT_CLOSEOUT_ISSUE = "M272-E002"

A002_CONTRACT_ID = "objc3c-part9-dispatch-intent-source-completion/m272-a002-v1"
B003_CONTRACT_ID = "objc3c-part9-dynamism-control-compatibility-diagnostics/m272-b003-v1"
C003_CONTRACT_ID = "objc3c-part9-dispatch-metadata-interface-preservation/m272-c003-v1"
D002_CONTRACT_ID = "objc3c-part9-live-dispatch-fast-path-and-cache-integration/m272-d002-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_performance_and_dynamism_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_e001_performance_and_dynamism_conformance_gate_contract_and_architecture_freeze_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
DECISIONS_SPEC = ROOT / "spec" / "DECISIONS_LOG.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m272_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DOC_BUILDER = ROOT / "scripts" / "build_objc3c_native_docs.py"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m272" / "M272-E001" / "performance_dynamism_conformance_gate_summary.json"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m272" / "M272-E001" / "ensure_objc3c_native_build_summary.json"

A002_CHECKER = ROOT / "scripts" / "check_m272_a002_frontend_attribute_and_defaulting_surface_completion_core_feature_implementation.py"
B003_CHECKER = ROOT / "scripts" / "check_m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion.py"
C003_CHECKER = ROOT / "scripts" / "check_m272_c003_metadata_and_interface_preservation_for_dynamism_controls_core_feature_expansion.py"
D002_CHECKER = ROOT / "scripts" / "check_m272_d002_live_dispatch_fast_path_and_cache_integration_core_feature_implementation.py"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M272-A002", ROOT / "tmp" / "reports" / "m272" / "M272-A002" / "dispatch_intent_source_completion_summary.json", A002_CONTRACT_ID),
    ("M272-B003", ROOT / "tmp" / "reports" / "m272" / "M272-B003" / "dispatch_control_compatibility_summary.json", B003_CONTRACT_ID),
    ("M272-C003", ROOT / "tmp" / "reports" / "m272" / "M272-C003" / "dispatch_metadata_interface_preservation_summary.json", C003_CONTRACT_ID),
    ("M272-D002", ROOT / "tmp" / "reports" / "m272" / "M272-D002" / "live_dispatch_fast_path_summary.json", D002_CONTRACT_ID),
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
        SnippetCheck("M272-E001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M272-E001-EXP-02", "M272-D002"),
        SnippetCheck("M272-E001-EXP-03", "M272-E002"),
    ),
    PACKET_DOC: (
        SnippetCheck("M272-E001-PKT-01", "Packet: `M272-E001`"),
        SnippetCheck("M272-E001-PKT-02", "Issue: `#7344`"),
        SnippetCheck("M272-E001-PKT-03", "`M272-D002`"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M272-E001-SRC-01", "## M272 performance and dynamism conformance gate"),
        SnippetCheck("M272-E001-SRC-02", "`M272-D002` live summary"),
        SnippetCheck("M272-E001-SRC-03", "`M272-E002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M272-E001-NDOC-01", "## M272 performance and dynamism conformance gate"),
        SnippetCheck("M272-E001-NDOC-02", "`M272-D002` live summary"),
        SnippetCheck("M272-E001-NDOC-03", "`M272-E002`"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M272-E001-ATTR-01", "## M272 performance and dynamism conformance gate (E001)"),
        SnippetCheck("M272-E001-ATTR-02", "seeded fast-path baseline"),
    ),
    DECISIONS_SPEC: (
        SnippetCheck("M272-E001-DEC-01", "## D-034: Part 9 lane E freezes the integrated dispatch-control gate on the published D002 runtime proof"),
        SnippetCheck("M272-E001-DEC-02", "driver/manifest/frontend publication surface"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M272-E001-DRV-01", "M272-E001 performance/dynamism conformance gate anchor"),
        SnippetCheck("M272-E001-DRV-02", "published driver artifact surface"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M272-E001-MAN-01", "M272-E001 performance/dynamism conformance gate anchor"),
        SnippetCheck("M272-E001-MAN-02", "front-door Part 9 publication path remains intentionally singular"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M272-E001-FAPI-01", "M272-E001 performance/dynamism conformance gate anchor"),
        SnippetCheck("M272-E001-FAPI-02", "D002 live fast-path/runtime proof"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M272-E001-RUN-01", "run_m272_a002_lane_a_readiness.py"),
        SnippetCheck("M272-E001-RUN-02", "run_m272_b003_lane_b_readiness.py"),
        SnippetCheck("M272-E001-RUN-03", "run_m272_c003_lane_c_readiness.py"),
        SnippetCheck("M272-E001-RUN-04", "run_m272_d002_lane_d_readiness.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M272-E001-PKG-01", '"check:objc3c:m272-e001-performance-and-dynamism-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M272-E001-PKG-02", '"test:tooling:m272-e001-performance-and-dynamism-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M272-E001-PKG-03", '"check:objc3c:m272-e001-lane-e-readiness"'),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict[str, Any]:
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
        failures.append(Finding(display_path(path), "M272-E001-MISSING", f"required artifact missing: {display_path(path)}"))
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
        "m272-e001-gate-refresh",
        "--summary-out",
        str(ENSURE_BUILD_SUMMARY),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M272-E001-UP-01", f"fast build failed: {build.stdout}{build.stderr}", failures)

    docs = run_process([sys.executable, str(DOC_BUILDER)])
    checks_total += 1
    checks_passed += require(docs.returncode == 0, display_path(DOC_BUILDER), "M272-E001-UP-02", f"docs rebuild failed: {docs.stdout}{docs.stderr}", failures)

    upstream_commands = [
        (A002_CHECKER, [sys.executable, str(A002_CHECKER), "--skip-dynamic-probes"]),
        (B003_CHECKER, [sys.executable, str(B003_CHECKER), "--skip-dynamic-probes"]),
        (C003_CHECKER, [sys.executable, str(C003_CHECKER), "--skip-dynamic-probes"]),
        (D002_CHECKER, [sys.executable, str(D002_CHECKER)] if not skip_dynamic_probes else [sys.executable, str(D002_CHECKER), "--skip-dynamic-probes"]),
    ]
    for index, (artifact, command) in enumerate(upstream_commands, start=3):
        completed = run_process(command)
        checks_total += 1
        checks_passed += require(completed.returncode == 0, display_path(artifact), f"M272-E001-UP-{index:02d}", f"upstream refresh failed: {completed.stdout}{completed.stderr}", failures)
    return checks_total, checks_passed


def validate_upstream_summaries(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    upstream: dict[str, Any] = {}
    for issue, path, contract_id in UPSTREAM_SUMMARIES:
        artifact = display_path(path)
        checks_total += 1
        checks_passed += require(path.exists(), artifact, f"{issue}-SUM-01", "missing upstream summary", failures)
        if not path.exists():
            upstream[issue] = {"missing": True}
            continue
        payload = read_json(path)
        total = payload.get("checks_total", payload.get("total_checks", 0))
        passed = payload.get("checks_passed", payload.get("passed_checks", -1))
        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(total == passed, artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(total > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": total,
            "checks_passed": passed,
        }
    return checks_total, checks_passed, upstream


def validate_d002_runtime_proof(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    issue, path, _ = UPSTREAM_SUMMARIES[-1]
    payload = read_json(path)
    artifact = display_path(path)
    dynamic = payload.get("dynamic", {}) if isinstance(payload, dict) else {}
    probe = dynamic.get("probe_values", {}) if isinstance(dynamic, dict) else {}
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M272-E001-D002-01", "expected D002 summary to include live runtime evidence", failures)

    expected_pairs = {
        "baseline_fast_path_seed_count": "4",
        "dynamic_entry_fast_path_seeded": "1",
        "dynamic_entry_fast_path_reason": "class-final",
        "mixed_first_delta_cache_hit_count": "1",
        "mixed_first_delta_slow_path_lookup_count": "0",
        "mixed_first_delta_fast_path_hit_count": "1",
        "mixed_first_state_last_dispatch_used_fast_path": "1",
        "fallback_first_delta_cache_miss_count": "1",
        "fallback_first_delta_fallback_dispatch_count": "1",
        "fallback_entry_fast_path_seeded": "0",
    }
    for index, (key, expected) in enumerate(expected_pairs.items(), start=2):
        checks_total += 1
        checks_passed += require(
            probe.get(key) == expected,
            artifact,
            f"M272-E001-D002-{index:02d}",
            f"expected {key}={expected}, got {probe.get(key)!r}",
            failures,
        )
    return checks_total, checks_passed, {issue: {"probe_values": probe}}


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    refresh_total, refresh_passed = refresh_upstream_evidence(skip_dynamic_probes, failures)
    checks_total += refresh_total
    checks_passed += refresh_passed

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_total, upstream_passed, upstream = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    dynamic: dict[str, Any]
    if skip_dynamic_probes:
        dynamic = {"skipped": True}
    else:
        dynamic_total, dynamic_passed, dynamic = validate_d002_runtime_proof(failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "upstream": upstream,
        "dynamic": dynamic,
    }
    return summary, failures


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    summary, failures = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(f"[ok] M272-E001 checker passed ({summary['checks_passed']}/{summary['checks_total']} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
