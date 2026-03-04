#!/usr/bin/env python3
"""Fail-closed checker for M245-A011 frontend behavior parity integration closeout and gate sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_a011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_packet.md"
)
DEFAULT_A010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_frontend_behavior_parity_across_toolchains_conformance_corpus_expansion_a010_expectations.md"
)
DEFAULT_A010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_a010_frontend_behavior_parity_across_toolchains_conformance_corpus_expansion_packet.md"
)
DEFAULT_A010_CHECKER = (
    ROOT / "scripts" / "check_m245_a010_frontend_behavior_parity_across_toolchains_conformance_corpus_expansion_contract.py"
)
DEFAULT_A010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m245_a010_frontend_behavior_parity_across_toolchains_conformance_corpus_expansion_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-A011/frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_summary.json"
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
        "M245-A011-DOC-EXP-01",
        "# M245 Frontend Behavior Parity Across Toolchains Integration Closeout and Gate Sign-Off Expectations (A011)",
    ),
    SnippetCheck(
        "M245-A011-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff/m245-a011-v1`",
    ),
    SnippetCheck("M245-A011-DOC-EXP-03", "Dependencies: `M245-A010`"),
    SnippetCheck(
        "M245-A011-DOC-EXP-04",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-A011-DOC-EXP-05",
        "docs/contracts/m245_frontend_behavior_parity_across_toolchains_conformance_corpus_expansion_a010_expectations.md",
    ),
    SnippetCheck(
        "M245-A011-DOC-EXP-06",
        "scripts/check_m245_a010_frontend_behavior_parity_across_toolchains_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M245-A011-DOC-EXP-07",
        "`check:objc3c:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract`",
    ),
    SnippetCheck(
        "M245-A011-DOC-EXP-08",
        "`tmp/reports/m245/M245-A011/frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-A011-DOC-PKT-01",
        "# M245-A011 Frontend Behavior Parity Across Toolchains Integration Closeout and Gate Sign-Off Packet",
    ),
    SnippetCheck("M245-A011-DOC-PKT-02", "Packet: `M245-A011`"),
    SnippetCheck("M245-A011-DOC-PKT-03", "Dependencies: `M245-A010`"),
    SnippetCheck(
        "M245-A011-DOC-PKT-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M245-A011-DOC-PKT-05",
        "scripts/check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M245-A011-DOC-PKT-06",
        "tests/tooling/test_check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck("M245-A011-DOC-PKT-07", "`npm run check:objc3c:m245-a011-lane-a-readiness`"),
)

A010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-A011-A010-DOC-01",
        "# M245 Frontend Behavior Parity Across Toolchains Conformance Corpus Expansion Expectations (A010)",
    ),
    SnippetCheck(
        "M245-A011-A010-DOC-02",
        "Contract ID: `objc3c-frontend-behavior-parity-toolchains-conformance-corpus-expansion/m245-a010-v1`",
    ),
)

A010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-A011-A010-PKT-01", "Packet: `M245-A010`"),
    SnippetCheck("M245-A011-A010-PKT-02", "Dependencies: `M245-A009`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-A011-ARCH-01",
        "M245 lane-A A011 frontend behavior parity integration closeout and gate sign-off",
    ),
    SnippetCheck(
        "M245-A011-ARCH-02",
        "anchors explicit lane-A integration-closeout artifacts in",
    ),
    SnippetCheck(
        "M245-A011-ARCH-03",
        "docs/contracts/m245_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_a011_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-A011-SPC-01",
        "frontend behavior parity integration closeout and gate sign-off governance shall preserve explicit",
    ),
    SnippetCheck(
        "M245-A011-SPC-02",
        "lane-A dependency anchors (`M245-A010`) and fail closed on integration closeout and gate sign-off evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-A011-META-01",
        "deterministic lane-A frontend behavior parity integration closeout and gate sign-off metadata anchors for `M245-A011`",
    ),
    SnippetCheck(
        "M245-A011-META-02",
        "explicit `M245-A010` dependency continuity so integration closeout and gate sign-off drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-A011-PKG-01",
        '"check:objc3c:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract": '
        '"python scripts/check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract.py"',
    ),
    SnippetCheck(
        "M245-A011-PKG-02",
        '"test:tooling:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract": '
        '"python -m pytest tests/tooling/test_check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract.py -q"',
    ),
    SnippetCheck(
        "M245-A011-PKG-03",
        '"check:objc3c:m245-a011-lane-a-readiness": '
        '"npm run check:objc3c:m245-a010-lane-a-readiness '
        '&& npm run check:objc3c:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract '
        '&& npm run test:tooling:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract"',
    ),
    SnippetCheck("M245-A011-PKG-04", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M245-A011-PKG-05", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--a010-expectations-doc", type=Path, default=DEFAULT_A010_EXPECTATIONS_DOC)
    parser.add_argument("--a010-packet-doc", type=Path, default=DEFAULT_A010_PACKET_DOC)
    parser.add_argument("--a010-checker", type=Path, default=DEFAULT_A010_CHECKER)
    parser.add_argument("--a010-test", type=Path, default=DEFAULT_A010_TEST)
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
        (args.expectations_doc, "M245-A011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M245-A011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a010_expectations_doc, "M245-A011-A010-DOC-EXISTS", A010_EXPECTATIONS_SNIPPETS),
        (args.a010_packet_doc, "M245-A011-A010-PKT-EXISTS", A010_PACKET_SNIPPETS),
        (args.architecture_doc, "M245-A011-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M245-A011-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M245-A011-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M245-A011-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a010_checker, "M245-A011-DEP-A010-ARG-01"),
        (args.a010_test, "M245-A011-DEP-A010-ARG-02"),
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



