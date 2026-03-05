#!/usr/bin/env python3
"""Fail-closed checker for M235-B029 qualifier/generic semantic inference advanced diagnostics workpack shard 3."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m235-b029-qualifier-and-generic-semantic-inference-advanced-diagnostics-workpack-shard-3-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m235_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_b029_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m235"
    / "m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_packet.md"
)
DEFAULT_CONTRACT_CHECKER = (
    ROOT
    / "scripts"
    / "check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py"
)
DEFAULT_CONTRACT_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m235/M235-B029/qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B029-DOC-EXP-01",
        "# M235 Qualifier/Generic Semantic Inference Advanced Diagnostics Workpack (Shard 3) Expectations (B029)",
    ),
    SnippetCheck(
        "M235-B029-DOC-EXP-02",
        "Contract ID: `objc3c-qualifier-and-generic-semantic-inference-advanced-diagnostics-workpack-shard-3/m235-b029-v1`",
    ),
    SnippetCheck("M235-B029-DOC-EXP-03", "Dependencies: `M235-B028`"),
    SnippetCheck(
        "M235-B029-DOC-EXP-04",
        "Issue `#5809` defines canonical lane-B advanced diagnostics workpack (shard 3) scope.",
    ),
    SnippetCheck(
        "M235-B029-DOC-EXP-05",
        "Immediate predecessor issue `#5808` (`M235-B028`) is mandatory dependency continuity.",
    ),
    SnippetCheck(
        "M235-B029-DOC-EXP-06",
        "`scripts/check_m235_b028_qualifier_and_generic_semantic_inference_advanced_edge_compatibility_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M235-B029-DOC-EXP-07",
        "`scripts/check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M235-B029-DOC-EXP-08",
        "`tests/tooling/test_check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py`",
    ),
    SnippetCheck("M235-B029-DOC-EXP-09", "advanced_diagnostics_workpack_shard_3_consistent"),
    SnippetCheck("M235-B029-DOC-EXP-10", "advanced_diagnostics_workpack_shard_3_key"),
    SnippetCheck(
        "M235-B029-DOC-EXP-11",
        "`tmp/reports/m235/M235-B029/qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract_summary.json`",
    ),
    SnippetCheck(
        "M235-B029-DOC-EXP-12",
        "`python scripts/check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py --summary-out tmp/reports/m235/M235-B029/local_check_summary.json`",
    ),
)

EXPECTATIONS_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M235-B029-DOC-EXP-FORB-01", "Dependencies: `M235-B026`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B029-DOC-PKT-01",
        "# M235-B029 Qualifier/Generic Semantic Inference Advanced Diagnostics Workpack (Shard 3) Packet",
    ),
    SnippetCheck("M235-B029-DOC-PKT-02", "Packet: `M235-B029`"),
    SnippetCheck("M235-B029-DOC-PKT-03", "Issue: `#5809`"),
    SnippetCheck("M235-B029-DOC-PKT-04", "Dependencies: `M235-B028`"),
    SnippetCheck("M235-B029-DOC-PKT-05", "Theme: `advanced diagnostics workpack shard 3`"),
    SnippetCheck(
        "M235-B029-DOC-PKT-06",
        "`scripts/check_m235_b028_qualifier_and_generic_semantic_inference_advanced_edge_compatibility_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M235-B029-DOC-PKT-07",
        "`scripts/check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M235-B029-DOC-PKT-08",
        "`tests/tooling/test_check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M235-B029-DOC-PKT-09",
        "`python -m pytest tests/tooling/test_check_m235_b029_qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract.py -q`",
    ),
    SnippetCheck(
        "M235-B029-DOC-PKT-10",
        "`tmp/reports/m235/M235-B029/qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract_summary.json`",
    ),
)

PACKET_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M235-B029-DOC-PKT-FORB-01", "Dependencies: `M235-B026`"),
)

CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M235-B029-SRC-CHK-01",
        'MODE = "m235-b029-qualifier-and-generic-semantic-inference-advanced-diagnostics-workpack-shard-3-contract-v1"',
    ),
    SnippetCheck(
        "M235-B029-SRC-CHK-02",
        '"tmp/reports/m235/M235-B029/qualifier_and_generic_semantic_inference_advanced_diagnostics_workpack_shard_3_contract_summary.json"',
    ),
)

TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M235-B029-SRC-TST-01", "def test_contract_passes_on_repository_sources"),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--contract-checker", type=Path, default=DEFAULT_CONTRACT_CHECKER)
    parser.add_argument("--contract-test", type=Path, default=DEFAULT_CONTRACT_TEST)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"forbidden snippet present: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets, forbidden in (
        (
            args.expectations_doc,
            "M235-B029-DOC-EXP-EXISTS",
            EXPECTATIONS_SNIPPETS,
            EXPECTATIONS_FORBIDDEN_SNIPPETS,
        ),
        (
            args.packet_doc,
            "M235-B029-DOC-PKT-EXISTS",
            PACKET_SNIPPETS,
            PACKET_FORBIDDEN_SNIPPETS,
        ),
        (args.contract_checker, "M235-B029-SRC-CHK-EXISTS", CHECKER_SNIPPETS, ()),
        (args.contract_test, "M235-B029-SRC-TST-EXISTS", TEST_SNIPPETS, ()),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
            forbidden_snippets=forbidden,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.contract_checker, "M235-B029-DEP-B028-ARG-01"),
        (args.contract_test, "M235-B029-DEP-B028-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

