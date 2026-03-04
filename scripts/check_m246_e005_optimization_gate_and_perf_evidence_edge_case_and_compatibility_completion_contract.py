#!/usr/bin/env python3
"""Fail-closed checker for M246-E005 optimization gate/perf edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e005-optimization-gate-perf-evidence-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_e005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e005_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_E004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_core_feature_expansion_e004_expectations.md"
)
DEFAULT_A004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_core_feature_expansion_a004_expectations.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E005/optimization_gate_perf_evidence_edge_case_compatibility_completion_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M246-E005-DEP-E004-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_core_feature_expansion_e004_expectations.md"),
    ),
    AssetCheck(
        "M246-E005-DEP-E004-02",
        Path("spec/planning/compiler/m246/m246_e004_optimization_gate_and_perf_evidence_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M246-E005-DEP-E004-03",
        Path("scripts/check_m246_e004_optimization_gate_and_perf_evidence_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E005-DEP-E004-04",
        Path("tests/tooling/test_check_m246_e004_optimization_gate_and_perf_evidence_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E005-DEP-A004-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_core_feature_expansion_a004_expectations.md"),
    ),
    AssetCheck(
        "M246-E005-DEP-A004-02",
        Path("spec/planning/compiler/m246/m246_a004_frontend_optimization_hint_capture_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M246-E005-DEP-A004-03",
        Path("scripts/check_m246_a004_frontend_optimization_hint_capture_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E005-DEP-A004-04",
        Path("tests/tooling/test_check_m246_a004_frontend_optimization_hint_capture_core_feature_expansion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Edge-Case and Compatibility Completion Expectations (E005)",
    ),
    SnippetCheck(
        "M246-E005-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-edge-case-and-compatibility-completion/m246-e005-v1`",
    ),
    SnippetCheck("M246-E005-DOC-EXP-03", "`M246-E004`"),
    SnippetCheck("M246-E005-DOC-EXP-04", "`M246-A004`"),
    SnippetCheck("M246-E005-DOC-EXP-05", "`M246-B005`"),
    SnippetCheck(
        "M246-E005-DOC-EXP-06",
        "| `M246-C009` | Dependency token `M246-C009` is mandatory as pending seeded lane-C edge-case completion assets. |",
    ),
    SnippetCheck("M246-E005-DOC-EXP-07", "`M246-D004`"),
    SnippetCheck(
        "M246-E005-DOC-EXP-08",
        "`check:objc3c:m246-e005-optimization-gate-perf-evidence-edge-case-and-compatibility-completion-contract`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-DOC-PKT-01",
        "# M246-E005 Optimization Gate and Perf Evidence Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M246-E005-DOC-PKT-02", "Packet: `M246-E005`"),
    SnippetCheck("M246-E005-DOC-PKT-03", "Issue: `#6696`"),
    SnippetCheck(
        "M246-E005-DOC-PKT-04",
        "Dependencies: `M246-E004`, `M246-A004`, `M246-B005`, `M246-C009`, `M246-D004`",
    ),
    SnippetCheck("M246-E005-DOC-PKT-05", "Pending seeded dependency tokens:"),
)

E004_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-E004-01",
        "# M246 Optimization Gate and Perf Evidence Core Feature Expansion Expectations (E004)",
    ),
)

A004_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-A004-01",
        "# M246 Frontend Optimization Hint Capture Core Feature Expansion Expectations (A004)",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-ARCH-01",
        "M246 lane-E E005 optimization gate and perf evidence edge-case and compatibility completion anchors",
    ),
    SnippetCheck(
        "M246-E005-ARCH-02",
        "`M246-E004`, `M246-A004`, `M246-B005`, `M246-C009`, and `M246-D004`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-SPC-01",
        "optimization gate and perf evidence edge-case and compatibility completion wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E005-SPC-02",
        "`M246-E004`, `M246-A004`, `M246-B005`,",
    ),
    SnippetCheck(
        "M246-E005-SPC-03",
        "`M246-C009`, and `M246-D004`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case and compatibility completion dependency anchors for",
    ),
    SnippetCheck(
        "M246-E005-META-02",
        "`M246-E004`, `M246-A004`, `M246-B005`, `M246-C009`, and `M246-D004`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E005-PKG-01",
        '"check:objc3c:m246-e005-optimization-gate-perf-evidence-edge-case-and-compatibility-completion-contract": '
        '"python scripts/check_m246_e005_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_contract.py"',
    ),
    SnippetCheck(
        "M246-E005-PKG-02",
        '"test:tooling:m246-e005-optimization-gate-perf-evidence-edge-case-and-compatibility-completion-contract": '
        '"python -m pytest tests/tooling/test_check_m246_e005_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_contract.py -q"',
    ),
    SnippetCheck(
        "M246-E005-PKG-03",
        '"check:objc3c:m246-e005-lane-e-readiness": '
        '"npm run check:objc3c:m246-e004-lane-e-readiness '
        '&& npm run check:objc3c:m246-a004-lane-a-readiness '
        '&& npm run check:objc3c:m246-b005-lane-b-readiness '
        '&& npm run check:objc3c:m246-c009-lane-c-readiness '
        '&& npm run check:objc3c:m246-d004-lane-d-readiness '
        '&& npm run check:objc3c:m246-e005-optimization-gate-perf-evidence-edge-case-and-compatibility-completion-contract '
        '&& npm run test:tooling:m246-e005-optimization-gate-perf-evidence-edge-case-and-compatibility-completion-contract"',
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
    parser.add_argument("--e004-expectations-doc", type=Path, default=DEFAULT_E004_EXPECTATIONS_DOC)
    parser.add_argument("--a004-expectations-doc", type=Path, default=DEFAULT_A004_EXPECTATIONS_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M246-E005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e004_expectations_doc, "M246-E005-E004-DOC-EXISTS", E004_SNIPPETS),
        (args.a004_expectations_doc, "M246-E005-A004-DOC-EXISTS", A004_SNIPPETS),
        (args.architecture_doc, "M246-E005-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E005-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E005-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E005-PKG-EXISTS", PACKAGE_SNIPPETS),
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
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
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

