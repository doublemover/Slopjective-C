#!/usr/bin/env python3
"""Fail-closed validator for M232-C014 message-send release-candidate and replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m232-c014-message-send-lowering-and-call-emission-release-candidate-and-replay-dry-run-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m232_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_c014_expectations.md",
    "runbook_doc": ROOT / "docs" / "runbooks" / "m232_wave_execution_runbook.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m232"
    / "m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_packet.md",
    "c005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m232_message_send_lowering_and_call_emission_docs_and_operator_runbook_synchronization_c013_expectations.md",
    "c005_checker": ROOT
    / "scripts"
    / "check_m232_c013_message_send_lowering_and_call_emission_docs_and_operator_runbook_synchronization_contract.py",
    "c005_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m232_c013_message_send_lowering_and_call_emission_docs_and_operator_runbook_synchronization_contract.py",
    "c005_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m232"
    / "m232_c013_message_send_lowering_and_call_emission_docs_and_operator_runbook_synchronization_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M232-C014-DOC-01",
            "Contract ID: `objc3c-message-send-lowering-and-call-emission-release-candidate-and-replay-dry-run/m232-c014-v1`",
        ),
        ("M232-C014-DOC-02", "Execute issue `#5624`"),
        ("M232-C014-DOC-03", "Dependencies: `M232-C013`"),
        (
            "M232-C014-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M232-C014-DOC-05", "`docs/runbooks/m232_wave_execution_runbook.md`"),
        (
            "M232-C014-DOC-06",
            "scripts/check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M232-C014-DOC-07",
            "tests/tooling/test_check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py",
        ),
        ("M232-C014-DOC-08", "`npm run check:objc3c:m232-c014-lane-c-readiness`"),
        (
            "M232-C014-DOC-09",
            "tmp/reports/m232/M232-C014/message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_summary.json",
        ),
    ),
    "runbook_doc": (
        (
            "M232-C014-RUN-01",
            "objc3c-message-send-lowering-and-call-emission-release-candidate-and-replay-dry-run/m232-c014-v1",
        ),
        (
            "M232-C014-RUN-02",
            "python scripts/check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M232-C014-RUN-03",
            "python -m pytest tests/tooling/test_check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py -q",
        ),
        ("M232-C014-RUN-04", "npm run check:objc3c:m232-c014-lane-c-readiness"),
        ("M232-C014-RUN-05", "tmp/reports/m232/"),
    ),
    "planning_packet": (
        ("M232-C014-PKT-01", "Packet: `M232-C014`"),
        ("M232-C014-PKT-02", "Issue: `#5624`"),
        ("M232-C014-PKT-03", "Dependencies: `M232-C013`"),
        ("M232-C014-PKT-04", "docs/runbooks/m232_wave_execution_runbook.md"),
        (
            "M232-C014-PKT-05",
            "scripts/check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M232-C014-PKT-06",
            "tests/tooling/test_check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py",
        ),
        ("M232-C014-PKT-07", "check:objc3c:m232-c014-lane-c-readiness"),
        (
            "M232-C014-PKT-08",
            "tmp/reports/m232/M232-C014/message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_summary.json",
        ),
    ),
    "c005_contract_doc": (
        (
            "M232-C014-DEP-01",
            "Contract ID: `objc3c-message-send-lowering-and-call-emission-docs-and-operator-runbook-synchronization/m232-c013-v1`",
        ),
    ),
    "c005_checker": (
        (
            "M232-C014-DEP-02",
            'MODE = "m232-c013-message-send-lowering-and-call-emission-docs-and-operator-runbook-synchronization-v1"',
        ),
    ),
    "c005_tooling_test": (
        ("M232-C014-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "c005_packet_doc": (
        ("M232-C014-DEP-04", "Packet: `M232-C013`"),
        ("M232-C014-DEP-05", "Issue: `#5623`"),
    ),
    "package_json": (
        ("M232-C014-PKG-01", '"check:objc3c:m232-c013-lane-c-readiness"'),
        (
            "M232-C014-PKG-02",
            '"check:objc3c:m232-c014-message-send-lowering-and-call-emission-release-candidate-and-replay-dry-run-contract"',
        ),
        (
            "M232-C014-PKG-03",
            '"test:tooling:m232-c014-message-send-lowering-and-call-emission-release-candidate-and-replay-dry-run-contract"',
        ),
        ("M232-C014-PKG-04", '"check:objc3c:m232-c014-lane-c-readiness"'),
        (
            "M232-C014-PKG-05",
            '"check:objc3c:m232-c014-lane-c-readiness": "npm run check:objc3c:m232-c013-lane-c-readiness &&',
        ),
        (
            "M232-C014-PKG-06",
            "scripts/check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py",
        ),
        (
            "M232-C014-PKG-07",
            "tests/tooling/test_check_m232_c014_message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_contract.py -q",
        ),
        ("M232-C014-PKG-08", '"compile:objc3c"'),
        ("M232-C014-PKG-09", '"proof:objc3c"'),
        ("M232-C014-PKG-10", '"test:objc3c:execution-replay-proof"'),
        ("M232-C014-PKG-11", '"test:objc3c:perf-budget"'),
    ),
    "architecture_doc": (
        (
            "M232-C014-ARC-01",
            "M232 lane-C C014 message send lowering and call emission release-candidate and replay dry-run anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M232-C014-SPC-01",
            "message send lowering and call emission release-candidate and replay dry-run governance shall preserve explicit lane-C dependency anchors (`M232-C014`, `M232-C013`)",
        ),
    ),
    "metadata_spec": (
        (
            "M232-C014-META-01",
            "deterministic lane-C message-send lowering and call-emission release-candidate and replay dry-run metadata anchors for `M232-C014`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M232-C014-FORB-01",
            '"check:objc3c:m232-c014-lane-c-readiness": "npm run check:objc3c:m232-c014-message-send-lowering-and-call-emission-release-candidate-and-replay-dry-run-contract"',
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
            "tmp/reports/m232/M232-C014/message_send_lowering_and_call_emission_release_candidate_and_replay_dry_run_summary.json"
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



















