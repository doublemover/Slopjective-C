#!/usr/bin/env python3
"""Checker for M314-C003 public command budget and CLI docs sync."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-C003" / "public_command_budget_cli_docs_sync_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_public_command_budget_enforcement_and_cli_docs_synchronization_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c003_public_command_budget_enforcement_and_cli_docs_synchronization_core_feature_expansion_packet.md"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c003_public_command_budget_enforcement_and_cli_docs_synchronization_core_feature_expansion_registry.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"
RUNBOOK = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
RENDERER = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"


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


def run_json(*args: str) -> dict[str, object]:
    result = subprocess.run(
        [sys.executable, str(RUNNER), *args],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    return json.loads(result.stdout)


def parse_readme_public_scripts(text: str) -> list[str]:
    lines = text.splitlines()
    in_section = False
    scripts: list[str] = []
    for line in lines:
        if line.strip() == "## Public Command Surface":
            in_section = True
            continue
        if in_section and line.startswith("## "):
            break
        if in_section and line.startswith("- `npm run ") and line.endswith("`"):
            command = line[len("- `npm run ") : -1]
            if " -- " in command:
                command = command.split(" -- ", 1)[0]
            scripts.append(command)
    return scripts


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    registry = json.loads(read_text(REGISTRY_JSON))
    package = json.loads(read_text(PACKAGE_JSON))
    surface = package["objc3cCommandSurface"]
    readme = read_text(README)
    runbook = read_text(RUNBOOK)
    live_list = run_json("--list-json")
    render_check = subprocess.run(
        [sys.executable, str(RENDERER), "--check"],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
    )
    live_scripts = [entry["public_scripts"][0] for entry in live_list["actions"]]
    readme_scripts = parse_readme_public_scripts(readme)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-public-command-budget-and-cli-doc-sync/m314-c003-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-C003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("generated from the live runner metadata" in expectations, str(EXPECTATIONS_DOC), "M314-C003-EXP-02", "expectations missing generated-doc rule", failures)
    checks_passed += require("public command budget" in packet.lower() and "live runner" in packet.lower(), str(PACKET_DOC), "M314-C003-PKT-01", "packet missing budget/live-runner note", failures)
    checks_passed += require("Next issue: `M314-D001`." in packet, str(PACKET_DOC), "M314-C003-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(registry.get("mode") == "m314-c003-public-command-budget-and-cli-doc-sync-v1", str(REGISTRY_JSON), "M314-C003-REG-01", "mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-cleanup-public-command-budget-and-cli-doc-sync/m314-c003-v1", str(REGISTRY_JSON), "M314-C003-REG-02", "contract id drifted", failures)
    checks_passed += require(registry.get("depends_on") == "M314-C002", str(REGISTRY_JSON), "M314-C003-REG-03", "dependency drifted", failures)
    checks_passed += require(registry.get("public_script_budget_maximum") == 25, str(REGISTRY_JSON), "M314-C003-REG-04", "budget maximum drifted", failures)
    checks_passed += require(registry.get("public_script_count") == 17, str(REGISTRY_JSON), "M314-C003-REG-05", "public script count drifted", failures)
    checks_passed += require(registry.get("runbook_path") == "docs/runbooks/objc3c_public_command_surface.md", str(REGISTRY_JSON), "M314-C003-REG-06", "runbook path drifted", failures)
    checks_passed += require(registry.get("renderer_path") == "scripts/render_objc3c_public_command_surface.py", str(REGISTRY_JSON), "M314-C003-REG-07", "renderer path drifted", failures)
    checks_passed += require(registry.get("runner_list_command") == "python scripts/objc3c_public_workflow_runner.py --list-json", str(REGISTRY_JSON), "M314-C003-REG-08", "runner list command drifted", failures)
    checks_passed += require(registry.get("next_issue") == "M314-D001", str(REGISTRY_JSON), "M314-C003-REG-09", "next issue drifted", failures)

    checks_total += 10
    checks_passed += require(len(surface["publicScripts"]) <= registry["public_script_budget_maximum"], str(PACKAGE_JSON), "M314-C003-PKG-01", "public scripts exceed budget", failures)
    checks_passed += require(len(surface["publicScripts"]) == registry["public_script_count"], str(PACKAGE_JSON), "M314-C003-PKG-02", "public script count mismatches registry", failures)
    checks_passed += require(surface.get("publicDocumentationPath") == registry["runbook_path"], str(PACKAGE_JSON), "M314-C003-PKG-03", "package runbook path drifted", failures)
    checks_passed += require(surface.get("budgetEnforcementOwnerIssue") == "M314-C003", str(PACKAGE_JSON), "M314-C003-PKG-04", "budget owner drifted", failures)
    checks_passed += require(live_list.get("action_count") == registry["public_script_count"], str(RUNNER), "M314-C003-LIVE-01", "live action count drifted", failures)
    checks_passed += require(live_scripts == surface["publicScripts"], str(RUNNER), "M314-C003-LIVE-02", "live runner scripts drift from package public scripts", failures)
    checks_passed += require(readme_scripts == surface["publicScripts"], str(README), "M314-C003-RD-01", "README public scripts drift from package surface", failures)
    checks_passed += require("See `docs/runbooks/objc3c_public_command_surface.md`" in readme, str(README), "M314-C003-RD-02", "README missing runbook link", failures)
    checks_passed += require(render_check.returncode == 0, str(RENDERER), "M314-C003-DOC-01", f"runbook renderer check failed: {render_check.stderr.strip()}", failures)
    checks_passed += require("Current public script count: `17`" in runbook and "No additional package-script compatibility aliases remain supported." in runbook, str(RUNBOOK), "M314-C003-DOC-02", "runbook missing synchronized command rows", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": registry["mode"],
        "contract_id": registry["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "live_public_scripts": live_scripts,
        "readme_public_scripts": readme_scripts,
        "budget_maximum": registry["public_script_budget_maximum"],
        "next_issue": "M314-D001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-C003 public command budget/docs sync checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
