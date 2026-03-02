#!/usr/bin/env python3
"""Fail-closed validator for M226-A001 parser architecture freeze contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-architecture-freeze-contract-a001-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h",
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "ast_builder_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.cpp",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_architecture_expectations.md",
    "grammar_spec": ROOT / "spec" / "FORMAL_GRAMMAR_AND_PRECEDENCE.md",
    "syntax_spec": ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_header": (
        ("M226-A001-PARSE-01", '#include "parse/objc3_parser_contract.h"'),
        ("M226-A001-PARSE-02", "struct Objc3ParseResult {"),
        ("M226-A001-PARSE-03", "Objc3ParsedProgram program;"),
        ("M226-A001-PARSE-04", "Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens);"),
    ),
    "parser_contract": (
        ("M226-A001-CNT-01", "struct Objc3ParsedProgram {"),
        ("M226-A001-CNT-02", "inline const Objc3Program &Objc3ParsedProgramAst("),
    ),
    "parser_source": (
        ("M226-A001-SRC-01", "Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens) {"),
        ("M226-A001-SRC-02", "Objc3Parser parser(tokens);"),
        ("M226-A001-SRC-03", "result.program = parser.Parse();"),
        ("M226-A001-SRC-04", "result.diagnostics = parser.TakeDiagnostics();"),
    ),
    "ast_builder_contract": (
        ("M226-A001-AST-01", "Objc3ParseResult parse_result = ParseObjc3Program(tokens);"),
    ),
    "pipeline_source": (
        ("M226-A001-PIPE-01", '#include "parse/objc3_ast_builder_contract.h"'),
        ("M226-A001-PIPE-02", "Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);"),
    ),
    "architecture_doc": (
        ("M226-A001-ARCH-01", "`main.cpp` is now driver-only"),
        ("M226-A001-ARCH-02", "Parser recovery behavior must remain replay-proof and deterministic."),
    ),
    "contract_doc": (
        ("M226-A001-DOC-01", "Contract ID: `objc3c-parser-architecture-freeze-contract/m226-a001-v1`"),
        ("M226-A001-DOC-02", "pipeline/objc3_frontend_pipeline.cpp"),
        ("M226-A001-DOC-03", "spec/FORMAL_GRAMMAR_AND_PRECEDENCE.md"),
        ("M226-A001-DOC-04", "spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "pipeline_source": (
        ("M226-A001-PIPE-03", '#include "parse/objc3_parser.h"'),
    ),
    "architecture_doc": (
        ("M226-A001-ARCH-03", "monolithic `main.cpp`"),
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
        default=Path("tmp/reports/m226/M226-A001/parser_architecture_contract_summary.json"),
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
