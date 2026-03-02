#!/usr/bin/env python3
"""Fail-closed validator for M250-B004 semantic core-feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-semantic-stability-core-feature-expansion-contract-b004-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "core_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_stability_core_feature_implementation_surface.h",
    "b003_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_core_feature_implementation_b003_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_core_feature_expansion_b004_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_b004_semantic_stability_spec_delta_closure_core_feature_expansion_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-B004-TYP-01", "bool typed_core_feature_expansion_accounting_consistent = false;"),
        ("M250-B004-TYP-02", "bool parse_conformance_accounting_consistent = false;"),
        ("M250-B004-TYP-03", "bool replay_keys_ready = false;"),
        ("M250-B004-TYP-04", "bool expansion_ready = false;"),
        ("M250-B004-TYP-05", "std::string expansion_key;"),
    ),
    "core_surface": (
        ("M250-B004-SUR-01", "typed_core_feature_expansion_accounting_consistent ="),
        ("M250-B004-SUR-02", "parse_conformance_accounting_consistent ="),
        ("M250-B004-SUR-03", "replay_keys_ready ="),
        ("M250-B004-SUR-04", "expansion_ready ="),
        ("M250-B004-SUR-05", "surface.expansion_key ="),
        ("M250-B004-SUR-06", "semantic stability core feature expansion is not ready"),
    ),
    "b003_contract_doc": (
        ("M250-B004-DEP-01", "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-implementation/m250-b003-v1`"),
    ),
    "contract_doc": (
        ("M250-B004-DOC-01", "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-expansion/m250-b004-v1`"),
        ("M250-B004-DOC-02", "typed_core_feature_expansion_accounting_consistent"),
        ("M250-B004-DOC-03", "npm run check:objc3c:m250-b004-lane-b-readiness"),
    ),
    "packet_doc": (
        ("M250-B004-PKT-01", "Packet: `M250-B004`"),
        ("M250-B004-PKT-02", "Dependencies: `M250-B003`"),
        ("M250-B004-PKT-03", "scripts/check_m250_b004_semantic_stability_spec_delta_closure_core_feature_expansion_contract.py"),
    ),
    "package_json": (
        ("M250-B004-CFG-01", '"check:objc3c:m250-b004-semantic-stability-spec-delta-closure-core-feature-expansion-contract"'),
        ("M250-B004-CFG-02", '"test:tooling:m250-b004-semantic-stability-spec-delta-closure-core-feature-expansion-contract"'),
        ("M250-B004-CFG-03", '"check:objc3c:m250-b004-lane-b-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface": (
        ("M250-B004-FORB-01", "surface.expansion_ready = true;"),
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
            "tmp/reports/m250/M250-B004/semantic_stability_spec_delta_closure_core_feature_expansion_contract_summary.json"
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
