#!/usr/bin/env python3
"""Checker for M314-E001 workflow simplification gate."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-E001" / "workflow_simplification_gate_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_workflow_simplification_gate_contract_and_architecture_freeze_e001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_e001_workflow_simplification_gate_contract_and_architecture_freeze_packet.md"
GATE_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_e001_workflow_simplification_gate_contract_and_architecture_freeze_gate.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
RENDERER = ROOT / "scripts" / "render_objc3c_public_command_surface.py"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def git_ls_files(relative_root: str) -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", relative_root],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    return [line.strip().replace("\\", "/") for line in result.stdout.splitlines() if line.strip()]


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    gate = json.loads(read_text(GATE_JSON))
    package = json.loads(read_text(PACKAGE_JSON))
    readme = read_text(README)
    predecessor_payloads: list[tuple[str, dict[str, object]]] = []
    for rel_path in gate["predecessor_summaries"]:
        predecessor_payloads.append((rel_path, json.loads(read_text(ROOT / rel_path))))

    runner_list = json.loads(
        subprocess.run(
            [sys.executable, str(RUNNER), "--list-json"],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        ).stdout
    )
    render_check = subprocess.run(
        [sys.executable, str(RENDERER), "--check"],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
    )
    tracked_compiler_files = [
        path for path in git_ls_files(gate["live_rules"]["compiler_git_ls_root"]) if not path.endswith("__pycache__") and not path.endswith(".pyc")
    ]

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-workflow-simplification-gate/m314-e001-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-E001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("predecessor summary chain" in expectations, str(EXPECTATIONS_DOC), "M314-E001-EXP-02", "expectations missing predecessor-chain note", failures)
    checks_passed += require("pre-closeout gate" in packet, str(PACKET_DOC), "M314-E001-PKT-01", "packet missing pre-closeout gate summary", failures)
    checks_passed += require("Next issue: `M314-E002`." in packet, str(PACKET_DOC), "M314-E001-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(gate.get("mode") == "m314-e001-workflow-simplification-gate-v1", str(GATE_JSON), "M314-E001-GATE-01", "mode drifted", failures)
    checks_passed += require(gate.get("contract_id") == "objc3c-cleanup-workflow-simplification-gate/m314-e001-v1", str(GATE_JSON), "M314-E001-GATE-02", "contract id drifted", failures)
    checks_passed += require(gate.get("depends_on") == "M314-D001", str(GATE_JSON), "M314-E001-GATE-03", "dependency drifted", failures)
    checks_passed += require(len(gate.get("predecessor_summaries", [])) == 11, str(GATE_JSON), "M314-E001-GATE-04", "predecessor summary count drifted", failures)
    checks_passed += require(len(gate.get("historical_baseline_summaries", [])) == 2, str(GATE_JSON), "M314-E001-GATE-04A", "historical baseline summary set drifted", failures)
    checks_passed += require(gate.get("live_rules", {}).get("public_script_budget_maximum") == 25, str(GATE_JSON), "M314-E001-GATE-05", "budget maximum drifted", failures)
    checks_passed += require(gate.get("live_rules", {}).get("public_script_count") == 17, str(GATE_JSON), "M314-E001-GATE-06", "public script count drifted", failures)
    checks_passed += require(gate.get("live_rules", {}).get("runbook_check_command") == "python scripts/render_objc3c_public_command_surface.py --check", str(GATE_JSON), "M314-E001-GATE-07", "runbook check command drifted", failures)
    checks_passed += require(gate.get("next_issue") == "M314-E002", str(GATE_JSON), "M314-E001-GATE-08", "next issue drifted", failures)

    checks_total += 8
    historical_baselines = set(gate.get("historical_baseline_summaries", []))
    predecessor_ok = all(
        payload.get("ok") is True or rel_path in historical_baselines
        for rel_path, payload in predecessor_payloads
    )
    checks_passed += require(predecessor_ok, "predecessor summaries", "M314-E001-LIVE-01", "one or more non-baseline predecessor summaries are not ok", failures)
    checks_passed += require(len(package["objc3cCommandSurface"]["publicScripts"]) == gate["live_rules"]["public_script_count"], str(PACKAGE_JSON), "M314-E001-LIVE-02", "package public script count drifted", failures)
    checks_passed += require(len(package["objc3cCommandSurface"]["publicScripts"]) <= gate["live_rules"]["public_script_budget_maximum"], str(PACKAGE_JSON), "M314-E001-LIVE-03", "package public scripts exceed budget", failures)
    checks_passed += require(runner_list.get("action_count") == gate["live_rules"]["public_script_count"], str(RUNNER), "M314-E001-LIVE-04", "runner action count drifted", failures)
    checks_passed += require(render_check.returncode == 0, str(RENDERER), "M314-E001-LIVE-05", f"runbook check failed: {render_check.stderr.strip()}", failures)
    checks_passed += require(tracked_compiler_files == [], "git ls-files compiler", "M314-E001-LIVE-06", "tracked compiler files reappeared under compiler/", failures)
    checks_passed += require("docs/runbooks/objc3c_maintainer_workflows.md" in readme, str(README), "M314-E001-LIVE-07", "README missing maintainer workflow link", failures)
    checks_passed += require(package.get("objc3cOperatorWorkflows", {}).get("contractId") == "objc3c-cleanup-maintainer-operator-workflow-docs/m314-d001-v1", str(PACKAGE_JSON), "M314-E001-LIVE-08", "package operator workflow contract drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": gate["mode"],
        "contract_id": gate["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "predecessor_count": len(predecessor_payloads),
        "historical_baseline_count": len(historical_baselines),
        "public_script_count": gate["live_rules"]["public_script_count"],
        "next_issue": "M314-E002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-E001 workflow simplification gate checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
