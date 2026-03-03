#!/usr/bin/env python3
"""Fail-closed contract checker for M243-B003 semantic diagnostic taxonomy core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-b003-semantic-diagnostic-taxonomy-and-fixit-synthesis-core-feature-implementation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_b003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_packet.md"
)
DEFAULT_B002_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_b002_expectations.md"
)
DEFAULT_B002_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_packet.md"
)
DEFAULT_B002_CHECKER = (
    ROOT / "scripts" / "check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py"
)
DEFAULT_B002_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py"
)
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_CORE_SURFACE_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
)
DEFAULT_FRONTEND_PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m243/M243-B003/semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_summary.json"
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
        "M243-B003-DOC-EXP-01",
        "# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Core Feature Implementation Expectations (B003)",
    ),
    SnippetCheck(
        "M243-B003-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-core-feature-implementation/m243-b003-v1`",
    ),
    SnippetCheck("M243-B003-DOC-EXP-03", "- Dependencies: `M243-B002`"),
    SnippetCheck(
        "M243-B003-DOC-EXP-04",
        "`scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py`",
    ),
    SnippetCheck(
        "M243-B003-DOC-EXP-05",
        "Code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M243-B003-DOC-EXP-06",
        "improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M243-B003-DOC-EXP-07",
        "`check:objc3c:m243-b003-lane-b-readiness`",
    ),
    SnippetCheck(
        "M243-B003-DOC-EXP-08",
        "`tmp/reports/m243/M243-B003/semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-DOC-PKT-01",
        "# M243-B003 Semantic Diagnostic Taxonomy and Fix-it Synthesis Core Feature Implementation Packet",
    ),
    SnippetCheck("M243-B003-DOC-PKT-02", "Packet: `M243-B003`"),
    SnippetCheck("M243-B003-DOC-PKT-03", "Dependencies: `M243-B002`"),
    SnippetCheck(
        "M243-B003-DOC-PKT-04",
        "`scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`",
    ),
    SnippetCheck(
        "M243-B003-DOC-PKT-05",
        "`check:objc3c:m243-b003-lane-b-readiness`",
    ),
    SnippetCheck(
        "M243-B003-DOC-PKT-06",
        "`test:objc3c:lowering-regression`",
    ),
    SnippetCheck(
        "M243-B003-DOC-PKT-07",
        "code/spec anchors and milestone",
    ),
    SnippetCheck(
        "M243-B003-DOC-PKT-08",
        "optimization improvements as mandatory scope inputs.",
    ),
)

B002_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-B002-DOC-01",
        "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-modular-split-scaffolding/m243-b002-v1`",
    ),
)

B002_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M243-B003-B002-PKT-01", "Packet: `M243-B002`"),
    SnippetCheck("M243-B003-B002-PKT-02", "Dependencies: `M243-B001`"),
    SnippetCheck(
        "M243-B003-B002-PKT-03",
        "scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py",
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-TYP-01",
        "struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface {",
    ),
    SnippetCheck("M243-B003-TYP-02", "bool core_feature_impl_ready = false;"),
    SnippetCheck(
        "M243-B003-TYP-03",
        "semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface;",
    ),
)

CORE_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-SUR-01",
        "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationKey(",
    ),
    SnippetCheck(
        "M243-B003-SUR-02",
        "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(",
    ),
    SnippetCheck(
        "M243-B003-SUR-03",
        "surface.diagnostics_case_accounting_consistent =",
    ),
    SnippetCheck(
        "M243-B003-SUR-04",
        "surface.arc_diagnostics_fixit_case_accounting_consistent =",
    ),
    SnippetCheck(
        "M243-B003-SUR-05",
        "surface.replay_keys_ready =",
    ),
    SnippetCheck(
        "M243-B003-SUR-06",
        "surface.core_feature_impl_ready =",
    ),
    SnippetCheck(
        "M243-B003-SUR-07",
        "semantic diagnostics case accounting is inconsistent",
    ),
    SnippetCheck(
        "M243-B003-SUR-08",
        "IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurfaceReady(",
    ),
)

FRONTEND_PIPELINE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-PIP-01",
        '#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"',
    ),
    SnippetCheck(
        "M243-B003-PIP-02",
        "result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface =",
    ),
    SnippetCheck(
        "M243-B003-PIP-03",
        "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-ARCH-01",
        "M243 lane-B B003 semantic diagnostic taxonomy/fix-it synthesis core feature",
    ),
    SnippetCheck(
        "M243-B003-ARCH-02",
        "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-SPC-01",
        "semantic diagnostic taxonomy and fix-it synthesis core feature implementation",
    ),
    SnippetCheck(
        "M243-B003-SPC-02",
        "lane-B dependency anchors (`M243-B002`) and fail closed on",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-META-01",
        "deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis core",
    ),
    SnippetCheck(
        "M243-B003-META-02",
        "`M243-B003` with explicit `M243-B002`",
    ),
    SnippetCheck(
        "M243-B003-META-03",
        "dependency continuity so core feature implementation drift fails closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-B003-PKG-01",
        '"check:objc3c:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract": '
        '"python scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py"',
    ),
    SnippetCheck(
        "M243-B003-PKG-02",
        '"test:tooling:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract": '
        '"python -m pytest tests/tooling/test_check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py -q"',
    ),
    SnippetCheck(
        "M243-B003-PKG-03",
        '"check:objc3c:m243-b003-lane-b-readiness": '
        '"npm run check:objc3c:m243-b002-lane-b-readiness '
        '&& npm run check:objc3c:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract '
        '&& npm run test:tooling:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract"',
    ),
    SnippetCheck("M243-B003-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M243-B003-PKG-05", '"test:objc3c:lowering-regression": '),
)

FORBIDDEN_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M243-B003-FORB-01", "surface.core_feature_impl_ready = true;"),
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
    parser.add_argument("--b002-expectations-doc", type=Path, default=DEFAULT_B002_EXPECTATIONS_DOC)
    parser.add_argument("--b002-packet-doc", type=Path, default=DEFAULT_B002_PACKET_DOC)
    parser.add_argument("--b002-checker", type=Path, default=DEFAULT_B002_CHECKER)
    parser.add_argument("--b002-test", type=Path, default=DEFAULT_B002_TEST)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--core-surface-header", type=Path, default=DEFAULT_CORE_SURFACE_HEADER)
    parser.add_argument("--frontend-pipeline-source", type=Path, default=DEFAULT_FRONTEND_PIPELINE_SOURCE)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
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
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"forbidden snippet present: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets, forbidden in (
        (args.expectations_doc, "M243-B003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M243-B003-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.b002_expectations_doc, "M243-B003-B002-DOC-EXISTS", B002_EXPECTATIONS_SNIPPETS, ()),
        (args.b002_packet_doc, "M243-B003-B002-PKT-EXISTS", B002_PACKET_SNIPPETS, ()),
        (args.frontend_types, "M243-B003-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS, ()),
        (
            args.core_surface_header,
            "M243-B003-SUR-EXISTS",
            CORE_SURFACE_SNIPPETS,
            FORBIDDEN_SURFACE_SNIPPETS,
        ),
        (args.frontend_pipeline_source, "M243-B003-PIP-EXISTS", FRONTEND_PIPELINE_SNIPPETS, ()),
        (args.architecture_doc, "M243-B003-ARCH-EXISTS", ARCHITECTURE_SNIPPETS, ()),
        (args.lowering_spec, "M243-B003-SPC-EXISTS", LOWERING_SPEC_SNIPPETS, ()),
        (args.metadata_spec, "M243-B003-META-EXISTS", METADATA_SPEC_SNIPPETS, ()),
        (args.package_json, "M243-B003-PKG-EXISTS", PACKAGE_SNIPPETS, ()),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
            forbidden_snippets=forbidden,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b002_checker, "M243-B003-DEP-B002-ARG-01"),
        (args.b002_test, "M243-B003-DEP-B002-ARG-02"),
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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
