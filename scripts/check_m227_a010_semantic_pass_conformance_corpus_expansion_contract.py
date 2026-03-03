#!/usr/bin/env python3
"""Fail-closed validator for M227-A010 semantic pass conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-semantic-pass-conformance-corpus-expansion-contract-a010-v1"

ARTIFACTS: dict[str, Path] = {
    "a009_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_conformance_matrix_implementation_expectations.md",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "handoff_scaffold": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "sema_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_conformance_corpus_expansion_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a010_semantic_pass_conformance_corpus_expansion_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a009_contract_doc": (
        (
            "M227-A010-DEP-01",
            "Contract ID: `objc3c-semantic-pass-conformance-matrix-implementation/m227-a009-v1`",
        ),
    ),
    "sema_contract": (
        ("M227-A010-CNT-01", "struct Objc3ParserSemaConformanceCorpus {"),
        ("M227-A010-CNT-02", "std::size_t required_case_count = 0;"),
        ("M227-A010-CNT-03", "std::size_t passed_case_count = 0;"),
        ("M227-A010-CNT-04", "std::size_t failed_case_count = 0;"),
        ("M227-A010-CNT-05", "bool has_recovery_replay_case = false;"),
        ("M227-A010-CNT-06", "bool recovery_replay_case_passed = false;"),
        ("M227-A010-CNT-07", "bool deterministic = false;"),
    ),
    "handoff_scaffold": (
        ("M227-A010-HOF-01", "corpus.required_case_count ="),
        ("M227-A010-HOF-02", "corpus.passed_case_count ="),
        ("M227-A010-HOF-03", "corpus.failed_case_count ="),
        ("M227-A010-HOF-04", "corpus.required_case_count == 5u &&"),
        ("M227-A010-HOF-05", "corpus.passed_case_count == corpus.required_case_count &&"),
        ("M227-A010-HOF-06", "corpus.failed_case_count == 0u;"),
    ),
    "sema_manager": (
        ("M227-A010-SRC-01", "result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;"),
        ("M227-A010-SRC-02", "result.deterministic_parser_sema_conformance_corpus ="),
        ("M227-A010-SRC-03", "result.parser_sema_conformance_corpus.required_case_count > 0u &&"),
        ("M227-A010-SRC-04", "result.parser_sema_conformance_corpus.passed_case_count =="),
        ("M227-A010-SRC-05", "result.parser_sema_conformance_corpus.failed_case_count == 0u;"),
        ("M227-A010-SRC-06", "result.parity_surface.parser_sema_conformance_corpus ="),
        ("M227-A010-SRC-07", "result.parity_surface.deterministic_parser_sema_conformance_corpus ="),
    ),
    "architecture_doc": (
        (
            "M227-A010-ARC-01",
            "M227 lane-A A010 conformance corpus expansion anchors semantic-pass",
        ),
        (
            "M227-A010-ARC-02",
            "`parser_sema_conformance_corpus` and",
        ),
    ),
    "lowering_spec": (
        (
            "M227-A010-SPC-01",
            "semantic-pass conformance corpus expansion governance shall preserve explicit",
        ),
        ("M227-A010-SPC-02", "lane-A dependency anchor (`M227-A010`) and fail closed"),
    ),
    "metadata_spec": (
        (
            "M227-A010-MTD-01",
            "deterministic lane-A semantic-pass conformance corpus metadata anchors for `M227-A010`",
        ),
        (
            "M227-A010-MTD-02",
            "parser/sema conformance-corpus replay evidence and fail-closed continuity",
        ),
    ),
    "contract_doc": (
        ("M227-A010-DOC-01", "Contract ID: `objc3c-semantic-pass-conformance-corpus-expansion/m227-a010-v1`"),
        ("M227-A010-DOC-02", "Dependencies: `M227-A009`"),
        (
            "M227-A010-DOC-03",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M227-A010-DOC-04", "required_case_count"),
        ("M227-A010-DOC-05", "passed_case_count"),
        ("M227-A010-DOC-06", "failed_case_count"),
        ("M227-A010-DOC-07", "check:objc3c:m227-a010-lane-a-readiness"),
    ),
    "packet_doc": (
        ("M227-A010-PKT-01", "Packet: `M227-A010`"),
        ("M227-A010-PKT-02", "Dependencies: `M227-A009`"),
        (
            "M227-A010-PKT-03",
            "scripts/check_m227_a010_semantic_pass_conformance_corpus_expansion_contract.py",
        ),
        (
            "M227-A010-PKT-04",
            "tests/tooling/test_check_m227_a010_semantic_pass_conformance_corpus_expansion_contract.py",
        ),
    ),
    "package_json": (
        ("M227-A010-CFG-01", '"check:objc3c:m227-a010-semantic-pass-conformance-corpus-expansion-contract"'),
        ("M227-A010-CFG-02", '"test:tooling:m227-a010-semantic-pass-conformance-corpus-expansion-contract"'),
        ("M227-A010-CFG-03", '"check:objc3c:m227-a010-lane-a-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_manager": (
        ("M227-A010-FORB-01", "result.parser_sema_conformance_corpus.required_case_count == 0u"),
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
            "tmp/reports/m227/M227-A010/semantic_pass_conformance_corpus_expansion_summary.json"
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
