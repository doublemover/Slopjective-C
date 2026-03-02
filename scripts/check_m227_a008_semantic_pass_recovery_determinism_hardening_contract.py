#!/usr/bin/env python3
"""Fail-closed validator for M227-A008 semantic pass recovery/determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-recovery-determinism-hardening-a008-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "sema_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "artifact_projection": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_recovery_determinism_hardening_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_contract": (
        ("M227-A008-CNT-01", "bool parser_recovery_replay_ready = false;"),
        ("M227-A008-CNT-02", "bool parser_recovery_replay_case_present = false;"),
        ("M227-A008-CNT-03", "bool parser_recovery_replay_case_passed = false;"),
        ("M227-A008-CNT-04", "bool recovery_replay_contract_satisfied = false;"),
        ("M227-A008-CNT-05", "std::string recovery_replay_key;"),
        ("M227-A008-CNT-06", "bool recovery_replay_key_deterministic = false;"),
        ("M227-A008-CNT-07", "bool recovery_determinism_hardening_satisfied = false;"),
        ("M227-A008-CNT-08", "surface.pass_flow_recovery_replay_contract_satisfied &&"),
        ("M227-A008-CNT-09", "!surface.pass_flow_recovery_replay_key.empty() &&"),
        ("M227-A008-CNT-10", "surface.pass_flow_recovery_replay_key_deterministic &&"),
        ("M227-A008-CNT-11", "surface.pass_flow_recovery_determinism_hardening_satisfied &&"),
    ),
    "sema_manager": (
        ("M227-A008-SRC-01", "const bool parser_recovery_replay_ready ="),
        ("M227-A008-SRC-02", "const bool parser_recovery_replay_case_present ="),
        ("M227-A008-SRC-03", "const bool parser_recovery_replay_case_passed ="),
        ("M227-A008-SRC-04", "const bool parser_recovery_replay_contract_satisfied ="),
        ("M227-A008-SRC-05", "std::ostringstream recovery_replay_key_stream;"),
        ("M227-A008-SRC-06", "const std::string recovery_replay_key = recovery_replay_key_stream.str();"),
        ("M227-A008-SRC-07", "const bool recovery_replay_key_deterministic ="),
        ("M227-A008-SRC-08", "const bool recovery_determinism_hardening_satisfied ="),
        ("M227-A008-SRC-09", "result.sema_pass_flow_summary.recovery_replay_contract_satisfied ="),
        ("M227-A008-SRC-10", "result.sema_pass_flow_summary.recovery_replay_key = recovery_replay_key;"),
        ("M227-A008-SRC-11", "result.parity_surface.pass_flow_recovery_replay_contract_satisfied ="),
        ("M227-A008-SRC-12", "result.parity_surface.pass_flow_recovery_replay_key ="),
        ("M227-A008-SRC-13", "result.parity_surface.pass_flow_recovery_replay_key_deterministic ="),
        ("M227-A008-SRC-14", "result.parity_surface.pass_flow_recovery_determinism_hardening_satisfied ="),
    ),
    "artifact_projection": (
        ("M227-A008-ART-01", "pass_flow_recovery_replay_contract_satisfied"),
        ("M227-A008-ART-02", "pass_flow_recovery_replay_key"),
        ("M227-A008-ART-03", "pass_flow_recovery_replay_key_deterministic"),
        ("M227-A008-ART-04", "pass_flow_recovery_determinism_hardening_satisfied"),
        ("M227-A008-ART-05", "pass_flow_parser_recovery_replay_ready"),
        ("M227-A008-ART-06", "pass_flow_parser_recovery_replay_case_present"),
        ("M227-A008-ART-07", "pass_flow_parser_recovery_replay_case_passed"),
    ),
    "contract_doc": (
        (
            "M227-A008-DOC-01",
            "Contract ID: `objc3c-sema-pass-recovery-determinism-hardening/m227-a008-v1`",
        ),
        ("M227-A008-DOC-02", "recovery_replay_contract_satisfied"),
        ("M227-A008-DOC-03", "recovery_replay_key"),
        (
            "M227-A008-DOC-04",
            "tmp/reports/m227/M227-A008/semantic_pass_recovery_determinism_hardening_contract_summary.json",
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
            "tmp/reports/m227/M227-A008/semantic_pass_recovery_determinism_hardening_contract_summary.json"
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
