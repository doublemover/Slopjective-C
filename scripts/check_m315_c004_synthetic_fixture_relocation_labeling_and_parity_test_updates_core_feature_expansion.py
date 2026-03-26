#!/usr/bin/env python3
"""Checker for M315-C004 synthetic fixture labeling and parity updates."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m315" / "M315-C004" / "synthetic_fixture_labeling_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m315_synthetic_fixture_relocation_labeling_and_parity_test_updates_core_feature_expansion_c004_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_c004_synthetic_fixture_relocation_labeling_and_parity_test_updates_core_feature_expansion_packet.md"
RESULT_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_c004_synthetic_fixture_relocation_labeling_and_parity_test_updates_core_feature_expansion_result.json"
PARITY_SCRIPT = ROOT / "scripts" / "check_objc3c_library_cli_parity.py"
A003_CHECKER = ROOT / "scripts" / "check_m315_a003_artifact_authenticity_and_provenance_classification_inventory_source_completion.py"
B004_CHECKER = ROOT / "scripts" / "check_m315_b004_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation.py"
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native" / "library_cli_parity"
LIBRARY_DIR = FIXTURE_ROOT / "library"
CLI_DIR = FIXTURE_ROOT / "cli"
GOLDEN_SUMMARY = FIXTURE_ROOT / "golden_summary.json"


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


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise ValueError(f"expected JSON object at {path}")
    return payload


def parse_ll_metadata(path: Path) -> dict[str, str]:
    metadata: dict[str, str] = {}
    for line in read_text(path).splitlines()[:16]:
        stripped = line.strip()
        if not stripped.startswith(";") or ":" not in stripped:
            continue
        key, value = stripped[1:].split(":", 1)
        metadata[key.strip()] = value.strip()
    return metadata


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    result = read_json(RESULT_JSON)
    golden = read_json(GOLDEN_SUMMARY)
    library_manifest = read_json(LIBRARY_DIR / "module.manifest.json")
    cli_manifest = read_json(CLI_DIR / "module.manifest.json")
    library_ll_metadata = parse_ll_metadata(LIBRARY_DIR / "module.ll")
    cli_ll_metadata = parse_ll_metadata(CLI_DIR / "module.ll")

    checks_total += 6
    checks_passed += require("objc3c-cleanup-synthetic-fixture-labeling-parity-updates/m315-c004-v1" in expectations, str(EXPECTATIONS_DOC), "M315-C004-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Physical relocation is intentionally not performed" in expectations, str(EXPECTATIONS_DOC), "M315-C004-EXP-02", "expectations missing relocation decision", failures)
    checks_passed += require("artifact-authenticity model introduced in `M315-C002`" in packet, str(PACKET_DOC), "M315-C004-PKT-01", "packet missing authenticity motivation", failures)
    checks_passed += require("Next issue: `M315-D001`." in packet, str(PACKET_DOC), "M315-C004-PKT-02", "packet missing next issue", failures)
    checks_passed += require(result.get("relocation_decision") == "filesystem_root_retained_labeling_promoted_to_authenticity_contract", str(RESULT_JSON), "M315-C004-RES-01", "result relocation decision drifted", failures)
    checks_passed += require(result.get("next_issue") == "M315-D001", str(RESULT_JSON), "M315-C004-RES-02", "result next issue drifted", failures)

    expected_manifest_auth = {
        "authenticity_schema_id": "objc3c.artifact.authenticity.schema.v1",
        "artifact_family_id": "objc3c.fixture.synthetic.librarycliparity.manifest.v1",
        "provenance_class": "synthetic_fixture",
        "provenance_mode": "fixture_curated",
        "content_role": "fixture_parity_manifest",
        "fixture_family_id": "objc3c.fixture.synthetic.librarycliparity.v1",
        "synthetic_reason": "fixture parity IR for CLI versus library behavior",
        "explicit_fixture_label": "fixture parity IR",
    }
    expected_summary_auth = {
        "authenticity_schema_id": "objc3c.artifact.authenticity.schema.v1",
        "artifact_family_id": "objc3c.fixture.synthetic.librarycliparity.golden_summary.v1",
        "provenance_class": "synthetic_fixture",
        "provenance_mode": "fixture_curated",
        "content_role": "fixture_parity_golden_summary",
        "fixture_family_id": "objc3c.fixture.synthetic.librarycliparity.v1",
        "synthetic_reason": "fixture parity IR for CLI versus library behavior",
        "explicit_fixture_label": "fixture parity IR",
    }
    expected_ll = {
        "artifact_family_id": "objc3c.fixture.synthetic.librarycliparity.v1",
        "provenance_class": "synthetic_fixture",
        "provenance_mode": "fixture_curated",
        "fixture_family_id": "objc3c.fixture.synthetic.librarycliparity.v1",
        "explicit_fixture_label": "fixture parity IR",
    }

    checks_total += 8
    checks_passed += require(library_manifest.get("artifact_authenticity") == expected_manifest_auth, str(LIBRARY_DIR / "module.manifest.json"), "M315-C004-FIX-01", "library manifest authenticity envelope drifted", failures)
    checks_passed += require(cli_manifest.get("artifact_authenticity") == expected_manifest_auth, str(CLI_DIR / "module.manifest.json"), "M315-C004-FIX-02", "cli manifest authenticity envelope drifted", failures)
    checks_passed += require(golden.get("artifact_authenticity") == expected_summary_auth, str(GOLDEN_SUMMARY), "M315-C004-FIX-03", "golden summary authenticity envelope drifted", failures)
    checks_passed += require(golden.get("synthetic_fixture_contract", {}).get("relocation_decision") == "filesystem_root_retained_labeling_promoted_to_authenticity_contract", str(GOLDEN_SUMMARY), "M315-C004-FIX-04", "golden summary relocation decision drifted", failures)
    checks_passed += require(library_ll_metadata == expected_ll, str(LIBRARY_DIR / "module.ll"), "M315-C004-FIX-05", "library ll authenticity header drifted", failures)
    checks_passed += require(cli_ll_metadata == expected_ll, str(CLI_DIR / "module.ll"), "M315-C004-FIX-06", "cli ll authenticity header drifted", failures)
    checks_passed += require(golden.get("authenticity_checks", {}).get("applied") is True, str(GOLDEN_SUMMARY), "M315-C004-FIX-07", "golden summary missing authenticity checks", failures)
    checks_passed += require(golden.get("authenticity_checks", {}).get("failure_count") == 0, str(GOLDEN_SUMMARY), "M315-C004-FIX-08", "golden summary authenticity failure count drifted", failures)

    a003_result = run_command([sys.executable, str(A003_CHECKER)])
    b004_result = run_command([sys.executable, str(B004_CHECKER)])
    parity_pass_result = run_command(
        [
            sys.executable,
            str(PARITY_SCRIPT),
            "--library-dir",
            str(LIBRARY_DIR),
            "--cli-dir",
            str(CLI_DIR),
            "--summary-out",
            str(ROOT / "tmp" / "reports" / "m315" / "M315-C004" / "golden_validation_summary.json"),
            "--golden-summary",
            str(GOLDEN_SUMMARY),
            "--check-golden",
        ]
    )

    tampered_root = ROOT / "tmp" / "reports" / "m315" / "M315-C004" / "tampered_fixture"
    shutil.copytree(FIXTURE_ROOT, tampered_root, dirs_exist_ok=True)
    tampered_cli_manifest_path = tampered_root / "cli" / "module.manifest.json"
    tampered_cli_manifest = read_json(tampered_cli_manifest_path)
    tampered_cli_manifest.pop("artifact_authenticity", None)
    tampered_cli_manifest_path.write_text(json.dumps(tampered_cli_manifest, indent=2) + "\n", encoding="utf-8")
    parity_fail_result = run_command(
        [
            sys.executable,
            str(PARITY_SCRIPT),
            "--library-dir",
            str(tampered_root / "library"),
            "--cli-dir",
            str(tampered_root / "cli"),
            "--summary-out",
            str(ROOT / "tmp" / "reports" / "m315" / "M315-C004" / "tampered_validation_summary.json"),
            "--artifacts",
            "module.manifest.json",
            "module.ll",
        ]
    )

    checks_total += 4
    checks_passed += require(a003_result.returncode == 0, str(A003_CHECKER), "M315-C004-COMPAT-01", f"A003 checker failed: {a003_result.stderr.strip()}", failures)
    checks_passed += require(b004_result.returncode == 0, str(B004_CHECKER), "M315-C004-COMPAT-02", f"B004 checker failed: {b004_result.stderr.strip()}", failures)
    checks_passed += require(parity_pass_result.returncode == 0, str(PARITY_SCRIPT), "M315-C004-PARITY-01", f"committed parity check failed: {parity_pass_result.stderr.strip()}", failures)
    checks_passed += require(parity_fail_result.returncode == 1 and "synthetic fixture authenticity mismatch" in parity_fail_result.stderr, str(PARITY_SCRIPT), "M315-C004-PARITY-02", "tampered parity fixture did not fail closed on missing authenticity envelope", failures)

    summary = {
        "mode": result["mode"],
        "contract_id": result["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "fixture_root": result["fixture_root"],
        "relocation_decision": result["relocation_decision"],
        "committed_manifest_authenticity_class": library_manifest["artifact_authenticity"]["provenance_class"],
        "golden_summary_authenticity_class": golden["artifact_authenticity"]["provenance_class"],
        "ll_header_keys": sorted(library_ll_metadata.keys()),
        "a003_checker_ok": a003_result.returncode == 0,
        "b004_checker_ok": b004_result.returncode == 0,
        "parity_golden_ok": parity_pass_result.returncode == 0,
        "tampered_fail_closed": parity_fail_result.returncode == 1,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M315-C004 synthetic fixture labeling checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
