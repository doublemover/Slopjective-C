#!/usr/bin/env python3
"""Checker for M315-B004 IR fixture compatibility semantics."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from subprocess import check_output
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-B004" / "ir_fixture_compatibility_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation_b004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b004_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation_packet.md"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b004_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation_registry.json"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def tracked_ll_files() -> list[str]:
    return [line.replace('\\', '/') for line in check_output(['git', 'ls-files', '*.ll'], cwd=ROOT, text=True).splitlines() if line.strip()]


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    registry = json.loads(read_text(REGISTRY_JSON))
    ll_files = tracked_ll_files()

    synthetic_paths = [path for path in ll_files if path.startswith('tests/tooling/fixtures/native/library_cli_parity/')]
    synthetic_json_paths = sorted(
        path
        for path in (
            "tests/tooling/fixtures/native/library_cli_parity/cli/module.manifest.json",
            "tests/tooling/fixtures/native/library_cli_parity/library/module.manifest.json",
            "tests/tooling/fixtures/native/library_cli_parity/golden_summary.json",
        )
        if (ROOT / path).is_file()
    )
    replay_paths = [path for path in ll_files if path.startswith('tests/tooling/fixtures/objc3c/') and '/replay_run_' in path and path.endswith('/module.ll')]
    with_header = []
    without_header = []
    for path in replay_paths:
        text = (ROOT / path).read_text(encoding='utf-8')
        header = '\n'.join(text.splitlines()[:5]).lower()
        if 'objc3c native frontend ir' in header:
            with_header.append(path)
        else:
            without_header.append(path)
    explicit_synthetic = []
    for path in synthetic_paths:
        text = (ROOT / path).read_text(encoding='utf-8')
        if 'fixture parity IR' in '\n'.join(text.splitlines()[:5]):
            explicit_synthetic.append(path)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-ir-fixture-compatibility-semantics/m315-b004-v1`" in expectations, str(EXPECTATIONS_DOC), "M315-B004-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("generated-replay candidates" in expectations, str(EXPECTATIONS_DOC), "M315-B004-EXP-02", "expectations missing replay-candidate note", failures)
    checks_passed += require("explicit synthetic parity stubs" in packet, str(PACKET_DOC), "M315-B004-PKT-01", "packet missing semantics summary", failures)
    checks_passed += require("Next issue: `M315-B005`." in packet, str(PACKET_DOC), "M315-B004-PKT-02", "packet missing next issue", failures)

    checks_total += 11
    checks_passed += require(registry.get('mode') == 'm315-b004-ir-fixture-compatibility-semantics-v1', str(REGISTRY_JSON), 'M315-B004-REG-01', 'mode drifted', failures)
    checks_passed += require(registry.get('contract_id') == 'objc3c-cleanup-ir-fixture-compatibility-semantics/m315-b004-v1', str(REGISTRY_JSON), 'M315-B004-REG-02', 'contract id drifted', failures)
    checks_passed += require(registry.get('ll_fixture_totals', {}).get('tracked_ll_files') == len(ll_files) == 78, str(REGISTRY_JSON), 'M315-B004-REG-03', 'tracked ll count drifted', failures)
    checks_passed += require(registry.get('ll_fixture_totals', {}).get('synthetic_fixture_files') == len(synthetic_paths) == 2, str(REGISTRY_JSON), 'M315-B004-REG-04', 'synthetic ll count drifted', failures)
    checks_passed += require(registry.get('ll_fixture_totals', {}).get('generated_replay_candidate_files') == len(replay_paths) == 76, str(REGISTRY_JSON), 'M315-B004-REG-05', 'replay ll count drifted', failures)
    checks_passed += require(registry.get('ll_fixture_totals', {}).get('replay_with_frontend_header') == len(with_header) == 30, str(REGISTRY_JSON), 'M315-B004-REG-06', 'replay ll-with-header count drifted', failures)
    checks_passed += require(registry.get('ll_fixture_totals', {}).get('replay_without_frontend_header') == len(without_header) == 46, str(REGISTRY_JSON), 'M315-B004-REG-07', 'replay ll-without-header count drifted', failures)
    checks_passed += require(registry.get('classes', {}).get('synthetic_fixture', {}).get('required_header_substring') == 'fixture parity IR', str(REGISTRY_JSON), 'M315-B004-REG-08', 'synthetic header requirement drifted', failures)
    checks_passed += require(registry.get('classes', {}).get('synthetic_fixture', {}).get('required_json_root_field') == 'artifact_authenticity', str(REGISTRY_JSON), 'M315-B004-REG-08A', 'synthetic json root field drifted', failures)
    checks_passed += require(registry.get('classes', {}).get('synthetic_fixture', {}).get('required_fixture_family_id') == 'objc3c.fixture.synthetic.librarycliparity.v1', str(REGISTRY_JSON), 'M315-B004-REG-08B', 'synthetic fixture family drifted', failures)
    checks_passed += require(registry.get('classes', {}).get('generated_replay_candidate', {}).get('legacy_missing_header_owner_issue') == 'M315-C003', str(REGISTRY_JSON), 'M315-B004-REG-09', 'legacy missing-header owner drifted', failures)

    checks_total += 7
    checks_passed += require(registry.get('synthetic_fixture_paths') == synthetic_paths, str(REGISTRY_JSON), 'M315-B004-PATH-01', 'synthetic fixture path list drifted', failures)
    checks_passed += require(registry.get('synthetic_labelled_json_paths') == synthetic_json_paths, str(REGISTRY_JSON), 'M315-B004-PATH-01A', 'synthetic labelled json path list drifted', failures)
    checks_passed += require(len(explicit_synthetic) == 2, str(REGISTRY_JSON), 'M315-B004-PATH-02', 'explicit synthetic label count drifted', failures)
    checks_passed += require(registry.get('legacy_missing_header_examples', [None])[0] == without_header[0], str(REGISTRY_JSON), 'M315-B004-PATH-03', 'legacy missing-header example drifted', failures)
    checks_passed += require(registry.get('downstream_ownership', {}).get('fixture_relocation_and_labeling') == 'M315-C004', str(REGISTRY_JSON), 'M315-B004-PATH-04', 'fixture relocation owner drifted', failures)
    checks_passed += require(registry.get('relocation_resolution') == 'filesystem_root_retained_labeling_promoted_to_authenticity_contract', str(REGISTRY_JSON), 'M315-B004-PATH-04A', 'relocation resolution drifted', failures)
    checks_passed += require(registry.get('next_issue') == 'M315-B005', str(REGISTRY_JSON), 'M315-B004-PATH-05', 'next issue drifted', failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        'mode': registry['mode'],
        'contract_id': registry['contract_id'],
        'ok': not failures,
        'checks_total': checks_total,
        'checks_passed': checks_passed,
        'tracked_ll_files': len(ll_files),
        'synthetic_fixture_files': synthetic_paths,
        'synthetic_labelled_json_paths': synthetic_json_paths,
        'replay_with_header_count': len(with_header),
        'replay_without_header_count': len(without_header),
        'next_issue': 'M315-B005',
        'failures': [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-B004 IR fixture compatibility checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
