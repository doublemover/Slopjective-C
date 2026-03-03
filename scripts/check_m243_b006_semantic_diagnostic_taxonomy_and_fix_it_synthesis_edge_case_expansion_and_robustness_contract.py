#!/usr/bin/env python3
"""Fail-closed checker for M243-B006 semantic diagnostic taxonomy edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-b006-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-expansion-and-robustness-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_b006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_packet.md",
    "b005_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_b005_expectations.md",
    "b005_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_packet.md",
    "b005_checker": ROOT
    / "scripts"
    / "check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py",
    "b005_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "robustness_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-B006-DOC-EXP-01",
            "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Edge-Case Expansion and Robustness Expectations (B006)",
        ),
        (
            "M243-B006-DOC-EXP-02",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-expansion-and-robustness/m243-b006-v1`",
        ),
        ("M243-B006-DOC-EXP-03", "- Dependencies: `M243-B005`"),
        (
            "M243-B006-DOC-EXP-04",
            "`scripts/check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py`",
        ),
        ("M243-B006-DOC-EXP-05", "Code/spec anchors and milestone optimization"),
        ("M243-B006-DOC-EXP-06", "improvements are mandatory scope inputs."),
        ("M243-B006-DOC-EXP-07", "`check:objc3c:m243-b006-lane-b-readiness`"),
        (
            "M243-B006-DOC-EXP-08",
            "`tmp/reports/m243/M243-B006/semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_summary.json`",
        ),
    ),
    "packet_doc": (
        (
            "M243-B006-DOC-PKT-01",
            "# M243-B006 Semantic Diagnostic Taxonomy and Fix-it Synthesis Edge-Case Expansion and Robustness Packet",
        ),
        ("M243-B006-DOC-PKT-02", "Packet: `M243-B006`"),
        ("M243-B006-DOC-PKT-03", "Dependencies: `M243-B005`"),
        (
            "M243-B006-DOC-PKT-04",
            "`scripts/check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py`",
        ),
        ("M243-B006-DOC-PKT-05", "`check:objc3c:m243-b006-lane-b-readiness`"),
        ("M243-B006-DOC-PKT-06", "`test:objc3c:lowering-regression`"),
        ("M243-B006-DOC-PKT-07", "code/spec anchors and milestone"),
        ("M243-B006-DOC-PKT-08", "optimization improvements as mandatory"),
    ),
    "b005_expectations_doc": (
        (
            "M243-B006-B005-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-compatibility-completion/m243-b005-v1`",
        ),
    ),
    "b005_packet_doc": (
        ("M243-B006-B005-PKT-01", "Packet: `M243-B005`"),
        ("M243-B006-B005-PKT-02", "Dependencies: `M243-B004`"),
    ),
    "b005_checker": (
        (
            "M243-B006-B005-CHK-01",
            'MODE = "m243-b005-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-compatibility-completion-contract-v1"',
        ),
    ),
    "b005_test": (
        (
            "M243-B006-B005-TST-01",
            "check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-B006-TYP-01",
            "struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface {",
        ),
        ("M243-B006-TYP-02", "bool edge_case_expansion_consistent = false;"),
        ("M243-B006-TYP-03", "bool edge_case_robustness_ready = false;"),
        (
            "M243-B006-TYP-04",
            "semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface;",
        ),
    ),
    "robustness_surface_header": (
        (
            "M243-B006-SUR-01",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseRobustnessKey(",
        ),
        (
            "M243-B006-SUR-02",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(",
        ),
        ("M243-B006-SUR-03", "surface.edge_case_expansion_consistent ="),
        ("M243-B006-SUR-04", "surface.edge_case_robustness_ready ="),
        ("M243-B006-SUR-05", "surface.edge_case_robustness_key ="),
        (
            "M243-B006-SUR-06",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseRobustnessKey(",
        ),
        ("M243-B006-SUR-07", "surface.edge_case_compatibility_ready &&"),
        ("M243-B006-SUR-08", "surface.edge_case_expansion_consistent &&"),
        ("M243-B006-SUR-09", "!surface.edge_case_robustness_key.empty()"),
        (
            "M243-B006-SUR-10",
            "semantic diagnostic taxonomy/fix-it edge-case expansion is inconsistent",
        ),
        (
            "M243-B006-SUR-11",
            "semantic diagnostic taxonomy/fix-it edge-case robustness is not ready",
        ),
        (
            "M243-B006-SUR-12",
            "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurfaceReady(",
        ),
        (
            "M243-B006-SUR-13",
            "semantic diagnostic taxonomy/fix-it edge-case compatibility is not ready",
        ),
    ),
    "frontend_pipeline_source": (
        (
            "M243-B006-PIP-01",
            '#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"',
        ),
        (
            "M243-B006-PIP-02",
            "result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface =",
        ),
        (
            "M243-B006-PIP-03",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(",
        ),
    ),
    "architecture_doc": (
        (
            "M243-B006-ARCH-01",
            "M243 lane-B B006 semantic diagnostic taxonomy/fix-it synthesis edge-case expansion and robustness",
        ),
        (
            "M243-B006-ARCH-02",
            "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h",
        ),
    ),
    "lowering_spec": (
        (
            "M243-B006-SPC-01",
            "semantic diagnostic taxonomy and fix-it synthesis edge-case expansion and robustness",
        ),
        (
            "M243-B006-SPC-02",
            "lane-B dependency anchors (`M243-B005`) and fail closed on",
        ),
    ),
    "metadata_spec": (
        (
            "M243-B006-META-01",
            "deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis",
        ),
        (
            "M243-B006-META-02",
            "edge-case expansion and robustness metadata anchors for `M243-B006` with",
        ),
        (
            "M243-B006-META-03",
            "explicit `M243-B005` dependency continuity so edge-case expansion",
        ),
    ),
    "package_json": (
        (
            "M243-B006-PKG-01",
            '"check:objc3c:m243-b006-semantic-diagnostic-taxonomy-and-fix-it-synthesis-edge-case-expansion-and-robustness-contract": '
            '"python scripts/check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py"',
        ),
        (
            "M243-B006-PKG-02",
            '"test:tooling:m243-b006-semantic-diagnostic-taxonomy-and-fix-it-synthesis-edge-case-expansion-and-robustness-contract": '
            '"python -m pytest tests/tooling/test_check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py -q"',
        ),
        (
            "M243-B006-PKG-03",
            '"check:objc3c:m243-b006-lane-b-readiness": '
            '"npm run check:objc3c:m243-b005-lane-b-readiness '
            '&& npm run check:objc3c:m243-b006-semantic-diagnostic-taxonomy-and-fix-it-synthesis-edge-case-expansion-and-robustness-contract '
            '&& npm run test:tooling:m243-b006-semantic-diagnostic-taxonomy-and-fix-it-synthesis-edge-case-expansion-and-robustness-contract"',
        ),
        ("M243-B006-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
        ("M243-B006-PKG-05", '"test:objc3c:lowering-regression": '),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "robustness_surface_header": (
        ("M243-B006-FORB-01", "surface.edge_case_robustness_ready = true;"),
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
            "tmp/reports/m243/M243-B006/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_summary.json"
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
