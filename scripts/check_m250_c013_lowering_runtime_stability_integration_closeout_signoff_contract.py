#!/usr/bin/env python3
"""Fail-closed validator for M250-C013 lowering/runtime integration closeout sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-lowering-runtime-stability-integration-closeout-signoff-contract-c013-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "core_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_stability_core_feature_implementation_surface.h",
    "c012_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_cross_lane_integration_sync_c012_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_integration_closeout_signoff_c013_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_c013_lowering_runtime_stability_integration_closeout_signoff_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-C013-TYP-01", "bool integration_closeout_consistent = false;"),
        ("M250-C013-TYP-02", "bool gate_signoff_ready = false;"),
        ("M250-C013-TYP-03", "std::string integration_closeout_key;"),
    ),
    "core_surface": (
        ("M250-C013-SUR-01", "const bool integration_closeout_consistent ="),
        ("M250-C013-SUR-02", "const bool gate_signoff_ready ="),
        ("M250-C013-SUR-03", "surface.integration_closeout_consistent ="),
        ("M250-C013-SUR-04", "surface.gate_signoff_ready ="),
        ("M250-C013-SUR-05", "surface.integration_closeout_key ="),
        ("M250-C013-SUR-06", "BuildObjc3LoweringRuntimeIntegrationCloseoutKey("),
        ("M250-C013-SUR-07", "const bool integration_closeout_expansion_ready ="),
        ("M250-C013-SUR-08", ";integration-closeout-consistent="),
        ("M250-C013-SUR-09", ";gate-signoff-ready="),
        ("M250-C013-SUR-10", ";integration-closeout-key-ready="),
        ("M250-C013-SUR-11", ";integration-closeout-expansion-ready="),
        ("M250-C013-SUR-12", "lowering/runtime integration closeout is inconsistent"),
        ("M250-C013-SUR-13", "lowering/runtime gate sign-off is not ready"),
        ("M250-C013-SUR-14", "lowering/runtime integration closeout expansion is not ready"),
    ),
    "c012_contract_doc": (
        (
            "M250-C013-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-stability-cross-lane-integration-sync/m250-c012-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-C013-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-stability-integration-closeout-signoff/m250-c013-v1`",
        ),
        ("M250-C013-DOC-02", "integration_closeout_consistent"),
        ("M250-C013-DOC-03", "gate_signoff_ready"),
        (
            "M250-C013-DOC-04",
            "scripts/check_m250_c013_lowering_runtime_stability_integration_closeout_signoff_contract.py",
        ),
        (
            "M250-C013-DOC-05",
            "tests/tooling/test_check_m250_c013_lowering_runtime_stability_integration_closeout_signoff_contract.py",
        ),
        ("M250-C013-DOC-06", "npm run check:objc3c:m250-c013-lane-c-readiness"),
    ),
    "packet_doc": (
        ("M250-C013-PKT-01", "Packet: `M250-C013`"),
        ("M250-C013-PKT-02", "Dependencies: `M250-C012`"),
        (
            "M250-C013-PKT-03",
            "scripts/check_m250_c013_lowering_runtime_stability_integration_closeout_signoff_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface": (
        ("M250-C013-FORB-01", "const bool gate_signoff_ready = true;"),
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
            "tmp/reports/m250/M250-C013/lowering_runtime_stability_integration_closeout_signoff_contract_summary.json"
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
