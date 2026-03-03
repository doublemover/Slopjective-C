#!/usr/bin/env python3
"""Fail-closed contract checker for M248-B008 semantic/lowering recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_recovery_and_determinism_hardening_b008_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_B007_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_diagnostics_hardening_b007_expectations.md"
)
DEFAULT_B007_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_packet.md"
)
DEFAULT_B007_CHECKER = (
    ROOT / "scripts" / "check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py"
)
DEFAULT_B007_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-B008/semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract_summary.json"
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
        "M248-B008-DOC-EXP-01",
        "# M248 Semantic/Lowering Test Architecture Recovery and Determinism Hardening Expectations (B008)",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-recovery-and-determinism-hardening/m248-b008-v1`",
    ),
    SnippetCheck("M248-B008-DOC-EXP-03", "- Dependencies: `M248-B007`"),
    SnippetCheck(
        "M248-B008-DOC-EXP-04",
        "Issue `#6808` defines canonical lane-B recovery and determinism hardening scope.",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-05",
        "`scripts/check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-06",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-07",
        "as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-08",
        "`check:objc3c:m248-b008-lane-b-readiness`",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-09",
        "`spec/planning/compiler/m248/m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_packet.md`",
    ),
    SnippetCheck(
        "M248-B008-DOC-EXP-10",
        "`tmp/reports/m248/M248-B008/semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B008-DOC-PKT-01",
        "# M248-B008 Semantic/Lowering Test Architecture Recovery and Determinism Hardening Packet",
    ),
    SnippetCheck("M248-B008-DOC-PKT-02", "Packet: `M248-B008`"),
    SnippetCheck("M248-B008-DOC-PKT-03", "Issue: `#6808`"),
    SnippetCheck("M248-B008-DOC-PKT-04", "Dependencies: `M248-B007`"),
    SnippetCheck(
        "M248-B008-DOC-PKT-05",
        "`scripts/check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`",
    ),
    SnippetCheck(
        "M248-B008-DOC-PKT-06",
        "`check:objc3c:m248-b008-lane-b-readiness`",
    ),
    SnippetCheck(
        "M248-B008-DOC-PKT-07",
        "`test:objc3c:lowering-regression`",
    ),
    SnippetCheck(
        "M248-B008-DOC-PKT-08",
        "improvements as mandatory scope inputs.",
    ),
)

B007_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B008-B007-DOC-01",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-diagnostics-hardening/m248-b007-v1`",
    ),
    SnippetCheck("M248-B008-B007-DOC-02", "- Dependencies: `M248-B006`"),
    SnippetCheck(
        "M248-B008-B007-DOC-03",
        "scripts/check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py",
    ),
)

B007_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B008-B007-PKT-01", "Packet: `M248-B007`"),
    SnippetCheck("M248-B008-B007-PKT-02", "Dependencies: `M248-B006`"),
    SnippetCheck(
        "M248-B008-B007-PKT-03",
        "`scripts/check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M248-B008-B007-PKT-04",
        "`check:objc3c:m248-b007-lane-b-readiness`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B008-ARCH-01",
        "M248 lane-B B001 semantic/lowering test architecture anchors explicit lane-B",
    ),
    SnippetCheck(
        "M248-B008-ARCH-02",
        "M248 lane-B B002 semantic/lowering modular split/scaffolding anchors explicit",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B008-SPC-01",
        "semantic/lowering test architecture governance shall preserve explicit lane-B",
    ),
    SnippetCheck(
        "M248-B008-SPC-02",
        "dependency anchors (`M248-B001`) and fail closed on modular split evidence",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B008-META-01",
        "deterministic lane-B semantic/lowering metadata anchors for `M248-B001`",
    ),
    SnippetCheck(
        "M248-B008-META-02",
        "`M248-B002` with explicit `M248-B001` dependency continuity so semantic",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B008-PKG-01",
        '"check:objc3c:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract": '
        '"python scripts/check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py"',
    ),
    SnippetCheck(
        "M248-B008-PKG-02",
        '"test:tooling:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract": '
        '"python -m pytest tests/tooling/test_check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py -q"',
    ),
    SnippetCheck(
        "M248-B008-PKG-03",
        '"check:objc3c:m248-b008-lane-b-readiness": '
        '"npm run check:objc3c:m248-b007-lane-b-readiness '
        "&& npm run check:objc3c:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract "
        '&& npm run test:tooling:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract"',
    ),
    SnippetCheck("M248-B008-PKG-04", '"check:objc3c:m248-b007-lane-b-readiness": '),
    SnippetCheck("M248-B008-PKG-05", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M248-B008-PKG-06", '"test:objc3c:lowering-regression": '),
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
    parser.add_argument("--b007-expectations-doc", type=Path, default=DEFAULT_B007_EXPECTATIONS_DOC)
    parser.add_argument("--b007-packet-doc", type=Path, default=DEFAULT_B007_PACKET_DOC)
    parser.add_argument("--b007-checker", type=Path, default=DEFAULT_B007_CHECKER)
    parser.add_argument("--b007-test", type=Path, default=DEFAULT_B007_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
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
        (args.expectations_doc, "M248-B008-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-B008-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b007_expectations_doc, "M248-B008-B007-DOC-EXISTS", B007_EXPECTATIONS_SNIPPETS),
        (args.b007_packet_doc, "M248-B008-B007-PKT-EXISTS", B007_PACKET_SNIPPETS),
        (args.architecture_doc, "M248-B008-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M248-B008-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M248-B008-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M248-B008-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b007_checker, "M248-B008-DEP-B007-ARG-01"),
        (args.b007_test, "M248-B008-DEP-B007-ARG-02"),
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

    failures = sorted(failures, key=lambda failure: (failure.artifact, failure.check_id, failure.detail))
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
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
