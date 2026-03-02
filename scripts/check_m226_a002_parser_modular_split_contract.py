#!/usr/bin/env python3
"""Fail-closed validator for M226-A002 parser modular split contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-modular-split-contract-a002-v1"

ARTIFACTS: dict[str, Path] = {
    "support_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parse_support.h",
    "support_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parse_support.cpp",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_modular_split_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "support_header": (
        ("M226-A002-H-01", "namespace objc3c::parse::support {"),
        ("M226-A002-H-02", "bool ParseIntegerLiteralValue(const std::string &text, int &value);"),
        (
            "M226-A002-H-03",
            "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message);",
        ),
    ),
    "support_source": (
        ("M226-A002-CPP-01", '#include "parse/objc3_parse_support.h"'),
        ("M226-A002-CPP-02", "bool ParseIntegerLiteralValue(const std::string &text, int &value) {"),
        (
            "M226-A002-CPP-03",
            "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {",
        ),
    ),
    "parser_source": (
        ("M226-A002-PARSE-01", '#include "parse/objc3_parse_support.h"'),
        ("M226-A002-PARSE-02", "using objc3c::parse::support::MakeDiag;"),
        ("M226-A002-PARSE-03", "using objc3c::parse::support::ParseIntegerLiteralValue;"),
    ),
    "cmake": (
        ("M226-A002-CMAKE-01", "src/parse/objc3_parse_support.cpp"),
    ),
    "build_script": (
        ("M226-A002-BLD-01", "native/objc3c/src/parse/objc3_parse_support.cpp"),
    ),
    "contract_doc": (
        ("M226-A002-DOC-01", "Contract ID: `objc3c-parser-modular-split-contract/m226-a002-v1`"),
        ("M226-A002-DOC-02", "native/objc3c/src/parse/objc3_parse_support.cpp"),
        ("M226-A002-DOC-03", "scripts/build_objc3c_native.ps1"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_source": (
        ("M226-A002-PARSE-04", "static bool ParseIntegerLiteralValue("),
        ("M226-A002-PARSE-05", "static std::string MakeDiag("),
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
        default=Path("tmp/reports/m226/M226-A002/parser_modular_split_contract_summary.json"),
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
