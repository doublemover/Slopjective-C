#!/usr/bin/env python3
"""Fail-closed validator for M226-B006 parser-sema edge robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-sema-edge-robustness-contract-b006-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_handoff": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_edge_robustness_b006_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_handoff": (
        ("M226-B006-SEM-01", "const bool parser_diagnostic_budget_consistent ="),
        (
            "M226-B006-SEM-02",
            "snapshot.token_count == 0u || snapshot.parser_diagnostic_count <= snapshot.token_count",
        ),
        ("M226-B006-SEM-03", "const bool parser_token_top_level_budget_consistent ="),
        (
            "M226-B006-SEM-04",
            "snapshot.token_count == 0u || snapshot.token_count >= snapshot.top_level_declaration_count",
        ),
        ("M226-B006-SEM-05", "const bool parser_subset_count_consistent ="),
        ("M226-B006-SEM-06", "snapshot.interface_category_decl_count <= snapshot.interface_decl_count"),
        (
            "M226-B006-SEM-07",
            "snapshot.implementation_category_decl_count <= snapshot.implementation_decl_count",
        ),
        ("M226-B006-SEM-08", "snapshot.function_prototype_count <= snapshot.function_decl_count"),
        ("M226-B006-SEM-09", "snapshot.function_pure_count <= snapshot.function_decl_count"),
        ("M226-B006-SEM-10", "parser_diagnostic_budget_consistent &&"),
        ("M226-B006-SEM-11", "parser_token_top_level_budget_consistent &&"),
        ("M226-B006-SEM-12", "parser_subset_count_consistent &&"),
    ),
    "contract_doc": (
        (
            "M226-B006-DOC-01",
            "Contract ID: `objc3c-parser-sema-edge-robustness-contract/m226-b006-v1`",
        ),
        (
            "M226-B006-DOC-02",
            "`parser_diagnostic_count <= token_count` when `token_count > 0`",
        ),
        (
            "M226-B006-DOC-03",
            "`top_level_declaration_count <= token_count` when `token_count > 0`",
        ),
        (
            "M226-B006-DOC-04",
            "`interface_category_decl_count <= interface_decl_count`",
        ),
        (
            "M226-B006-DOC-05",
            "`implementation_category_decl_count <= implementation_decl_count`",
        ),
        (
            "M226-B006-DOC-06",
            "`function_prototype_count <= function_decl_count`",
        ),
        (
            "M226-B006-DOC-07",
            "`function_pure_count <= function_decl_count`",
        ),
        (
            "M226-B006-DOC-08",
            "`python scripts/check_m226_b006_parser_sema_edge_robustness_contract.py`",
        ),
        (
            "M226-B006-DOC-09",
            "`python -m pytest tests/tooling/test_check_m226_b006_parser_sema_edge_robustness_contract.py -q`",
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
        default=Path("tmp/reports/m226/M226-B006/parser_sema_edge_robustness_summary.json"),
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
