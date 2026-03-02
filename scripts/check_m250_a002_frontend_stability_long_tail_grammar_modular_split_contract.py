#!/usr/bin/env python3
"""Fail-closed validator for M250-A002 frontend stability modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-frontend-stability-long-tail-grammar-modular-split-contract-a002-v1"

ARTIFACTS: dict[str, Path] = {
    "parse_support_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parse_support.h",
    "parse_support_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parse_support.cpp",
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "ast_builder_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.h",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "readiness_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "package_json": ROOT / "package.json",
    "a001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_closure_a001_expectations.md",
    "a001_checker": ROOT / "scripts" / "check_m250_a001_frontend_stability_long_tail_grammar_closure_contract.py",
    "a001_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m250_a001_frontend_stability_long_tail_grammar_closure_contract.py",
    "a001_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_a001_frontend_stability_long_tail_grammar_contract_freeze.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_modular_split_a002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_a002_frontend_stability_long_tail_grammar_modular_split_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_support_header": (
        ("M250-A002-PSH-01", "namespace objc3c::parse::support {"),
        ("M250-A002-PSH-02", "bool ParseIntegerLiteralValue(const std::string &text, int &value);"),
        ("M250-A002-PSH-03", "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message);"),
    ),
    "parse_support_source": (
        ("M250-A002-PSS-01", "#include \"parse/objc3_parse_support.h\""),
        ("M250-A002-PSS-02", "namespace objc3c::parse::support {"),
        ("M250-A002-PSS-03", "bool ParseIntegerLiteralValue(const std::string &text, int &value) {"),
        ("M250-A002-PSS-04", "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {"),
    ),
    "parser_contract": (
        ("M250-A002-PCT-01", "struct Objc3ParserContractSnapshot {"),
        ("M250-A002-PCT-02", "inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot("),
    ),
    "ast_builder_contract": (
        ("M250-A002-ABC-01", "struct Objc3AstBuilderResult {"),
        ("M250-A002-ABC-02", "Objc3ParserContractSnapshot contract_snapshot;"),
    ),
    "parser_source": (
        ("M250-A002-PRS-01", "#include \"parse/objc3_parse_support.h\""),
        ("M250-A002-PRS-02", "using objc3c::parse::support::MakeDiag;"),
        ("M250-A002-PRS-03", "using objc3c::parse::support::ParseIntegerLiteralValue;"),
        ("M250-A002-PRS-04", "result.contract_snapshot = BuildObjc3ParserContractSnapshot("),
    ),
    "pipeline_source": (
        ("M250-A002-PIPE-01", "result.parser_contract_snapshot = parse_result.contract_snapshot;"),
    ),
    "readiness_surface": (
        ("M250-A002-REA-01", "surface.parser_contract_deterministic = parser_snapshot.deterministic_handoff;"),
        ("M250-A002-REA-02", "surface.parser_recovery_replay_ready = parser_snapshot.parser_recovery_replay_ready;"),
    ),
    "package_json": (
        (
            "M250-A002-PKG-01",
            '"check:objc3c:m250-a002-frontend-stability-modular-split-contract": '
            '"python scripts/check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py"',
        ),
        (
            "M250-A002-PKG-02",
            '"test:tooling:m250-a002-frontend-stability-modular-split-contract": '
            '"python -m pytest tests/tooling/test_check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py -q"',
        ),
        (
            "M250-A002-PKG-03",
            '"check:objc3c:m250-a002-lane-a-readiness": '
            '"npm run check:objc3c:m250-a002-frontend-stability-modular-split-contract '
            '&& npm run test:tooling:m250-a002-frontend-stability-modular-split-contract"',
        ),
    ),
    "a001_contract_doc": (
        ("M250-A002-A001-01", "Contract ID: `objc3c-frontend-stability-long-tail-grammar-closure-freeze/m250-a001-v1`"),
    ),
    "contract_doc": (
        ("M250-A002-DOC-01", "Contract ID: `objc3c-frontend-stability-long-tail-grammar-modular-split/m250-a002-v1`"),
        ("M250-A002-DOC-02", "parse/objc3_parse_support.h"),
        ("M250-A002-DOC-03", "BuildObjc3ParserContractSnapshot"),
        ("M250-A002-DOC-04", "check:objc3c:m250-a002-lane-a-readiness"),
        (
            "M250-A002-DOC-05",
            "tmp/reports/m250/M250-A002/frontend_stability_long_tail_grammar_modular_split_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M250-A002-PKT-01", "Packet: `M250-A002`"),
        ("M250-A002-PKT-02", "Dependencies: `M250-A001`"),
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
            "tmp/reports/m250/M250-A002/frontend_stability_long_tail_grammar_modular_split_contract_summary.json"
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
