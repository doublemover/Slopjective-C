#!/usr/bin/env python3
"""Fail-closed validator for M226-A015 parser advanced core workpack."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-advanced-core-workpack-contract-a015-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_advanced_core_workpack_a015_expectations.md",
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "frontend_artifacts": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        ("M226-A015-DOC-01", "Contract ID: `objc3c-parser-advanced-core-workpack-contract/m226-a015-v1`"),
        ("M226-A015-DOC-02", "`python scripts/check_m226_a015_parser_advanced_core_workpack_contract.py`"),
        ("M226-A015-DOC-03", "`python -m pytest tests/tooling/test_check_m226_a015_parser_advanced_core_workpack_contract.py -q`"),
    ),
    "parser_contract": (
        ("M226-A015-PRS-01", "std::size_t interface_category_decl_count = 0;"),
        ("M226-A015-PRS-02", "std::size_t implementation_category_decl_count = 0;"),
        ("M226-A015-PRS-03", "std::size_t function_prototype_count = 0;"),
        ("M226-A015-PRS-04", "std::size_t function_pure_count = 0;"),
        ("M226-A015-PRS-05", "snapshot.interface_category_decl_count ="),
        ("M226-A015-PRS-06", "snapshot.implementation_category_decl_count ="),
        ("M226-A015-PRS-07", "snapshot.function_prototype_count ="),
        ("M226-A015-PRS-08", "snapshot.function_pure_count ="),
    ),
    "sema_handoff": (
        ("M226-A015-SEM-01", "BuildObjc3ParserInterfaceCategoryDeclCountFromProgram"),
        ("M226-A015-SEM-02", "BuildObjc3ParserImplementationCategoryDeclCountFromProgram"),
        ("M226-A015-SEM-03", "BuildObjc3ParserFunctionPrototypeCountFromProgram"),
        ("M226-A015-SEM-04", "BuildObjc3ParserFunctionPureCountFromProgram"),
        ("M226-A015-SEM-05", "snapshot.interface_category_decl_count == 0u"),
        ("M226-A015-SEM-06", "snapshot.implementation_category_decl_count == 0u"),
        ("M226-A015-SEM-07", "snapshot.function_prototype_count == 0u"),
        ("M226-A015-SEM-08", "snapshot.function_pure_count == 0u"),
    ),
    "frontend_artifacts": (
        ("M226-A015-ART-01", ",\\\"interface_categories\\\":"),
        ("M226-A015-ART-02", ",\\\"implementation_categories\\\":"),
        ("M226-A015-ART-03", ",\\\"function_prototypes\\\":"),
        ("M226-A015-ART-04", ",\\\"function_pure\\\":"),
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
        default=Path("tmp/reports/m226/M226-A015/parser_advanced_core_workpack_summary.json"),
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
