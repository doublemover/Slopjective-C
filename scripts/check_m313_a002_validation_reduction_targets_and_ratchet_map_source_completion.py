#!/usr/bin/env python3
"""Checker for M313-A002 validation reduction targets and ratchet map."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-A002" / "validation_reduction_targets_and_ratchet_map_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_validation_reduction_targets_and_ratchet_map_source_completion_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_packet.md"
RATCHET_MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
BASELINE_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json"
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
    ratchet_map = json.loads(read_text(RATCHET_MAP_JSON))
    baseline = json.loads(read_text(BASELINE_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-validation-reduction-ratchet-map/m313-a002-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-A002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Freeze concrete reduction targets and ratchet stages" in expectations, str(EXPECTATIONS_DOC), "M313-A002-EXP-02", "expectations missing purpose statement", failures)
    checks_passed += require("stage-by-stage numeric caps" in packet, str(PACKET_DOC), "M313-A002-PKT-01", "packet missing staged-cap focus", failures)
    checks_passed += require("Next issue: `M313-A003`." in packet, str(PACKET_DOC), "M313-A002-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(ratchet_map.get("mode") == "m313-a002-validation-reduction-ratchet-map-v1", str(RATCHET_MAP_JSON), "M313-A002-CON-01", "mode drifted", failures)
    checks_passed += require(ratchet_map.get("contract_id") == "objc3c-cleanup-validation-reduction-ratchet-map/m313-a002-v1", str(RATCHET_MAP_JSON), "M313-A002-CON-02", "contract id drifted", failures)
    checks_passed += require(ratchet_map.get("baseline_source") == "spec/planning/compiler/m313/m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json", str(RATCHET_MAP_JSON), "M313-A002-CON-03", "baseline source drifted", failures)
    checks_passed += require(ratchet_map.get("baseline_counts") == {"check_scripts": 2790, "readiness_runners": 599, "pytest_check_files": 2777}, str(RATCHET_MAP_JSON), "M313-A002-CON-04", "baseline counts drifted", failures)
    checks_passed += require([entry.get("owner_issue") for entry in ratchet_map.get("ratchet_stages", [])] == ["M313-A003", "M313-B004", "M313-C003", "M313-E001"], str(RATCHET_MAP_JSON), "M313-A002-CON-05", "ratchet stage order drifted", failures)
    checks_passed += require(ratchet_map.get("closeout_maximums") == {"check_scripts": 558, "readiness_runners": 179, "pytest_check_files": 555}, str(RATCHET_MAP_JSON), "M313-A002-CON-06", "closeout maximums drifted", failures)
    checks_passed += require(ratchet_map.get("no_new_growth_without_exception") is True, str(RATCHET_MAP_JSON), "M313-A002-CON-07", "no-growth flag drifted", failures)
    checks_passed += require(ratchet_map.get("active_acceptance_inputs_not_in_budget") == ["tests/tooling/runtime/*.cpp", "tests/tooling/fixtures/native/**/*.objc3"], str(RATCHET_MAP_JSON), "M313-A002-CON-08", "active acceptance inputs drifted", failures)
    checks_passed += require(ratchet_map.get("next_issue") == "M313-A003", str(RATCHET_MAP_JSON), "M313-A002-CON-09", "next issue drifted", failures)

    baseline_counts = {key: baseline["measured_counts"][key] for key in ["check_scripts", "readiness_runners", "pytest_check_files"]}
    checks_total += 1
    checks_passed += require(ratchet_map.get("baseline_counts") == baseline_counts, str(RATCHET_MAP_JSON), "M313-A002-BASE-01", "ratchet map baselines no longer match M313-A001 inventory", failures)

    stages = ratchet_map.get("ratchet_stages", [])
    for index, entry in enumerate(stages):
        maximums = entry.get("maximums", {})
        checks_total += 1
        checks_passed += require(set(maximums.keys()) == {"check_scripts", "readiness_runners", "pytest_check_files"}, str(RATCHET_MAP_JSON), f"M313-A002-STAGE-{index+1:02d}", f"ratchet stage {entry.get('owner_issue')} has incomplete maximum keys", failures)
        if index > 0:
            previous = stages[index - 1]["maximums"]
            checks_total += 3
            checks_passed += require(maximums["check_scripts"] <= previous["check_scripts"], str(RATCHET_MAP_JSON), f"M313-A002-STAGE-{index+1:02d}-CHK", f"check_scripts cap increased at stage {entry.get('owner_issue')}", failures)
            checks_passed += require(maximums["readiness_runners"] <= previous["readiness_runners"], str(RATCHET_MAP_JSON), f"M313-A002-STAGE-{index+1:02d}-RUN", f"readiness_runners cap increased at stage {entry.get('owner_issue')}", failures)
            checks_passed += require(maximums["pytest_check_files"] <= previous["pytest_check_files"], str(RATCHET_MAP_JSON), f"M313-A002-STAGE-{index+1:02d}-PYT", f"pytest_check_files cap increased at stage {entry.get('owner_issue')}", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-a002-validation-reduction-targets-and-ratchet-map-source-completion"' in package, str(PACKAGE_JSON), "M313-A002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-a002-validation-reduction-targets-and-ratchet-map-source-completion"' in package, str(PACKAGE_JSON), "M313-A002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-a002-lane-a-readiness"' in package, str(PACKAGE_JSON), "M313-A002-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": ratchet_map["mode"],
        "contract_id": ratchet_map["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "baseline_counts": ratchet_map["baseline_counts"],
        "closeout_maximums": ratchet_map["closeout_maximums"],
        "ratchet_stage_owners": [entry["owner_issue"] for entry in stages],
        "next_issue": "M313-A003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-A002 validation reduction targets checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
