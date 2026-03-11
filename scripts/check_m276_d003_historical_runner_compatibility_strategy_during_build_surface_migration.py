#!/usr/bin/env python3
"""Fail-closed checker for M276-D003 historical runner compatibility."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-d003-historical-runner-compatibility-v1"
CONTRACT_ID = "objc3c-historical-runner-build-surface-compatibility/m276-d003-v1"
NEXT_ISSUE = "M276-E001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-D003" / "historical_runner_build_surface_compatibility_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_historical_runner_compatibility_strategy_during_build_surface_migration_d003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_d003_historical_runner_compatibility_strategy_during_build_surface_migration_packet.md"
README = ROOT / "README.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
PACKAGE_JSON = ROOT / "package.json"
ACTIVE_RUNNER = ROOT / "scripts" / "run_m262_a001_lane_a_readiness.py"
HISTORICAL_RUNNER = ROOT / "scripts" / "run_m257_a001_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_d003_historical_runner_compatibility_strategy_during_build_surface_migration.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_d003_lane_d_readiness.py"


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
        SnippetCheck("M276-D003-EXP-01", "Contract ID: `objc3c-historical-runner-build-surface-compatibility/m276-d003-v1`"),
        SnippetCheck("M276-D003-EXP-02", "Older readiness runners that still call `npm run build:objc3c-native` directly must remain truthful and deterministic during the migration."),
        SnippetCheck("M276-D003-EXP-03", "One representative historical runner must still use the raw default build command."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-D003-PKT-01", "Issue: `#7395`"),
        SnippetCheck("M276-D003-PKT-02", "The representative historical raw-build path is `scripts/run_m257_a001_lane_a_readiness.py`."),
        SnippetCheck("M276-D003-PKT-03", "Hands off closeout and operator-facing adoption work to `M276-E001`."),
    ),
    README: (
        SnippetCheck("M276-D003-README-01", "historical raw-build callers remain supported through the public default build"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-D003-DOC-01", "## Historical raw-build compatibility during migration (M276-D003)"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-D003-NDOC-01", "## Historical raw-build compatibility during migration (M276-D003)"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-D003-ARCH-01", "## M276 Historical Raw-Build Compatibility During Migration (D003)"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-D003-BLD-01", '[ValidateSet("full", "binaries-only", "packets-source", "packets-binary", "packets-closeout", "packets-all")]'),
        SnippetCheck("M276-D003-BLD-02", 'Write-BuildStep "artifact_generation_mode=none"'),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-D003-PKG-01", '"build:objc3c-native": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1 -ExecutionMode binaries-only"'),
        SnippetCheck("M276-D003-PKG-02", '"check:objc3c:m276-d003-historical-runner-compatibility": "python scripts/check_m276_d003_historical_runner_compatibility_strategy_during_build_surface_migration.py"'),
        SnippetCheck("M276-D003-PKG-03", '"check:objc3c:m276-d003-lane-d-readiness": "python scripts/run_m276_d003_lane_d_readiness.py"'),
    ),
    ACTIVE_RUNNER: (
        SnippetCheck("M276-D003-ACT-01", "ensure_objc3c_native_build.py"),
        SnippetCheck("M276-D003-ACT-02", '"--mode",'),
        SnippetCheck("M276-D003-ACT-03", '"fast"'),
    ),
    HISTORICAL_RUNNER: (
        SnippetCheck("M276-D003-HST-01", "raw package-default compatibility build path"),
        SnippetCheck("M276-D003-HST-02", "build:objc3c-native"),
    ),
    TEST_FILE: (
        SnippetCheck("M276-D003-TST-01", 'CONTRACT_ID = "objc3c-historical-runner-build-surface-compatibility/m276-d003-v1"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-D003-RDY-01", "check_m276_d003_historical_runner_compatibility_strategy_during_build_surface_migration.py"),
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


def runner_probe(path: Path, label: str) -> tuple[int, list[Finding], dict[str, Any]]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    checks_total = 1
    findings: list[Finding] = []
    completed = run([sys.executable, str(path)])
    log_path = summary_dir / f"{path.stem}.log"
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    if completed.returncode != 0:
        findings.append(Finding(display_path(path), f"{label}-RUNNER-RC", f"runner failed with exit code {completed.returncode}"))
        return checks_total, findings, {"log": display_path(log_path)}
    probe_summary: dict[str, Any] = {"log": display_path(log_path)}
    if label == "ACTIVE":
        checks_total += 2
        helper_summary = ROOT / "tmp" / "reports" / "m262" / "M262-A001" / "ensure_objc3c_native_build_summary.json"
        if not helper_summary.exists():
            findings.append(Finding(display_path(path), "ACTIVE-HELPER-SUMMARY", f"missing helper summary: {display_path(helper_summary)}"))
        else:
            payload = json.loads(helper_summary.read_text(encoding="utf-8"))
            if payload.get("mode") != "fast":
                findings.append(Finding(display_path(path), "ACTIVE-HELPER-MODE", f"wrong helper mode: {payload.get('mode')}"))
            probe_summary["helper_summary"] = display_path(helper_summary)
            probe_summary["mode"] = payload.get("mode")
            probe_summary["execution_mode"] = payload.get("execution_mode")
    else:
        checks_total += 1
        if "build:objc3c-native" not in log_path.read_text(encoding="utf-8"):
            findings.append(Finding(display_path(path), "HISTORICAL-RAW-BUILD-LOG", "historical runner log did not show raw build:objc3c-native invocation"))
    return checks_total, findings, probe_summary


def dynamic_proof() -> tuple[int, list[Finding], dict[str, Any]]:
    checks_total = 0
    findings: list[Finding] = []
    active_total, active_findings, active_summary = runner_probe(ACTIVE_RUNNER, "ACTIVE")
    historical_total, historical_findings, historical_summary = runner_probe(HISTORICAL_RUNNER, "HISTORICAL")
    checks_total += active_total + historical_total
    findings.extend(active_findings)
    findings.extend(historical_findings)
    return checks_total, findings, {
        "active_runner": active_summary,
        "historical_runner": historical_summary,
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
    print(f"[ok] M276-D003 historical runner compatibility validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
