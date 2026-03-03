#!/usr/bin/env python3
"""Fail-closed checker for M243-B009 semantic diagnostic taxonomy conformance matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-b009-semantic-diagnostic-taxonomy-and-fixit-synthesis-"
    "conformance-matrix-implementation-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_b009_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_packet.md",
    "b008_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_b008_expectations.md",
    "b008_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_packet.md",
    "b008_checker": ROOT
    / "scripts"
    / "check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py",
    "b008_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "conformance_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-B009-DOC-EXP-01",
            "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Conformance Matrix Implementation Expectations (B009)",
        ),
        (
            "M243-B009-DOC-EXP-02",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-conformance-matrix-implementation/m243-b009-v1`",
        ),
        ("M243-B009-DOC-EXP-03", "- Dependencies: `M243-B008`"),
        (
            "M243-B009-DOC-EXP-04",
            "`scripts/check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py`",
        ),
        (
            "M243-B009-DOC-EXP-05",
            "`scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`",
        ),
        (
            "M243-B009-DOC-EXP-06",
            "`tests/tooling/test_check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`",
        ),
        (
            "M243-B009-DOC-EXP-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-B009-DOC-EXP-08",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationKey(...)",
        ),
        (
            "M243-B009-DOC-EXP-09",
            "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurfaceReady(...)",
        ),
        (
            "M243-B009-DOC-EXP-10",
            "`tmp/reports/m243/M243-B009/semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_summary.json`",
        ),
    ),
    "packet_doc": (
        (
            "M243-B009-DOC-PKT-01",
            "# M243-B009 Semantic Diagnostic Taxonomy and Fix-it Synthesis Conformance Matrix Implementation Packet",
        ),
        ("M243-B009-DOC-PKT-02", "Packet: `M243-B009`"),
        ("M243-B009-DOC-PKT-03", "Dependencies: `M243-B008`"),
        (
            "M243-B009-DOC-PKT-04",
            "`scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`",
        ),
        (
            "M243-B009-DOC-PKT-05",
            "`tests/tooling/test_check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`",
        ),
        ("M243-B009-DOC-PKT-06", "`check:objc3c:m243-b009-lane-b-readiness`"),
        ("M243-B009-DOC-PKT-07", "`test:objc3c:sema-pass-manager-diagnostics-bus`"),
        ("M243-B009-DOC-PKT-08", "`test:objc3c:lowering-regression`"),
    ),
    "b008_expectations_doc": (
        (
            "M243-B009-B008-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-recovery-determinism-hardening/m243-b008-v1`",
        ),
    ),
    "b008_packet_doc": (
        ("M243-B009-B008-PKT-01", "Packet: `M243-B008`"),
        ("M243-B009-B008-PKT-02", "Dependencies: `M243-B007`"),
    ),
    "b008_checker": (
        (
            "M243-B009-B008-CHK-01",
            'MODE = (\n    "m243-b008-semantic-diagnostic-taxonomy-and-fixit-synthesis-"\n    "recovery-determinism-hardening-contract-v1"\n)',
        ),
    ),
    "b008_test": (
        (
            "M243-B009-B008-TST-01",
            "check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-B009-TYP-01",
            "struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface {",
        ),
        ("M243-B009-TYP-02", "bool conformance_matrix_consistent = false;"),
        ("M243-B009-TYP-03", "bool conformance_matrix_ready = false;"),
        ("M243-B009-TYP-04", "std::string conformance_matrix_key;"),
        (
            "M243-B009-TYP-05",
            "semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface;",
        ),
    ),
    "conformance_surface_header": (
        (
            "M243-B009-SUR-01",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationKey(",
        ),
        (
            "M243-B009-SUR-02",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(",
        ),
        ("M243-B009-SUR-03", "const bool conformance_matrix_consistent ="),
        ("M243-B009-SUR-04", "const bool conformance_matrix_ready ="),
        ("M243-B009-SUR-05", "surface.conformance_matrix_consistent ="),
        ("M243-B009-SUR-06", "surface.conformance_matrix_ready ="),
        ("M243-B009-SUR-07", "surface.conformance_matrix_key ="),
        ("M243-B009-SUR-08", ";conformance-matrix-consistent="),
        ("M243-B009-SUR-09", ";conformance-matrix-ready="),
        ("M243-B009-SUR-10", ";conformance-matrix-key-ready="),
        (
            "M243-B009-SUR-11",
            "semantic diagnostic taxonomy/fix-it recovery determinism is not ready",
        ),
        (
            "M243-B009-SUR-12",
            "semantic diagnostic taxonomy/fix-it conformance matrix is inconsistent",
        ),
        (
            "M243-B009-SUR-13",
            "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurfaceReady(",
        ),
    ),
    "frontend_pipeline_source": (
        (
            "M243-B009-PIP-01",
            '#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"',
        ),
        (
            "M243-B009-PIP-02",
            "result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface =",
        ),
        (
            "M243-B009-PIP-03",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(",
        ),
    ),
    "package_json": (
        (
            "M243-B009-PKG-01",
            '"check:objc3c:m243-b009-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-matrix-implementation-contract": ',
        ),
        (
            "M243-B009-PKG-02",
            '"test:tooling:m243-b009-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-matrix-implementation-contract": ',
        ),
        (
            "M243-B009-PKG-03",
            '"check:objc3c:m243-b009-lane-b-readiness": "npm run check:objc3c:m243-b008-lane-b-readiness',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "conformance_surface_header": (
        ("M243-B009-FORB-01", "const bool conformance_matrix_ready = true;"),
        ("M243-B009-FORB-02", "surface.conformance_matrix_ready = true;"),
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
            "tmp/reports/m243/M243-B009/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_summary.json"
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

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
