#!/usr/bin/env python3
"""Fail-closed checker for M228-E008 replay/performance closeout recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m228_lane_e_replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_e008_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_e008_replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m228/M228-E008/"
    "replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M228-E008-E007-01",
        "M228-E007",
        Path(
            "docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_diagnostics_hardening_e007_expectations.md"
        ),
    ),
    AssetCheck(
        "M228-E008-E007-02",
        "M228-E007",
        Path(
            "scripts/check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract.py"
        ),
    ),
    AssetCheck(
        "M228-E008-E007-03",
        "M228-E007",
        Path(
            "tests/tooling/test_check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract.py"
        ),
    ),
    AssetCheck(
        "M228-E008-A008-01",
        "M228-A008",
        Path(
            "docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_recovery_determinism_hardening_a008_expectations.md"
        ),
    ),
    AssetCheck(
        "M228-E008-B008-01",
        "M228-B008",
        Path(
            "docs/contracts/m228_ownership_aware_lowering_behavior_recovery_determinism_hardening_b008_expectations.md"
        ),
    ),
    AssetCheck(
        "M228-E008-D008-01",
        "M228-D008",
        Path(
            "docs/contracts/m228_object_emission_link_path_reliability_recovery_determinism_hardening_d008_expectations.md"
        ),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E008-DOC-EXP-01",
        "Contract ID: `objc3c-lane-e-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract/m228-e008-v1`",
    ),
    SnippetCheck("M228-E008-DOC-EXP-02", "`M228-E007`"),
    SnippetCheck("M228-E008-DOC-EXP-03", "`M228-A008`"),
    SnippetCheck("M228-E008-DOC-EXP-04", "`M228-B008`"),
    SnippetCheck("M228-E008-DOC-EXP-05", "`M228-D008`"),
    SnippetCheck("M228-E008-DOC-EXP-06", "Issue `#5288` dependency list includes pending-lane token `M228-C008`."),
    SnippetCheck("M228-E008-DOC-EXP-07", "`M228-C008` is preserved as a pending-lane continuity token"),
    SnippetCheck("M228-E008-DOC-EXP-08", "`check:objc3c:m228-e008-lane-e-readiness`"),
    SnippetCheck(
        "M228-E008-DOC-EXP-09",
        "`tmp/reports/m228/M228-E008/replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M228-E008-DOC-PKT-01", "Packet: `M228-E008`"),
    SnippetCheck(
        "M228-E008-DOC-PKT-02",
        "Dependencies: `M228-E007`, `M228-A008`, `M228-B008`, `M228-D008`",
    ),
    SnippetCheck("M228-E008-DOC-PKT-03", "Issue dependency continuity tokens:"),
    SnippetCheck("M228-E008-DOC-PKT-04", "`M228-C008`"),
    SnippetCheck(
        "M228-E008-DOC-PKT-05",
        "`scripts/check_m228_e008_replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_contract.py`",
    ),
    SnippetCheck(
        "M228-E008-DOC-PKT-06",
        "`tests/tooling/test_check_m228_e008_replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_contract.py`",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M228-E008-CFG-01",
        "check:objc3c:m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract",
    ),
    PackageScriptKeyCheck(
        "M228-E008-CFG-02",
        "test:tooling:m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract",
    ),
    PackageScriptKeyCheck("M228-E008-CFG-03", "check:objc3c:m228-e008-lane-e-readiness"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M228-E008-CFG-04",
        "check:objc3c:m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract",
        "python scripts/check_m228_e008_replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_contract.py",
    ),
    PackageScriptCheck(
        "M228-E008-CFG-05",
        "test:tooling:m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract",
        "python -m pytest tests/tooling/test_check_m228_e008_replay_proof_and_performance_closeout_gate_recovery_determinism_hardening_contract.py -q",
    ),
    PackageScriptCheck(
        "M228-E008-CFG-06",
        "check:objc3c:m228-e008-lane-e-readiness",
        "npm run check:objc3c:m228-e007-lane-e-readiness && npm run check:objc3c:m228-a008-lane-a-readiness && npm run check:objc3c:m228-b008-lane-b-readiness && npm run check:objc3c:m228-c008-lane-c-readiness && npm run check:objc3c:m228-d008-lane-d-readiness && npm run check:objc3c:m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract && npm run test:tooling:m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E008-ARCH-01",
        "M228 lane-E E008 replay-proof/performance closeout recovery and determinism hardening anchors",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E008-SPC-01",
        "replay-proof/performance recovery and determinism hardening closeout wiring",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E008-META-01",
        "deterministic lane-E recovery and determinism hardening dependency anchors",
    ),
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
    artifact_name: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact_name,
                    check_id=snippet_check.check_id,
                    detail=f"expected snippet missing: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E008-CFG-00",
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E008-CFG-00",
                detail=f"unable to parse package.json: {exc}",
            )
        )
        return checks_total, findings

    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(
            Finding(
                artifact="package_json",
                check_id="M228-E008-CFG-00",
                detail='expected top-level "scripts" object in package.json',
            )
        )
        return checks_total, findings

    for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if key_check.script_key not in scripts:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=key_check.check_id,
                    detail=f'expected scripts["{key_check.script_key}"] to exist',
                )
            )

    for script_check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual = scripts.get(script_check.script_key)
        if actual != script_check.expected_value:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=script_check.check_id,
                    detail=(
                        f'expected scripts["{script_check.script_key}"] to equal '
                        f'"{script_check.expected_value}"'
                    ),
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total, findings = check_prerequisite_assets()

    expectations_checks, expectations_findings = check_doc_contract(
        artifact_name="expectations_doc",
        path=args.expectations_doc,
        exists_check_id="M228-E008-DOC-EXP-00",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M228-E008-DOC-PKT-00",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    package_checks, package_findings = check_package_contract(args.package_json)
    checks_total += package_checks
    findings.extend(package_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M228-E008-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M228-E008-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M228-E008-META-00",
        snippets=METADATA_SPEC_SNIPPETS,
    )
    checks_total += metadata_checks
    findings.extend(metadata_findings)

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if not findings:
        print("m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract: OK")
        return 0

    print(
        "m228-e008-replay-proof-performance-closeout-gate-recovery-determinism-hardening-contract: "
        f"contract drift detected ({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
