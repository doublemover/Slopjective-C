#!/usr/bin/env python3
"""Fail-closed checker for M276-D002 shared native build helper."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-d002-shared-native-build-helper-v1"
CONTRACT_ID = "objc3c-shared-native-build-helper/m276-d002-v1"
NEXT_ISSUE = "M276-D001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-D002" / "shared_native_build_helper_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_shared_native_build_helper_for_readiness_runners_and_checkers_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_d002_shared_native_build_helper_for_readiness_runners_and_checkers_packet.md"
HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
RUNNER_ONE = ROOT / "scripts" / "run_m262_b001_lane_b_readiness.py"
RUNNER_TWO = ROOT / "scripts" / "run_m263_a001_lane_a_readiness.py"
README = ROOT / "README.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_d002_shared_native_build_helper_for_readiness_runners_and_checkers.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_d002_lane_d_readiness.py"


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
        SnippetCheck("M276-D002-EXP-01", "Contract ID: `objc3c-shared-native-build-helper/m276-d002-v1`"),
        SnippetCheck("M276-D002-EXP-02", "scripts/ensure_objc3c_native_build.py"),
        SnippetCheck("M276-D002-EXP-03", "M276-D001"),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-D002-PKT-01", "Issue: `#7394`"),
        SnippetCheck("M276-D002-PKT-02", "scripts/ensure_objc3c_native_build.py"),
        SnippetCheck("M276-D002-PKT-03", "Run helper `full --force-reconfigure`."),
    ),
    HELPER: (
        SnippetCheck("M276-D002-HLP-01", 'MODE_TO_EXECUTION_MODE = {'),
        SnippetCheck("M276-D002-HLP-02", '"fast": "binaries-only"'),
        SnippetCheck("M276-D002-HLP-03", '"contracts": "packets-binary"'),
        SnippetCheck("M276-D002-HLP-04", '"full": "full"'),
        SnippetCheck("M276-D002-HLP-05", '"--force-reconfigure"'),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-D002-BLD-01", "[switch]$ForceReconfigure"),
        SnippetCheck("M276-D002-BLD-02", 'Write-BuildStep "cmake_configure=force-reconfigure"'),
    ),
    RUNNER_ONE: (
        SnippetCheck("M276-D002-RUN-01", "ensure_objc3c_native_build.py"),
        SnippetCheck("M276-D002-RUN-02", '"--mode",'),
    ),
    RUNNER_TWO: (
        SnippetCheck("M276-D002-RUN-03", "ensure_objc3c_native_build.py"),
        SnippetCheck("M276-D002-RUN-04", '"m263-a001-lane-a-readiness"'),
    ),
    README: (
        SnippetCheck("M276-D002-README-01", "shared native build helper"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-D002-DOC-01", "## Shared native build helper (M276-D002)"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-D002-NDOC-01", "## Shared native build helper (M276-D002)"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-D002-ARCH-01", "## M276 Shared Native Build Helper (D002)"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-D002-PKG-01", '"check:objc3c:m276-d002-shared-native-build-helper": "python scripts/check_m276_d002_shared_native_build_helper_for_readiness_runners_and_checkers.py"'),
        SnippetCheck("M276-D002-PKG-02", '"check:objc3c:m276-d002-lane-d-readiness": "python scripts/run_m276_d002_lane_d_readiness.py"'),
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


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(cmd: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, check=False)


def helper_probe(mode: str, force_reconfigure: bool, summary_dir: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    args = [sys.executable, str(HELPER), "--mode", mode, "--reason", f"m276-d002-{mode}", "--summary-out", str(summary_dir / f"helper_{mode}.json")]
    if force_reconfigure:
        args.append("--force-reconfigure")
    completed = run(args)
    log_path = summary_dir / f"helper_{mode}.log"
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    findings: list[Finding] = []
    checks_total = 1
    if completed.returncode != 0:
        findings.append(Finding(f"dynamic/helper-{mode}", f"M276-D002-DYN-{mode.upper()}-RC", f"helper mode {mode} failed: {completed.returncode}"))
        return checks_total, findings, {"log": display_path(log_path)}
    summary = json.loads((summary_dir / f"helper_{mode}.json").read_text(encoding="utf-8"))
    checks_total += 3
    if summary["mode"] != mode:
        findings.append(Finding(f"dynamic/helper-{mode}", f"M276-D002-DYN-{mode.upper()}-MODE", f"helper summary recorded wrong mode: {summary['mode']}"))
    if not summary["ok"]:
        findings.append(Finding(f"dynamic/helper-{mode}", f"M276-D002-DYN-{mode.upper()}-SUMMARY", f"helper summary reported failure for mode {mode}"))
    if force_reconfigure and not summary["saw_cmake_configure_force_reconfigure"]:
        findings.append(Finding(f"dynamic/helper-{mode}", f"M276-D002-DYN-{mode.upper()}-RECONFIGURE", "forced reconfigure did not reach the wrapper"))
    return checks_total, findings, {
        "summary": display_path(summary_dir / f"helper_{mode}.json"),
        "log": display_path(log_path),
        "execution_mode": summary["execution_mode"],
        "saw_cmake_build_start": summary["saw_cmake_build_start"],
        "saw_cmake_build_skip": summary["saw_cmake_build_skip"],
        "saw_cmake_configure_force_reconfigure": summary["saw_cmake_configure_force_reconfigure"],
    }


def dynamic_proof() -> tuple[int, list[Finding], dict[str, Any]]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    checks_total = 0
    findings: list[Finding] = []

    fast_total, fast_findings, fast_summary = helper_probe("fast", False, summary_dir)
    contracts_total, contracts_findings, contracts_summary = helper_probe("contracts", False, summary_dir)
    full_total, full_findings, full_summary = helper_probe("full", True, summary_dir)
    checks_total += fast_total + contracts_total + full_total
    findings.extend(fast_findings)
    findings.extend(contracts_findings)
    findings.extend(full_findings)

    return checks_total, findings, {
        "fast": fast_summary,
        "contracts": contracts_summary,
        "full": full_summary,
    }


def main() -> int:
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)

    dynamic_summary: dict[str, Any] = {}
    if not failures:
        dynamic_total, dynamic_failures, dynamic_summary = dynamic_proof()
        checks_total += dynamic_total
        failures.extend(dynamic_failures)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_summary": dynamic_summary,
        "findings": [finding.__dict__ for finding in failures],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}", file=sys.stderr)
        return 1
    print(f"[ok] M276-D002 shared native build helper validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
