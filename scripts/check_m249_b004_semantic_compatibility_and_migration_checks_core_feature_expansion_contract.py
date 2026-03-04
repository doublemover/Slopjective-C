#!/usr/bin/env python3
"""Fail-closed contract checker for the M249-B004 semantic compatibility core feature expansion packet."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-b004-semantic-compatibility-migration-checks-core-feature-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_semantic_compatibility_and_migration_checks_core_feature_expansion_b004_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_packet.md"
)
DEFAULT_B003_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_semantic_compatibility_and_migration_checks_core_feature_implementation_b003_expectations.md"
)
DEFAULT_B003_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_packet.md"
)
DEFAULT_B003_CHECKER = (
    ROOT / "scripts" / "check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py"
)
DEFAULT_B003_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m249_b004_lane_b_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-B004/semantic_compatibility_and_migration_checks_core_feature_expansion_summary.json"
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
        "M249-B004-DOC-EXP-01",
        "# M249 Semantic Compatibility and Migration Checks Core Feature Expansion Expectations (B004)",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-core-feature-expansion/m249-b004-v1`",
    ),
    SnippetCheck("M249-B004-DOC-EXP-03", "- Dependencies: `M249-B003`"),
    SnippetCheck(
        "M249-B004-DOC-EXP-04",
        "`scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py`",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-05",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-06",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-07",
        "`check:objc3c:m249-b004-lane-b-readiness`",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-08",
        "`scripts/run_m249_b004_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-09",
        "`spec/planning/compiler/m249/m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_packet.md`",
    ),
    SnippetCheck(
        "M249-B004-DOC-EXP-10",
        "`tmp/reports/m249/M249-B004/semantic_compatibility_and_migration_checks_core_feature_expansion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B004-DOC-PKT-01",
        "# M249-B004 Semantic Compatibility and Migration Checks Core Feature Expansion Packet",
    ),
    SnippetCheck("M249-B004-DOC-PKT-02", "Packet: `M249-B004`"),
    SnippetCheck("M249-B004-DOC-PKT-03", "Dependencies: `M249-B003`"),
    SnippetCheck(
        "M249-B004-DOC-PKT-04",
        "`scripts/check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M249-B004-DOC-PKT-05",
        "`scripts/run_m249_b004_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M249-B004-DOC-PKT-06",
        "`check:objc3c:m249-b004-lane-b-readiness`",
    ),
    SnippetCheck(
        "M249-B004-DOC-PKT-07",
        "`test:objc3c:lowering-regression`",
    ),
    SnippetCheck(
        "M249-B004-DOC-PKT-08",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M249-B004-DOC-PKT-09",
        "improvements as mandatory scope inputs.",
    ),
)

B003_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B004-B003-DOC-01",
        "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-core-feature-implementation/m249-b003-v1`",
    ),
    SnippetCheck("M249-B004-B003-DOC-02", "- Dependencies: `M249-B002`"),
    SnippetCheck(
        "M249-B004-B003-DOC-03",
        "scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py",
    ),
)

B003_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-B004-B003-PKT-01", "Packet: `M249-B003`"),
    SnippetCheck("M249-B004-B003-PKT-02", "Dependencies: `M249-B002`"),
    SnippetCheck(
        "M249-B004-B003-PKT-03",
        "scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M249-B004-B003-PKT-04",
        "tmp/reports/m249/M249-B003/semantic_compatibility_and_migration_checks_core_feature_implementation_summary.json",
    ),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-B004-RUN-01", '"""Run M249-B004 lane-B readiness checks without deep npm nesting."""'),
    SnippetCheck(
        "M249-B004-RUN-02",
        "scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M249-B004-RUN-03",
        "tests/tooling/test_check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M249-B004-RUN-04",
        "scripts/check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M249-B004-RUN-05",
        "tests/tooling/test_check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py",
    ),
    SnippetCheck("M249-B004-RUN-06", "[ok] M249-B004 lane-B readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B004-ARCH-01",
        "M249 lane-B B003 semantic compatibility/migration core feature implementation anchors",
    ),
    SnippetCheck(
        "M249-B004-ARCH-02",
        "docs/contracts/m249_semantic_compatibility_and_migration_checks_core_feature_implementation_b003_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B004-SPC-01",
        "semantic compatibility and migration checks core feature implementation shall",
    ),
    SnippetCheck(
        "M249-B004-SPC-02",
        "lane-B dependency anchors (`M249-B002`) and fail closed on core-feature evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B004-META-01",
        "deterministic lane-B semantic compatibility/migration core feature metadata anchors for",
    ),
    SnippetCheck(
        "M249-B004-META-02",
        "`M249-B003` with explicit",
    ),
    SnippetCheck(
        "M249-B004-META-03",
        "`M249-B002` dependency continuity so core feature implementation drift fails closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B004-PKG-01",
        '"check:objc3c:m249-b004-semantic-compatibility-migration-checks-core-feature-expansion-contract": '
        '"python scripts/check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py"',
    ),
    SnippetCheck(
        "M249-B004-PKG-02",
        '"test:tooling:m249-b004-semantic-compatibility-migration-checks-core-feature-expansion-contract": '
        '"python -m pytest tests/tooling/test_check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py -q"',
    ),
    SnippetCheck(
        "M249-B004-PKG-03",
        '"check:objc3c:m249-b004-lane-b-readiness": "python scripts/run_m249_b004_lane_b_readiness.py"',
    ),
    SnippetCheck("M249-B004-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M249-B004-PKG-05", '"test:objc3c:lowering-regression": '),
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
    parser.add_argument("--b003-expectations-doc", type=Path, default=DEFAULT_B003_EXPECTATIONS_DOC)
    parser.add_argument("--b003-packet-doc", type=Path, default=DEFAULT_B003_PACKET_DOC)
    parser.add_argument("--b003-checker", type=Path, default=DEFAULT_B003_CHECKER)
    parser.add_argument("--b003-test", type=Path, default=DEFAULT_B003_TEST)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
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
        (args.expectations_doc, "M249-B004-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-B004-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b003_expectations_doc, "M249-B004-B003-DOC-EXISTS", B003_EXPECTATIONS_SNIPPETS),
        (args.b003_packet_doc, "M249-B004-B003-PKT-EXISTS", B003_PACKET_SNIPPETS),
        (args.readiness_runner, "M249-B004-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-B004-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-B004-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-B004-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-B004-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b003_checker, "M249-B004-DEP-B003-ARG-01"),
        (args.b003_test, "M249-B004-DEP-B003-ARG-02"),
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
