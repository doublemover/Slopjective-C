#!/usr/bin/env python3
"""Fail-closed checker for M276-D001 readiness-runner migration."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-d001-readiness-runner-fast-vs-full-migration-v1"
CONTRACT_ID = "objc3c-readiness-runner-fast-vs-full-migration/m276-d001-v1"
NEXT_ISSUE = "M276-D003"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-D001" / "readiness_runner_fast_vs_full_migration_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy_packet.md"
README = ROOT / "README.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_d001_lane_d_readiness.py"

ACTIVE_RUNNERS: tuple[Path, ...] = (
    ROOT / "scripts" / "run_m262_a001_lane_a_readiness.py",
    ROOT / "scripts" / "run_m262_a002_lane_a_readiness.py",
    ROOT / "scripts" / "run_m262_b001_lane_b_readiness.py",
    ROOT / "scripts" / "run_m262_b002_lane_b_readiness.py",
    ROOT / "scripts" / "run_m263_a001_lane_a_readiness.py",
    ROOT / "scripts" / "run_m263_a002_lane_a_readiness.py",
    ROOT / "scripts" / "run_m263_b001_lane_b_readiness.py",
    ROOT / "scripts" / "run_m263_b002_lane_b_readiness.py",
    ROOT / "scripts" / "run_m263_b003_lane_b_readiness.py",
    ROOT / "scripts" / "run_m263_c001_lane_c_readiness.py",
    ROOT / "scripts" / "run_m263_c002_lane_c_readiness.py",
    ROOT / "scripts" / "run_m263_c003_lane_c_readiness.py",
    ROOT / "scripts" / "run_m263_d001_lane_d_readiness.py",
    ROOT / "scripts" / "run_m263_d002_lane_d_readiness.py",
    ROOT / "scripts" / "run_m263_d003_lane_d_readiness.py",
)

PROBE_RUNNERS: tuple[Path, ...] = (
    ROOT / "scripts" / "run_m262_a001_lane_a_readiness.py",
    ROOT / "scripts" / "run_m263_a001_lane_a_readiness.py",
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
        SnippetCheck("M276-D001-EXP-01", "Contract ID: `objc3c-readiness-runner-fast-vs-full-migration/m276-d001-v1`"),
        SnippetCheck("M276-D001-EXP-02", "The active `M262` and `M263` lane `A` through `D` readiness runners must acquire native binaries through `scripts/ensure_objc3c_native_build.py`."),
        SnippetCheck("M276-D001-EXP-03", "At least one migrated `M262` readiness chain and one migrated `M263` readiness chain must complete successfully after the migration."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-D001-PKT-01", "Issue: `#7389`"),
        SnippetCheck("M276-D001-PKT-02", "Active `M262` and `M263` lane `A` through `D` readiness runners now acquire binaries through `scripts/ensure_objc3c_native_build.py --mode fast`."),
        SnippetCheck("M276-D001-PKT-03", "Leaves historical runner-compatibility expansion to `M276-D003`."),
    ),
    README: (
        SnippetCheck("M276-D001-README-01", "active `M262`/`M263` issue-work range onto the"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-D001-DOC-01", "## Active readiness-runner fast-path migration (M276-D001)"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-D001-NDOC-01", "## Active readiness-runner fast-path migration (M276-D001)"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-D001-ARCH-01", "## M276 Active Readiness-Runner Fast-Path Migration (D001)"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-D001-PKG-01", '"check:objc3c:m276-d001-readiness-runner-migration": "python scripts/check_m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy.py"'),
        SnippetCheck("M276-D001-PKG-02", '"check:objc3c:m276-d001-lane-d-readiness": "python scripts/run_m276_d001_lane_d_readiness.py"'),
    ),
    TEST_FILE: (
        SnippetCheck("M276-D001-TST-01", "CONTRACT_ID = \"objc3c-readiness-runner-fast-vs-full-migration/m276-d001-v1\""),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-D001-RDY-01", "check_m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy.py"),
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


def check_active_runner(path: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    findings: list[Finding] = []
    checks_total = 6
    if not path.exists():
        findings.append(Finding(display_path(path), "RUNNER-EXISTS", "missing active readiness runner"))
        return checks_total, findings, {}
    text = path.read_text(encoding="utf-8")
    if "ensure_objc3c_native_build.py" not in text:
        findings.append(Finding(display_path(path), "RUNNER-HELPER", "runner does not use shared build helper"))
    if '"--mode"' not in text or '"fast"' not in text:
        findings.append(Finding(display_path(path), "RUNNER-MODE", "runner does not request helper fast mode"))
    if "build:objc3c-native" in text:
        findings.append(Finding(display_path(path), "RUNNER-RAW-BUILD", "runner still hard-codes raw build:objc3c-native"))
    if "tmp/reports/" not in text or "ensure_objc3c_native_build_summary.json" not in text:
        findings.append(Finding(display_path(path), "RUNNER-SUMMARY", "runner does not emit deterministic helper summary path"))
    if "cwd=ROOT" not in text:
        findings.append(Finding(display_path(path), "RUNNER-CWD", "runner does not execute commands relative to repo root"))
    return checks_total, findings, {"runner": display_path(path)}


def run(cmd: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, check=False)


def runner_probe(path: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    summary_dir = SUMMARY_OUT.parent
    summary_dir.mkdir(parents=True, exist_ok=True)
    checks_total = 3
    findings: list[Finding] = []
    completed = run([sys.executable, str(path)])
    log_path = summary_dir / f"{path.stem}.log"
    log_path.write_text(completed.stdout + completed.stderr, encoding="utf-8")
    if completed.returncode != 0:
        findings.append(Finding(display_path(path), "RUNNER-RC", f"runner failed with exit code {completed.returncode}"))
        return checks_total, findings, {"log": display_path(log_path)}
    issue_bits = path.stem.split("_")[1:3]
    milestone = issue_bits[0]
    issue = issue_bits[1].upper()
    helper_summary = ROOT / "tmp" / "reports" / milestone / f"{milestone.upper()}-{issue}" / "ensure_objc3c_native_build_summary.json"
    if not helper_summary.exists():
        findings.append(Finding(display_path(path), "RUNNER-SUMMARY-EXISTS", f"missing helper summary: {display_path(helper_summary)}"))
        return checks_total, findings, {"log": display_path(log_path)}
    payload = json.loads(helper_summary.read_text(encoding="utf-8"))
    if payload.get("mode") != "fast":
        findings.append(Finding(display_path(path), "RUNNER-SUMMARY-MODE", f"helper summary recorded wrong mode: {payload.get('mode')}"))
    return checks_total, findings, {
        "log": display_path(log_path),
        "helper_summary": display_path(helper_summary),
        "mode": payload.get("mode"),
        "execution_mode": payload.get("execution_mode"),
    }


def dynamic_proof() -> tuple[int, list[Finding], dict[str, Any]]:
    checks_total = 0
    findings: list[Finding] = []
    probes: dict[str, Any] = {}
    for runner in PROBE_RUNNERS:
        probe_total, probe_findings, probe_summary = runner_probe(runner)
        checks_total += probe_total
        findings.extend(probe_findings)
        probes[runner.stem] = probe_summary
    return checks_total, findings, probes


def main() -> int:
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)

    active_runner_summary: list[dict[str, Any]] = []
    for path in ACTIVE_RUNNERS:
        total, local_failures, runner_summary = check_active_runner(path)
        checks_total += total
        failures.extend(local_failures)
        active_runner_summary.append(runner_summary)

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
        "active_runner_count": len(ACTIVE_RUNNERS),
        "active_runner_summary": active_runner_summary,
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
    print(f"[ok] M276-D001 readiness-runner migration validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(SUMMARY_OUT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
