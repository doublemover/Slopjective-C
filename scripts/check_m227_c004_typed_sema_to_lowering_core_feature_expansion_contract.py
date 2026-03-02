#!/usr/bin/env python3
"""Fail-closed validator for M227-C004 typed sema-to-lowering core-feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-typed-sema-to-lowering-core-feature-expansion-c004-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "typed_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M227-C004-TYP-01", "bool protocol_category_handoff_deterministic = false;"),
        ("M227-C004-TYP-02", "bool class_protocol_category_linking_handoff_deterministic = false;"),
        ("M227-C004-TYP-03", "bool selector_normalization_handoff_deterministic = false;"),
        ("M227-C004-TYP-04", "bool property_attribute_handoff_deterministic = false;"),
        ("M227-C004-TYP-05", "bool typed_core_feature_expansion_consistent = false;"),
        ("M227-C004-TYP-06", "std::size_t typed_core_feature_expansion_case_count = 0;"),
        ("M227-C004-TYP-07", "std::size_t typed_core_feature_expansion_passed_case_count = 0;"),
        ("M227-C004-TYP-08", "std::size_t typed_core_feature_expansion_failed_case_count = 0;"),
        ("M227-C004-TYP-09", "std::string typed_core_feature_expansion_key;"),
    ),
    "typed_surface": (
        ("M227-C004-SUR-01", "kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount = 4u"),
        ("M227-C004-SUR-02", "BuildObjc3TypedSemaToLoweringCoreFeatureExpansionKey"),
        ("M227-C004-SUR-03", "surface.protocol_category_handoff_deterministic ="),
        ("M227-C004-SUR-04", "surface.class_protocol_category_linking_handoff_deterministic ="),
        ("M227-C004-SUR-05", "surface.selector_normalization_handoff_deterministic ="),
        ("M227-C004-SUR-06", "surface.property_attribute_handoff_deterministic ="),
        ("M227-C004-SUR-07", "surface.typed_core_feature_expansion_case_count ="),
        ("M227-C004-SUR-08", "surface.typed_core_feature_expansion_consistent ="),
        ("M227-C004-SUR-09", "surface.typed_core_feature_expansion_key ="),
        ("M227-C004-SUR-10", "typed_core_feature_expansion_key_ready"),
        ("M227-C004-SUR-11", "surface.typed_core_feature_expansion_consistent &&"),
    ),
    "parse_readiness": (
        ("M227-C004-REA-01", "surface.protocol_category_deterministic ="),
        ("M227-C004-REA-02", "surface.class_protocol_category_linking_deterministic ="),
        ("M227-C004-REA-03", "surface.selector_normalization_deterministic ="),
        ("M227-C004-REA-04", "surface.property_attribute_deterministic ="),
        ("M227-C004-REA-05", "surface.typed_sema_core_feature_expansion_consistent ="),
        ("M227-C004-REA-06", "surface.typed_sema_core_feature_expansion_case_count ="),
        ("M227-C004-REA-07", "surface.typed_sema_core_feature_expansion_passed_case_count ="),
        ("M227-C004-REA-08", "surface.typed_sema_core_feature_expansion_failed_case_count ="),
        ("M227-C004-REA-09", "surface.typed_sema_core_feature_expansion_key ="),
        ("M227-C004-REA-10", "surface.typed_core_feature_expansion_case_count > 0 ||"),
        ("M227-C004-REA-11", "!surface.typed_core_feature_expansion_key.empty() ||"),
    ),
    "contract_doc": (
        (
            "M227-C004-DOC-01",
            "Contract ID: `objc3c-typed-sema-to-lowering-core-feature-expansion/m227-c004-v1`",
        ),
        ("M227-C004-DOC-02", "typed_core_feature_expansion_consistent"),
        ("M227-C004-DOC-03", "typed_core_feature_expansion_key"),
        (
            "M227-C004-DOC-04",
            "tmp/reports/m227/M227-C004/typed_sema_to_lowering_core_feature_expansion_contract_summary.json",
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
        default=Path("tmp/reports/m227/M227-C004/typed_sema_to_lowering_core_feature_expansion_contract_summary.json"),
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
