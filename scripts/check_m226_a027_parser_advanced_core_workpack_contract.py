#!/usr/bin/env python3
"""Fail-closed validator for M226-A027 parser advanced core workpack."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-advanced-core-workpack-contract-a027-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_advanced_core_workpack_a027_expectations.md",
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        ("M226-A027-DOC-01", "Contract ID: `objc3c-parser-advanced-core-workpack-contract/m226-a027-v1`"),
        ("M226-A027-DOC-02", "`python scripts/check_m226_a027_parser_advanced_core_workpack_contract.py`"),
        (
            "M226-A027-DOC-03",
            "`python -m pytest tests/tooling/test_check_m226_a027_parser_advanced_core_workpack_contract.py -q`",
        ),
    ),
    "parser_contract": (
        ("M226-A027-PRS-01", "std::size_t protocol_class_method_decl_count = 0;"),
        ("M226-A027-PRS-02", "std::size_t protocol_instance_method_decl_count = 0;"),
        ("M226-A027-PRS-03", "std::size_t interface_class_method_decl_count = 0;"),
        ("M226-A027-PRS-04", "std::size_t interface_instance_method_decl_count = 0;"),
        ("M226-A027-PRS-05", "std::size_t implementation_class_method_decl_count = 0;"),
        ("M226-A027-PRS-06", "std::size_t implementation_instance_method_decl_count = 0;"),
        ("M226-A027-PRS-07", "snapshot.protocol_class_method_decl_count"),
        ("M226-A027-PRS-08", "snapshot.interface_class_method_decl_count"),
        ("M226-A027-PRS-09", "snapshot.implementation_class_method_decl_count"),
    ),
    "sema_handoff": (
        ("M226-A027-SEM-01", "BuildObjc3ParserProtocolClassMethodDeclCountFromProgram"),
        ("M226-A027-SEM-02", "BuildObjc3ParserProtocolInstanceMethodDeclCountFromProgram"),
        ("M226-A027-SEM-03", "BuildObjc3ParserInterfaceClassMethodDeclCountFromProgram"),
        ("M226-A027-SEM-04", "BuildObjc3ParserInterfaceInstanceMethodDeclCountFromProgram"),
        ("M226-A027-SEM-05", "BuildObjc3ParserImplementationClassMethodDeclCountFromProgram"),
        ("M226-A027-SEM-06", "BuildObjc3ParserImplementationInstanceMethodDeclCountFromProgram"),
        ("M226-A027-SEM-07", "inline bool AreObjc3ParserMethodDeclBucketsConsistent("),
        ("M226-A027-SEM-08", "matrix.protocol_class_method_decl_count_matches"),
        ("M226-A027-SEM-09", "matrix.interface_class_method_decl_count_matches"),
        ("M226-A027-SEM-10", "matrix.implementation_class_method_decl_count_matches"),
    ),
    "sema_contract": (
        ("M226-A027-CON-01", "parser_protocol_class_method_decl_count"),
        ("M226-A027-CON-02", "parser_protocol_instance_method_decl_count"),
        ("M226-A027-CON-03", "parser_interface_class_method_decl_count"),
        ("M226-A027-CON-04", "parser_interface_instance_method_decl_count"),
        ("M226-A027-CON-05", "parser_implementation_class_method_decl_count"),
        ("M226-A027-CON-06", "parser_implementation_instance_method_decl_count"),
        ("M226-A027-CON-07", "protocol_class_method_decl_count_matches"),
        ("M226-A027-CON-08", "interface_class_method_decl_count_matches"),
        ("M226-A027-CON-09", "implementation_class_method_decl_count_matches"),
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
        default=Path("tmp/reports/m226/M226-A027/parser_advanced_core_workpack_summary.json"),
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
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
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
