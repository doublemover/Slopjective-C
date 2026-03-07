#!/usr/bin/env python3
"""Fail-closed checker for M230-D005 Developer CLI and diagnostics ergonomics contract freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m230_developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_d005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m230"
    / "m230_d005_developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m230/M230-D005/developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_summary.json"
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
        "M230-D005-DOC-EXP-01",
        "# M230 Developer CLI and diagnostics ergonomics Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M230-D005-DOC-EXP-02",
        "Contract ID: `objc3c-developer-cli-and-diagnostics-ergonomics/m230-d005-v1`",
    ),
    SnippetCheck("M230-D005-DOC-EXP-03", "Issue: `#5444`"),
    SnippetCheck("M230-D005-DOC-EXP-04", "Dependencies: `M230-D004`"),
    SnippetCheck(
        "M230-D005-DOC-EXP-05",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M230-D005-DOC-EXP-06",
        "`check:objc3c:m230-d005-lane-d-readiness`",
    ),
    SnippetCheck(
        "M230-D005-DOC-EXP-07",
        "`tmp/reports/m230/M230-D005/developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-D005-DOC-PKT-01",
        "# M230-D005 Developer CLI and diagnostics ergonomics Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M230-D005-DOC-PKT-02", "Packet: `M230-D005`"),
    SnippetCheck("M230-D005-DOC-PKT-03", "Issue: `#5444`"),
    SnippetCheck("M230-D005-DOC-PKT-04", "Dependencies: `M230-D004`"),
    SnippetCheck(
        "M230-D005-DOC-PKT-05",
        "check:objc3c:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract",
    ),
    SnippetCheck(
        "M230-D005-DOC-PKT-06",
        "tmp/reports/m230/M230-D005/developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_summary.json",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-D005-ARCH-01",
        "M230 lane-D A001 Developer CLI and diagnostics ergonomics contract-freeze anchors",
    ),
    SnippetCheck(
        "M230-D005-ARCH-02",
        "m230_developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_d005_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-D005-SPC-01",
        "Developer CLI and diagnostics ergonomics contract-freeze governance shall preserve explicit",
    ),
    SnippetCheck(
        "M230-D005-SPC-02",
        "lane-D deterministic boundary anchors (`M230-D005`) and fail closed on contract-freeze evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-D005-META-01",
        "deterministic lane-D Developer CLI and diagnostics ergonomics contract-freeze anchors for `M230-D005`",
    ),
    SnippetCheck(
        "M230-D005-META-02",
        "explicit lane-D contract-freeze metadata continuity so Developer CLI and diagnostics ergonomics drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-D005-PKG-01",
        '"check:objc3c:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract": '
        '"python scripts/check_m230_d005_developer_cli_and_diagnostics_ergonomics_contract.py"',
    ),
    SnippetCheck(
        "M230-D005-PKG-02",
        '"test:tooling:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract": '
        '"python -m pytest tests/tooling/test_check_m230_d005_developer_cli_and_diagnostics_ergonomics_contract.py -q"',
    ),
    SnippetCheck(
        "M230-D005-PKG-03",
        '"check:objc3c:m230-d005-lane-d-readiness": '
        '"npm run check:objc3c:m230-d004-lane-d-readiness && npm run check:objc3c:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract '
        '&& npm run test:tooling:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract"',
    ),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(list(argv))


def ensure_file(path: Path, artifact: str, check_id: str, findings: list[Finding]) -> str:
    if not path.is_file():
        findings.append(Finding(artifact=artifact, check_id=check_id, detail=f"missing file: {display_path(path)}"))
        return ""
    return path.read_text(encoding="utf-8")


def ensure_snippets(
    *,
    artifact: str,
    content: str,
    checks: Sequence[SnippetCheck],
    findings: list[Finding],
) -> int:
    passed = 0
    for check in checks:
        if check.snippet in content:
            passed += 1
        else:
            findings.append(Finding(artifact=artifact, check_id=check.check_id, detail=f"missing snippet: {check.snippet}"))
    return passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifact_inputs = (
        ("expectations_doc", "M230-D005-EXPECTATIONS", args.expectations_doc, EXPECTATIONS_SNIPPETS),
        ("packet_doc", "M230-D005-PACKET", args.packet_doc, PACKET_SNIPPETS),
        ("architecture_doc", "M230-D005-ARCHITECTURE", args.architecture_doc, ARCHITECTURE_SNIPPETS),
        ("lowering_spec", "M230-D005-LOWERING-SPEC", args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", "M230-D005-METADATA-SPEC", args.metadata_spec, METADATA_SPEC_SNIPPETS),
        ("package_json", "M230-D005-PACKAGE", args.package_json, PACKAGE_SNIPPETS),
    )

    for _, artifact, path, snippets in artifact_inputs:
        checks_total += len(snippets) + 1
        content = ensure_file(path, artifact, f"{artifact}-FILE", findings)
        if content:
            checks_passed += 1
        checks_passed += ensure_snippets(
            artifact=artifact,
            content=content,
            checks=snippets,
            findings=findings,
        )

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        print(
            f"[fail] {MODE}: {checks_passed}/{checks_total} checks passed; "
            f"{len(findings)} failure(s). Summary: {display_path(args.summary_out)}",
            file=sys.stderr,
        )
        for finding in findings:
            print(f" - ({finding.check_id}) [{finding.artifact}] {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


def main() -> int:
    return run(sys.argv[1:])


if __name__ == "__main__":
    raise SystemExit(main())









