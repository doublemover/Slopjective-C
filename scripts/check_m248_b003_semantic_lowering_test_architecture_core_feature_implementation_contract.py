#!/usr/bin/env python3
"""Fail-closed contract checker for the M248-B003 semantic/lowering core feature implementation packet."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_core_feature_implementation_b003_expectations.md"
)
DEFAULT_B002_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_modular_split_scaffolding_b002_expectations.md"
)
DEFAULT_B002_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_packet.md"
)
DEFAULT_B002_CHECKER = (
    ROOT / "scripts" / "check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py"
)
DEFAULT_B002_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-B003/semantic_lowering_test_architecture_core_feature_implementation_contract_summary.json"
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
        "M248-B003-DOC-EXP-01",
        "# M248 Semantic/Lowering Test Architecture Core Feature Implementation Expectations (B003)",
    ),
    SnippetCheck(
        "M248-B003-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-core-feature-implementation/m248-b003-v1`",
    ),
    SnippetCheck("M248-B003-DOC-EXP-03", "- Dependencies: `M248-B002`"),
    SnippetCheck(
        "M248-B003-DOC-EXP-04",
        "`scripts/check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py`",
    ),
    SnippetCheck(
        "M248-B003-DOC-EXP-05",
        "code/spec anchors and milestone",
    ),
    SnippetCheck(
        "M248-B003-DOC-EXP-06",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-B003-DOC-EXP-07",
        "`check:objc3c:m248-b003-lane-b-readiness`",
    ),
    SnippetCheck(
        "M248-B003-DOC-EXP-08",
        "`tmp/reports/m248/M248-B003/semantic_lowering_test_architecture_core_feature_implementation_contract_summary.json`",
    ),
)

B002_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B003-B002-DOC-01",
        "# M248 Semantic/Lowering Test Architecture Modular Split Scaffolding Expectations (B002)",
    ),
    SnippetCheck(
        "M248-B003-B002-DOC-02",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-modular-split-scaffolding/m248-b002-v1`",
    ),
    SnippetCheck("M248-B003-B002-DOC-03", "Dependencies: `M248-B001`"),
)

B002_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B003-B002-PKT-01", "Packet: `M248-B002`"),
    SnippetCheck("M248-B003-B002-PKT-02", "Dependencies: `M248-B001`"),
    SnippetCheck(
        "M248-B003-B002-PKT-03",
        "`tmp/reports/m248/M248-B002/semantic_lowering_test_architecture_modular_split_scaffolding_contract_summary.json`",
    ),
    SnippetCheck(
        "M248-B003-B002-PKT-04",
        "scripts/check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B003-PKG-01",
        '"check:objc3c:m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract": '
        '"python scripts/check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py"',
    ),
    SnippetCheck(
        "M248-B003-PKG-02",
        '"test:tooling:m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract": '
        '"python -m pytest tests/tooling/test_check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py -q"',
    ),
    SnippetCheck(
        "M248-B003-PKG-03",
        '"check:objc3c:m248-b003-lane-b-readiness": '
        '"npm run check:objc3c:m248-b002-lane-b-readiness '
        '&& npm run check:objc3c:m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract '
        '&& npm run test:tooling:m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract"',
    ),
    SnippetCheck("M248-B003-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M248-B003-PKG-05", '"test:objc3c:lowering-regression": '),
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
    parser.add_argument("--b002-expectations-doc", type=Path, default=DEFAULT_B002_EXPECTATIONS_DOC)
    parser.add_argument("--b002-packet-doc", type=Path, default=DEFAULT_B002_PACKET_DOC)
    parser.add_argument("--b002-checker", type=Path, default=DEFAULT_B002_CHECKER)
    parser.add_argument("--b002-test", type=Path, default=DEFAULT_B002_TEST)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M248-B003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.b002_expectations_doc, "M248-B003-B002-DOC-EXISTS", B002_EXPECTATIONS_SNIPPETS),
        (args.b002_packet_doc, "M248-B003-B002-PKT-EXISTS", B002_PACKET_SNIPPETS),
        (args.package_json, "M248-B003-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b002_checker, "M248-B003-DEP-B002-ARG-01"),
        (args.b002_test, "M248-B003-DEP-B002-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
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
