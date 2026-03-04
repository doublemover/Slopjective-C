#!/usr/bin/env python3
"""Fail-closed checker for M246-E006 optimization gate/perf edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e006-optimization-gate-perf-evidence-edge-case-expansion-and-robustness-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_e006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_E005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_e005_expectations.md"
)
DEFAULT_A005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_a005_expectations.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E006/optimization_gate_perf_evidence_edge_case_expansion_robustness_summary.json"
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
        "M246-E006-DEP-E005-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_e005_expectations.md"),
    ),
    AssetCheck(
        "M246-E006-DEP-E005-02",
        Path("spec/planning/compiler/m246/m246_e005_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M246-E006-DEP-E005-03",
        Path("scripts/check_m246_e005_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E006-DEP-E005-04",
        Path("tests/tooling/test_check_m246_e005_optimization_gate_and_perf_evidence_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E006-DEP-A005-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_a005_expectations.md"),
    ),
    AssetCheck(
        "M246-E006-DEP-A005-02",
        Path("spec/planning/compiler/m246/m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M246-E006-DEP-A005-03",
        Path("scripts/check_m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E006-DEP-A005-04",
        Path("tests/tooling/test_check_m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Edge-Case Expansion and Robustness Expectations (E006)",
    ),
    SnippetCheck(
        "M246-E006-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-edge-case-expansion-and-robustness/m246-e006-v1`",
    ),
    SnippetCheck("M246-E006-DOC-EXP-03", "`M246-E005`"),
    SnippetCheck("M246-E006-DOC-EXP-04", "`M246-A005`"),
    SnippetCheck("M246-E006-DOC-EXP-05", "`M246-B006`"),
    SnippetCheck(
        "M246-E006-DOC-EXP-06",
        "| `M246-C011` | Dependency token `M246-C011` is mandatory as pending seeded lane-C edge-case completion assets. |",
    ),
    SnippetCheck("M246-E006-DOC-EXP-07", "`M246-D005`"),
    SnippetCheck(
        "M246-E006-DOC-EXP-08",
        "`check:objc3c:m246-e006-optimization-gate-perf-evidence-edge-case-expansion-and-robustness-contract`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-DOC-PKT-01",
        "# M246-E006 Optimization Gate and Perf Evidence Edge-Case Expansion and Robustness Packet",
    ),
    SnippetCheck("M246-E006-DOC-PKT-02", "Packet: `M246-E006`"),
    SnippetCheck("M246-E006-DOC-PKT-03", "Issue: `#6697`"),
    SnippetCheck(
        "M246-E006-DOC-PKT-04",
        "Dependencies: `M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, `M246-D005`",
    ),
    SnippetCheck("M246-E006-DOC-PKT-05", "Pending seeded dependency tokens:"),
)

E005_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-E005-01",
        "# M246 Optimization Gate and Perf Evidence Edge-Case and Compatibility Completion Expectations (E005)",
    ),
)

A005_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-A005-01",
        "# M246 Frontend Optimization Hint Capture Edge-Case and Compatibility Completion Expectations (A005)",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-ARCH-01",
        "M246 lane-E E006 optimization gate and perf evidence edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M246-E006-ARCH-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-SPC-01",
        "optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E006-SPC-02",
        "`M246-E005`, `M246-A005`, `M246-B006`,",
    ),
    SnippetCheck(
        "M246-E006-SPC-03",
        "`M246-C011`, and `M246-D005`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for",
    ),
    SnippetCheck(
        "M246-E006-META-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E006-PKG-01",
        '"check:objc3c:m246-e006-optimization-gate-perf-evidence-edge-case-expansion-and-robustness-contract": '
        '"python scripts/check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py"',
    ),
    SnippetCheck(
        "M246-E006-PKG-02",
        '"test:tooling:m246-e006-optimization-gate-perf-evidence-edge-case-expansion-and-robustness-contract": '
        '"python -m pytest tests/tooling/test_check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py -q"',
    ),
    SnippetCheck(
        "M246-E006-PKG-03",
        '"check:objc3c:m246-e006-lane-e-readiness": '
        '"npm run check:objc3c:m246-e005-lane-e-readiness '
        '&& npm run check:objc3c:m246-a005-lane-a-readiness '
        '&& npm run check:objc3c:m246-b006-lane-b-readiness '
        '&& npm run check:objc3c:m246-c011-lane-c-readiness '
        '&& npm run check:objc3c:m246-d005-lane-d-readiness '
        '&& npm run check:objc3c:m246-e006-optimization-gate-perf-evidence-edge-case-expansion-and-robustness-contract '
        '&& npm run test:tooling:m246-e006-optimization-gate-perf-evidence-edge-case-expansion-and-robustness-contract"',
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
    parser.add_argument("--e005-expectations-doc", type=Path, default=DEFAULT_E005_EXPECTATIONS_DOC)
    parser.add_argument("--a005-expectations-doc", type=Path, default=DEFAULT_A005_EXPECTATIONS_DOC)
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
        (args.expectations_doc, "M246-E006-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E006-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e005_expectations_doc, "M246-E006-E005-DOC-EXISTS", E005_SNIPPETS),
        (args.a005_expectations_doc, "M246-E006-A005-DOC-EXISTS", A005_SNIPPETS),
        (args.architecture_doc, "M246-E006-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E006-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E006-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E006-PKG-EXISTS", PACKAGE_SNIPPETS),
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

