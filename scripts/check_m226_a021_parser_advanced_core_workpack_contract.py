#!/usr/bin/env python3
"""Fail-closed validator for M226-A021 parser advanced core workpack."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-advanced-core-workpack-contract-a021-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_advanced_core_workpack_a021_expectations.md",
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        ("M226-A021-DOC-01", "Contract ID: `objc3c-parser-advanced-core-workpack-contract/m226-a021-v1`"),
        ("M226-A021-DOC-02", "`python scripts/check_m226_a021_parser_advanced_core_workpack_contract.py`"),
        (
            "M226-A021-DOC-03",
            "`python -m pytest tests/tooling/test_check_m226_a021_parser_advanced_core_workpack_contract.py -q`",
        ),
    ),
    "parser_contract": (
        ("M226-A021-PRS-01", "std::size_t protocol_property_decl_count = 0;"),
        ("M226-A021-PRS-02", "std::size_t protocol_method_decl_count = 0;"),
        ("M226-A021-PRS-03", "std::size_t interface_property_decl_count = 0;"),
        ("M226-A021-PRS-04", "std::size_t interface_method_decl_count = 0;"),
        ("M226-A021-PRS-05", "std::size_t implementation_property_decl_count = 0;"),
        ("M226-A021-PRS-06", "std::size_t implementation_method_decl_count = 0;"),
        ("M226-A021-PRS-07", "snapshot.protocol_property_decl_count += protocol_decl.properties.size();"),
        ("M226-A021-PRS-08", "snapshot.protocol_method_decl_count += protocol_decl.methods.size();"),
        ("M226-A021-PRS-09", "snapshot.interface_property_decl_count += interface_decl.properties.size();"),
        ("M226-A021-PRS-10", "snapshot.interface_method_decl_count += interface_decl.methods.size();"),
        (
            "M226-A021-PRS-11",
            "snapshot.implementation_property_decl_count += implementation_decl.properties.size();",
        ),
        (
            "M226-A021-PRS-12",
            "snapshot.implementation_method_decl_count += implementation_decl.methods.size();",
        ),
    ),
    "sema_handoff": (
        ("M226-A021-SEM-01", "BuildObjc3ParserProtocolPropertyDeclCountFromProgram"),
        ("M226-A021-SEM-02", "BuildObjc3ParserProtocolMethodDeclCountFromProgram"),
        ("M226-A021-SEM-03", "BuildObjc3ParserInterfacePropertyDeclCountFromProgram"),
        ("M226-A021-SEM-04", "BuildObjc3ParserInterfaceMethodDeclCountFromProgram"),
        ("M226-A021-SEM-05", "BuildObjc3ParserImplementationPropertyDeclCountFromProgram"),
        ("M226-A021-SEM-06", "BuildObjc3ParserImplementationMethodDeclCountFromProgram"),
        ("M226-A021-SEM-07", "snapshot.protocol_property_decl_count == 0u"),
        ("M226-A021-SEM-08", "snapshot.protocol_method_decl_count == 0u"),
        ("M226-A021-SEM-09", "snapshot.interface_property_decl_count == 0u"),
        ("M226-A021-SEM-10", "snapshot.interface_method_decl_count == 0u"),
        ("M226-A021-SEM-11", "snapshot.implementation_property_decl_count == 0u"),
        ("M226-A021-SEM-12", "snapshot.implementation_method_decl_count == 0u"),
        ("M226-A021-SEM-13", "snapshot.protocol_property_decl_count == protocol_property_count"),
        ("M226-A021-SEM-14", "snapshot.interface_method_decl_count == interface_method_count"),
        (
            "M226-A021-SEM-15",
            "snapshot.implementation_method_decl_count == implementation_method_count",
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
        default=Path("tmp/reports/m226/M226-A021/parser_advanced_core_workpack_summary.json"),
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
