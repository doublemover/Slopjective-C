#!/usr/bin/env python3
"""Fail-closed checker for M271-E001 strict system conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m271-e001-strict-system-conformance-gate-v1"
CONTRACT_ID = "objc3c-strict-system-conformance-gate/m271-e001-v1"
EVIDENCE_MODEL = "a003-b004-c003-summary-chain-plus-linked-d002-helper-runtime-proof"
FAILURE_MODEL = "fail-closed-on-strict-system-gate-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M271-E002"

A003_CONTRACT_ID = "objc3c-part8-retainable-c-family-source-completion/m271-a003-v1"
B004_CONTRACT_ID = "objc3c-part8-capture-list-retainable-family-legality/m271-b004-v1"
C003_CONTRACT_ID = "objc3c-part8-borrowed-retainable-family-abi-completion/m271-c003-v1"
D002_CONTRACT_ID = "objc3c-part8-live-cleanup-retainable-runtime-integration/m271-d002-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m271_strict_system_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m271" / "m271_e001_strict_system_conformance_gate_contract_and_architecture_freeze_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
CONFORMANCE_SPEC = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m271_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
D002_CHECKER = ROOT / "scripts" / "check_m271_d002_live_cleanup_helpers_and_retainable_family_integration_core_feature_implementation.py"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m271" / "M271-E001" / "strict_system_conformance_gate_summary.json"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-E001" / "ensure_objc3c_native_build_summary.json"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M271-A003", ROOT / "tmp" / "reports" / "m271" / "M271-A003" / "retainable_c_family_source_completion_summary.json", A003_CONTRACT_ID),
    ("M271-B004", ROOT / "tmp" / "reports" / "m271" / "M271-B004" / "capture_list_and_retainable_family_legality_completion_summary.json", B004_CONTRACT_ID),
    ("M271-C003", ROOT / "tmp" / "reports" / "m271" / "M271-C003" / "borrowed_retainable_abi_completion_summary.json", C003_CONTRACT_ID),
    ("M271-D002", ROOT / "tmp" / "reports" / "m271" / "M271-D002" / "live_cleanup_retainable_runtime_integration_summary.json", D002_CONTRACT_ID),
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
        SnippetCheck("M271-E001-EXP-01", "# M271 Strict System Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M271-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M271-E001-EXP-03", "M271-D002"),
        SnippetCheck("M271-E001-EXP-04", "The next issue is `M271-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M271-E001-PKT-01", "# M271-E001 Strict System Conformance Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M271-E001-PKT-02", "Issue: `#7332`"),
        SnippetCheck("M271-E001-PKT-03", "- `M271-A003`"),
        SnippetCheck("M271-E001-PKT-04", "- `M271-D002`"),
        SnippetCheck("M271-E001-PKT-05", "- `M271-E002` is the next issue"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M271-E001-SRC-01", "## M271 strict system conformance gate"),
        SnippetCheck("M271-E001-SRC-02", "`M271-D002` linked"),
        SnippetCheck("M271-E001-SRC-03", "`M271-E002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M271-E001-NDOC-01", "## M271 strict system conformance gate"),
        SnippetCheck("M271-E001-NDOC-02", "`M271-D002` linked"),
        SnippetCheck("M271-E001-NDOC-03", "`M271-E002`"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M271-E001-ABS-01", "M271-E001 strict system conformance gate note:"),
        SnippetCheck("M271-E001-ABS-02", "`M271-A003`, `M271-B004`, `M271-C003`, and `M271-D002`"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M271-E001-ATTR-01", "Current implementation status (`M271-E001`):"),
        SnippetCheck("M271-E001-ATTR-02", "linked `M271-D002` `helperSurface`"),
        SnippetCheck("M271-E001-ATTR-03", "`M271-E002`"),
    ),
    CONFORMANCE_SPEC: (
        SnippetCheck("M271-E001-CONF-01", "M271-E001 strict system conformance gate note:"),
        SnippetCheck("M271-E001-CONF-02", "`M271-A003`, `M271-B004`, `M271-C003`, and `M271-D002`"),
        SnippetCheck("M271-E001-CONF-03", "`M271-E002`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M271-E001-ARCH-01", "## M271 Part 8 Strict System Conformance Gate (E001)"),
        SnippetCheck("M271-E001-ARCH-02", "linked `M271-D002` `helperSurface`"),
        SnippetCheck("M271-E001-ARCH-03", "`M271-E002`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M271-E001-RTR-01", "## M271 strict system conformance gate"),
        SnippetCheck("M271-E001-RTR-02", "does not add a new runtime probe"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M271-E001-DRV-01", "M271-E001 strict system conformance gate anchor"),
        SnippetCheck("M271-E001-DRV-02", "front-door publication"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M271-E001-MAN-01", "M271-E001 strict system conformance gate anchor"),
        SnippetCheck("M271-E001-MAN-02", "front-door Part 8 publication path remains intentionally fail-closed"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M271-E001-FAPI-01", "M271-E001 strict system conformance gate anchor"),
        SnippetCheck("M271-E001-FAPI-02", "D002 live cleanup/runtime proof"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M271-E001-RUN-01", "run_m271_a003_lane_a_readiness.py"),
        SnippetCheck("M271-E001-RUN-02", "run_m271_b004_lane_b_readiness.py"),
        SnippetCheck("M271-E001-RUN-03", "run_m271_c003_lane_c_readiness.py"),
        SnippetCheck("M271-E001-RUN-04", "run_m271_d002_lane_d_readiness.py"),
        SnippetCheck("M271-E001-RUN-05", "test_check_m271_e001_strict_system_conformance_gate_contract_and_architecture_freeze.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M271-E001-PKG-01", '"check:objc3c:m271-e001-strict-system-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M271-E001-PKG-02", '"test:tooling:m271-e001-strict-system-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M271-E001-PKG-03", '"check:objc3c:m271-e001-lane-e-readiness"'),
    ),
}


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
        failures.append(Finding(display_path(path), "M271-E001-MISSING", f"required artifact missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


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
    checks_total = 0
    checks_passed = 0
    payload = read_json(UPSTREAM_SUMMARIES[-1][1])
    artifact = display_path(UPSTREAM_SUMMARIES[-1][1])
    dynamic = payload.get("dynamic", {})
    probe = dynamic.get("probe_payload", {}) if isinstance(dynamic, dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M271-E001-D002-01", "expected D002 summary to include dynamic probe evidence", failures)

    required_pairs = {
        "result": "77",
        "close_fd_count": "1",
        "release_temp_count": "1",
        "drained_autorelease_value_count": "1",
        "retain_call_count": "1",
        "release_call_count": "1",
        "autorelease_call_count": "1",
        "autoreleasepool_push_count": "1",
        "autoreleasepool_pop_count": "1",
    }
    for key, expected in required_pairs.items():
        checks_total += 1
        checks_passed += require(
            probe.get(key) == expected,
            artifact,
            f"M271-E001-D002-{key}",
            f"expected {key}={expected}, got {probe.get(key)!r}",
            failures,
        )

    return checks_passed, checks_total, {"probe_payload": probe}


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_summary: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        total = len(snippets)
        passed = ensure_snippets(path, snippets, failures)
        checks_total += total
        checks_passed += passed
        static_summary[display_path(path)] = {"checks": total, "ok": passed == total}

    upstream_total, upstream_passed, upstream_summary = validate_upstream_summaries(failures)
    checks_total += upstream_total
    checks_passed += upstream_passed

    dynamic: dict[str, Any] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        build = run_process([
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m271-e001-strict-system-conformance-gate",
            "--summary-out",
            str(ENSURE_BUILD_SUMMARY),
        ])
        checks_total += 1
        checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M271-E001-BUILD-01", build.stderr or build.stdout or "fast native build failed", failures)
        if build.returncode == 0:
            d002_refresh = run_process([sys.executable, str(D002_CHECKER)])
            checks_total += 1
            checks_passed += require(
                d002_refresh.returncode == 0,
                display_path(D002_CHECKER),
                "M271-E001-D002-REFRESH-01",
                d002_refresh.stderr or d002_refresh.stdout or "failed to refresh D002 full summary",
                failures,
            )
            dyn_passed, dyn_total, dyn_payload = validate_d002_runtime_proof(failures)
            checks_total += dyn_total
            checks_passed += dyn_passed
            dynamic = {
                "skipped": False,
                "d002_summary_refresh": display_path(UPSTREAM_SUMMARIES[-1][1]),
                "d002_runtime_gate": dyn_payload,
            }

    summary = {
        "issue": "M271-E001",
        "contract_id": CONTRACT_ID,
        "mode": MODE,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": checks_total - checks_passed,
        "ok": not failures,
        "dynamic_probes_executed": not skip_dynamic_probes,
        "static": static_summary,
        "upstream": upstream_summary,
        "dynamic": dynamic,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }
    return summary, failures


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    summary, failures = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
