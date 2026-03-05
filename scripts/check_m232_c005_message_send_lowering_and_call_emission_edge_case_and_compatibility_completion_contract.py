#!/usr/bin/env python3
"""Fail-closed validator for M232-C005 message-send edge-case compatibility."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m232_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_c005_expectations.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m232_wave_execution_runbook.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m232"
    / "m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_packet.md",
    "c004_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m232_message_send_lowering_and_call_emission_core_feature_expansion_c004_expectations.md",
    "c004_checker": ROOT
    / "scripts"
    / "check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py",
    "c004_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py",
    "c004_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m232"
    / "m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M232-C005-DOC-01",
            "Contract ID: `objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1`",
        ),
        ("M232-C005-DOC-02", "Execute issue `#5615`"),
        ("M232-C005-DOC-03", "Dependencies: `M232-C004`"),
        (
            "M232-C005-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M232-C005-DOC-05", "`docs/runbooks/m232_wave_execution_runbook.md`"),
        (
            "M232-C005-DOC-06",
            "scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M232-C005-DOC-07",
            "tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py",
        ),
        ("M232-C005-DOC-08", "`npm run check:objc3c:m232-c005-lane-c-readiness`"),
        (
            "M232-C005-DOC-09",
            "tmp/reports/m232/M232-C005/message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_summary.json",
        ),
    ),
    "runbook_doc": (
        (
            "M232-C005-RUN-01",
            "objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1",
        ),
        (
            "M232-C005-RUN-02",
            "python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M232-C005-RUN-03",
            "python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q",
        ),
        ("M232-C005-RUN-04", "npm run check:objc3c:m232-c005-lane-c-readiness"),
        ("M232-C005-RUN-05", "tmp/reports/m232/"),
    ),
    "planning_packet": (
        ("M232-C005-PKT-01", "Packet: `M232-C005`"),
        ("M232-C005-PKT-02", "Issue: `#5615`"),
        ("M232-C005-PKT-03", "Dependencies: `M232-C004`"),
        ("M232-C005-PKT-04", "docs/runbooks/m232_wave_execution_runbook.md"),
        (
            "M232-C005-PKT-05",
            "scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M232-C005-PKT-06",
            "tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py",
        ),
        ("M232-C005-PKT-07", "check:objc3c:m232-c005-lane-c-readiness"),
        (
            "M232-C005-PKT-08",
            "tmp/reports/m232/M232-C005/message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_summary.json",
        ),
    ),
    "c004_contract_doc": (
        (
            "M232-C005-DEP-01",
            "Contract ID: `objc3c-message-send-lowering-and-call-emission-core-feature-expansion/m232-c004-v1`",
        ),
    ),
    "c004_checker": (
        (
            "M232-C005-DEP-02",
            'MODE = "m232-c004-message-send-lowering-and-call-emission-core-feature-expansion-v1"',
        ),
    ),
    "c004_tooling_test": (
        ("M232-C005-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "c004_packet_doc": (
        ("M232-C005-DEP-04", "Packet: `M232-C004`"),
        ("M232-C005-DEP-05", "Issue: `#5614`"),
    ),
    "package_json": (
        ("M232-C005-PKG-01", '"check:objc3c:m232-c004-lane-c-readiness"'),
        (
            "M232-C005-PKG-02",
            '"check:objc3c:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract"',
        ),
        (
            "M232-C005-PKG-03",
            '"test:tooling:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract"',
        ),
        ("M232-C005-PKG-04", '"check:objc3c:m232-c005-lane-c-readiness"'),
        (
            "M232-C005-PKG-05",
            '"check:objc3c:m232-c005-lane-c-readiness": "npm run check:objc3c:m232-c004-lane-c-readiness &&',
        ),
        (
            "M232-C005-PKG-06",
            "scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M232-C005-PKG-07",
            "tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q",
        ),
        ("M232-C005-PKG-08", '"compile:objc3c"'),
        ("M232-C005-PKG-09", '"proof:objc3c"'),
        ("M232-C005-PKG-10", '"test:objc3c:execution-replay-proof"'),
        ("M232-C005-PKG-11", '"test:objc3c:perf-budget"'),
    ),
    "architecture_doc": (
        (
            "M232-C005-ARC-01",
            "M232 lane-C C005 message send lowering and call emission edge-case and compatibility completion anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M232-C005-SPC-01",
            "message send lowering and call emission edge-case and compatibility completion governance shall preserve explicit lane-C dependency anchors (`M232-C005`, `M232-C004`)",
        ),
    ),
    "metadata_spec": (
        (
            "M232-C005-META-01",
            "deterministic lane-C message-send lowering and call-emission edge-case/compatibility metadata anchors for `M232-C005`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M232-C005-FORB-01",
            '"check:objc3c:m232-c005-lane-c-readiness": "npm run check:objc3c:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract"',
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
            "tmp/reports/m232/M232-C005/message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_summary.json"
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
