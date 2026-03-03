#!/usr/bin/env python3
"""Fail-closed checker for M243-B010 semantic diagnostic taxonomy conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-b010-semantic-diagnostic-taxonomy-and-fixit-synthesis-"
    "conformance-corpus-expansion-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_b010_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_packet.md",
    "b009_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_b009_expectations.md",
    "b009_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_packet.md",
    "b009_checker": ROOT
    / "scripts"
    / "check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py",
    "b009_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "conformance_corpus_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-B010-DOC-EXP-01",
            "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Conformance Corpus Expansion Expectations (B010)",
        ),
        (
            "M243-B010-DOC-EXP-02",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-conformance-corpus-expansion/m243-b010-v1`",
        ),
        ("M243-B010-DOC-EXP-03", "- Dependencies: `M243-B009`"),
        (
            "M243-B010-DOC-EXP-04",
            "`scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`",
        ),
        (
            "M243-B010-DOC-EXP-05",
            "`scripts/check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`",
        ),
        (
            "M243-B010-DOC-EXP-06",
            "`tests/tooling/test_check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`",
        ),
        (
            "M243-B010-DOC-EXP-07",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-B010-DOC-EXP-08",
            "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurfaceReady(...)",
        ),
        (
            "M243-B010-DOC-EXP-09",
            "`tmp/reports/m243/M243-B010/semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_summary.json`",
        ),
    ),
    "packet_doc": (
        (
            "M243-B010-DOC-PKT-01",
            "# M243-B010 Semantic Diagnostic Taxonomy and Fix-it Synthesis Conformance Corpus Expansion Packet",
        ),
        ("M243-B010-DOC-PKT-02", "Packet: `M243-B010`"),
        ("M243-B010-DOC-PKT-03", "Dependencies: `M243-B009`"),
        (
            "M243-B010-DOC-PKT-04",
            "`scripts/check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`",
        ),
        (
            "M243-B010-DOC-PKT-05",
            "`tests/tooling/test_check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`",
        ),
        ("M243-B010-DOC-PKT-06", "`check:objc3c:m243-b010-lane-b-readiness`"),
        ("M243-B010-DOC-PKT-07", "`test:objc3c:sema-pass-manager-diagnostics-bus`"),
        ("M243-B010-DOC-PKT-08", "`test:objc3c:lowering-regression`"),
    ),
    "b009_expectations_doc": (
        (
            "M243-B010-B009-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-conformance-matrix-implementation/m243-b009-v1`",
        ),
    ),
    "b009_packet_doc": (
        ("M243-B010-B009-PKT-01", "Packet: `M243-B009`"),
        ("M243-B010-B009-PKT-02", "Dependencies: `M243-B008`"),
    ),
    "b009_checker": (
        (
            "M243-B010-B009-CHK-01",
            'MODE = (\n    "m243-b009-semantic-diagnostic-taxonomy-and-fixit-synthesis-"\n    "conformance-matrix-implementation-contract-v1"\n)',
        ),
    ),
    "b009_test": (
        (
            "M243-B010-B009-TST-01",
            "check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-B010-TYP-01",
            "struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface {",
        ),
        ("M243-B010-TYP-02", "bool conformance_corpus_consistent = false;"),
        ("M243-B010-TYP-03", "bool conformance_corpus_ready = false;"),
        ("M243-B010-TYP-04", "std::string conformance_corpus_key;"),
        (
            "M243-B010-TYP-05",
            "semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface;",
        ),
    ),
    "conformance_corpus_surface_header": (
        (
            "M243-B010-SUR-01",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionKey(",
        ),
        (
            "M243-B010-SUR-02",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(",
        ),
        ("M243-B010-SUR-03", "const bool conformance_corpus_consistent ="),
        ("M243-B010-SUR-04", "const bool conformance_corpus_ready ="),
        ("M243-B010-SUR-05", "surface.conformance_corpus_consistent ="),
        ("M243-B010-SUR-06", "surface.conformance_corpus_ready ="),
        ("M243-B010-SUR-07", "surface.conformance_corpus_key ="),
        ("M243-B010-SUR-08", ";conformance-corpus-consistent="),
        ("M243-B010-SUR-09", ";conformance-corpus-ready="),
        ("M243-B010-SUR-10", ";conformance-corpus-key-ready="),
        (
            "M243-B010-SUR-11",
            "semantic diagnostic taxonomy/fix-it conformance corpus is inconsistent",
        ),
        (
            "M243-B010-SUR-12",
            "semantic diagnostic taxonomy/fix-it conformance corpus is not ready",
        ),
        (
            "M243-B010-SUR-13",
            "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurfaceReady(",
        ),
    ),
    "frontend_pipeline_source": (
        (
            "M243-B010-PIP-01",
            '#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"',
        ),
        (
            "M243-B010-PIP-02",
            "result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface =",
        ),
        (
            "M243-B010-PIP-03",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(",
        ),
    ),
    "architecture_doc": (
        (
            "M243-B010-ARCH-01",
            "M243 lane-B B010 semantic diagnostic taxonomy/fix-it synthesis conformance corpus expansion",
        ),
        (
            "M243-B010-ARCH-02",
            "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h",
        ),
    ),
    "lowering_spec": (
        (
            "M243-B010-SPC-01",
            "semantic diagnostic taxonomy and fix-it synthesis conformance corpus expansion",
        ),
        (
            "M243-B010-SPC-02",
            "lane-B dependency anchors (`M243-B009`) and fail closed on",
        ),
    ),
    "metadata_spec": (
        (
            "M243-B010-META-01",
            "deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis",
        ),
        (
            "M243-B010-META-02",
            "conformance corpus expansion metadata anchors for `M243-B010` with",
        ),
        (
            "M243-B010-META-03",
            "explicit `M243-B009` dependency continuity so conformance corpus",
        ),
    ),
    "package_json": (
        (
            "M243-B010-PKG-01",
            '"check:objc3c:m243-b010-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-corpus-expansion-contract": ',
        ),
        (
            "M243-B010-PKG-02",
            '"test:tooling:m243-b010-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-corpus-expansion-contract": ',
        ),
        (
            "M243-B010-PKG-03",
            '"check:objc3c:m243-b010-lane-b-readiness": "npm run check:objc3c:m243-b009-lane-b-readiness',
        ),
        ("M243-B010-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
        ("M243-B010-PKG-05", '"test:objc3c:lowering-regression": '),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "conformance_corpus_surface_header": (
        ("M243-B010-FORB-01", "const bool conformance_corpus_ready = true;"),
        ("M243-B010-FORB-02", "surface.conformance_corpus_ready = true;"),
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
            "tmp/reports/m243/M243-B010/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_summary.json"
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
