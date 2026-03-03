#!/usr/bin/env python3
"""Fail-closed checker for M243-B012 semantic diagnostic taxonomy cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-b012-semantic-diagnostic-taxonomy-and-fixit-synthesis-"
    "cross-lane-integration-sync-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_b012_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_packet.md",
    "b011_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_b011_expectations.md",
    "b011_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_packet.md",
    "b011_checker": ROOT
    / "scripts"
    / "check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py",
    "b011_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-B012-DOC-EXP-01",
            "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Cross-Lane Integration Sync Expectations (B012)",
        ),
        (
            "M243-B012-DOC-EXP-02",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-cross-lane-integration-sync/m243-b012-v1`",
        ),
        ("M243-B012-DOC-EXP-03", "- Dependencies: `M243-B011`"),
        (
            "M243-B012-DOC-EXP-04",
            "`scripts/check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py`",
        ),
        (
            "M243-B012-DOC-EXP-05",
            "`scripts/check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py`",
        ),
        (
            "M243-B012-DOC-EXP-06",
            "`tests/tooling/test_check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py`",
        ),
        (
            "M243-B012-DOC-EXP-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        ("M243-B012-DOC-EXP-08", "`check:objc3c:m243-b012-lane-b-readiness`"),
        (
            "M243-B012-DOC-EXP-09",
            "`tmp/reports/m243/M243-B012/semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_summary.json`",
        ),
    ),
    "packet_doc": (
        (
            "M243-B012-DOC-PKT-01",
            "# M243-B012 Semantic Diagnostic Taxonomy and Fix-it Synthesis Cross-Lane Integration Sync Packet",
        ),
        ("M243-B012-DOC-PKT-02", "Packet: `M243-B012`"),
        ("M243-B012-DOC-PKT-03", "Dependencies: `M243-B011`"),
        (
            "M243-B012-DOC-PKT-04",
            "`scripts/check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py`",
        ),
        (
            "M243-B012-DOC-PKT-05",
            "`tests/tooling/test_check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py`",
        ),
        ("M243-B012-DOC-PKT-06", "`check:objc3c:m243-b012-lane-b-readiness`"),
        ("M243-B012-DOC-PKT-07", "`test:objc3c:sema-pass-manager-diagnostics-bus`"),
        ("M243-B012-DOC-PKT-08", "`test:objc3c:lowering-regression`"),
    ),
    "b011_expectations_doc": (
        (
            "M243-B012-B011-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-performance-quality-guardrails/m243-b011-v1`",
        ),
    ),
    "b011_packet_doc": (
        ("M243-B012-B011-PKT-01", "Packet: `M243-B011`"),
        ("M243-B012-B011-PKT-02", "Dependencies: `M243-B010`"),
    ),
    "b011_checker": (
        (
            "M243-B012-B011-CHK-01",
            'MODE = (\n    "m243-b011-semantic-diagnostic-taxonomy-and-fixit-synthesis-"\n    "performance-quality-guardrails-contract-v1"\n)',
        ),
    ),
    "b011_test": (
        (
            "M243-B012-B011-TST-01",
            "check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M243-B012-ARCH-01",
            "M243 lane-B B012 semantic diagnostic taxonomy/fix-it synthesis cross-lane integration sync",
        ),
        (
            "M243-B012-ARCH-02",
            "`check:objc3c:m243-b012-lane-b-readiness`",
        ),
    ),
    "lowering_spec": (
        (
            "M243-B012-SPC-01",
            "semantic diagnostic taxonomy and fix-it synthesis cross-lane integration sync",
        ),
        (
            "M243-B012-SPC-02",
            "lane-B dependency anchors (`M243-B011`) and fail closed on",
        ),
    ),
    "metadata_spec": (
        (
            "M243-B012-META-01",
            "deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis",
        ),
        (
            "M243-B012-META-02",
            "cross-lane integration sync metadata anchors for `M243-B012` with",
        ),
        (
            "M243-B012-META-03",
            "explicit `M243-B011` dependency continuity so cross-lane integration sync",
        ),
    ),
    "package_json": (
        (
            "M243-B012-PKG-01",
            '"check:objc3c:m243-b012-semantic-diagnostic-taxonomy-and-fix-it-synthesis-cross-lane-integration-sync-contract": ',
        ),
        (
            "M243-B012-PKG-02",
            '"test:tooling:m243-b012-semantic-diagnostic-taxonomy-and-fix-it-synthesis-cross-lane-integration-sync-contract": ',
        ),
        (
            "M243-B012-PKG-03",
            '"check:objc3c:m243-b012-lane-b-readiness": "npm run check:objc3c:m243-b011-lane-b-readiness',
        ),
        ("M243-B012-PKG-04", '"test:objc3c:lowering-regression": '),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M243-B012-FORB-01",
            '"check:objc3c:m243-b012-lane-b-readiness": "npm run check:objc3c:m243-b010-lane-b-readiness',
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
            "tmp/reports/m243/M243-B012/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_summary.json"
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
                    f"M243-B012-MISSING-{artifact.upper()}",
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
