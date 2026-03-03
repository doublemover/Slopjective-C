#!/usr/bin/env python3
"""Fail-closed validator for M243-B002 semantic diagnostic taxonomy/fix-it modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-semantic-diagnostic-taxonomy-and-fixit-synthesis-modular-split-scaffolding-contract-b002-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_b002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M243-B002-TYP-01", "struct Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold {"),
        ("M243-B002-TYP-02", "bool diagnostics_taxonomy_consistent = false;"),
        ("M243-B002-TYP-03", "bool arc_diagnostics_fixit_handoff_deterministic = false;"),
        ("M243-B002-TYP-04", "bool typed_sema_handoff_deterministic = false;"),
        (
            "M243-B002-TYP-05",
            "Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold",
        ),
        (
            "M243-B002-TYP-06",
            "semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold;",
        ),
    ),
    "scaffold_header": (
        (
            "M243-B002-SCA-01",
            "inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffoldKey(",
        ),
        (
            "M243-B002-SCA-02",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(",
        ),
        ("M243-B002-SCA-03", "scaffold.diagnostics_taxonomy_consistent ="),
        ("M243-B002-SCA-04", "scaffold.arc_diagnostics_fixit_handoff_deterministic ="),
        ("M243-B002-SCA-05", "scaffold.typed_sema_handoff_deterministic ="),
        ("M243-B002-SCA-06", "scaffold.modular_split_ready ="),
        (
            "M243-B002-SCA-07",
            "inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffoldReady(",
        ),
    ),
    "frontend_pipeline_source": (
        (
            "M243-B002-PIP-01",
            '#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h"',
        ),
        (
            "M243-B002-PIP-02",
            "result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold =",
        ),
        (
            "M243-B002-PIP-03",
            "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(",
        ),
    ),
    "architecture_doc": (
        (
            "M243-B002-ARCH-01",
            "M243 lane-B B002 modular split scaffolding anchors semantic diagnostic",
        ),
        (
            "M243-B002-ARCH-02",
            "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h",
        ),
    ),
    "contract_doc": (
        (
            "M243-B002-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-modular-split-scaffolding/m243-b002-v1`",
        ),
        ("M243-B002-DOC-02", "Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold"),
        ("M243-B002-DOC-03", "BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold"),
        (
            "M243-B002-DOC-04",
            "python scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-B002-DOC-05",
            "python -m pytest tests/tooling/test_check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py -q",
        ),
        ("M243-B002-DOC-06", "npm run check:objc3c:m243-b002-lane-b-readiness"),
        (
            "M243-B002-DOC-07",
            "tmp/reports/m243/M243-B002/semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-B002-PKT-01", "Packet: `M243-B002`"),
        ("M243-B002-PKT-02", "Dependencies: `M243-B001`"),
        (
            "M243-B002-PKT-03",
            "scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py",
        ),
        (
            "M243-B002-PKT-04",
            "tests/tooling/test_check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py",
        ),
    ),
    "package_json": (
        (
            "M243-B002-CFG-01",
            '"check:objc3c:m243-b002-semantic-diagnostic-taxonomy-and-fix-it-synthesis-modular-split-scaffolding-contract"',
        ),
        (
            "M243-B002-CFG-02",
            '"test:tooling:m243-b002-semantic-diagnostic-taxonomy-and-fix-it-synthesis-modular-split-scaffolding-contract"',
        ),
        ("M243-B002-CFG-03", '"check:objc3c:m243-b002-lane-b-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M243-B002-FORB-01", "scaffold.modular_split_ready = true;"),
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
            "tmp/reports/m243/M243-B002/"
            "semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract_summary.json"
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
