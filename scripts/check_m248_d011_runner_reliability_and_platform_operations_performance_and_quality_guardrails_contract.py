#!/usr/bin/env python3
"""Fail-closed checker for M248-D011 runner/platform operations performance and quality guardrails contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_performance_and_quality_guardrails_d011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_d011_runner_reliability_and_platform_operations_performance_and_quality_guardrails_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-D011/runner_reliability_and_platform_operations_performance_and_quality_guardrails_contract_summary.json"
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
        "M248-D011-D010-01",
        "M248-D010",
        Path(
            "docs/contracts/"
            "m248_runner_reliability_and_platform_operations_conformance_corpus_expansion_d010_expectations.md"
        ),
    ),
    AssetCheck(
        "M248-D011-D010-02",
        "M248-D010",
        Path(
            "spec/planning/compiler/m248/"
            "m248_d010_runner_reliability_and_platform_operations_conformance_corpus_expansion_packet.md"
        ),
    ),
    AssetCheck(
        "M248-D011-D010-03",
        "M248-D010",
        Path(
            "scripts/"
            "check_m248_d010_runner_reliability_and_platform_operations_conformance_corpus_expansion_contract.py"
        ),
    ),
    AssetCheck(
        "M248-D011-D010-04",
        "M248-D010",
        Path(
            "tests/tooling/"
            "test_check_m248_d010_runner_reliability_and_platform_operations_conformance_corpus_expansion_contract.py"
        ),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D011-DOC-EXP-01",
        "Contract ID: `objc3c-runner-reliability-platform-operations-performance-quality-guardrails/m248-d011-v1`",
    ),
    SnippetCheck("M248-D011-DOC-EXP-02", "Dependencies: `M248-D010`"),
    SnippetCheck(
        "M248-D011-DOC-EXP-03",
        "Issue `#6846` defines canonical lane-D performance and quality guardrails scope.",
    ),
    SnippetCheck("M248-D011-DOC-EXP-04", "`check:objc3c:m248-d011-lane-d-readiness`"),
    SnippetCheck("M248-D011-DOC-EXP-05", "`check:objc3c:m248-d010-lane-d-readiness`"),
    SnippetCheck("M248-D011-DOC-EXP-06", "`test:objc3c:execution-replay-proof`"),
    SnippetCheck(
        "M248-D011-DOC-EXP-07",
        "tmp/reports/m248/M248-D011/runner_reliability_and_platform_operations_performance_and_quality_guardrails_contract_summary.json",
    ),
    SnippetCheck(
        "M248-D011-DOC-EXP-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-D011-DOC-PKT-01", "Packet: `M248-D011`"),
    SnippetCheck("M248-D011-DOC-PKT-02", "Issue: `#6846`"),
    SnippetCheck("M248-D011-DOC-PKT-03", "Dependencies"),
    SnippetCheck("M248-D011-DOC-PKT-04", "`M248-D010`"),
    SnippetCheck(
        "M248-D011-DOC-PKT-05",
        "`scripts/check_m248_d011_runner_reliability_and_platform_operations_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M248-D011-DOC-PKT-06",
        "`tests/tooling/test_check_m248_d011_runner_reliability_and_platform_operations_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck("M248-D011-DOC-PKT-07", "`check:objc3c:m248-d011-lane-d-readiness`"),
    SnippetCheck(
        "M248-D011-DOC-PKT-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M248-D011-CFG-01",
        "check:objc3c:m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract",
    ),
    PackageScriptKeyCheck(
        "M248-D011-CFG-02",
        "test:tooling:m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract",
    ),
    PackageScriptKeyCheck("M248-D011-CFG-03", "check:objc3c:m248-d011-lane-d-readiness"),
    PackageScriptKeyCheck("M248-D011-CFG-07", "compile:objc3c"),
    PackageScriptKeyCheck("M248-D011-CFG-08", "proof:objc3c"),
    PackageScriptKeyCheck("M248-D011-CFG-09", "test:objc3c:execution-replay-proof"),
    PackageScriptKeyCheck("M248-D011-CFG-10", "test:objc3c:perf-budget"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M248-D011-CFG-04",
        "check:objc3c:m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract",
        "python scripts/check_m248_d011_runner_reliability_and_platform_operations_performance_and_quality_guardrails_contract.py",
    ),
    PackageScriptCheck(
        "M248-D011-CFG-05",
        "test:tooling:m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract",
        "python -m pytest tests/tooling/test_check_m248_d011_runner_reliability_and_platform_operations_performance_and_quality_guardrails_contract.py -q",
    ),
    PackageScriptCheck(
        "M248-D011-CFG-06",
        "check:objc3c:m248-d011-lane-d-readiness",
        "npm run check:objc3c:m248-d010-lane-d-readiness && npm run check:objc3c:m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract && npm run test:tooling:m248-d011-runner-reliability-platform-operations-performance-quality-guardrails-contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D011-ARCH-01",
        "M248 lane-D D011 runner/platform operations performance and quality guardrails anchors",
    ),
    SnippetCheck(
        "M248-D011-ARCH-02",
        "docs/contracts/m248_runner_reliability_and_platform_operations_performance_and_quality_guardrails_d011_expectations.md",
    ),
    SnippetCheck(
        "M248-D011-ARCH-03",
        "and fail-closed against `M248-D010` dependency drift.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D011-SPC-01",
        "runner/platform operations performance and quality guardrails governance",
    ),
    SnippetCheck(
        "M248-D011-SPC-02",
        "shall preserve explicit lane-D dependency anchors (`M248-D010`) and fail",
    ),
    SnippetCheck(
        "M248-D011-SPC-03",
        "closed on performance and quality guardrails evidence drift before downstream",
    ),
    SnippetCheck("M248-D011-SPC-04", "platform replay and lane-e conformance matrix advances."),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-D011-META-01",
        "deterministic lane-D runner/platform operations performance and quality guardrails metadata anchors for `M248-D011`",
    ),
    SnippetCheck(
        "M248-D011-META-02",
        "explicit `M248-D010` dependency continuity and fail-closed performance and quality evidence continuity",
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
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
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


def check_doc_contract(*, artifact_name: str, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(Finding(artifact=display_path(path), check_id=exists_check_id, detail=f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(Finding(artifact=display_path(path), check_id=exists_check_id, detail=f"unable to read required document: {exc}"))
        return checks_total, findings
    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(Finding(artifact=artifact_name, check_id=snippet_check.check_id, detail=f"expected snippet missing: {snippet_check.snippet}"))
    return checks_total, findings


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(Finding(artifact=display_path(path), check_id="M248-D011-CFG-00", detail=f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as exc:
        findings.append(Finding(artifact=display_path(path), check_id="M248-D011-CFG-00", detail=f"unable to parse package.json: {exc}"))
        return checks_total, findings
    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(Finding(artifact="package_json", check_id="M248-D011-CFG-00", detail='expected top-level "scripts" object in package.json'))
        return checks_total, findings
    for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if key_check.script_key not in scripts:
            findings.append(Finding(artifact="package_json", check_id=key_check.check_id, detail=f"missing required script key: {key_check.script_key}"))
    for script_check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual_value = scripts.get(script_check.script_key)
        if actual_value != script_check.expected_value:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=script_check.check_id,
                    detail=(f"script {script_check.script_key} mismatch; expected {script_check.expected_value!r}, got {actual_value!r}"),
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    count, findings = check_prerequisite_assets()
    checks_total += count
    failures.extend(findings)

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M248-D011-DOC-EXP-00", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M248-D011-DOC-PKT-00", PACKET_SNIPPETS),
        ("architecture_doc", args.architecture_doc, "M248-D011-ARCH-00", ARCHITECTURE_SNIPPETS),
        ("lowering_spec", args.lowering_spec, "M248-D011-SPC-00", LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, "M248-D011-META-00", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_doc_contract(artifact_name=artifact_name, path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    count, findings = check_package_contract(args.package_json)
    checks_total += count
    failures.extend(findings)

    failures = sorted(failures, key=lambda finding: (finding.artifact, finding.check_id, finding.detail))
    checks_passed = checks_total - len(failures)

    summary_payload = {
        "mode": MODE,
        "ok": len(failures) == 0,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail} for finding in failures],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            sys.stderr.write(f"[{finding.check_id}] {finding.artifact}: {finding.detail}\n")
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

