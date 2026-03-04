#!/usr/bin/env python3
"""Fail-closed contract checker for the M245-B004 semantic parity/platform constraints core feature expansion packet."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-b004-semantic-parity-platform-constraints-core-feature-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_semantic_parity_and_platform_constraints_core_feature_expansion_b004_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_b004_semantic_parity_and_platform_constraints_core_feature_expansion_packet.md"
)
DEFAULT_B003_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_semantic_parity_and_platform_constraints_core_feature_implementation_b003_expectations.md"
)
DEFAULT_B003_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_b003_semantic_parity_and_platform_constraints_core_feature_implementation_packet.md"
)
DEFAULT_B003_CHECKER = (
    ROOT / "scripts" / "check_m245_b003_semantic_parity_and_platform_constraints_core_feature_implementation_contract.py"
)
DEFAULT_B003_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m245_b003_semantic_parity_and_platform_constraints_core_feature_implementation_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-B004/semantic_parity_and_platform_constraints_core_feature_expansion_summary.json"
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
        "M245-B004-DOC-EXP-01",
        "# M245 Semantic Parity and Platform Constraints Core Feature Expansion Expectations (B004)",
    ),
    SnippetCheck(
        "M245-B004-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-parity-platform-constraints-core-feature-expansion/m245-b004-v1`",
    ),
    SnippetCheck("M245-B004-DOC-EXP-03", "- Issue: `#6626`"),
    SnippetCheck("M245-B004-DOC-EXP-04", "- Dependencies: `M245-B003`"),
    SnippetCheck(
        "M245-B004-DOC-EXP-05",
        "`scripts/check_m245_b003_semantic_parity_and_platform_constraints_core_feature_implementation_contract.py`",
    ),
    SnippetCheck(
        "M245-B004-DOC-EXP-06",
        "architecture/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M245-B004-DOC-EXP-07",
        "improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-B004-DOC-EXP-08",
        "`check:objc3c:m245-b004-lane-b-readiness`",
    ),
    SnippetCheck(
        "M245-B004-DOC-EXP-09",
        "`tmp/reports/m245/M245-B004/semantic_parity_and_platform_constraints_core_feature_expansion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B004-DOC-PKT-01",
        "# M245-B004 Semantic Parity and Platform Constraints Core Feature Expansion Packet",
    ),
    SnippetCheck("M245-B004-DOC-PKT-02", "Packet: `M245-B004`"),
    SnippetCheck("M245-B004-DOC-PKT-03", "Issue: `#6626`"),
    SnippetCheck("M245-B004-DOC-PKT-04", "Dependencies: `M245-B003`"),
    SnippetCheck(
        "M245-B004-DOC-PKT-05",
        "`scripts/check_m245_b004_semantic_parity_and_platform_constraints_core_feature_expansion_contract.py`",
    ),
    SnippetCheck(
        "M245-B004-DOC-PKT-06",
        "`check:objc3c:m245-b004-lane-b-readiness`",
    ),
    SnippetCheck(
        "M245-B004-DOC-PKT-07",
        "`test:objc3c:lowering-regression`",
    ),
    SnippetCheck(
        "M245-B004-DOC-PKT-08",
        "architecture/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M245-B004-DOC-PKT-09",
        "improvements as mandatory scope inputs.",
    ),
)

B003_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B004-B003-DOC-01",
        "# M245 Semantic Parity and Platform Constraints Core Feature Implementation Expectations (B003)",
    ),
    SnippetCheck(
        "M245-B004-B003-DOC-02",
        "Contract ID: `objc3c-semantic-parity-platform-constraints-core-feature-implementation/m245-b003-v1`",
    ),
    SnippetCheck("M245-B004-B003-DOC-03", "Dependencies: `M245-B002`"),
)

B003_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-B004-B003-PKT-01", "Packet: `M245-B003`"),
    SnippetCheck("M245-B004-B003-PKT-02", "Dependencies: `M245-B002`"),
    SnippetCheck(
        "M245-B004-B003-PKT-03",
        "`tmp/reports/m245/M245-B003/semantic_parity_and_platform_constraints_core_feature_implementation_summary.json`",
    ),
    SnippetCheck(
        "M245-B004-B003-PKT-04",
        "scripts/check_m245_b003_semantic_parity_and_platform_constraints_core_feature_implementation_contract.py",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B004-ARCH-01",
        "M245 lane-B B004 semantic parity/platform constraints core feature expansion anchors",
    ),
    SnippetCheck(
        "M245-B004-ARCH-02",
        "docs/contracts/m245_semantic_parity_and_platform_constraints_core_feature_expansion_b004_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B004-SPC-01",
        "semantic parity and platform constraints core feature expansion shall",
    ),
    SnippetCheck(
        "M245-B004-SPC-02",
        "lane-B dependency anchors (`M245-B003`) and fail closed on core-feature expansion evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B004-META-01",
        "deterministic lane-B semantic parity/platform constraints core feature expansion metadata anchors for",
    ),
    SnippetCheck(
        "M245-B004-META-02",
        "`M245-B004` with explicit `M245-B003` dependency continuity so core feature expansion drift fails closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-B004-PKG-01",
        '"check:objc3c:m245-b004-semantic-parity-platform-constraints-core-feature-expansion-contract": '
        '"python scripts/check_m245_b004_semantic_parity_and_platform_constraints_core_feature_expansion_contract.py"',
    ),
    SnippetCheck(
        "M245-B004-PKG-02",
        '"test:tooling:m245-b004-semantic-parity-platform-constraints-core-feature-expansion-contract": '
        '"python -m pytest tests/tooling/test_check_m245_b004_semantic_parity_and_platform_constraints_core_feature_expansion_contract.py -q"',
    ),
    SnippetCheck(
        "M245-B004-PKG-03",
        '"check:objc3c:m245-b004-lane-b-readiness": '
        '"npm run check:objc3c:m245-b003-lane-b-readiness '
        '&& npm run check:objc3c:m245-b004-semantic-parity-platform-constraints-core-feature-expansion-contract '
        '&& npm run test:tooling:m245-b004-semantic-parity-platform-constraints-core-feature-expansion-contract"',
    ),
    SnippetCheck("M245-B004-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M245-B004-PKG-05", '"test:objc3c:lowering-regression": '),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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
        (args.expectations_doc, "M245-B004-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-B004-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b003_expectations_doc, "M245-B004-B003-DOC-EXISTS", B003_EXPECTATIONS_SNIPPETS),
        (args.b003_packet_doc, "M245-B004-B003-PKT-EXISTS", B003_PACKET_SNIPPETS),
        (args.architecture_doc, "M245-B004-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M245-B004-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M245-B004-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M245-B004-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b003_checker, "M245-B004-DEP-B003-ARG-01"),
        (args.b003_test, "M245-B004-DEP-B003-ARG-02"),
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

    failures = sorted(
        failures,
        key=lambda finding: (finding.check_id, finding.artifact, finding.detail),
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

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        if not args.emit_json:
            for finding in failures:
                print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
