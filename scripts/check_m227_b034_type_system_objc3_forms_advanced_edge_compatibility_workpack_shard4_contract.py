#!/usr/bin/env python3
"""Fail-closed validator for M227-B034 ObjC3 type-form advanced edge compatibility workpack (shard 4) contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-advanced-edge-compatibility-workpack-shard4-b034-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_b034_expectations.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m227_wave_execution_runbook.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_packet.md",
    "b033_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_advanced_core_workpack_shard4_b033_expectations.md",
    "b033_checker": ROOT
    / "scripts"
    / "check_m227_b033_type_system_objc3_forms_advanced_core_workpack_shard4_contract.py",
    "b033_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_b033_type_system_objc3_forms_advanced_core_workpack_shard4_contract.py",
    "b033_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b033_type_system_objc3_forms_advanced_core_workpack_shard4_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M227-B034-DOC-01",
            "Contract ID: `objc3c-type-system-objc3-forms-advanced-edge-compatibility-workpack-shard4/m227-b034-v1`",
        ),
        ("M227-B034-DOC-02", "Execute issue `#5115`"),
        ("M227-B034-DOC-03", "Dependencies: `M227-B033`"),
        (
            "M227-B034-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M227-B034-DOC-05", "`docs/runbooks/m227_wave_execution_runbook.md`"),
        (
            "M227-B034-DOC-06",
            "scripts/check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py",
        ),
        (
            "M227-B034-DOC-07",
            "tests/tooling/test_check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py",
        ),
        ("M227-B034-DOC-08", "`npm run check:objc3c:m227-b034-lane-b-readiness`"),
        (
            "M227-B034-DOC-09",
            "tmp/reports/m227/M227-B034/type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract_summary.json",
        ),
    ),
    "runbook_doc": (
        (
            "M227-B034-RUN-01",
            "objc3c-type-system-objc3-forms-advanced-core-workpack-shard4/m227-b033-v1",
        ),
        (
            "M227-B034-RUN-02",
            "objc3c-type-system-objc3-forms-advanced-edge-compatibility-workpack-shard4/m227-b034-v1",
        ),
        (
            "M227-B034-RUN-03",
            "python scripts/check_m227_b033_type_system_objc3_forms_advanced_core_workpack_shard4_contract.py",
        ),
        (
            "M227-B034-RUN-04",
            "python scripts/check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py",
        ),
        (
            "M227-B034-RUN-05",
            "python -m pytest tests/tooling/test_check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py -q",
        ),
        ("M227-B034-RUN-06", "npm run check:objc3c:m227-b034-lane-b-readiness"),
        ("M227-B034-RUN-07", "tmp/reports/m227/"),
    ),
    "planning_packet": (
        ("M227-B034-PKT-01", "Packet: `M227-B034`"),
        ("M227-B034-PKT-02", "Issue: `#5115`"),
        ("M227-B034-PKT-03", "Dependencies: `M227-B033`"),
        ("M227-B034-PKT-04", "docs/runbooks/m227_wave_execution_runbook.md"),
        (
            "M227-B034-PKT-05",
            "scripts/check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py",
        ),
        (
            "M227-B034-PKT-06",
            "tests/tooling/test_check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py",
        ),
        ("M227-B034-PKT-07", "check:objc3c:m227-b034-lane-b-readiness"),
        (
            "M227-B034-PKT-08",
            "tmp/reports/m227/M227-B034/type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract_summary.json",
        ),
    ),
    "b033_contract_doc": (
        (
            "M227-B034-DEP-01",
            "Contract ID: `objc3c-type-system-objc3-forms-advanced-core-workpack-shard4/m227-b033-v1`",
        ),
    ),
    "b033_checker": (
        (
            "M227-B034-DEP-02",
            'MODE = "m227-type-system-objc3-forms-advanced-core-workpack-shard4-b033-v1"',
        ),
    ),
    "b033_tooling_test": (
        ("M227-B034-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b033_packet_doc": (
        ("M227-B034-DEP-04", "Packet: `M227-B033`"),
        ("M227-B034-DEP-05", "Issue: `#5114`"),
    ),
    "package_json": (
        ("M227-B034-PKG-01", '"check:objc3c:m227-b033-lane-b-readiness"'),
        (
            "M227-B034-PKG-02",
            '"check:objc3c:m227-b034-type-system-objc3-forms-advanced-edge-compatibility-workpack-shard4-contract"',
        ),
        (
            "M227-B034-PKG-03",
            '"test:tooling:m227-b034-type-system-objc3-forms-advanced-edge-compatibility-workpack-shard4-contract"',
        ),
        ("M227-B034-PKG-04", '"check:objc3c:m227-b034-lane-b-readiness"'),
        (
            "M227-B034-PKG-05",
            '"check:objc3c:m227-b034-lane-b-readiness": "npm run check:objc3c:m227-b033-lane-b-readiness &&',
        ),
        (
            "M227-B034-PKG-06",
            "scripts/check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py",
        ),
        (
            "M227-B034-PKG-07",
            "tests/tooling/test_check_m227_b034_type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract.py -q",
        ),
        ("M227-B034-PKG-08", '"compile:objc3c"'),
        ("M227-B034-PKG-09", '"proof:objc3c"'),
        ("M227-B034-PKG-10", '"test:objc3c:execution-replay-proof"'),
        ("M227-B034-PKG-11", '"test:objc3c:perf-budget"'),
    ),
    "architecture_doc": (
        (
            "M227-B034-ARC-01",
            "M227 lane-B B034 type-system advanced edge compatibility workpack (shard 4) anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M227-B034-SPC-01",
            "type-system advanced edge compatibility workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B034`, `M227-B033`)",
        ),
    ),
    "metadata_spec": (
        (
            "M227-B034-META-01",
            "deterministic lane-B type-system advanced edge compatibility workpack (shard 4) metadata anchors for `M227-B034`",
        ),
        (
            "M227-B034-META-02",
            "plus explicit `M227-B033` dependency continuity so advanced edge compatibility workpack (shard 4) drift fails closed.",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M227-B034-FORB-01",
            '"check:objc3c:m227-b034-lane-b-readiness": "npm run check:objc3c:m227-b027-lane-b-readiness',
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
            "tmp/reports/m227/M227-B034/type_system_objc3_forms_advanced_edge_compatibility_workpack_shard4_contract_summary.json"
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
