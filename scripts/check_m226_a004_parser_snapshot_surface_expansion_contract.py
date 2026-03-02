#!/usr/bin/env python3
"""Fail-closed validator for M226-A004 parser snapshot surface expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-snapshot-surface-expansion-contract-a004-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "frontend_artifacts": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_snapshot_surface_expansion_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_contract": (
        ("M226-A004-PAR-01", "std::size_t token_count = 0;"),
        ("M226-A004-PAR-02", "std::size_t top_level_declaration_count = 0;"),
        ("M226-A004-PAR-03", "const std::size_t token_count) {"),
        ("M226-A004-PAR-04", "snapshot.token_count = token_count;"),
        ("M226-A004-PAR-05", "snapshot.top_level_declaration_count = snapshot.global_decl_count +"),
    ),
    "parser_source": (
        ("M226-A004-PARSE-01", "BuildObjc3ParserContractSnapshot(result.program, result.diagnostics.size(), tokens.size())"),
    ),
    "frontend_artifacts": (
        ("M226-A004-ART-01", "pipeline_result.parser_contract_snapshot.token_count"),
        ("M226-A004-ART-02", "pipeline_result.parser_contract_snapshot.top_level_declaration_count"),
        ("M226-A004-ART-03", "pipeline_result.parser_contract_snapshot.top_level_declaration_count"),
    ),
    "contract_doc": (
        (
            "M226-A004-DOC-01",
            "Contract ID: `objc3c-parser-snapshot-surface-expansion-contract/m226-a004-v1`",
        ),
        ("M226-A004-DOC-02", "`token_count`"),
        ("M226-A004-DOC-03", "`top_level_declaration_count`"),
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
        default=Path("tmp/reports/m226/M226-A004/parser_snapshot_surface_expansion_summary.json"),
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
