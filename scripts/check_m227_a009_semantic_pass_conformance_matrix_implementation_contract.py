#!/usr/bin/env python3
"""Fail-closed validator for M227-A009 semantic pass conformance matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-semantic-pass-conformance-matrix-implementation-contract-a009-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "sema_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "handoff_scaffold": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_conformance_matrix_implementation_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a009_semantic_pass_conformance_matrix_implementation_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_contract": (
        ("M227-A009-CNT-01", "Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;"),
        ("M227-A009-CNT-02", "Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;"),
        ("M227-A009-CNT-03", "bool deterministic_parser_sema_conformance_matrix = false;"),
        ("M227-A009-CNT-04", "surface.parser_sema_conformance_matrix.parser_recovery_replay_ready &&"),
        ("M227-A009-CNT-05", "surface.parser_sema_conformance_corpus.required_case_count == 5u &&"),
        ("M227-A009-CNT-06", "surface.parser_sema_conformance_corpus.failed_case_count == 0u &&"),
    ),
    "sema_manager": (
        ("M227-A009-SRC-01", "result.parser_sema_conformance_matrix = handoff.parser_sema_conformance_matrix;"),
        ("M227-A009-SRC-02", "result.deterministic_parser_sema_conformance_matrix ="),
        ("M227-A009-SRC-03", "result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;"),
        ("M227-A009-SRC-04", "result.deterministic_parser_sema_conformance_corpus ="),
        ("M227-A009-SRC-05", "result.parser_sema_conformance_corpus.required_case_count > 0u &&"),
        ("M227-A009-SRC-06", "result.parser_sema_conformance_corpus.failed_case_count == 0u;"),
        ("M227-A009-SRC-07", "result.parity_surface.parser_sema_conformance_matrix ="),
        ("M227-A009-SRC-08", "result.parity_surface.deterministic_parser_sema_conformance_matrix ="),
    ),
    "handoff_scaffold": (
        ("M227-A009-HOF-01", "BuildObjc3ParserSemaConformanceMatrix("),
        ("M227-A009-HOF-02", "BuildObjc3ParserSemaConformanceCorpus(scaffold.parser_sema_conformance_matrix);"),
        ("M227-A009-HOF-03", "corpus.required_case_count ="),
        ("M227-A009-HOF-04", "corpus.passed_case_count ="),
        ("M227-A009-HOF-05", "corpus.failed_case_count ="),
        ("M227-A009-HOF-06", "guardrails.conformance_matrix_builder_budget_guarded ="),
    ),
    "architecture_doc": (
        ("M227-A009-ARC-01", "M227 lane-A A009 conformance matrix implementation anchors explicit semantic-pass"),
        ("M227-A009-ARC-02", "parser/sema conformance matrix gates (`parser_sema_conformance_matrix`,"),
    ),
    "lowering_spec": (
        ("M227-A009-SPC-01", "semantic-pass conformance matrix implementation governance shall preserve explicit"),
        ("M227-A009-SPC-02", "lane-A dependency anchor (`M227-A009`) and fail closed"),
    ),
    "metadata_spec": (
        ("M227-A009-MTD-01", "deterministic lane-A semantic-pass conformance matrix metadata anchors for `M227-A009`"),
        ("M227-A009-MTD-02", "parser/sema conformance-matrix evidence and corpus replay continuity"),
    ),
    "contract_doc": (
        ("M227-A009-DOC-01", "Contract ID: `objc3c-semantic-pass-conformance-matrix-implementation/m227-a009-v1`"),
        ("M227-A009-DOC-02", "parser_sema_conformance_matrix"),
        ("M227-A009-DOC-03", "parser_sema_conformance_corpus"),
        ("M227-A009-DOC-04", "check:objc3c:m227-a009-lane-a-readiness"),
        ("M227-A009-DOC-05", "tmp/reports/m227/M227-A009/semantic_pass_conformance_matrix_implementation_summary.json"),
    ),
    "packet_doc": (
        ("M227-A009-PKT-01", "Packet: `M227-A009`"),
        ("M227-A009-PKT-02", "Dependencies: `M227-A008`"),
        (
            "M227-A009-PKT-03",
            "scripts/check_m227_a009_semantic_pass_conformance_matrix_implementation_contract.py",
        ),
        (
            "M227-A009-PKT-04",
            "tests/tooling/test_check_m227_a009_semantic_pass_conformance_matrix_implementation_contract.py",
        ),
    ),
    "package_json": (
        (
            "M227-A009-CFG-01",
            '"check:objc3c:m227-a009-semantic-pass-conformance-matrix-implementation-contract"',
        ),
        (
            "M227-A009-CFG-02",
            '"test:tooling:m227-a009-semantic-pass-conformance-matrix-implementation-contract"',
        ),
        ("M227-A009-CFG-03", '"check:objc3c:m227-a009-lane-a-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_manager": (
        ("M227-A009-FORB-01", "parser_recovery_replay_contract_satisfied = true;"),
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
            "tmp/reports/m227/M227-A009/semantic_pass_conformance_matrix_implementation_summary.json"
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
