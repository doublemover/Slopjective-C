#!/usr/bin/env python3
"""Fail-closed checker for M234-A007 property and ivar syntax surface completion diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_property_and_ivar_syntax_surface_completion_diagnostics_hardening_a007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_packet.md"
)
DEFAULT_A006_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_a006_expectations.md"
)
DEFAULT_A006_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_A006_CHECKER = (
    ROOT / "scripts" / "check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_A006_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-A007/property_and_ivar_syntax_surface_completion_diagnostics_hardening_summary.json"
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
        "M234-A007-DOC-EXP-01",
        "# M234 Property and Ivar Syntax Surface Completion Diagnostics Hardening Expectations (A007)",
    ),
    SnippetCheck(
        "M234-A007-DOC-EXP-02",
        "Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-diagnostics-hardening/m234-a007-v1`",
    ),
    SnippetCheck("M234-A007-DOC-EXP-03", "Dependencies: `M234-A006`"),
    SnippetCheck(
        "M234-A007-DOC-EXP-04",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-A007-DOC-EXP-05",
        "docs/contracts/m234_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_a006_expectations.md",
    ),
    SnippetCheck(
        "M234-A007-DOC-EXP-06",
        "scripts/check_m234_a006_property_and_ivar_syntax_surface_completion_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M234-A007-DOC-EXP-07",
        "`check:objc3c:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract`",
    ),
    SnippetCheck(
        "M234-A007-DOC-EXP-08",
        "`tmp/reports/m234/M234-A007/property_and_ivar_syntax_surface_completion_diagnostics_hardening_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-A007-DOC-PKT-01",
        "# M234-A007 Property and Ivar Syntax Surface Completion Diagnostics Hardening Packet",
    ),
    SnippetCheck("M234-A007-DOC-PKT-02", "Packet: `M234-A007`"),
    SnippetCheck("M234-A007-DOC-PKT-03", "Dependencies: `M234-A006`"),
    SnippetCheck(
        "M234-A007-DOC-PKT-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-A007-DOC-PKT-05",
        "scripts/check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py",
    ),
    SnippetCheck(
        "M234-A007-DOC-PKT-06",
        "tests/tooling/test_check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py",
    ),
    SnippetCheck("M234-A007-DOC-PKT-07", "`npm run check:objc3c:m234-a007-lane-a-readiness`"),
)

A006_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-A007-A006-DOC-01",
        "# M234 Property and Ivar Syntax Surface Completion Edge-Case Expansion and Robustness Expectations (A006)",
    ),
    SnippetCheck(
        "M234-A007-A006-DOC-02",
        "Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-edge-case-expansion-and-robustness/m234-a006-v1`",
    ),
)

A006_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-A007-A006-PKT-01", "Packet: `M234-A006`"),
    SnippetCheck("M234-A007-A006-PKT-02", "Dependencies: `M234-A005`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-A007-ARCH-01",
        "M234 lane-A A007 property and ivar syntax surface completion diagnostics hardening anchors",
    ),
    SnippetCheck(
        "M234-A007-ARCH-02",
        "docs/contracts/m234_property_and_ivar_syntax_surface_completion_diagnostics_hardening_a007_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-A007-SPC-01",
        "property and ivar syntax surface completion diagnostics hardening governance shall preserve explicit",
    ),
    SnippetCheck(
        "M234-A007-SPC-02",
        "lane-A dependency anchors (`M234-A006`) and fail closed on diagnostics hardening evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-A007-META-01",
        "deterministic lane-A property and ivar syntax surface completion diagnostics hardening metadata anchors for `M234-A007`",
    ),
    SnippetCheck(
        "M234-A007-META-02",
        "explicit `M234-A006` dependency continuity so diagnostics hardening drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-A007-PKG-01",
        '"check:objc3c:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract": '
        '"python scripts/check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py"',
    ),
    SnippetCheck(
        "M234-A007-PKG-02",
        '"test:tooling:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract": '
        '"python -m pytest tests/tooling/test_check_m234_a007_property_and_ivar_syntax_surface_completion_diagnostics_hardening_contract.py -q"',
    ),
    SnippetCheck(
        "M234-A007-PKG-03",
        '"check:objc3c:m234-a007-lane-a-readiness": '
        '"npm run check:objc3c:m234-a006-lane-a-readiness '
        '&& npm run check:objc3c:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract '
        '&& npm run test:tooling:m234-a007-property-and-ivar-syntax-surface-completion-diagnostics-hardening-contract"',
    ),
    SnippetCheck("M234-A007-PKG-04", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M234-A007-PKG-05", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--a006-expectations-doc", type=Path, default=DEFAULT_A006_EXPECTATIONS_DOC)
    parser.add_argument("--a006-packet-doc", type=Path, default=DEFAULT_A006_PACKET_DOC)
    parser.add_argument("--a006-checker", type=Path, default=DEFAULT_A006_CHECKER)
    parser.add_argument("--a006-test", type=Path, default=DEFAULT_A006_TEST)
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
        (args.expectations_doc, "M234-A007-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-A007-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a006_expectations_doc, "M234-A007-A006-DOC-EXISTS", A006_EXPECTATIONS_SNIPPETS),
        (args.a006_packet_doc, "M234-A007-A006-PKT-EXISTS", A006_PACKET_SNIPPETS),
        (args.architecture_doc, "M234-A007-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M234-A007-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M234-A007-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M234-A007-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a006_checker, "M234-A007-DEP-A006-ARG-01"),
        (args.a006_test, "M234-A007-DEP-A006-ARG-02"),
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



