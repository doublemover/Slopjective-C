#!/usr/bin/env python3
"""Fail-closed checker for M234-D003 runtime property metadata integration core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-d003-runtime-property-metadata-integration-core-feature-implementation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_runtime_property_metadata_integration_core_feature_implementation_d003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_d003_runtime_property_metadata_integration_core_feature_implementation_packet.md"
)
DEFAULT_D002_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_runtime_property_metadata_integration_modular_split_scaffolding_d002_expectations.md"
)
DEFAULT_D002_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_packet.md"
)
DEFAULT_D002_CHECKER = (
    ROOT / "scripts" / "check_m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_contract.py"
)
DEFAULT_D002_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-D003/runtime_property_metadata_integration_core_feature_implementation_contract_summary.json"
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
        "M234-D003-DOC-EXP-01",
        "# M234 Runtime Property Metadata Integration Core Feature Implementation Expectations (D003)",
    ),
    SnippetCheck(
        "M234-D003-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-property-metadata-integration-core-feature-implementation/m234-d003-v1`",
    ),
    SnippetCheck("M234-D003-DOC-EXP-03", "Dependencies: `M234-D002`"),
    SnippetCheck(
        "M234-D003-DOC-EXP-04",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M234-D003-DOC-EXP-05",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-D003-DOC-EXP-06",
        "docs/contracts/m234_runtime_property_metadata_integration_modular_split_scaffolding_d002_expectations.md",
    ),
    SnippetCheck(
        "M234-D003-DOC-EXP-07",
        "scripts/check_m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M234-D003-DOC-EXP-08",
        "`check:objc3c:m234-d003-runtime-property-metadata-integration-core-feature-implementation-contract`",
    ),
    SnippetCheck(
        "M234-D003-DOC-EXP-09",
        "`tmp/reports/m234/M234-D003/runtime_property_metadata_integration_core_feature_implementation_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D003-DOC-PKT-01",
        "# M234-D003 Runtime Property Metadata Integration Core Feature Implementation Packet",
    ),
    SnippetCheck("M234-D003-DOC-PKT-02", "Packet: `M234-D003`"),
    SnippetCheck("M234-D003-DOC-PKT-03", "Dependencies: `M234-D002`"),
    SnippetCheck(
        "M234-D003-DOC-PKT-04",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M234-D003-DOC-PKT-05",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-D003-DOC-PKT-06",
        "scripts/check_m234_d003_runtime_property_metadata_integration_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M234-D003-DOC-PKT-07",
        "tests/tooling/test_check_m234_d003_runtime_property_metadata_integration_core_feature_implementation_contract.py",
    ),
    SnippetCheck("M234-D003-DOC-PKT-08", "`npm run check:objc3c:m234-d003-lane-d-readiness`"),
)

D002_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D003-D002-DOC-01",
        "# M234 Runtime Property Metadata Integration Modular Split Scaffolding Expectations (D002)",
    ),
    SnippetCheck(
        "M234-D003-D002-DOC-02",
        "Contract ID: `objc3c-runtime-property-metadata-integration-modular-split-scaffolding/m234-d002-v1`",
    ),
)

D002_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-D003-D002-PKT-01", "Packet: `M234-D002`"),
    SnippetCheck("M234-D003-D002-PKT-02", "Dependencies: `M234-D001`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D003-ARCH-01",
        "M234 lane-D D003 runtime property metadata integration core feature implementation anchors",
    ),
    SnippetCheck(
        "M234-D003-ARCH-02",
        "docs/contracts/m234_runtime_property_metadata_integration_core_feature_implementation_d003_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D003-SPC-01",
        "runtime property metadata integration core feature implementation shall",
    ),
    SnippetCheck(
        "M234-D003-SPC-02",
        "preserve explicit lane-D dependency anchors (`M234-D002`) and fail closed on",
    ),
    SnippetCheck(
        "M234-D003-SPC-03",
        "core-feature evidence drift before architecture freeze readiness advances.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D003-META-01",
        "deterministic lane-D runtime property metadata integration core feature metadata anchors for `M234-D003`",
    ),
    SnippetCheck(
        "M234-D003-META-02",
        "explicit `M234-D002` dependency continuity so core feature implementation drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D003-PKG-01",
        '"check:objc3c:m234-d003-runtime-property-metadata-integration-core-feature-implementation-contract": '
        '"python scripts/check_m234_d003_runtime_property_metadata_integration_core_feature_implementation_contract.py"',
    ),
    SnippetCheck(
        "M234-D003-PKG-02",
        '"test:tooling:m234-d003-runtime-property-metadata-integration-core-feature-implementation-contract": '
        '"python -m pytest tests/tooling/test_check_m234_d003_runtime_property_metadata_integration_core_feature_implementation_contract.py -q"',
    ),
    SnippetCheck(
        "M234-D003-PKG-03",
        '"check:objc3c:m234-d003-lane-d-readiness": '
        '"npm run check:objc3c:m234-d002-lane-d-readiness '
        '&& npm run check:objc3c:m234-d003-runtime-property-metadata-integration-core-feature-implementation-contract '
        '&& npm run test:tooling:m234-d003-runtime-property-metadata-integration-core-feature-implementation-contract"',
    ),
    SnippetCheck("M234-D003-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M234-D003-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M234-D003-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M234-D003-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--d002-expectations-doc", type=Path, default=DEFAULT_D002_EXPECTATIONS_DOC)
    parser.add_argument("--d002-packet-doc", type=Path, default=DEFAULT_D002_PACKET_DOC)
    parser.add_argument("--d002-checker", type=Path, default=DEFAULT_D002_CHECKER)
    parser.add_argument("--d002-test", type=Path, default=DEFAULT_D002_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
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
        (args.expectations_doc, "M234-D003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-D003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d002_expectations_doc, "M234-D003-D002-DOC-EXISTS", D002_EXPECTATIONS_SNIPPETS),
        (args.d002_packet_doc, "M234-D003-D002-PKT-EXISTS", D002_PACKET_SNIPPETS),
        (args.architecture_doc, "M234-D003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-D003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-D003-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-D003-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d002_checker, "M234-D003-DEP-D002-ARG-01"),
        (args.d002_test, "M234-D003-DEP-D002-ARG-02"),
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


