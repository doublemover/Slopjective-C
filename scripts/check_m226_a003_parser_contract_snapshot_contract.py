#!/usr/bin/env python3
"""Fail-closed validator for M226-A003 parser contract snapshot integration."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-contract-snapshot-contract-a003-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "parser_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "ast_builder_contract_h": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.h",
    "ast_builder_contract_cpp": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.cpp",
    "frontend_types_h": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "frontend_pipeline_cpp": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "frontend_artifacts_cpp": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_contract_snapshot_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_contract": (
        ("M226-A003-PAR-01", "struct Objc3ParserContractSnapshot {"),
        ("M226-A003-PAR-02", "std::size_t global_decl_count = 0;"),
        ("M226-A003-PAR-03", "std::size_t parser_diagnostic_count = 0;"),
        ("M226-A003-PAR-04", "bool parser_recovery_replay_ready = true;"),
        ("M226-A003-PAR-05", "inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot("),
    ),
    "parser_header": (
        ("M226-A003-PAR-06", "Objc3ParserContractSnapshot contract_snapshot;"),
    ),
    "parser_source": (
        ("M226-A003-PAR-07", "result.contract_snapshot = BuildObjc3ParserContractSnapshot(result.program, result.diagnostics.size());"),
    ),
    "ast_builder_contract_h": (
        ("M226-A003-AST-01", "Objc3ParserContractSnapshot contract_snapshot;"),
    ),
    "ast_builder_contract_cpp": (
        ("M226-A003-AST-02", "builder_result.contract_snapshot = parse_result.contract_snapshot;"),
    ),
    "frontend_types_h": (
        ("M226-A003-PIPE-01", "Objc3ParserContractSnapshot parser_contract_snapshot;"),
    ),
    "frontend_pipeline_cpp": (
        ("M226-A003-PIPE-02", "result.parser_contract_snapshot = parse_result.contract_snapshot;"),
    ),
    "frontend_artifacts_cpp": (
        ("M226-A003-ART-01", "pipeline_result.parser_contract_snapshot.deterministic_handoff"),
        ("M226-A003-ART-02", "pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready"),
        ("M226-A003-ART-03", "pipeline_result.parser_contract_snapshot.function_decl_count"),
    ),
    "contract_doc": (
        ("M226-A003-DOC-01", "Contract ID: `objc3c-parser-contract-snapshot-contract/m226-a003-v1`"),
        ("M226-A003-DOC-02", "Objc3ParserContractSnapshot"),
        ("M226-A003-DOC-03", "frontend.pipeline.stages.parser"),
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
        default=Path("tmp/reports/m226/M226-A003/parser_contract_snapshot_contract_summary.json"),
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
