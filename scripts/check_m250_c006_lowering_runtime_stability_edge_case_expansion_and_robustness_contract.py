#!/usr/bin/env python3
"""Fail-closed validator for M250-C006 lowering/runtime edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-lowering-runtime-stability-edge-case-expansion-and-robustness-contract-c006-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "core_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_stability_core_feature_implementation_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "c005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_edge_case_compatibility_completion_c005_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_edge_case_expansion_and_robustness_c006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_c006_lowering_runtime_stability_edge_case_expansion_and_robustness_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-C006-TYP-01", "bool edge_case_expansion_consistent = false;"),
        ("M250-C006-TYP-02", "bool edge_case_robustness_ready = false;"),
        ("M250-C006-TYP-03", "std::string edge_case_robustness_key;"),
    ),
    "core_surface": (
        ("M250-C006-SUR-01", "const bool edge_case_expansion_consistent ="),
        ("M250-C006-SUR-02", "const bool edge_case_robustness_ready ="),
        ("M250-C006-SUR-03", "surface.edge_case_expansion_consistent ="),
        ("M250-C006-SUR-04", "surface.edge_case_robustness_ready ="),
        ("M250-C006-SUR-05", "surface.edge_case_robustness_key ="),
        ("M250-C006-SUR-06", "      edge_case_robustness_ready;"),
        ("M250-C006-SUR-07", ";edge-expansion-consistent="),
        ("M250-C006-SUR-08", ";edge-robustness-ready="),
        ("M250-C006-SUR-09", ";edge-robustness-key-ready="),
        ("M250-C006-SUR-10", "lowering/runtime edge-case expansion is inconsistent"),
        ("M250-C006-SUR-11", "lowering/runtime edge-case robustness is not ready"),
    ),
    "architecture_doc": (
        ("M250-C006-ARC-01", "M250 lane-C C006 edge-case expansion and robustness anchors explicit"),
        ("M250-C006-ARC-02", "edge_case_*_robustness*"),
    ),
    "c005_contract_doc": (
        (
            "M250-C006-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-stability-edge-case-compatibility-completion/m250-c005-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-C006-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-stability-edge-case-expansion-and-robustness/m250-c006-v1`",
        ),
        ("M250-C006-DOC-02", "edge_case_expansion_consistent"),
        ("M250-C006-DOC-03", "edge_case_robustness_ready"),
        (
            "M250-C006-DOC-04",
            "scripts/check_m250_c006_lowering_runtime_stability_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M250-C006-DOC-05",
            "tests/tooling/test_check_m250_c006_lowering_runtime_stability_edge_case_expansion_and_robustness_contract.py",
        ),
        ("M250-C006-DOC-06", "npm run check:objc3c:m250-c006-lane-c-readiness"),
    ),
    "packet_doc": (
        ("M250-C006-PKT-01", "Packet: `M250-C006`"),
        ("M250-C006-PKT-02", "Dependencies: `M250-C005`"),
        (
            "M250-C006-PKT-03",
            "scripts/check_m250_c006_lowering_runtime_stability_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-C006-CFG-01",
            '"check:objc3c:m250-c006-lowering-runtime-stability-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M250-C006-CFG-02",
            '"test:tooling:m250-c006-lowering-runtime-stability-edge-case-expansion-and-robustness-contract"',
        ),
        ("M250-C006-CFG-03", '"check:objc3c:m250-c006-lane-c-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface": (
        ("M250-C006-FORB-01", "const bool edge_case_robustness_ready = true;"),
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
            "tmp/reports/m250/M250-C006/"
            "lowering_runtime_stability_edge_case_expansion_and_robustness_contract_summary.json"
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
