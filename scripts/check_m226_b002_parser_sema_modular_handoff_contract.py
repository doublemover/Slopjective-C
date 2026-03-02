#!/usr/bin/env python3
"""Fail-closed modular handoff contract checker for M226-B002."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-b002-parser-sema-modular-handoff-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "parser_contract_header": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "sema_handoff_scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_parser_sema_handoff_scaffold.h",
    "sema_pass_manager_contract_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "m226_contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_modular_handoff_expectations.md",
}

ARTIFACT_ORDER: tuple[str, ...] = tuple(ARTIFACTS.keys())
ARTIFACT_RANK = {name: index for index, name in enumerate(ARTIFACT_ORDER)}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_contract_header": (
        ("M226-B002-PAR-01", "struct Objc3ParsedProgram {"),
        ("M226-B002-PAR-02", "inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot("),
    ),
    "sema_handoff_scaffold_header": (
        ("M226-B002-SCAF-01", "struct Objc3ParserSemaHandoffScaffold {"),
        ("M226-B002-SCAF-02", "Objc3ParserContractSnapshot parser_contract_snapshot;"),
        ("M226-B002-SCAF-03", "bool parser_contract_snapshot_matches_program = false;"),
        ("M226-B002-SCAF-04", "inline Objc3ParserSemaHandoffScaffold BuildObjc3ParserSemaHandoffScaffold("),
        ("M226-B002-SCAF-05", "ResolveObjc3ParserContractSnapshotForSemaHandoff(input);"),
        ("M226-B002-SCAF-06", "IsObjc3ParserContractSnapshotConsistentWithProgram("),
    ),
    "sema_pass_manager_contract_header": (
        ("M226-B002-SEM-01", "const Objc3ParserContractSnapshot *parser_contract_snapshot = nullptr;"),
        ("M226-B002-SEM-02", "Objc3ParserContractSnapshot parser_contract_snapshot;"),
        ("M226-B002-SEM-03", "bool deterministic_parser_sema_handoff = false;"),
    ),
    "sema_pass_manager_source": (
        ("M226-B002-SEM-04", '#include "sema/objc3_parser_sema_handoff_scaffold.h"'),
        ("M226-B002-SEM-05", "const Objc3ParserSemaHandoffScaffold handoff = BuildObjc3ParserSemaHandoffScaffold(input);"),
        ("M226-B002-SEM-06", "result.parser_contract_snapshot = handoff.parser_contract_snapshot;"),
        ("M226-B002-SEM-07", "result.deterministic_parser_sema_handoff = handoff.deterministic;"),
        ("M226-B002-SEM-08", "if (!handoff.deterministic) {"),
        ("M226-B002-SEM-09", "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {"),
    ),
    "m226_contract_doc": (
        ("M226-B002-DOC-01", "# Parser-to-Sema Modular Handoff Scaffolding Expectations (M226-B002)"),
        ("M226-B002-DOC-02", "Contract ID: `objc3c-parser-sema-modular-handoff-contract/m226-b002-v1`"),
        ("M226-B002-DOC-03", "`M226-B002-INV-01`"),
        ("M226-B002-DOC-04", "`M226-B002-INV-05`"),
        ("M226-B002-DOC-05", "`python scripts/check_m226_b002_parser_sema_modular_handoff_contract.py`"),
        (
            "M226-B002-DOC-06",
            "`python -m pytest tests/tooling/test_check_m226_b002_parser_sema_modular_handoff_contract.py -q`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_source": (
        ("M226-B002-FORB-01", "BuildObjc3ParserContractSnapshot(*input.program"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists():
        raise ValueError(f"{artifact} file does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{artifact} path is not a file: {display_path(path)}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{artifact} file is not valid UTF-8: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {artifact} file {display_path(path)}: {exc}") from exc


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226_b002_parser_sema_modular_handoff_contract_summary.json"),
    )
    return parser.parse_args(argv)


def finding_sort_key(finding: Finding) -> tuple[int, str, str]:
    if finding.artifact not in ARTIFACT_RANK:
        raise ValueError(f"unknown artifact in findings: {finding.artifact}")
    return (ARTIFACT_RANK[finding.artifact], finding.check_id, finding.detail)


def collect_required_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"expected snippet missing: {snippet}",
                )
            )
    return findings


def collect_forbidden_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if snippet in text:
            findings.append(
                Finding(
                    artifact=artifact,
                    check_id=check_id,
                    detail=f"forbidden snippet present: {snippet}",
                )
            )
    return findings


def total_checks() -> int:
    return sum(len(v) for v in REQUIRED_SNIPPETS.values()) + sum(len(v) for v in FORBIDDEN_SNIPPETS.values())


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        findings: list[Finding] = []
        for artifact, path in ARTIFACTS.items():
            text = load_text(path, artifact=artifact)
            findings.extend(collect_required_findings(artifact=artifact, text=text))
            findings.extend(collect_forbidden_findings(artifact=artifact, text=text))
        findings = sorted(findings, key=finding_sort_key)
    except ValueError as exc:
        print(f"m226-b002-parser-sema-modular-handoff-contract: error: {exc}", file=sys.stderr)
        return 2

    checks = total_checks()
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks,
        "checks_passed": checks - len(findings),
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

    if findings:
        print(
            "m226-b002-parser-sema-modular-handoff-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("m226-b002-parser-sema-modular-handoff-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
