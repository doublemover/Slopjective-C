#!/usr/bin/env python3
"""Fail-closed contract checker for M249-B001 semantic compatibility/migration checks freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-b001-semantic-compatibility-migration-checks-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_b001_semantic_compatibility_and_migration_checks_contract_and_architecture_freeze_packet.md"
)
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
DEFAULT_SEMA_SCAFFOLD = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_flow_scaffold.cpp"
DEFAULT_PARSE_READINESS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-B001/semantic_compatibility_and_migration_checks_contract_summary.json"
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
        "M249-B001-DOC-EXP-01",
        "# M249 Semantic Compatibility and Migration Checks Contract Freeze Expectations (B001)",
    ),
    SnippetCheck(
        "M249-B001-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-freeze/m249-b001-v1`",
    ),
    SnippetCheck(
        "M249-B001-DOC-EXP-03",
        "`sema/objc3_sema_pass_manager_contract.h`",
    ),
    SnippetCheck(
        "M249-B001-DOC-EXP-04",
        "`pipeline/objc3_parse_lowering_readiness_surface.h` keeps compatibility",
    ),
    SnippetCheck(
        "M249-B001-DOC-EXP-05",
        "`check:objc3c:m249-b001-lane-b-readiness`",
    ),
    SnippetCheck(
        "M249-B001-DOC-EXP-06",
        "`tmp/reports/m249/M249-B001/semantic_compatibility_and_migration_checks_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-DOC-PKT-01",
        "# M249-B001 Semantic Compatibility and Migration Checks Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M249-B001-DOC-PKT-02", "Packet: `M249-B001`"),
    SnippetCheck("M249-B001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck(
        "M249-B001-DOC-PKT-04",
        "`scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`",
    ),
    SnippetCheck(
        "M249-B001-DOC-PKT-05",
        "`tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`",
    ),
    SnippetCheck(
        "M249-B001-DOC-PKT-06",
        "`check:objc3c:m249-b001-lane-b-readiness`",
    ),
)

SEMA_CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-B001-SEM-01", "enum class Objc3SemaCompatibilityMode : std::uint8_t {"),
    SnippetCheck("M249-B001-SEM-02", "struct Objc3SemaMigrationHints {"),
    SnippetCheck(
        "M249-B001-SEM-03",
        "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;",
    ),
    SnippetCheck("M249-B001-SEM-04", "bool migration_assist_enabled = false;"),
    SnippetCheck("M249-B001-SEM-05", "std::size_t migration_legacy_literal_total = 0;"),
    SnippetCheck("M249-B001-SEM-06", "bool compatibility_handoff_consistent = false;"),
    SnippetCheck("M249-B001-SEM-07", "summary.compatibility_handoff_consistent &&"),
    SnippetCheck("M249-B001-SEM-08", "struct Objc3SemaPassManagerInput {"),
    SnippetCheck("M249-B001-SEM-09", "bool migration_assist = false;"),
)

SEMA_SCAFFOLD_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M249-B001-SCF-01", "summary.compatibility_handoff_consistent ="),
    SnippetCheck("M249-B001-SCF-02", "(!summary.migration_assist_enabled ||"),
    SnippetCheck(
        "M249-B001-SCF-03",
        "summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical) &&",
    ),
    SnippetCheck(
        "M249-B001-SCF-04",
        "(summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical ||",
    ),
    SnippetCheck(
        "M249-B001-SCF-05",
        "summary.compatibility_mode == Objc3SemaCompatibilityMode::Legacy);",
    ),
    SnippetCheck(
        "M249-B001-SCF-06",
        "fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.migration_assist_enabled ? 1u : 0u));",
    ),
    SnippetCheck(
        "M249-B001-SCF-07",
        "fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.migration_legacy_literal_total));",
    ),
    SnippetCheck(
        "M249-B001-SCF-08",
        '<< ":compat=" << (summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical ? "canonical" : "legacy")',
    ),
)

PARSE_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-PRS-01",
        "inline std::string BuildObjc3CompatibilityHandoffKey(",
    ),
    SnippetCheck("M249-B001-PRS-02", 'return "compatibility_mode=" +'),
    SnippetCheck(
        "M249-B001-PRS-03",
        '";migration_assist=" + (options.migration_assist ? "true" : "false") +',
    ),
    SnippetCheck(
        "M249-B001-PRS-04",
        '";legacy_literals=" + std::to_string(migration_hints.legacy_yes_count) + ":" +',
    ),
    SnippetCheck("M249-B001-PRS-05", "surface.compatibility_handoff_consistent ="),
    SnippetCheck(
        "M249-B001-PRS-06",
        "surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey(",
    ),
    SnippetCheck(
        "M249-B001-PRS-07",
        "surface.long_tail_grammar_compatibility_handoff_ready =",
    ),
    SnippetCheck(
        "M249-B001-PRS-08",
        'surface.failure_reason = "compatibility handoff is inconsistent";',
    ),
)

PARSE_READINESS_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-PRS-FORB-01",
        "surface.compatibility_handoff_consistent = true;",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-ARCH-01",
        "M249 lane-B B001 semantic compatibility and migration checks anchors explicit",
    ),
    SnippetCheck(
        "M249-B001-ARCH-02",
        "docs/contracts/m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-SPC-01",
        "semantic compatibility and migration checks governance shall preserve explicit",
    ),
    SnippetCheck(
        "M249-B001-SPC-02",
        "lane-B compatibility-mode and migration-assist handoff anchors",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-META-01",
        "deterministic lane-B semantic compatibility/migration metadata anchors for `M249-B001`",
    ),
    SnippetCheck(
        "M249-B001-META-02",
        "sema pass-flow compatibility evidence and parse/lowering compatibility handoff continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-B001-PKG-01",
        '"check:objc3c:m249-b001-semantic-compatibility-migration-checks-contract": '
        '"python scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py"',
    ),
    SnippetCheck(
        "M249-B001-PKG-02",
        '"test:tooling:m249-b001-semantic-compatibility-migration-checks-contract": '
        '"python -m pytest tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py -q"',
    ),
    SnippetCheck(
        "M249-B001-PKG-03",
        '"check:objc3c:m249-b001-lane-b-readiness": '
        '"npm run check:objc3c:m249-b001-semantic-compatibility-migration-checks-contract '
        '&& npm run test:tooling:m249-b001-semantic-compatibility-migration-checks-contract"',
    ),
    SnippetCheck("M249-B001-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M249-B001-PKG-05", '"test:objc3c:lowering-regression": '),
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
    parser.add_argument("--sema-contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-scaffold", type=Path, default=DEFAULT_SEMA_SCAFFOLD)
    parser.add_argument("--parse-readiness", type=Path, default=DEFAULT_PARSE_READINESS)
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
    required_snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
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
    for snippet in required_snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"forbidden snippet present: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, required_snippets, forbidden_snippets in (
        (args.expectations_doc, "M249-B001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M249-B001-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.sema_contract, "M249-B001-SEM-EXISTS", SEMA_CONTRACT_SNIPPETS, ()),
        (args.sema_scaffold, "M249-B001-SCF-EXISTS", SEMA_SCAFFOLD_SNIPPETS, ()),
        (
            args.parse_readiness,
            "M249-B001-PRS-EXISTS",
            PARSE_READINESS_SNIPPETS,
            PARSE_READINESS_FORBIDDEN_SNIPPETS,
        ),
        (args.architecture_doc, "M249-B001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS, ()),
        (args.lowering_spec, "M249-B001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS, ()),
        (args.metadata_spec, "M249-B001-META-EXISTS", METADATA_SPEC_SNIPPETS, ()),
        (args.package_json, "M249-B001-PKG-EXISTS", PACKAGE_SNIPPETS, ()),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += count
        failures.extend(findings)

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
