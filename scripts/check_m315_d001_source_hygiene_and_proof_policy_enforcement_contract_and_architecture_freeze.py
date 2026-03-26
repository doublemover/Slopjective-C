#!/usr/bin/env python3
"""Checker for M315-D001 source-hygiene and proof-policy enforcement contract."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence


ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-D001" / "source_hygiene_proof_policy_enforcement_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_source_hygiene_and_proof_policy_enforcement_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_d001_source_hygiene_and_proof_policy_enforcement_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_d001_source_hygiene_and_proof_policy_enforcement_contract_and_architecture_freeze_contract.json"
B005_RESULT = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b005_comment_constexpr_and_contract_string_decontamination_sweep_edge_case_and_compatibility_completion_result.json"


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


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise ValueError(f"expected JSON object at {path}")
    return payload


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def tracked_compiler_python_sources() -> list[str]:
    output = subprocess.check_output(["git", "ls-files", "compiler/objc3c/*.py"], cwd=ROOT, text=True)
    return [line.replace("\\", "/") for line in output.splitlines() if line.strip()]


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = read_json(CONTRACT_JSON)
    b005 = read_json(B005_RESULT)
    tracked_compiler_sources = tracked_compiler_python_sources()

    checks_total += 6
    checks_passed += require("objc3c.cleanup.source-hygiene-and-proof-policy.enforcement/m315-d001-v1" in expectations, str(EXPECTATIONS_DOC), "M315-D001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require(
        "transitional_source_model" in expectations
        and "legacy_m248_surface_identifier" in expectations,
        str(EXPECTATIONS_DOC),
        "M315-D001-EXP-02",
        "expectations missing zero-target scope",
        failures,
    )
    checks_passed += require("zero residuals for:" in packet and "transitional_source_model" in packet and "legacy_m248_surface_identifier" in packet, str(PACKET_DOC), "M315-D001-PKT-01", "packet missing D002 zero-target classes", failures)
    checks_passed += require("Next issue: `M315-D002`." in packet, str(PACKET_DOC), "M315-D001-PKT-02", "packet missing next issue", failures)
    checks_passed += require(contract.get("future_enforcement_owner") == "M315-D002", str(CONTRACT_JSON), "M315-D001-CON-01", "future enforcement owner drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M315-D002", str(CONTRACT_JSON), "M315-D001-CON-02", "next issue drifted", failures)

    d002_zero_target_classes = contract.get("d002_zero_target_classes", {})
    quarantined_residual_classes = contract.get("quarantined_residual_classes", {})
    b005_residual_classes = b005.get("allowed_residual_classes", {})

    checks_total += 9
    checks_passed += require(d002_zero_target_classes.get("transitional_source_model", {}).get("count") == 12, str(CONTRACT_JSON), "M315-D001-CON-03", "transitional source-model target count drifted", failures)
    checks_passed += require(d002_zero_target_classes.get("legacy_m248_surface_identifier", {}).get("count") == 34, str(CONTRACT_JSON), "M315-D001-CON-04", "m248 target count drifted", failures)
    checks_passed += require(b005_residual_classes.get("transitional_source_model", 0) == 0, str(B005_RESULT), "M315-D001-CON-03A", "current B005 result still carries transitional source-model residue", failures)
    checks_passed += require(b005_residual_classes.get("legacy_m248_surface_identifier", 0) == 0, str(B005_RESULT), "M315-D001-CON-04A", "current B005 result still carries legacy m248 residue", failures)
    checks_passed += require(quarantined_residual_classes.get("legacy_fixture_path_reference", {}).get("count") == b005_residual_classes.get("legacy_fixture_path_reference", {}).get("count") == 6, str(CONTRACT_JSON), "M315-D001-CON-05", "legacy fixture quarantine count drifted", failures)
    checks_passed += require(quarantined_residual_classes.get("dependency_issue_array", {}).get("count") == b005_residual_classes.get("dependency_issue_array", {}).get("count") == 3, str(CONTRACT_JSON), "M315-D001-CON-06", "dependency issue-array quarantine count drifted", failures)
    checks_passed += require(quarantined_residual_classes.get("next_issue_schema_field", {}).get("count") == b005_residual_classes.get("next_issue_schema_field", {}).get("count") == 40, str(CONTRACT_JSON), "M315-D001-CON-07", "next_issue quarantine count drifted", failures)
    checks_passed += require(quarantined_residual_classes.get("issue_key_schema_field", {}).get("count") == b005_residual_classes.get("issue_key_schema_field", {}).get("count") == 8, str(CONTRACT_JSON), "M315-D001-CON-08", "issue_key quarantine count drifted", failures)
    checks_passed += require(contract.get("tracked_compiler_python_sources") == len(tracked_compiler_sources) == 0, str(CONTRACT_JSON), "M315-D001-CON-09", "tracked compiler python source count drifted", failures)

    synthetic_contract = contract.get("synthetic_fixture_contract", {})
    checks_total += 4
    checks_passed += require(synthetic_contract.get("fixture_root") == "tests/tooling/fixtures/native/library_cli_parity", str(CONTRACT_JSON), "M315-D001-SYN-01", "synthetic fixture root drifted", failures)
    checks_passed += require(len(synthetic_contract.get("required_ll_paths", [])) == 2, str(CONTRACT_JSON), "M315-D001-SYN-02", "synthetic ll path count drifted", failures)
    checks_passed += require(len(synthetic_contract.get("required_json_paths", [])) == 3, str(CONTRACT_JSON), "M315-D001-SYN-03", "synthetic json path count drifted", failures)
    checks_passed += require(synthetic_contract.get("required_fixture_family_id") == "objc3c.fixture.synthetic.librarycliparity.v1", str(CONTRACT_JSON), "M315-D001-SYN-04", "synthetic fixture family drifted", failures)

    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "d002_zero_target_classes": {
            key: value["count"] for key, value in d002_zero_target_classes.items()
        },
        "current_b005_zero_target_residuals": {
            key: b005_residual_classes.get(key, 0) for key in d002_zero_target_classes
        },
        "quarantined_residual_classes": {
            key: value["count"] for key, value in quarantined_residual_classes.items()
        },
        "tracked_compiler_python_sources": tracked_compiler_sources,
        "synthetic_fixture_root": synthetic_contract.get("fixture_root"),
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-D001 source-hygiene/proof-policy contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
