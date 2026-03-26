#!/usr/bin/env python3
"""Checker for M313-B002 shared compiler/runtime harness."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-B002" / "shared_compiler_runtime_harness_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_shared_compiler_runtime_integration_harness_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation_packet.md"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation_registry.json"
HARNESS = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
PACKAGE_JSON = ROOT / "package.json"


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


def run_json(command: list[str]) -> tuple[int, dict[str, object] | None, str]:
    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)
    if completed.returncode != 0:
        return completed.returncode, None, completed.stderr or completed.stdout
    try:
        return completed.returncode, json.loads(completed.stdout), ""
    except json.JSONDecodeError as exc:
        return 1, None, str(exc)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    registry = json.loads(read_text(REGISTRY_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-shared-compiler-runtime-harness/m313-b002-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-B002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Expose a stable CLI" in expectations, str(EXPECTATIONS_DOC), "M313-B002-EXP-02", "expectations missing CLI requirement", failures)
    checks_passed += require("shared suite registry" in packet, str(PACKET_DOC), "M313-B002-PKT-01", "packet missing registry focus", failures)
    checks_passed += require("Next issue: `M313-B003`." in packet, str(PACKET_DOC), "M313-B002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(registry.get("mode") == "m313-b002-shared-compiler-runtime-harness-v1", str(REGISTRY_JSON), "M313-B002-CON-01", "mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-cleanup-shared-compiler-runtime-harness/m313-b002-v1", str(REGISTRY_JSON), "M313-B002-CON-02", "contract id drifted", failures)
    checks_passed += require([entry.get("suite_id") for entry in registry.get("suite_registry", [])] == ["runtime_bootstrap_dispatch", "frontend_split_recovery", "module_parity_packaging", "native_fixture_corpus_and_runtime_probes"], str(REGISTRY_JSON), "M313-B002-CON-03", "suite registry drifted", failures)
    checks_passed += require(registry.get("cli_modes") == ["list-suites", "show-suite", "check-roots"], str(REGISTRY_JSON), "M313-B002-CON-04", "CLI modes drifted", failures)
    checks_passed += require(registry.get("next_issue") == "M313-B003", str(REGISTRY_JSON), "M313-B002-CON-05", "next issue drifted", failures)
    checks_passed += require(HARNESS.exists(), str(HARNESS), "M313-B002-CON-06", "shared harness script missing", failures)

    rc, list_payload, list_error = run_json([sys.executable, str(HARNESS), "--list-suites"])
    checks_total += 2
    checks_passed += require(rc == 0, str(HARNESS), "M313-B002-HARNESS-01", f"list-suites failed: {list_error}", failures)
    checks_passed += require(isinstance(list_payload, dict) and [entry.get("suite_id") for entry in list_payload.get("suite_registry", [])] == ["runtime_bootstrap_dispatch", "frontend_split_recovery", "module_parity_packaging", "native_fixture_corpus_and_runtime_probes"], str(HARNESS), "M313-B002-HARNESS-02", "list-suites payload drifted", failures)

    root_summary_path = ROOT / "tmp" / "reports" / "m313" / "M313-B002" / "shared_harness_root_check.json"
    rc, root_payload, root_error = run_json([sys.executable, str(HARNESS), "--check-roots", "--summary-out", str(root_summary_path)])
    checks_total += 3
    checks_passed += require(rc == 0, str(HARNESS), "M313-B002-HARNESS-03", f"check-roots failed: {root_error}", failures)
    checks_passed += require(isinstance(root_payload, dict) and root_payload.get("ok") is True, str(HARNESS), "M313-B002-HARNESS-04", "root-check payload missing ok=true", failures)
    checks_passed += require(root_summary_path.exists(), str(root_summary_path), "M313-B002-HARNESS-05", "root-check summary output missing", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-b002-shared-compiler-runtime-integration-harness-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-B002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-b002-shared-compiler-runtime-integration-harness-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-B002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-b002-lane-b-readiness"' in package, str(PACKAGE_JSON), "M313-B002-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": registry["mode"],
        "contract_id": registry["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "suite_ids": [entry["suite_id"] for entry in registry["suite_registry"]],
        "cli_modes": registry["cli_modes"],
        "next_issue": "M313-B003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-B002 shared harness checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
