#!/usr/bin/env python3
"""Fail-closed contract checker for M241-D007 interop behavior for qualified generic APIs."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m241-d007-build-system-and-artifact-orchestration-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m241_build_system_and_artifact_orchestration_diagnostics_hardening_d007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m241"
    / "m241_d007_build_system_and_artifact_orchestration_diagnostics_hardening_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m241/M241-D007/build_system_and_artifact_orchestration_contract_summary.json"
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
        "M241-D007-DOC-EXP-01",
        "# M241 Interop Behavior for Qualified Generic APIs Diagnostics Hardening Expectations (D007)",
    ),
    SnippetCheck(
        "M241-D007-DOC-EXP-02",
        "Contract ID: `objc3c-build-system-and-artifact-orchestration/m241-d007-v1`",
    ),
    SnippetCheck(
        "M241-D007-DOC-EXP-03",
        "Issue `#6281` defines canonical lane-D diagnostics hardening scope.",
    ),
    SnippetCheck("M241-D007-DOC-EXP-04", "Dependencies: `M241-C001`"),
    SnippetCheck(
        "M241-D007-DOC-EXP-05",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M241-D007-DOC-EXP-06",
        "scripts/check_m241_d007_build_system_and_artifact_orchestration_contract.py",
    ),
    SnippetCheck(
        "M241-D007-DOC-EXP-07",
        "scripts/check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py",
    ),
    SnippetCheck(
        "M241-D007-DOC-EXP-08",
        "`tmp/reports/m241/M241-D007/build_system_and_artifact_orchestration_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M241-D007-DOC-PKT-01",
        "# M241-D007 Interop Behavior for Qualified Generic APIs Diagnostics Hardening Packet",
    ),
    SnippetCheck("M241-D007-DOC-PKT-02", "Packet: `M241-D007`"),
    SnippetCheck("M241-D007-DOC-PKT-03", "Issue: `#5831`"),
    SnippetCheck("M241-D007-DOC-PKT-04", "Dependencies: `M241-C001`"),
    SnippetCheck(
        "M241-D007-DOC-PKT-05",
        "scripts/check_m241_d007_build_system_and_artifact_orchestration_contract.py",
    ),
    SnippetCheck(
        "M241-D007-DOC-PKT-06",
        "scripts/check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py",
    ),
    SnippetCheck("M241-D007-DOC-PKT-07", "`compile:objc3c`"),
    SnippetCheck("M241-D007-DOC-PKT-08", "`check:objc3c:m241-c001-lane-c-readiness`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M241-D007-ARCH-01",
        "M241 lane-C C001 qualified type lowering and ABI representation anchors explicit",
    ),
    SnippetCheck(
        "M241-D007-ARCH-02",
        "docs/contracts/m241_incremental_lowering_and_artifact_reuse_diagnostics_hardening_c001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M241-D007-SPC-01",
        "qualified type lowering and ABI representation governance shall preserve deterministic lane-C",
    ),
    SnippetCheck(
        "M241-D007-SPC-02",
        "nullability/generics/qualifier lowering and ABI representation drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M241-D007-META-01",
        "deterministic lane-C qualified type lowering and ABI representation metadata anchors for `M241-C001`",
    ),
    SnippetCheck(
        "M241-D007-META-02",
        "qualified-type lowering and ABI representation evidence and lowering replay-budget continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M241-D007-PKG-01",
        '"check:objc3c:m241-c001-incremental-lowering-and-artifact-reuse-contract": '
        '"python scripts/check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py"',
    ),
    SnippetCheck(
        "M241-D007-PKG-02",
        '"test:tooling:m241-c001-incremental-lowering-and-artifact-reuse-contract": '
        '"python -m pytest tests/tooling/test_check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py -q"',
    ),
    SnippetCheck(
        "M241-D007-PKG-03",
        '"check:objc3c:m241-c001-lane-c-readiness": '
        '"npm run check:objc3c:m241-c001-incremental-lowering-and-artifact-reuse-contract '
        '&& npm run test:tooling:m241-c001-incremental-lowering-and-artifact-reuse-contract"',
    ),
    SnippetCheck("M241-D007-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M241-D007-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M241-D007-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M241-D007-PKG-07", '"test:objc3c:perf-budget": '),
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
        (args.expectations_doc, "M241-D007-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M241-D007-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M241-D007-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M241-D007-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M241-D007-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M241-D007-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

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







