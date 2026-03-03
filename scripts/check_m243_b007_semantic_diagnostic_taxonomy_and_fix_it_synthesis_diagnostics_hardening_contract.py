#!/usr/bin/env python3
"""Fail-closed checker for M243-B007 semantic diagnostic taxonomy diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-b007-semantic-diagnostic-taxonomy-and-fixit-synthesis-diagnostics-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_b007_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_packet.md",
    "b006_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_b006_expectations.md",
    "b006_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_packet.md",
    "b006_checker": ROOT
    / "scripts"
    / "check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py",
    "b006_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "diagnostics_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-B007-DOC-EXP-01",
            "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Diagnostics Hardening Expectations (B007)",
        ),
        (
            "M243-B007-DOC-EXP-02",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-diagnostics-hardening/m243-b007-v1`",
        ),
        ("M243-B007-DOC-EXP-03", "- Dependencies: `M243-B006`"),
        (
            "M243-B007-DOC-EXP-04",
            "`scripts/check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py`",
        ),
        ("M243-B007-DOC-EXP-05", "Code/spec anchors and milestone optimization"),
        ("M243-B007-DOC-EXP-06", "improvements are mandatory scope inputs."),
        ("M243-B007-DOC-EXP-07", "`check:objc3c:m243-b007-lane-b-readiness`"),
        (
            "M243-B007-DOC-EXP-08",
            "`tmp/reports/m243/M243-B007/semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_summary.json`",
        ),
    ),
    "packet_doc": (
        (
            "M243-B007-DOC-PKT-01",
            "# M243-B007 Semantic Diagnostic Taxonomy and Fix-it Synthesis Diagnostics Hardening Packet",
        ),
        ("M243-B007-DOC-PKT-02", "Packet: `M243-B007`"),
        ("M243-B007-DOC-PKT-03", "Dependencies: `M243-B006`"),
        (
            "M243-B007-DOC-PKT-04",
            "`scripts/check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py`",
        ),
        ("M243-B007-DOC-PKT-05", "`check:objc3c:m243-b007-lane-b-readiness`"),
        ("M243-B007-DOC-PKT-06", "`test:objc3c:lowering-regression`"),
        ("M243-B007-DOC-PKT-07", "code/spec anchors and milestone"),
        ("M243-B007-DOC-PKT-08", "optimization improvements as mandatory"),
    ),
    "b006_expectations_doc": (
        (
            "M243-B007-B006-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-expansion-and-robustness/m243-b006-v1`",
        ),
    ),
    "b006_packet_doc": (
        ("M243-B007-B006-PKT-01", "Packet: `M243-B006`"),
        ("M243-B007-B006-PKT-02", "Dependencies: `M243-B005`"),
    ),
    "b006_checker": (
        (
            "M243-B007-B006-CHK-01",
            'MODE = "m243-b006-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-expansion-and-robustness-contract-v1"',
        ),
    ),
    "b006_test": (
        (
            "M243-B007-B006-TST-01",
            "check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-B007-TYP-01",
            "struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface {",
        ),
        ("M243-B007-TYP-02", "bool diagnostics_hardening_consistent = false;"),
        ("M243-B007-TYP-03", "bool diagnostics_hardening_ready = false;"),
        (
            "M243-B007-TYP-04",
            "semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface;",
        ),
    ),
    "diagnostics_surface_header": (
        (
            "M243-B007-SUR-01",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningKey(",
        ),
        (
            "M243-B007-SUR-02",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(",
        ),
        ("M243-B007-SUR-03", "const bool diagnostics_hardening_consistent ="),
        ("M243-B007-SUR-04", "const bool diagnostics_hardening_ready ="),
        ("M243-B007-SUR-05", "surface.diagnostics_hardening_consistent ="),
        ("M243-B007-SUR-06", "surface.diagnostics_hardening_ready ="),
        ("M243-B007-SUR-07", "surface.diagnostics_hardening_key ="),
        ("M243-B007-SUR-08", ";diag-hardening-consistent="),
        ("M243-B007-SUR-09", ";diag-hardening-ready="),
        ("M243-B007-SUR-10", ";diag-hardening-key-ready="),
        (
            "M243-B007-SUR-11",
            "semantic diagnostic taxonomy/fix-it diagnostics hardening is inconsistent",
        ),
        (
            "M243-B007-SUR-12",
            "semantic diagnostic taxonomy/fix-it diagnostics hardening is not ready",
        ),
        (
            "M243-B007-SUR-13",
            "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurfaceReady(",
        ),
        (
            "M243-B007-SUR-14",
            "semantic diagnostic taxonomy/fix-it edge-case robustness is not ready",
        ),
    ),
    "frontend_pipeline_source": (
        (
            "M243-B007-PIP-01",
            '#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"',
        ),
        (
            "M243-B007-PIP-02",
            "result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface =",
        ),
        (
            "M243-B007-PIP-03",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(",
        ),
    ),
    "architecture_doc": (
        (
            "M243-B007-ARCH-01",
            "M243 lane-B B007 semantic diagnostic taxonomy/fix-it synthesis diagnostics hardening",
        ),
        (
            "M243-B007-ARCH-02",
            "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h",
        ),
    ),
    "lowering_spec": (
        (
            "M243-B007-SPC-01",
            "semantic diagnostic taxonomy and fix-it synthesis diagnostics hardening",
        ),
        (
            "M243-B007-SPC-02",
            "lane-B dependency anchors (`M243-B006`) and fail closed on",
        ),
    ),
    "metadata_spec": (
        (
            "M243-B007-META-01",
            "deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis",
        ),
        (
            "M243-B007-META-02",
            "diagnostics hardening metadata anchors for `M243-B007` with",
        ),
        (
            "M243-B007-META-03",
            "explicit `M243-B006` dependency continuity so diagnostics hardening",
        ),
    ),
    "package_json": (
        (
            "M243-B007-PKG-01",
            '"check:objc3c:m243-b007-semantic-diagnostic-taxonomy-and-fix-it-synthesis-diagnostics-hardening-contract": '
            '"python scripts/check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py"',
        ),
        (
            "M243-B007-PKG-02",
            '"test:tooling:m243-b007-semantic-diagnostic-taxonomy-and-fix-it-synthesis-diagnostics-hardening-contract": '
            '"python -m pytest tests/tooling/test_check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py -q"',
        ),
        (
            "M243-B007-PKG-03",
            '"check:objc3c:m243-b007-lane-b-readiness": '
            '"npm run check:objc3c:m243-b006-lane-b-readiness '
            '&& npm run check:objc3c:m243-b007-semantic-diagnostic-taxonomy-and-fix-it-synthesis-diagnostics-hardening-contract '
            '&& npm run test:tooling:m243-b007-semantic-diagnostic-taxonomy-and-fix-it-synthesis-diagnostics-hardening-contract"',
        ),
        ("M243-B007-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
        ("M243-B007-PKG-05", '"test:objc3c:lowering-regression": '),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "diagnostics_surface_header": (
        ("M243-B007-FORB-01", "const bool diagnostics_hardening_ready = true;"),
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
            "tmp/reports/m243/M243-B007/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
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

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
