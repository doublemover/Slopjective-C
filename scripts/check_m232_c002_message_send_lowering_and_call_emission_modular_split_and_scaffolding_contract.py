#!/usr/bin/env python3
"""Fail-closed validator for M232-C002 message-send modular split/scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m232-c002-message-send-lowering-and-call-emission-modular-split-and-scaffolding-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m232_message_send_lowering_and_call_emission_modular_split_and_scaffolding_c002_expectations.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m232_wave_execution_runbook.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m232"
    / "m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_packet.md",
    "c001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m232_message_send_lowering_and_call_emission_contract_and_architecture_freeze_c001_expectations.md",
    "c001_checker": ROOT
    / "scripts"
    / "check_m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_contract.py",
    "c001_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_contract.py",
    "c001_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m232"
    / "m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M232-C002-DOC-01",
            "Contract ID: `objc3c-message-send-lowering-and-call-emission-modular-split-and-scaffolding/m232-c002-v1`",
        ),
        ("M232-C002-DOC-02", "Execute issue `#5612`"),
        ("M232-C002-DOC-03", "Dependencies: `M232-C001`"),
        (
            "M232-C002-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M232-C002-DOC-05", "`docs/runbooks/m232_wave_execution_runbook.md`"),
        (
            "M232-C002-DOC-06",
            "scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py",
        ),
        (
            "M232-C002-DOC-07",
            "tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py",
        ),
        ("M232-C002-DOC-08", "`npm run check:objc3c:m232-c002-lane-c-readiness`"),
        (
            "M232-C002-DOC-09",
            "tmp/reports/m232/M232-C002/message_send_lowering_and_call_emission_modular_split_and_scaffolding_summary.json",
        ),
    ),
    "runbook_doc": (
        (
            "M232-C002-RUN-01",
            "objc3c-message-send-lowering-and-call-emission-contract-and-architecture-freeze/m232-c001-v1",
        ),
        (
            "M232-C002-RUN-02",
            "objc3c-message-send-lowering-and-call-emission-modular-split-and-scaffolding/m232-c002-v1",
        ),
        (
            "M232-C002-RUN-03",
            "python scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py",
        ),
        (
            "M232-C002-RUN-04",
            "python -m pytest tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py -q",
        ),
        ("M232-C002-RUN-05", "npm run check:objc3c:m232-c002-lane-c-readiness"),
        ("M232-C002-RUN-06", "tmp/reports/m232/"),
    ),
    "planning_packet": (
        ("M232-C002-PKT-01", "Packet: `M232-C002`"),
        ("M232-C002-PKT-02", "Issue: `#5612`"),
        ("M232-C002-PKT-03", "Dependencies: `M232-C001`"),
        ("M232-C002-PKT-04", "docs/runbooks/m232_wave_execution_runbook.md"),
        (
            "M232-C002-PKT-05",
            "scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py",
        ),
        (
            "M232-C002-PKT-06",
            "tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py",
        ),
        ("M232-C002-PKT-07", "check:objc3c:m232-c002-lane-c-readiness"),
        (
            "M232-C002-PKT-08",
            "tmp/reports/m232/M232-C002/message_send_lowering_and_call_emission_modular_split_and_scaffolding_summary.json",
        ),
    ),
    "c001_contract_doc": (
        (
            "M232-C002-DEP-01",
            "Contract ID: `objc3c-message-send-lowering-and-call-emission-contract-and-architecture-freeze/m232-c001-v1`",
        ),
    ),
    "c001_checker": (
        (
            "M232-C002-DEP-02",
            'MODE = "m232-c001-message-send-lowering-and-call-emission-contract-and-architecture-freeze-v1"',
        ),
    ),
    "c001_tooling_test": (
        ("M232-C002-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "c001_packet_doc": (
        ("M232-C002-DEP-04", "Packet: `M232-C001`"),
        ("M232-C002-DEP-05", "Issue: `#5611`"),
    ),
    "package_json": (
        ("M232-C002-PKG-01", '"check:objc3c:m232-c001-lane-c-readiness"'),
        (
            "M232-C002-PKG-02",
            '"check:objc3c:m232-c002-message-send-lowering-and-call-emission-modular-split-and-scaffolding-contract"',
        ),
        (
            "M232-C002-PKG-03",
            '"test:tooling:m232-c002-message-send-lowering-and-call-emission-modular-split-and-scaffolding-contract"',
        ),
        ("M232-C002-PKG-04", '"check:objc3c:m232-c002-lane-c-readiness"'),
        (
            "M232-C002-PKG-05",
            '"check:objc3c:m232-c002-lane-c-readiness": "npm run check:objc3c:m232-c001-lane-c-readiness &&',
        ),
        (
            "M232-C002-PKG-06",
            "scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py",
        ),
        (
            "M232-C002-PKG-07",
            "tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py -q",
        ),
        ("M232-C002-PKG-08", '"compile:objc3c"'),
        ("M232-C002-PKG-09", '"proof:objc3c"'),
        ("M232-C002-PKG-10", '"test:objc3c:execution-replay-proof"'),
        ("M232-C002-PKG-11", '"test:objc3c:perf-budget"'),
    ),
    "architecture_doc": (
        (
            "M232-C002-ARC-01",
            "M232 lane-C C002 message send lowering and call emission modular split and scaffolding anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M232-C002-SPC-01",
            "message send lowering and call emission modular split and scaffolding governance shall preserve explicit lane-C dependency anchors (`M232-C002`, `M232-C001`)",
        ),
    ),
    "metadata_spec": (
        (
            "M232-C002-META-01",
            "deterministic lane-C message-send lowering and call-emission modular split/scaffolding metadata anchors for `M232-C002`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M232-C002-FORB-01",
            '"check:objc3c:m232-c002-lane-c-readiness": "npm run check:objc3c:m232-c002-message-send-lowering-and-call-emission-modular-split-and-scaffolding-contract"',
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
            "tmp/reports/m232/M232-C002/message_send_lowering_and_call_emission_modular_split_and_scaffolding_summary.json"
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

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
