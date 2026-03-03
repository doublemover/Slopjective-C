#!/usr/bin/env python3
"""Fail-closed checker for M243-B014 semantic diagnostic taxonomy integration closeout/sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-b014-semantic-diagnostic-taxonomy-and-fixit-synthesis-"
    "integration-closeout-and-gate-signoff-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_b014_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_packet.md",
    "b013_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_docs_operator_runbook_synchronization_b013_expectations.md",
    "b013_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b013_semantic_diagnostic_taxonomy_and_fix_it_synthesis_docs_operator_runbook_synchronization_packet.md",
    "b013_checker": ROOT
    / "scripts"
    / "check_m243_b013_semantic_diagnostic_taxonomy_and_fix_it_synthesis_docs_operator_runbook_synchronization_contract.py",
    "b013_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b013_semantic_diagnostic_taxonomy_and_fix_it_synthesis_docs_operator_runbook_synchronization_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-B014-DOC-01",
            "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Integration Closeout and Gate Sign-off Expectations (B014)",
        ),
        (
            "M243-B014-DOC-02",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-integration-closeout-and-gate-signoff/m243-b014-v1`",
        ),
        ("M243-B014-DOC-03", "Dependencies: `M243-B013`"),
        (
            "M243-B014-DOC-04",
            "spec/planning/compiler/m243/m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_packet.md",
        ),
        (
            "M243-B014-DOC-05",
            "scripts/check_m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M243-B014-DOC-06",
            "tests/tooling/test_check_m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M243-B014-DOC-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M243-B014-DOC-08", "check:objc3c:m243-b014-lane-b-readiness"),
        (
            "M243-B014-DOC-09",
            "tmp/reports/m243/M243-B014/semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M243-B014-PKT-01",
            "# M243-B014 Semantic Diagnostic Taxonomy and Fix-it Synthesis Integration Closeout and Gate Sign-off Packet",
        ),
        ("M243-B014-PKT-02", "Packet: `M243-B014`"),
        ("M243-B014-PKT-03", "Dependencies: `M243-B013`"),
        (
            "M243-B014-PKT-04",
            "docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_b014_expectations.md",
        ),
        (
            "M243-B014-PKT-05",
            "scripts/check_m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M243-B014-PKT-06",
            "tests/tooling/test_check_m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_contract.py",
        ),
        ("M243-B014-PKT-07", "`check:objc3c:m243-b014-lane-b-readiness`"),
        (
            "M243-B014-PKT-08",
            "python scripts/check_m243_b014_semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_contract.py --emit-json",
        ),
    ),
    "b013_expectations_doc": (
        (
            "M243-B014-DEP-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-docs-operator-runbook-synchronization/m243-b013-v1`",
        ),
    ),
    "b013_packet_doc": (
        ("M243-B014-DEP-02", "Packet: `M243-B013`"),
        ("M243-B014-DEP-03", "Dependencies: `M243-B012`"),
    ),
    "b013_checker": (
        (
            "M243-B014-DEP-04",
            'MODE = (\n    "m243-b013-semantic-diagnostic-taxonomy-and-fixit-synthesis-"\n    "docs-operator-runbook-synchronization-contract-v1"\n)',
        ),
    ),
    "b013_test": (
        (
            "M243-B014-DEP-05",
            "check_m243_b013_semantic_diagnostic_taxonomy_and_fix_it_synthesis_docs_operator_runbook_synchronization_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M243-B014-ARCH-01",
            "M243 lane-B B014 semantic diagnostic taxonomy/fix-it synthesis integration closeout and gate sign-off",
        ),
    ),
    "lowering_spec": (
        (
            "M243-B014-SPC-01",
            "semantic diagnostic taxonomy and fix-it synthesis integration closeout and gate sign-off\n  shall preserve lane-B dependency anchors (`M243-B013`)",
        ),
    ),
    "metadata_spec": (
        (
            "M243-B014-META-01",
            "deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis integration closeout and gate sign-off metadata anchors for `M243-B014` with explicit `M243-B013` dependency continuity",
        ),
    ),
    "package_json": (
        (
            "M243-B014-PKG-01",
            '"check:objc3c:m243-b014-semantic-diagnostic-taxonomy-and-fix-it-synthesis-integration-closeout-and-gate-signoff-contract": ',
        ),
        (
            "M243-B014-PKG-02",
            '"test:tooling:m243-b014-semantic-diagnostic-taxonomy-and-fix-it-synthesis-integration-closeout-and-gate-signoff-contract": ',
        ),
        (
            "M243-B014-PKG-03",
            '"check:objc3c:m243-b014-lane-b-readiness": "npm run check:objc3c:m243-b013-lane-b-readiness',
        ),
        (
            "M243-B014-PKG-04",
            '"test:objc3c:lowering-regression": ',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M243-B014-FORB-01",
            '"check:objc3c:m243-b014-lane-b-readiness": "npm run check:objc3c:m243-b012-lane-b-readiness',
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m243/M243-B014/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_integration_closeout_and_gate_signoff_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit the summary JSON to stdout in addition to writing --summary-out.",
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise FileNotFoundError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        total_checks += 1
        try:
            text = load_text(path, artifact=artifact)
            passed_checks += 1
        except FileNotFoundError as exc:
            findings.append(
                Finding(
                    artifact,
                    f"M243-B014-MISSING-{artifact.upper()}",
                    str(exc),
                )
            )
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
