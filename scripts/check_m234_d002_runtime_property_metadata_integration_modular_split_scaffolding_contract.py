#!/usr/bin/env python3
"""Fail-closed contract checker for the M234-D002 runtime property metadata modular split/scaffolding packet."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-d002-runtime-property-metadata-integration-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_runtime_property_metadata_integration_modular_split_scaffolding_d002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_packet.md"
)
DEFAULT_D001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_runtime_property_metadata_integration_contract_and_architecture_freeze_d001_expectations.md"
)
DEFAULT_D001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_d001_runtime_property_metadata_integration_contract_and_architecture_freeze_packet.md"
)
DEFAULT_D001_CHECKER = ROOT / "scripts" / "check_m234_d001_runtime_property_metadata_integration_contract.py"
DEFAULT_D001_TEST = ROOT / "tests" / "tooling" / "test_check_m234_d001_runtime_property_metadata_integration_contract.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-D002/runtime_property_metadata_integration_modular_split_scaffolding_contract_summary.json"
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
        "M234-D002-DOC-EXP-01",
        "# M234 Runtime Property Metadata Integration Modular Split Scaffolding Expectations (D002)",
    ),
    SnippetCheck(
        "M234-D002-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-property-metadata-integration-modular-split-scaffolding/m234-d002-v1`",
    ),
    SnippetCheck("M234-D002-DOC-EXP-03", "- Dependencies: `M234-D001`"),
    SnippetCheck(
        "M234-D002-DOC-EXP-04",
        "`scripts/check_m234_d001_runtime_property_metadata_integration_contract.py`",
    ),
    SnippetCheck(
        "M234-D002-DOC-EXP-05",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M234-D002-DOC-EXP-06",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-D002-DOC-EXP-07",
        "`check:objc3c:m234-d002-lane-d-readiness`",
    ),
    SnippetCheck(
        "M234-D002-DOC-EXP-08",
        "`tmp/reports/m234/M234-D002/runtime_property_metadata_integration_modular_split_scaffolding_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D002-DOC-PKT-01",
        "# M234-D002 Runtime Property Metadata Integration Modular Split and Scaffolding Packet",
    ),
    SnippetCheck("M234-D002-DOC-PKT-02", "Packet: `M234-D002`"),
    SnippetCheck("M234-D002-DOC-PKT-03", "Dependencies: `M234-D001`"),
    SnippetCheck(
        "M234-D002-DOC-PKT-04",
        "`scripts/check_m234_d001_runtime_property_metadata_integration_contract.py`",
    ),
    SnippetCheck(
        "M234-D002-DOC-PKT-05",
        "`check:objc3c:m234-d002-lane-d-readiness`",
    ),
    SnippetCheck(
        "M234-D002-DOC-PKT-06",
        "`test:objc3c:execution-replay-proof`",
    ),
    SnippetCheck(
        "M234-D002-DOC-PKT-07",
        "code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M234-D002-DOC-PKT-08",
        "improvements as mandatory scope inputs.",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D002-ARCH-01",
        "M234 lane-D D002 runtime property metadata integration modular split/scaffolding anchors",
    ),
    SnippetCheck(
        "M234-D002-ARCH-02",
        "docs/contracts/m234_runtime_property_metadata_integration_modular_split_scaffolding_d002_expectations.md",
    ),
    SnippetCheck(
        "M234-D002-ARCH-03",
        "spec/planning/compiler/m234/m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_packet.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D002-SPC-01",
        "runtime property metadata integration modular split scaffolding shall",
    ),
    SnippetCheck(
        "M234-D002-SPC-02",
        "preserve explicit lane-D dependency anchors (`M234-D001`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D002-META-01",
        "deterministic lane-D runtime property metadata integration modular split metadata anchors for",
    ),
    SnippetCheck(
        "M234-D002-META-02",
        "`M234-D002` with explicit `M234-D001` dependency continuity so runtime property metadata",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-D002-PKG-01",
        '"check:objc3c:m234-d002-runtime-property-metadata-integration-modular-split-scaffolding-contract": '
        '"python scripts/check_m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M234-D002-PKG-02",
        '"test:tooling:m234-d002-runtime-property-metadata-integration-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m234_d002_runtime_property_metadata_integration_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M234-D002-PKG-03",
        '"check:objc3c:m234-d002-lane-d-readiness": '
        '"npm run check:objc3c:m234-d001-lane-d-readiness '
        '&& npm run check:objc3c:m234-d002-runtime-property-metadata-integration-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m234-d002-runtime-property-metadata-integration-modular-split-scaffolding-contract"',
    ),
    SnippetCheck("M234-D002-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M234-D002-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M234-D002-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M234-D002-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--d001-expectations-doc", type=Path, default=DEFAULT_D001_EXPECTATIONS_DOC)
    parser.add_argument("--d001-packet-doc", type=Path, default=DEFAULT_D001_PACKET_DOC)
    parser.add_argument("--d001-checker", type=Path, default=DEFAULT_D001_CHECKER)
    parser.add_argument("--d001-test", type=Path, default=DEFAULT_D001_TEST)
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
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def check_file_exists(path: Path, check_id: str) -> tuple[int, list[Finding]]:
    if path.exists() and path.is_file():
        return 1, []
    return 1, [Finding(display_path(path), check_id, f"required file is missing: {display_path(path)}")]


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M234-D002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-D002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.d001_expectations_doc, "M234-D002-D001-DOC-EXISTS", ()),
        (args.d001_packet_doc, "M234-D002-D001-PKT-EXISTS", ()),
        (args.architecture_doc, "M234-D002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-D002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-D002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-D002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d001_checker, "M234-D002-D001-CHECKER-EXISTS"),
        (args.d001_test, "M234-D002-D001-TEST-EXISTS"),
    ):
        count, findings = check_file_exists(path, check_id)
        checks_total += count
        failures.extend(findings)

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
