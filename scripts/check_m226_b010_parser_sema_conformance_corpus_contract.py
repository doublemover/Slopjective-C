#!/usr/bin/env python3
"""Fail-closed validator for M226-B010 parser/sema conformance corpus."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-sema-conformance-corpus-contract-b010-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_conformance_corpus_b010_expectations.md",
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "integration_test": ROOT / "tests" / "tooling" / "test_objc3c_parser_contract_sema_integration.py",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        ("M226-B010-DOC-01", "Contract ID: `objc3c-parser-sema-conformance-corpus/m226-b010-v1`"),
        ("M226-B010-DOC-02", "`python scripts/check_m226_b010_parser_sema_conformance_corpus_contract.py`"),
        (
            "M226-B010-DOC-03",
            "`python -m pytest tests/tooling/test_check_m226_b010_parser_sema_conformance_corpus_contract.py -q`",
        ),
        (
            "M226-B010-DOC-04",
            "`python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`",
        ),
    ),
    "sema_handoff": (
        ("M226-B010-SEM-01", "inline Objc3ParserSemaConformanceCorpus BuildObjc3ParserSemaConformanceCorpus("),
        ("M226-B010-SEM-02", "corpus.required_case_count ="),
        ("M226-B010-SEM-03", "corpus.passed_case_count ="),
        ("M226-B010-SEM-04", "corpus.failed_case_count ="),
        ("M226-B010-SEM-05", "corpus.deterministic = matrix.deterministic && corpus.required_case_count == 5u"),
        ("M226-B010-SEM-06", "scaffold.parser_sema_conformance_corpus ="),
        ("M226-B010-SEM-07", "scaffold.parser_sema_conformance_corpus.deterministic"),
    ),
    "sema_contract": (
        ("M226-B010-CON-01", "struct Objc3ParserSemaConformanceCorpus {"),
        ("M226-B010-CON-02", "Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;"),
        ("M226-B010-CON-03", "bool deterministic_parser_sema_conformance_corpus = false;"),
        ("M226-B010-CON-04", "surface.deterministic_parser_sema_conformance_corpus &&"),
        ("M226-B010-CON-05", "surface.parser_sema_conformance_corpus.required_case_count == 5u &&"),
    ),
    "sema_pass_manager": (
        ("M226-B010-PM-01", "result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;"),
        ("M226-B010-PM-02", "result.deterministic_parser_sema_conformance_corpus ="),
        ("M226-B010-PM-03", "if (!result.deterministic_parser_sema_conformance_corpus) {"),
        ("M226-B010-PM-04", "result.parity_surface.parser_sema_conformance_corpus ="),
        ("M226-B010-PM-05", "result.parity_surface.deterministic_parser_sema_conformance_corpus ="),
    ),
    "integration_test": (
        ("M226-B010-TST-01", 'assert "struct Objc3ParserSemaConformanceCorpus {" in pass_manager_contract'),
        ("M226-B010-TST-02", 'assert "BuildObjc3ParserSemaConformanceCorpus(" in sema_handoff'),
        ("M226-B010-TST-03", 'assert "result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;" in sema_pass_manager'),
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
        default=Path("tmp/reports/m226/M226-B010/parser_sema_conformance_corpus_summary.json"),
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
