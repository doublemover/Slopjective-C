#!/usr/bin/env python3
"""Fail-closed validator for M243-A003 diagnostic grammar hooks core feature."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-diagnostic-grammar-hooks-and-source-precision-core-feature-contract-a003-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a003_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_packet.md",
    "core_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "parse"
    / "objc3_diagnostic_grammar_hooks_core_feature.h",
    "core_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "parse"
    / "objc3_diagnostic_grammar_hooks_core_feature.cpp",
    "scaffold_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "parse"
    / "objc3_diagnostic_source_precision_scaffold.cpp",
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
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M243-A003-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-core-feature/m243-a003-v1`",
        ),
        (
            "M243-A003-DOC-02",
            "scripts/check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py",
        ),
        (
            "M243-A003-DOC-03",
            "tests/tooling/test_check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py",
        ),
    ),
    "packet_doc": (
        ("M243-A003-PKT-01", "Packet: `M243-A003`"),
        ("M243-A003-PKT-02", "Dependencies: `M243-A002`"),
        (
            "M243-A003-PKT-03",
            "native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp",
        ),
    ),
    "core_header": (
        ("M243-A003-H-01", "struct Objc3DiagnosticGrammarHooksCoreFeatureSurface {"),
        ("M243-A003-H-02", "std::size_t grammar_hook_code_count = 0;"),
        ("M243-A003-H-03", "bool IsObjc3DiagnosticGrammarHooksCoreFeatureReady("),
    ),
    "core_source": (
        ("M243-A003-SRC-01", "bool IsObjc3GrammarHookCode(const std::string &code) {"),
        ("M243-A003-SRC-02", 'if (!StartsWith(code, "O3P") || code.size() != 6u) {'),
        ("M243-A003-SRC-03", "BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface("),
        ("M243-A003-SRC-04", "surface.core_feature_consistent ="),
    ),
    "scaffold_source": (
        (
            "M243-A003-SCF-01",
            "BuildObjc3ParserDiagnosticSourcePrecisionScaffold(",
        ),
    ),
    "frontend_types": (
        (
            "M243-A003-TYP-01",
            "bool parser_diagnostic_grammar_hooks_core_feature_consistent = false;",
        ),
        (
            "M243-A003-TYP-02",
            "bool parser_diagnostic_grammar_hooks_core_feature_ready = false;",
        ),
        (
            "M243-A003-TYP-03",
            "std::string parser_diagnostic_grammar_hooks_core_feature_key;",
        ),
    ),
    "readiness_surface": (
        (
            "M243-A003-RDY-01",
            '#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"',
        ),
        (
            "M243-A003-RDY-02",
            "const Objc3DiagnosticGrammarHooksCoreFeatureSurface parser_diagnostic_grammar_hooks_core_feature =",
        ),
        (
            "M243-A003-RDY-03",
            "surface.parser_diagnostic_grammar_hooks_core_feature_ready =",
        ),
        (
            "M243-A003-RDY-04",
            "surface.parser_diagnostic_grammar_hooks_core_feature_consistent =",
        ),
        (
            "M243-A003-RDY-05",
            "parser diagnostic grammar hooks core feature is not ready",
        ),
    ),
    "cmake": (
        (
            "M243-A003-CMAKE-01",
            "src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp",
        ),
    ),
    "build_script": (
        (
            "M243-A003-BLD-01",
            "native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A003-ARC-01",
            "M243 lane-A A003 core feature implementation anchors parser diagnostic",
        ),
        (
            "M243-A003-ARC-02",
            "`parse/objc3_diagnostic_grammar_hooks_core_feature.cpp`",
        ),
    ),
    "package_json": (
        (
            "M243-A003-CFG-01",
            '"check:objc3c:m243-a003-diagnostic-grammar-hooks-and-source-precision-core-feature-contract"',
        ),
        (
            "M243-A003-CFG-02",
            '"test:tooling:m243-a003-diagnostic-grammar-hooks-and-source-precision-core-feature-contract"',
        ),
        ("M243-A003-CFG-03", '"check:objc3c:m243-a003-lane-a-readiness"'),
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
            "tmp/reports/m243/M243-A003/diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract_summary.json"
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
