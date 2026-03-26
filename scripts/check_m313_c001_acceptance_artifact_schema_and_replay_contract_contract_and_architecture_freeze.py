#!/usr/bin/env python3
"""Checker for M313-C001 acceptance artifact schema and replay contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-C001" / "acceptance_artifact_schema_replay_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_packet.md"
SCHEMA_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json"
PACKAGE_JSON = ROOT / "package.json"


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
    schema = json.loads(read_text(SCHEMA_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-acceptance-artifact-schema-replay-contract/m313-c001-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-C001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Require deterministic replay metadata" in expectations, str(EXPECTATIONS_DOC), "M313-C001-EXP-02", "expectations missing replay requirement", failures)
    checks_passed += require("canonical acceptance artifact envelope" in packet, str(PACKET_DOC), "M313-C001-PKT-01", "packet missing envelope focus", failures)
    checks_passed += require("Next issue: `M313-C002`." in packet, str(PACKET_DOC), "M313-C001-PKT-02", "packet missing next issue", failures)

    checks_total += 10
    checks_passed += require(schema.get("mode") == "m313-c001-acceptance-artifact-schema-replay-contract-v1", str(SCHEMA_JSON), "M313-C001-CON-01", "mode drifted", failures)
    checks_passed += require(schema.get("contract_id") == "objc3c-cleanup-acceptance-artifact-schema-replay-contract/m313-c001-v1", str(SCHEMA_JSON), "M313-C001-CON-02", "contract id drifted", failures)
    checks_passed += require(schema.get("artifact_layout") == {
        "suite_report_root": "tmp/reports/m313/acceptance/<suite_id>",
        "compatibility_bridge_root": "tmp/reports/m313/compatibility/<bridge_id>",
        "issue_local_root": "tmp/reports/m313/<issue-code>",
        "canonical_summary_filename": "summary.json",
    }, str(SCHEMA_JSON), "M313-C001-CON-03", "artifact layout drifted", failures)
    checks_passed += require(schema.get("required_envelope_fields") == [
        "schema_version",
        "contract_id",
        "suite_id",
        "artifact_class",
        "producer",
        "ok",
        "inputs",
        "outputs",
        "replay",
        "measurements",
    ], str(SCHEMA_JSON), "M313-C001-CON-04", "required envelope fields drifted", failures)
    checks_passed += require(sorted(schema.get("artifact_classes", {}).keys()) == ["compatibility_bridge_summary", "suite_execution_summary", "suite_root_check"], str(SCHEMA_JSON), "M313-C001-CON-05", "artifact classes drifted", failures)
    checks_passed += require(schema.get("artifact_classes", {}).get("suite_root_check", {}).get("required_measurement_fields") == ["suite_results"], str(SCHEMA_JSON), "M313-C001-CON-06", "suite_root_check requirements drifted", failures)
    checks_passed += require(schema.get("artifact_classes", {}).get("suite_execution_summary", {}).get("required_measurement_fields") == ["case_counts", "fixture_roots", "probe_roots"], str(SCHEMA_JSON), "M313-C001-CON-07", "suite_execution_summary requirements drifted", failures)
    checks_passed += require(schema.get("artifact_classes", {}).get("compatibility_bridge_summary", {}).get("required_measurement_fields") == ["legacy_wrappers_observed", "legacy_wrappers_remaining", "deprecation_owner_issue"], str(SCHEMA_JSON), "M313-C001-CON-08", "compatibility bridge requirements drifted", failures)
    checks_passed += require(schema.get("producer_required_fields") == ["tool", "issue", "validation_posture"] and schema.get("inputs_required_fields") == ["roots"] and schema.get("outputs_required_fields") == ["summary_path"] and schema.get("replay_required_fields") == ["commands", "cwd"], str(SCHEMA_JSON), "M313-C001-CON-09", "required field groups drifted", failures)
    checks_passed += require(schema.get("schema_version") == "m313-c001-v1" and schema.get("next_issue") == "M313-C002", str(SCHEMA_JSON), "M313-C001-CON-10", "schema version or next issue drifted", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-c001-acceptance-artifact-schema-and-replay-contract-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-C001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-c001-acceptance-artifact-schema-and-replay-contract-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-C001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-c001-lane-c-readiness"' in package, str(PACKAGE_JSON), "M313-C001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": schema["mode"],
        "contract_id": schema["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "artifact_classes": sorted(schema["artifact_classes"].keys()),
        "required_envelope_fields": schema["required_envelope_fields"],
        "next_issue": "M313-C002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-C001 acceptance artifact schema checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
