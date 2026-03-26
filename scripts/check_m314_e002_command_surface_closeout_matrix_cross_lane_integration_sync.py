#!/usr/bin/env python3
"""Checker for M314-E002 command-surface closeout matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-E002" / "command_surface_closeout_matrix_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_command_surface_closeout_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_e002_command_surface_closeout_matrix_cross_lane_integration_sync_packet.md"
MATRIX_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_e002_command_surface_closeout_matrix_cross_lane_integration_sync_matrix.json"
PACKAGE_JSON = ROOT / "package.json"
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


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    matrix = json.loads(read_text(MATRIX_JSON))
    package = json.loads(read_text(PACKAGE_JSON))
    predecessor_payloads: list[tuple[str, dict[str, object]]] = []
    for rel_path in matrix["predecessor_summaries"]:
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
    issue_probe = subprocess.run(
        [
            "gh",
            "issue",
            "list",
            "--milestone",
            str(matrix["milestone_number"]),
            "--state",
            "open",
            "--json",
            "number",
        ],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    open_issue_numbers = [entry["number"] for entry in json.loads(issue_probe.stdout)]
    historical_baselines = set(matrix.get("historical_baseline_summaries", []))
    predecessor_ok = all(
        payload.get("ok") is True or rel_path in historical_baselines
        for rel_path, payload in predecessor_payloads
    )

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-command-surface-closeout-matrix/m314-e002-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-E002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("pre-closeout time" in expectations, str(EXPECTATIONS_DOC), "M314-E002-EXP-02", "expectations missing pre-closeout milestone note", failures)
    checks_passed += require("final closeout matrix" in packet, str(PACKET_DOC), "M314-E002-PKT-01", "packet missing closeout summary", failures)
    checks_passed += require("This issue closes the milestone." in packet, str(PACKET_DOC), "M314-E002-PKT-02", "packet missing milestone closeout note", failures)

    checks_total += 8
    checks_passed += require(matrix.get("mode") == "m314-e002-command-surface-closeout-matrix-v1", str(MATRIX_JSON), "M314-E002-MAT-01", "mode drifted", failures)
    checks_passed += require(matrix.get("contract_id") == "objc3c-cleanup-command-surface-closeout-matrix/m314-e002-v1", str(MATRIX_JSON), "M314-E002-MAT-02", "contract id drifted", failures)
    checks_passed += require(matrix.get("depends_on") == "M314-E001", str(MATRIX_JSON), "M314-E002-MAT-03", "dependency drifted", failures)
    checks_passed += require(matrix.get("milestone_number") == 395, str(MATRIX_JSON), "M314-E002-MAT-04", "milestone number drifted", failures)
    checks_passed += require(len(matrix.get("predecessor_summaries", [])) == 12, str(MATRIX_JSON), "M314-E002-MAT-05", "predecessor summary count drifted", failures)
    checks_passed += require(len(matrix.get("historical_baseline_summaries", [])) == 2, str(MATRIX_JSON), "M314-E002-MAT-06", "historical baseline summary count drifted", failures)
    checks_passed += require(matrix.get("live_rules", {}).get("public_script_count") == 17, str(MATRIX_JSON), "M314-E002-MAT-07", "public script count drifted", failures)
    checks_passed += require(matrix.get("live_rules", {}).get("milestone_open_issue_numbers_preclose") == [7792], str(MATRIX_JSON), "M314-E002-MAT-08", "preclose open issue set drifted", failures)

    checks_total += 7
    checks_passed += require(predecessor_ok, "predecessor summaries", "M314-E002-LIVE-01", "one or more non-baseline predecessor summaries are not ok", failures)
    checks_passed += require(len(package["objc3cCommandSurface"]["publicScripts"]) == matrix["live_rules"]["public_script_count"], str(PACKAGE_JSON), "M314-E002-LIVE-02", "package public script count drifted", failures)
    checks_passed += require(runner_list.get("action_count") == matrix["live_rules"]["public_script_count"], str(RUNNER), "M314-E002-LIVE-03", "runner action count drifted", failures)
    checks_passed += require(render_check.returncode == 0, str(RENDERER), "M314-E002-LIVE-04", f"runbook check failed: {render_check.stderr.strip()}", failures)
    checks_passed += require(open_issue_numbers == matrix["live_rules"]["milestone_open_issue_numbers_preclose"], "gh issue list", "M314-E002-LIVE-05", f"unexpected open milestone issues: {open_issue_numbers}", failures)
    checks_passed += require(package.get("objc3cOperatorWorkflows", {}).get("maintainerRunbook") == "docs/runbooks/objc3c_maintainer_workflows.md", str(PACKAGE_JSON), "M314-E002-LIVE-06", "maintainer runbook metadata drifted", failures)
    checks_passed += require(package["objc3cCommandSurface"].get("publicDocumentationPath") == "docs/runbooks/objc3c_public_command_surface.md", str(PACKAGE_JSON), "M314-E002-LIVE-07", "public documentation path drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": matrix["mode"],
        "contract_id": matrix["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "predecessor_count": len(predecessor_payloads),
        "open_issue_numbers_preclose": open_issue_numbers,
        "public_script_count": matrix["live_rules"]["public_script_count"],
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-E002 command-surface closeout matrix checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
