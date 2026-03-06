#!/usr/bin/env python3
"""Fail-closed validator for M228-C013 IR emission docs/operator runbook synchronization contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-c013-ir-emission-completeness-docs-operator-runbook-sync-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_docs_operator_runbook_sync_c013_expectations.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m228_wave_execution_runbook.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c013_ir_emission_completeness_docs_operator_runbook_sync_packet.md",
    "c012_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ir_emission_completeness_cross_lane_integration_sync_c012_expectations.md",
    "c012_checker": ROOT
    / "scripts"
    / "check_m228_c012_ir_emission_completeness_cross_lane_integration_sync_contract.py",
    "c012_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_c012_ir_emission_completeness_cross_lane_integration_sync_contract.py",
    "c012_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_c012_ir_emission_completeness_cross_lane_integration_sync_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M228-C013-DOC-01",
            "Contract ID: `objc3c-ir-emission-completeness-docs-operator-runbook-sync/m228-c013-v1`",
        ),
        ("M228-C013-DOC-02", "Execute issue `#5229`"),
        ("M228-C013-DOC-03", "Dependencies: `M228-C012`"),
        (
            "M228-C013-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M228-C013-DOC-05", "`docs/runbooks/m228_wave_execution_runbook.md`"),
        (
            "M228-C013-DOC-06",
            "scripts/check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-C013-DOC-07",
            "tests/tooling/test_check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py",
        ),
        ("M228-C013-DOC-08", "`npm run check:objc3c:m228-c013-lane-c-readiness`"),
        (
            "M228-C013-DOC-09",
            "tmp/reports/m228/M228-C013/ir_emission_completeness_docs_operator_runbook_sync_contract_summary.json",
        ),
    ),
    "runbook_doc": (
        (
            "M228-C013-RUN-01",
            "objc3c-ir-emission-completeness-cross-lane-integration-sync/m228-c012-v1",
        ),
        (
            "M228-C013-RUN-02",
            "objc3c-ir-emission-completeness-docs-operator-runbook-sync/m228-c013-v1",
        ),
        (
            "M228-C013-RUN-03",
            "python scripts/check_m228_c012_ir_emission_completeness_cross_lane_integration_sync_contract.py",
        ),
        (
            "M228-C013-RUN-04",
            "python scripts/check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-C013-RUN-05",
            "python -m pytest tests/tooling/test_check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py -q",
        ),
        ("M228-C013-RUN-06", "npm run check:objc3c:m228-c013-lane-c-readiness"),
        ("M228-C013-RUN-07", "tmp/reports/m228/"),
    ),
    "planning_packet": (
        ("M228-C013-PKT-01", "Packet: `M228-C013`"),
        ("M228-C013-PKT-02", "Issue: `#5229`"),
        ("M228-C013-PKT-03", "Dependencies: `M228-C012`"),
        ("M228-C013-PKT-04", "docs/runbooks/m228_wave_execution_runbook.md"),
        (
            "M228-C013-PKT-05",
            "scripts/check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-C013-PKT-06",
            "tests/tooling/test_check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py",
        ),
        ("M228-C013-PKT-07", "check:objc3c:m228-c013-lane-c-readiness"),
        (
            "M228-C013-PKT-08",
            "tmp/reports/m228/M228-C013/ir_emission_completeness_docs_operator_runbook_sync_contract_summary.json",
        ),
    ),
    "c012_contract_doc": (
        (
            "M228-C013-DEP-01",
            "Contract ID: `objc3c-ir-emission-completeness-cross-lane-integration-sync/m228-c012-v1`",
        ),
    ),
    "c012_checker": (
        (
            "M228-C013-DEP-02",
            'MODE = "m228-c012-ir-emission-completeness-cross-lane-integration-sync-contract-v1"',
        ),
    ),
    "c012_tooling_test": (
        ("M228-C013-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "c012_packet_doc": (
        ("M228-C013-DEP-04", "Packet: `M228-C012`"),
        ("M228-C013-DEP-05", "Issue: `#5228`"),
    ),
    "package_json": (
        (
            "M228-C013-PKG-01",
            '"check:objc3c:m228-c012-ir-emission-completeness-cross-lane-integration-sync-contract"',
        ),
        (
            "M228-C013-PKG-02",
            '"check:objc3c:m228-c013-ir-emission-completeness-docs-operator-runbook-sync-contract"',
        ),
        (
            "M228-C013-PKG-03",
            '"test:tooling:m228-c013-ir-emission-completeness-docs-operator-runbook-sync-contract"',
        ),
        ("M228-C013-PKG-04", '"check:objc3c:m228-c013-lane-c-readiness"'),
        (
            "M228-C013-PKG-05",
            '"check:objc3c:m228-c013-lane-c-readiness": "npm run check:objc3c:m228-c012-lane-c-readiness &&',
        ),
        (
            "M228-C013-PKG-06",
            "scripts/check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py",
        ),
        (
            "M228-C013-PKG-07",
            "tests/tooling/test_check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py -q",
        ),
        ("M228-C013-PKG-08", '"test:objc3c:lowering-replay-proof"'),
    ),
    "architecture_doc": (
        (
            "M228-C013-ARC-01",
            "M228 lane-C C013 IR-emission docs and operator runbook synchronization anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M228-C013-SPC-01",
            "IR-emission docs and operator runbook synchronization governance",
        ),
        (
            "M228-C013-SPC-02",
            "lane-C dependency anchors (`M228-C013`, `M228-C012`)",
        ),
    ),
    "metadata_spec": (
        (
            "M228-C013-META-01",
            "deterministic lane-C IR-emission docs/operator runbook",
        ),
        (
            "M228-C013-META-02",
            "synchronization metadata anchors for `M228-C013` plus explicit",
        ),
        (
            "M228-C013-META-03",
            "`M228-C012` dependency continuity so docs/runbook synchronization drift",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M228-C013-FORB-01",
            '"check:objc3c:m228-c013-lane-c-readiness": "npm run check:objc3c:m228-c011-lane-c-readiness',
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m228/M228-C013/ir_emission_completeness_docs_operator_runbook_sync_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):  # noqa: B007
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):  # noqa: B007
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
