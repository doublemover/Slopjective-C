#!/usr/bin/env python3
"""Fail-closed checker for M269-E001 task and executor conformance gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m269-e001-task-executor-conformance-gate-v1"
CONTRACT_ID = "objc3c-task-executor-conformance-gate/m269-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d003-summary-chain-plus-hardened-d003-runtime-proof"
FAILURE_MODEL = "fail-closed-on-task-executor-gate-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M269-E002"

A002_CONTRACT_ID = "objc3c-part7-task-group-cancellation-source-closure/m269-a002-v1"
B003_CONTRACT_ID = "objc3c-part7-executor-hop-affinity-compatibility/m269-b003-v1"
C003_CONTRACT_ID = "objc3c-part7-task-runtime-abi-completion/m269-c003-v1"
D003_CONTRACT_ID = "objc3c-part7-task-runtime-hardening/m269-d003-v1"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m269_task_and_executor_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m269" / "m269_e001_task_and_executor_conformance_gate_contract_and_architecture_freeze_packet.md"
NATIVE_DOCSRC = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ABSTRACT_SPEC = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
ATTRIBUTE_SPEC = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
READINESS_RUNNER = ROOT / "scripts" / "run_m269_e001_lane_e_readiness.py"
PACKAGE_JSON = ROOT / "package.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m269" / "M269-E001" / "task_executor_conformance_gate_summary.json"
ENSURE_BUILD_SUMMARY = ROOT / "tmp" / "reports" / "m269" / "M269-E001" / "ensure_objc3c_native_build_summary.json"

UPSTREAM_SUMMARIES: tuple[tuple[str, Path, str], ...] = (
    ("M269-A002", ROOT / "tmp" / "reports" / "m269" / "M269-A002" / "task_group_cancellation_source_closure_summary.json", A002_CONTRACT_ID),
    ("M269-B003", ROOT / "tmp" / "reports" / "m269" / "M269-B003" / "executor_hop_affinity_compatibility_summary.json", B003_CONTRACT_ID),
    ("M269-C003", ROOT / "tmp" / "reports" / "m269" / "M269-C003" / "task_runtime_abi_completion_summary.json", C003_CONTRACT_ID),
    ("M269-D003", ROOT / "tmp" / "reports" / "m269" / "M269-D003" / "task_runtime_hardening_summary.json", D003_CONTRACT_ID),
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
        SnippetCheck("M269-E001-EXP-01", "# M269 Task And Executor Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
        SnippetCheck("M269-E001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M269-E001-EXP-03", "M269-D003"),
        SnippetCheck("M269-E001-EXP-04", "The next issue is `M269-E002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M269-E001-PKT-01", "# M269-E001 Task And Executor Conformance Gate Contract And Architecture Freeze Packet"),
        SnippetCheck("M269-E001-PKT-02", "Issue: `#7305`"),
        SnippetCheck("M269-E001-PKT-03", "- `M269-A002`"),
        SnippetCheck("M269-E001-PKT-04", "- `M269-D003`"),
        SnippetCheck("M269-E001-PKT-05", "- `M269-E002` is the next issue"),
    ),
    NATIVE_DOCSRC: (
        SnippetCheck("M269-E001-SRC-01", "## M269 task and executor conformance gate"),
        SnippetCheck("M269-E001-SRC-02", "`M269-D003` live runtime"),
        SnippetCheck("M269-E001-SRC-03", "`M269-E002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M269-E001-NDOC-01", "## M269 task and executor conformance gate"),
        SnippetCheck("M269-E001-NDOC-02", "`M269-D003` live runtime"),
        SnippetCheck("M269-E001-NDOC-03", "`M269-E002`"),
    ),
    ABSTRACT_SPEC: (
        SnippetCheck("M269-E001-ABS-01", "M269-E001 task and executor conformance gate note:"),
        SnippetCheck("M269-E001-ABS-02", "`M269-A002`, `M269-B003`, `M269-C003`, and `M269-D003`"),
    ),
    ATTRIBUTE_SPEC: (
        SnippetCheck("M269-E001-ATTR-01", "Current implementation status (`M269-E001`):"),
        SnippetCheck("M269-E001-ATTR-02", "hardened `M269-D003` runtime probe"),
        SnippetCheck("M269-E001-ATTR-03", "`M269-E002`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M269-E001-ARCH-01", "## M269 Part 7 Task And Executor Conformance Gate (E001)"),
        SnippetCheck("M269-E001-ARCH-02", "hardened `M269-D003` live runtime"),
        SnippetCheck("M269-E001-ARCH-03", "`M269-E002`"),
    ),
    RUNTIME_README: (
        SnippetCheck("M269-E001-RTR-01", "## M269 task and executor conformance gate"),
        SnippetCheck("M269-E001-RTR-02", "does not add a new runtime probe"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M269-E001-DRV-01", "M269-E001 task/executor conformance gate anchor"),
        SnippetCheck("M269-E001-DRV-02", "front-door publication path remains fail-closed"),
    ),
    MANIFEST_CPP: (
        SnippetCheck("M269-E001-MAN-01", "M269-E001 task/executor conformance gate anchor"),
        SnippetCheck("M269-E001-MAN-02", "front-door task publication path remains intentionally fail-closed"),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M269-E001-FAPI-01", "M269-E001 task/executor conformance gate anchor"),
        SnippetCheck("M269-E001-FAPI-02", "hardened D003 runtime proof"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M269-E001-RUN-01", "run_m269_a002_lane_a_readiness.py"),
        SnippetCheck("M269-E001-RUN-02", "run_m269_b003_lane_b_readiness.py"),
        SnippetCheck("M269-E001-RUN-03", "run_m269_c003_lane_c_readiness.py"),
        SnippetCheck("M269-E001-RUN-04", "run_m269_d003_lane_d_readiness.py"),
        SnippetCheck("M269-E001-RUN-05", "test_check_m269_e001_task_and_executor_conformance_gate_contract_and_architecture_freeze.py"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M269-E001-PKG-01", '"check:objc3c:m269-e001-task-and-executor-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M269-E001-PKG-02", '"test:tooling:m269-e001-task-and-executor-conformance-gate-contract-and-architecture-freeze"'),
        SnippetCheck("M269-E001-PKG-03", '"check:objc3c:m269-e001-lane-e-readiness"'),
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
        failures.append(Finding(display_path(path), "M269-E001-MISSING", f"required artifact missing: {display_path(path)}"))
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
        checks_total += 1
        checks_passed += require(payload.get("contract_id") == contract_id, artifact, f"{issue}-SUM-02", "upstream contract drifted", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_total", 0) == payload.get("checks_passed", -1), artifact, f"{issue}-SUM-03", "upstream summary lost full coverage", failures)
        checks_total += 1
        checks_passed += require(payload.get("checks_total", 0) > 0, artifact, f"{issue}-SUM-04", "upstream summary reports zero checks", failures)
        upstream[issue] = {
            "contract_id": payload.get("contract_id"),
            "checks_total": payload.get("checks_total"),
            "checks_passed": payload.get("checks_passed"),
        }
    return checks_total, checks_passed, upstream


def validate_hardened_runtime_summary(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    payload = read_json(UPSTREAM_SUMMARIES[-1][1])
    dynamic = payload.get("dynamic", {})
    runtime_probe = dynamic.get("runtime_probe_output", {}) if isinstance(dynamic, dict) else {}

    required_pairs = {
        "pass1_spawn_group": "111",
        "pass1_wait_next": "23",
        "pass1_cancel_all": "31",
        "pass1_autoreleasepool_depth": "0",
        "pass1_autoreleasepool_max_depth": "1",
        "replay_equal": "1",
    }
    for key, expected in required_pairs.items():
        checks_total += 1
        checks_passed += require(
            runtime_probe.get(key) == expected,
            display_path(UPSTREAM_SUMMARIES[-1][1]),
            f"M269-E001-DYN-{key}",
            f"expected {key}={expected}, got {runtime_probe.get(key)!r}",
            failures,
        )

    retained_probe = dynamic.get("retained_frontdoor_probe", {}) if isinstance(dynamic, dict) else {}
    diagnostics = retained_probe.get("diagnostics", "") if isinstance(retained_probe, dict) else ""
    checks_total += 1
    checks_passed += require(
        retained_probe.get("returncode") == 1,
        display_path(UPSTREAM_SUMMARIES[-1][1]),
        "M269-E001-DYN-FRONTDOOR-01",
        "expected retained front-door probe to remain fail-closed",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        "O3S260" in diagnostics or "O3L300" in diagnostics,
        display_path(UPSTREAM_SUMMARIES[-1][1]),
        "M269-E001-DYN-FRONTDOOR-02",
        "expected retained front-door probe diagnostics to stay truthful",
        failures,
    )

    return checks_passed, checks_total, {
        "runtime_probe_output": runtime_probe,
        "retained_frontdoor_probe": retained_probe,
    }


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
            "m269-e001-task-executor-conformance-gate",
            "--summary-out",
            str(ENSURE_BUILD_SUMMARY),
        ])
        checks_total += 1
        checks_passed += require(build.returncode == 0, display_path(BUILD_HELPER), "M269-E001-BUILD-01", build.stderr or build.stdout or "fast native build failed", failures)
        if build.returncode == 0:
            dyn_passed, dyn_total, dyn_payload = validate_hardened_runtime_summary(failures)
            checks_total += dyn_total
            checks_passed += dyn_passed
            dynamic = {"skipped": False, "hardened_runtime_gate": dyn_payload}

    summary = {
        "issue": "M269-E001",
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
