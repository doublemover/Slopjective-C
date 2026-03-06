#!/usr/bin/env python3
"""Fail-closed validator for M228-B020 ownership-aware lowering advanced performance workpack (shard 1) contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-b020-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_b020_expectations.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m228_wave_execution_runbook.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_packet.md",
    "b019_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_b019_expectations.md",
    "b019_checker": ROOT
    / "scripts"
    / "check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py",
    "b019_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py",
    "b019_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M228-B020-DOC-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1/m228-b020-v1`",
        ),
        ("M228-B020-DOC-02", "Execute issue `#5214`"),
        ("M228-B020-DOC-03", "Dependencies: `M228-B019`"),
        (
            "M228-B020-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M228-B020-DOC-05", "`docs/runbooks/m228_wave_execution_runbook.md`"),
        (
            "M228-B020-DOC-06",
            "scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py",
        ),
        (
            "M228-B020-DOC-07",
            "tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py",
        ),
        ("M228-B020-DOC-08", "`npm run check:objc3c:m228-b020-lane-b-readiness`"),
        (
            "M228-B020-DOC-09",
            "tmp/reports/m228/M228-B020/ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract_summary.json",
        ),
    ),
    "runbook_doc": (
        (
            "M228-B020-RUN-01",
            "objc3c-ownership-aware-lowering-behavior-advanced-integration-workpack-shard1/m228-b019-v1",
        ),
        (
            "M228-B020-RUN-02",
            "objc3c-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1/m228-b020-v1",
        ),
        (
            "M228-B020-RUN-03",
            "python scripts/check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py",
        ),
        (
            "M228-B020-RUN-04",
            "python scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py",
        ),
        (
            "M228-B020-RUN-05",
            "python -m pytest tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py -q",
        ),
        ("M228-B020-RUN-06", "npm run check:objc3c:m228-b020-lane-b-readiness"),
        ("M228-B020-RUN-07", "tmp/reports/m228/"),
    ),
    "planning_packet": (
        ("M228-B020-PKT-01", "Packet: `M228-B020`"),
        ("M228-B020-PKT-02", "Issue: `#5214`"),
        ("M228-B020-PKT-03", "Dependencies: `M228-B019`"),
        ("M228-B020-PKT-04", "docs/runbooks/m228_wave_execution_runbook.md"),
        (
            "M228-B020-PKT-05",
            "scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py",
        ),
        (
            "M228-B020-PKT-06",
            "tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py",
        ),
        ("M228-B020-PKT-07", "check:objc3c:m228-b020-lane-b-readiness"),
        (
            "M228-B020-PKT-08",
            "tmp/reports/m228/M228-B020/ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract_summary.json",
        ),
    ),
    "b019_contract_doc": (
        (
            "M228-B020-DEP-01",
            "Contract ID: `objc3c-ownership-aware-lowering-behavior-advanced-integration-workpack-shard1/m228-b019-v1`",
        ),
    ),
    "b019_checker": (
        (
            "M228-B020-DEP-02",
            'MODE = "m228-b019-ownership-aware-lowering-behavior-advanced-integration-workpack-shard1-contract-v1"',
        ),
    ),
    "b019_tooling_test": (
        ("M228-B020-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b019_packet_doc": (
        ("M228-B020-DEP-04", "Packet: `M228-B019`"),
        ("M228-B020-DEP-05", "Issue: `#5213`"),
    ),
    "package_json": (
        ("M228-B020-PKG-01", '"check:objc3c:m228-b019-lane-b-readiness"'),
        (
            "M228-B020-PKG-02",
            '"check:objc3c:m228-b020-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1-contract"',
        ),
        (
            "M228-B020-PKG-03",
            '"test:tooling:m228-b020-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1-contract"',
        ),
        ("M228-B020-PKG-04", '"check:objc3c:m228-b020-lane-b-readiness"'),
        (
            "M228-B020-PKG-05",
            '"check:objc3c:m228-b020-lane-b-readiness": "npm run check:objc3c:m228-b019-lane-b-readiness &&',
        ),
        (
            "M228-B020-PKG-06",
            "scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py",
        ),
        (
            "M228-B020-PKG-07",
            "tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py -q",
        ),
        ("M228-B020-PKG-08", '"compile:objc3c"'),
        ("M228-B020-PKG-09", '"proof:objc3c"'),
        ("M228-B020-PKG-10", '"test:objc3c:execution-smoke"'),
        ("M228-B020-PKG-11", '"test:objc3c:lowering-replay-proof"'),
    ),
    "architecture_doc": (
        (
            "M228-B020-ARC-01",
            "M228 lane-B B020 ownership-aware lowering advanced performance workpack (shard 1) anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M228-B020-SPC-01",
            "ownership-aware lowering advanced performance workpack (shard 1)",
        ),
        (
            "M228-B020-SPC-02",
            "shall preserve explicit lane-B dependency anchors (`M228-B020`, `M228-B019`)",
        ),
    ),
    "metadata_spec": (
        (
            "M228-B020-META-01",
            "deterministic lane-B ownership-aware lowering",
        ),
        (
            "M228-B020-META-02",
            "metadata anchors for `M228-B020` plus explicit `M228-B019` dependency",
        ),
        (
            "M228-B020-META-03",
            "advanced-performance-shard1 drift fails closed,",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M228-B020-FORB-01",
            '"check:objc3c:m228-b020-lane-b-readiness": "npm run check:objc3c:m228-b018-lane-b-readiness',
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
            "tmp/reports/m228/M228-B020/ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract_summary.json"
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
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
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




