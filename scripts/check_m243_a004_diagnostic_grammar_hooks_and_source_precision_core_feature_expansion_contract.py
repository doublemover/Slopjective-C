#!/usr/bin/env python3
"""Fail-closed validator for M243-A004 diagnostic grammar-hooks expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion-contract-a004-v1"

ARTIFACTS: dict[str, Path] = {
    "a003_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a003_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a004_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_packet.md",
    "expansion_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "parse"
    / "objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h",
    "frontend_types": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_types.h",
    "readiness_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a003_contract_doc": (
        (
            "M243-A004-DEP-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-core-feature/m243-a003-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M243-A004-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion/m243-a004-v1`",
        ),
        (
            "M243-A004-DOC-02",
            "parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent",
        ),
        (
            "M243-A004-DOC-03",
            "npm run check:objc3c:m243-a004-lane-a-readiness",
        ),
    ),
    "packet_doc": (
        ("M243-A004-PKT-01", "Packet: `M243-A004`"),
        ("M243-A004-PKT-02", "Dependencies: `M243-A003`"),
        (
            "M243-A004-PKT-03",
            "scripts/check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py",
        ),
    ),
    "expansion_surface_header": (
        (
            "M243-A004-SUR-01",
            "struct Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface {",
        ),
        ("M243-A004-SUR-02", "bool accounting_consistent = false;"),
        ("M243-A004-SUR-03", "bool replay_keys_ready = false;"),
        ("M243-A004-SUR-04", "bool core_feature_expansion_ready = false;"),
        ("M243-A004-SUR-05", "std::string expansion_key;"),
        (
            "M243-A004-SUR-06",
            "BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(",
        ),
    ),
    "frontend_types": (
        (
            "M243-A004-TYP-01",
            "bool parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent = false;",
        ),
        (
            "M243-A004-TYP-02",
            "bool parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready = false;",
        ),
        (
            "M243-A004-TYP-03",
            "bool parser_diagnostic_grammar_hooks_core_feature_expansion_ready = false;",
        ),
        (
            "M243-A004-TYP-04",
            "std::string parser_diagnostic_grammar_hooks_core_feature_expansion_key;",
        ),
    ),
    "readiness_surface": (
        (
            "M243-A004-RDY-01",
            '#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"',
        ),
        (
            "M243-A004-RDY-02",
            "const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface",
        ),
        (
            "M243-A004-RDY-03",
            "surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent =",
        ),
        (
            "M243-A004-RDY-04",
            "surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready =",
        ),
        (
            "M243-A004-RDY-05",
            "surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready =",
        ),
        (
            "M243-A004-RDY-06",
            "surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key =",
        ),
        (
            "M243-A004-RDY-07",
            "parser diagnostic grammar hooks core feature expansion replay keys are not ready",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A004-ARC-01",
            "M243 lane-A A004 core feature expansion anchors parser diagnostic",
        ),
        (
            "M243-A004-ARC-02",
            "`parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h`",
        ),
    ),
    "package_json": (
        (
            "M243-A004-CFG-01",
            '"check:objc3c:m243-a004-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion-contract"',
        ),
        (
            "M243-A004-CFG-02",
            '"test:tooling:m243-a004-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion-contract"',
        ),
        ("M243-A004-CFG-03", '"check:objc3c:m243-a004-lane-a-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface": (
        (
            "M243-A004-FORB-01",
            "surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready = true;",
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
            "tmp/reports/m243/M243-A004/diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract_summary.json"
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
