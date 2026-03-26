#!/usr/bin/env python3
"""Checker for M317-C001 planning packet and issue-template simplification contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m317-c001-planning-packet-issue-template-contract-v1"
CONTRACT_ID = "objc3c-cleanup-planning-packet-issue-template-contract/m317-c001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m317" / "M317-C001" / "planning_packet_issue_template_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m317_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_contract.json"
ISSUE_TEMPLATE_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c001_simplified_issue_body_template.md"
PACKET_TEMPLATE_DOC = ROOT / "spec" / "planning" / "compiler" / "m317" / "m317_c001_simplified_planning_packet_template.md"
PACKAGE_JSON = ROOT / "package.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M317-C001-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


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

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M317-C001-EXP-01", "# M317 Planning Packet And Issue Template Simplification Contract And Architecture Freeze Expectations (C001)"),
            SnippetCheck("M317-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M317-C001-EXP-03", "`M317-C002`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M317-C001-PKT-01", "# M317-C001 Packet: Planning packet and issue-template simplification contract - Contract and architecture freeze"),
            SnippetCheck("M317-C001-PKT-02", "Validation-posture taxonomy and defaults"),
            SnippetCheck("M317-C001-PKT-03", "Next issue: `M317-C002`."),
        ],
        ISSUE_TEMPLATE_DOC: [
            SnippetCheck("M317-C001-ISSUE-01", "# Simplified Roadmap Issue Body Template"),
            SnippetCheck("M317-C001-ISSUE-02", "## Validation posture"),
            SnippetCheck("M317-C001-ISSUE-03", "shared_acceptance_harness | static_policy_guard | migration_bridge | generator_contract_only"),
        ],
        PACKET_TEMPLATE_DOC: [
            SnippetCheck("M317-C001-PKT-TPL-01", "# Simplified Planning Packet Template"),
            SnippetCheck("M317-C001-PKT-TPL-02", "## Validation plan"),
            SnippetCheck("M317-C001-PKT-TPL-03", "no readiness runner by default"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M317-C001-PKG-01", '"check:objc3c:m317-c001-planning-packet-and-issue-template-simplification-contract-and-architecture-freeze"'),
            SnippetCheck("M317-C001-PKG-02", '"test:tooling:m317-c001-planning-packet-and-issue-template-simplification-contract-and-architecture-freeze"'),
            SnippetCheck("M317-C001-PKG-03", '"check:objc3c:m317-c001-lane-c-readiness"'),
        ],
    }
    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    contract = load_json(CONTRACT_JSON)
    checks_total += 9
    checks_passed += require(contract.get("mode") == MODE, display_path(CONTRACT_JSON), "M317-C001-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == CONTRACT_ID, display_path(CONTRACT_JSON), "M317-C001-CON-02", "contract id drifted", failures)
    checks_passed += require(contract.get("issue_body_contract", {}).get("required_sections") == ["## Outcome", "## Why this matters", "## Acceptance criteria", "## Primary implementation surfaces", "## Dependencies", "## Validation posture", "<!-- EXECUTION-ORDER-START -->"], display_path(CONTRACT_JSON), "M317-C001-CON-03", "issue-body required sections drifted", failures)
    checks_passed += require("## Required deliverables" not in contract.get("issue_body_contract", {}).get("required_sections", []), display_path(CONTRACT_JSON), "M317-C001-CON-04", "required-deliverables boilerplate returned to required set", failures)
    checks_passed += require(contract.get("planning_packet_contract", {}).get("required_sections") == ["## Intent", "## Scope", "## Contract", "## Validation plan", "## Closeout evidence", "## Next issue"], display_path(CONTRACT_JSON), "M317-C001-CON-05", "planning-packet required sections drifted", failures)
    checks_passed += require({item["code"] for item in contract.get("validation_postures", [])} == {"shared_acceptance_harness", "static_policy_guard", "migration_bridge", "generator_contract_only"}, display_path(CONTRACT_JSON), "M317-C001-CON-06", "validation posture set drifted", failures)
    checks_passed += require("validation_posture" in contract.get("generator_input_fields", []), display_path(CONTRACT_JSON), "M317-C001-CON-07", "generator input fields missing validation_posture", failures)
    checks_passed += require(any("dedicated checker" in item for item in contract.get("prohibited_defaults", [])), display_path(CONTRACT_JSON), "M317-C001-CON-08", "checker-default prohibition missing", failures)
    checks_passed += require(set(contract.get("future_implementation_surfaces", [])) == {".github/ISSUE_TEMPLATE/roadmap_execution.yml", ".github/ISSUE_TEMPLATE/conformance_execution.yml", "tmp/planning/cleanup_acceleration_program/generate_cleanup_acceleration_program.py", "tmp/planning/post_m292_refined_program/post_m292_refined_program_seed.json"}, display_path(CONTRACT_JSON), "M317-C001-CON-09", "future implementation surfaces drifted", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "next_issue": "M317-C002",
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M317-C001 planning packet/issue template contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
