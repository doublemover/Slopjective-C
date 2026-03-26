#!/usr/bin/env python3
"""Checker for M313-A003 subsystem acceptance-suite boundary map."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-A003" / "subsystem_acceptance_suite_boundary_map_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_a003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_packet.md"
MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json"
A001_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json"
A002_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
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
    payload = json.loads(read_text(MAP_JSON))
    a001 = json.loads(read_text(A001_JSON))
    a002 = json.loads(read_text(A002_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-subsystem-acceptance-suite-boundary-map/m313-a003-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-A003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Freeze the subsystem acceptance-suite boundaries" in expectations, str(EXPECTATIONS_DOC), "M313-A003-EXP-02", "expectations missing purpose statement", failures)
    checks_passed += require("explicit subsystem suite IDs" in packet, str(PACKET_DOC), "M313-A003-PKT-01", "packet missing suite-id focus", failures)
    checks_passed += require("Next issue: `M313-B001`." in packet, str(PACKET_DOC), "M313-A003-PKT-02", "packet missing next issue", failures)

    checks_total += 8
    checks_passed += require(payload.get("mode") == "m313-a003-subsystem-acceptance-suite-boundary-map-v1", str(MAP_JSON), "M313-A003-CON-01", "mode drifted", failures)
    checks_passed += require(payload.get("contract_id") == "objc3c-cleanup-subsystem-acceptance-suite-boundary-map/m313-a003-v1", str(MAP_JSON), "M313-A003-CON-02", "contract id drifted", failures)
    checks_passed += require(payload.get("baseline_sources") == [
        "spec/planning/compiler/m313/m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json",
        "spec/planning/compiler/m313/m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json",
    ], str(MAP_JSON), "M313-A003-CON-03", "baseline sources drifted", failures)
    checks_passed += require([entry.get("suite_id") for entry in payload.get("acceptance_suite_boundaries", [])] == ["runtime_bootstrap_dispatch", "frontend_split_recovery", "module_parity_packaging", "native_fixture_corpus_and_runtime_probes"], str(MAP_JSON), "M313-A003-CON-04", "suite ids drifted", failures)
    checks_passed += require(payload.get("migration_owner_sequence") == ["M313-B001", "M313-B002", "M313-B003", "M313-B004", "M313-C002", "M313-C003", "M313-D002", "M313-E002"], str(MAP_JSON), "M313-A003-CON-05", "migration owner sequence drifted", failures)
    checks_passed += require(payload.get("active_acceptance_aggregation", {}).get("roots") == ["tests/tooling/runtime", "tests/tooling/fixtures/native", "tmp/reports"], str(MAP_JSON), "M313-A003-CON-06", "active aggregation roots drifted", failures)
    checks_passed += require(payload.get("active_acceptance_aggregation", {}).get("note", "").startswith("Lane-D and lane-E work must aggregate executable subsystem suites"), str(MAP_JSON), "M313-A003-CON-07", "aggregation note drifted", failures)
    checks_passed += require(payload.get("next_issue") == "M313-B001", str(MAP_JSON), "M313-A003-CON-08", "next issue drifted", failures)

    checks_total += 3
    checks_passed += require(payload.get("baseline_sources", [None])[0].endswith(A001_JSON.name), str(MAP_JSON), "M313-A003-BASE-01", "A001 baseline source missing", failures)
    checks_passed += require(payload.get("baseline_sources", [None, None])[1].endswith(A002_JSON.name), str(MAP_JSON), "M313-A003-BASE-02", "A002 baseline source missing", failures)
    checks_passed += require(a001.get("acceptance_suite_roots") == ["tests/tooling/runtime", "tests/tooling/fixtures/native"], str(A001_JSON), "M313-A003-BASE-03", "A001 acceptance roots drifted from assumed boundary map inputs", failures)

    for idx, entry in enumerate(payload.get("acceptance_suite_boundaries", []), start=1):
        for root in entry.get("existing_roots", []):
            checks_total += 1
            checks_passed += require((ROOT / root).exists(), str(MAP_JSON), f"M313-A003-ROOT-{idx:02d}", f"suite root does not exist: {root}", failures)
        checks_total += 1
        checks_passed += require(entry.get("migration_owner_issue") in payload.get("migration_owner_sequence", []), str(MAP_JSON), f"M313-A003-OWNER-{idx:02d}", f"migration owner missing from owner sequence for {entry.get('suite_id')}", failures)
        checks_total += 1
        checks_passed += require(entry.get("ci_aggregation_owner_issue") in payload.get("migration_owner_sequence", []) or entry.get("ci_aggregation_owner_issue") == "M313-E002", str(MAP_JSON), f"M313-A003-CI-{idx:02d}", f"ci aggregation owner invalid for {entry.get('suite_id')}", failures)

    checks_total += 2
    checks_passed += require(a002.get("ratchet_stages", [])[0].get("owner_issue") == "M313-A003", str(A002_JSON), "M313-A003-RATCHET-01", "A002 no-growth stage no longer owned by M313-A003", failures)
    checks_passed += require(a002.get("no_new_growth_without_exception") is True, str(A002_JSON), "M313-A003-RATCHET-02", "A002 no-growth flag drifted", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-a003-subsystem-acceptance-suite-boundary-and-migration-map-source-completion"' in package, str(PACKAGE_JSON), "M313-A003-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-a003-subsystem-acceptance-suite-boundary-and-migration-map-source-completion"' in package, str(PACKAGE_JSON), "M313-A003-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-a003-lane-a-readiness"' in package, str(PACKAGE_JSON), "M313-A003-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": payload["mode"],
        "contract_id": payload["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "suite_ids": [entry["suite_id"] for entry in payload["acceptance_suite_boundaries"]],
        "migration_owner_sequence": payload["migration_owner_sequence"],
        "next_issue": "M313-B001",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-A003 subsystem acceptance boundary checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
