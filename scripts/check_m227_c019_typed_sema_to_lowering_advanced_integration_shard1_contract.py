#!/usr/bin/env python3
"""Fail-closed validator for M227-C019 typed sema-to-lowering advanced integration shard 1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-typed-sema-to-lowering-advanced-integration-shard1-c019-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "typed_surface": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h",
    "c018_contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_advanced_conformance_shard1_c018_expectations.md",
    "c018_checker": ROOT / "scripts" / "check_m227_c018_typed_sema_to_lowering_advanced_conformance_shard1_contract.py",
    "c018_tooling_test": ROOT / "tests" / "tooling" / "test_check_m227_c018_typed_sema_to_lowering_advanced_conformance_shard1_contract.py",
    "c018_packet_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c018_typed_sema_to_lowering_advanced_conformance_shard1_packet.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_advanced_integration_shard1_c019_expectations.md",
    "packet_doc": ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_c019_typed_sema_to_lowering_advanced_integration_shard1_packet.md",
    "package_json": ROOT / "package.json",
    "architecture": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_contracts": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_tables": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M227-C019-TYP-01", "bool typed_advanced_integration_shard1_consistent = false;"),
        ("M227-C019-TYP-02", "bool typed_advanced_integration_shard1_ready = false;"),
        ("M227-C019-TYP-03", "std::string typed_advanced_integration_shard1_key;"),
        ("M227-C019-TYP-04", "bool typed_sema_advanced_integration_shard1_consistent = false;"),
        ("M227-C019-TYP-05", "bool typed_sema_advanced_integration_shard1_ready = false;"),
        ("M227-C019-TYP-06", "std::string typed_sema_advanced_integration_shard1_key;"),
    ),
    "typed_surface": (
        ("M227-C019-SUR-01", "BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard1Key("),
        ("M227-C019-SUR-02", "surface.typed_advanced_integration_shard1_consistent ="),
        ("M227-C019-SUR-03", "surface.typed_advanced_integration_shard1_ready ="),
        ("M227-C019-SUR-04", "surface.typed_advanced_integration_shard1_key ="),
        ("M227-C019-SUR-05", "typed_advanced_integration_shard1_key_ready"),
        ("M227-C019-SUR-06", "typed sema-to-lowering advanced integration shard 1 is inconsistent"),
        ("M227-C019-SUR-07", "typed sema-to-lowering advanced integration shard 1 is not ready"),
        ("M227-C019-SUR-08", "typed sema-to-lowering advanced integration shard 1 key is empty"),
    ),
    "parse_readiness": (
        ("M227-C019-REA-01", "surface.typed_sema_advanced_integration_shard1_consistent ="),
        ("M227-C019-REA-02", "surface.typed_sema_advanced_integration_shard1_ready ="),
        ("M227-C019-REA-03", "surface.typed_sema_advanced_integration_shard1_key ="),
        ("M227-C019-REA-04", "const bool typed_advanced_integration_shard1_alignment ="),
        ("M227-C019-REA-05", "typed_advanced_integration_shard1_alignment &&"),
        ("M227-C019-REA-06", "!surface.typed_sema_advanced_integration_shard1_key.empty() &&"),
        ("M227-C019-REA-07", "typed sema-to-lowering advanced integration shard 1 drifted from parse/lowering readiness"),
    ),
    "c018_contract_doc": (("M227-C019-DEP-01", "m227-c018-v1"),),
    "c018_checker": (("M227-C019-DEP-02", 'MODE = "m227-typed-sema-to-lowering-advanced-conformance-shard1-c018-v1"'),),
    "c018_tooling_test": (("M227-C019-DEP-03", "def test_contract_passes_on_repository_sources"),),
    "c018_packet_doc": (("M227-C019-DEP-04", "Packet: `M227-C018`"),),
    "contract_doc": (
        ("M227-C019-DOC-01", "Contract ID: `objc3c-typed-sema-to-lowering-advanced-integration-shard1/m227-c019-v1`"),
        ("M227-C019-DOC-02", "Execute issue `#5139`"),
        ("M227-C019-DOC-03", "Dependencies: `M227-C018`"),
    ),
    "packet_doc": (
        ("M227-C019-PKT-01", "Packet: `M227-C019`"),
        ("M227-C019-PKT-02", "Issue: `#5139`"),
        ("M227-C019-PKT-03", "Dependencies: `M227-C018`"),
    ),
    "package_json": (
        ("M227-C019-PKG-01", '"check:objc3c:m227-c019-typed-sema-to-lowering-advanced-integration-shard1-contract"'),
        ("M227-C019-PKG-02", '"test:tooling:m227-c019-typed-sema-to-lowering-advanced-integration-shard1-contract"'),
        ("M227-C019-PKG-03", '"check:objc3c:m227-c019-lane-c-readiness"'),
        ("M227-C019-PKG-04", "scripts/check_m227_c018_typed_sema_to_lowering_advanced_conformance_shard1_contract.py"),
    ),
    "architecture": (("M227-C019-ARC-01", "M227 lane-C C019 typed sema-to-lowering advanced integration workpack (shard 1) anchors"),),
    "lowering_contracts": (("M227-C019-SPC-01", "typed sema-to-lowering advanced integration workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C019`, `M227-C018`)"),),
    "metadata_tables": (("M227-C019-META-01", "deterministic lane-C typed sema-to-lowering advanced-integration-shard1 metadata anchors for `M227-C019`"),),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "typed_surface": (("M227-C019-FORB-01", "surface.typed_advanced_integration_shard1_ready = true;"),),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m227/M227-C019/typed_sema_to_lowering_advanced_integration_shard1_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact)
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
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(
        json.dumps(
            {
                "mode": MODE,
                "ok": ok,
                "checks_total": total_checks,
                "checks_passed": passed_checks,
                "failures": [
                    {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in findings
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))



