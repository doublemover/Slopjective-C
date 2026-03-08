#!/usr/bin/env python3
"""Fail-closed contract checker for M254-E001 startup registration gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-e001-startup-registration-gate-v1"
CONTRACT_ID = "objc3c-runtime-startup-registration-gate/m254-e001-v1"
EVIDENCE_MODEL = "a002-b002-c003-d003-d004-summary-chain"
FAILURE_MODEL = "fail-closed-on-bootstrap-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M254-E002"
D004_LAUNCH_CONTRACT_ID = "objc3c-runtime-launch-integration/m254-d004-v1"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_startup_registration_gate_contract_and_architecture_freeze_e001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_e001_startup_registration_gate_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m254_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A002_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-A002" / "registration_manifests_and_constructor_root_ownership_summary.json"
DEFAULT_B002_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-B002" / "bootstrap_semantics_summary.json"
DEFAULT_C003_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-C003" / "registration_table_image_local_initialization_summary.json"
DEFAULT_D003_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-D003" / "deterministic_reset_replay_summary.json"
DEFAULT_D004_SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-D004" / "runtime_launch_integration_summary.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m254" / "M254-E001" / "startup_registration_gate_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M254-E001-DOC-EXP-01", "# M254 Startup Registration Gate Contract And Architecture Freeze Expectations (E001)"),
    SnippetCheck("M254-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-E001-DOC-EXP-03", "tmp/reports/m254/M254-D003/deterministic_reset_replay_summary.json"),
    SnippetCheck("M254-E001-DOC-EXP-04", "tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json"),
    SnippetCheck("M254-E001-DOC-EXP-05", "The gate must explicitly hand off to `M254-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-E001-DOC-PKT-01", "# M254-E001 Startup Registration Gate Contract And Architecture Freeze Packet"),
    SnippetCheck("M254-E001-DOC-PKT-02", "Packet: `M254-E001`"),
    SnippetCheck("M254-E001-DOC-PKT-03", "- `M254-C003`"),
    SnippetCheck("M254-E001-DOC-PKT-04", "- `M254-D004`"),
    SnippetCheck("M254-E001-DOC-PKT-05", "`check:objc3c:m254-e001-lane-e-readiness`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-E001-NDOC-01", "## Startup registration gate (M254-E001)"),
    SnippetCheck("M254-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck("M254-E001-NDOC-04", "tmp/reports/m254/M254-E001/startup_registration_gate_summary.json"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-E001-SPC-01", "## M254 startup registration gate (E001)"),
    SnippetCheck("M254-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-E001-SPC-03", f"`{FAILURE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-E001-META-01", "## M254 startup registration gate metadata anchors (E001)"),
    SnippetCheck("M254-E001-META-02", "`translation_unit_registration_order_ordinal`"),
    SnippetCheck("M254-E001-META-03", "replay-stable bootstrap evidence chain"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-E001-RTDOC-01", "`M254-E001` then freezes one lane-E startup-registration gate over the"),
    SnippetCheck("M254-E001-RTDOC-02", "`M254-D003` deterministic reset/replay proof"),
    SnippetCheck("M254-E001-RTDOC-03", "`M254-D004` operator launch contract"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-E001-DRV-01", "M254-E001 startup-registration gate anchor"),
    SnippetCheck("M254-E001-DRV-02", "A002/B002/C003/D003/D004 evidence chain"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-E001-ART-01", "M254-E001 startup-registration gate anchor"),
    SnippetCheck("M254-E001-ART-02", "semantic-surface registration manifest remains the canonical lane-E gate input"),
)
PROCESS_SNIPPETS = (
    SnippetCheck("M254-E001-PROC-01", "M254-E001 startup-registration gate anchor"),
    SnippetCheck("M254-E001-PROC-02", "replay-stable bootstrap evidence chain"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M254-E001-RUN-01", "check:objc3c:m254-a002-lane-a-readiness"),
    SnippetCheck("M254-E001-RUN-02", "check:objc3c:m254-b002-lane-b-readiness"),
    SnippetCheck("M254-E001-RUN-03", "check:objc3c:m254-c003-lane-c-readiness"),
    SnippetCheck("M254-E001-RUN-04", "check:objc3c:m254-d003-lane-d-readiness"),
    SnippetCheck("M254-E001-RUN-05", "check:objc3c:m254-d004-lane-d-readiness"),
    SnippetCheck("M254-E001-RUN-06", "tests/tooling/test_check_m254_e001_startup_registration_gate.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-E001-PKG-01", '"check:objc3c:m254-e001-startup-registration-gate": "python scripts/check_m254_e001_startup_registration_gate.py"'),
    SnippetCheck("M254-E001-PKG-02", '"test:tooling:m254-e001-startup-registration-gate": "python -m pytest tests/tooling/test_check_m254_e001_startup_registration_gate.py -q"'),
    SnippetCheck("M254-E001-PKG-03", '"check:objc3c:m254-e001-lane-e-readiness": "python scripts/run_m254_e001_lane_e_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--frontend-artifacts-cpp", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_CPP)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b002-summary", type=Path, default=DEFAULT_B002_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--d004-summary", type=Path, default=DEFAULT_D004_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M254-E001-MISSING", f"required artifact is missing: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def get_case_map(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    cases = payload.get("dynamic_cases", [])
    if isinstance(cases, list):
        result: dict[str, dict[str, Any]] = {}
        for index, case in enumerate(cases):
            if isinstance(case, dict):
                case_id = case.get("case_id") or case.get("fixture") or f"case-{index}"
                result[str(case_id)] = case
        return result
    return {}


def validate_a002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E001-A002-01", "A002 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E001-A002-02", "A002 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E001-A002-03", "A002 summary must report dynamic probes executed", failures)

    cases = payload.get("dynamic_cases")
    case = cases[0] if isinstance(cases, list) and cases else {}
    checks_total += 1
    checks_passed += require(isinstance(case, dict) and bool(case), artifact, "M254-E001-A002-04", "A002 summary must publish one dynamic case", failures)
    checks_total += 1
    checks_passed += require(str(case.get("fixture", "")).endswith("tests/tooling/fixtures/native/hello.objc3"), artifact, "M254-E001-A002-05", "A002 evidence must stay rooted on hello.objc3", failures)
    checks_total += 1
    checks_passed += require(case.get("backend") == "llvm-direct", artifact, "M254-E001-A002-06", "A002 backend must remain llvm-direct", failures)
    checks_total += 1
    checks_passed += require(str(case.get("registration_manifest_path", "")).endswith(REGISTRATION_MANIFEST_ARTIFACT), artifact, "M254-E001-A002-07", "A002 registration manifest path must stay canonical", failures)
    checks_total += 1
    checks_passed += require(case.get("runtime_support_library_archive_relative_path") == "artifacts/lib/objc3_runtime.lib", artifact, "M254-E001-A002-08", "A002 runtime archive path drifted", failures)
    checks_total += 1
    checks_passed += require(str(case.get("constructor_init_stub_symbol", "")).startswith("__objc3_runtime_register_image_init_stub_"), artifact, "M254-E001-A002-09", "A002 init-stub symbol prefix drifted", failures)

    distilled = {
        "ok": True,
        "backend": case.get("backend"),
        "runtime_archive": case.get("runtime_support_library_archive_relative_path"),
        "registration_manifest": case.get("registration_manifest_path"),
        "constructor_init_stub_symbol": case.get("constructor_init_stub_symbol"),
    }
    return checks_total, checks_passed, distilled


def validate_b002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E001-B002-01", "B002 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E001-B002-02", "B002 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E001-B002-03", "B002 summary must report dynamic probes executed", failures)

    cases = payload.get("dynamic_cases")
    case = cases[0] if isinstance(cases, list) and cases else {}
    probe = case.get("probe_payload", {}) if isinstance(case, dict) else {}
    snapshots = probe.get("snapshots", {}) if isinstance(probe, dict) else {}
    after_success = snapshots.get("after_success", {}) if isinstance(snapshots, dict) else {}
    after_duplicate = snapshots.get("after_duplicate", {}) if isinstance(snapshots, dict) else {}
    after_out_of_order = snapshots.get("after_out_of_order", {}) if isinstance(snapshots, dict) else {}
    after_invalid = snapshots.get("after_invalid", {}) if isinstance(snapshots, dict) else {}

    checks_total += 1
    checks_passed += require(isinstance(case, dict) and bool(case), artifact, "M254-E001-B002-04", "B002 summary must publish one dynamic case", failures)
    checks_total += 1
    checks_passed += require(probe.get("success_status") == 0, artifact, "M254-E001-B002-05", "B002 success status drifted", failures)
    checks_total += 1
    checks_passed += require(probe.get("duplicate_status") == -2, artifact, "M254-E001-B002-06", "B002 duplicate status drifted", failures)
    checks_total += 1
    checks_passed += require(probe.get("out_of_order_status") == -3, artifact, "M254-E001-B002-07", "B002 out-of-order status drifted", failures)
    checks_total += 1
    checks_passed += require(probe.get("invalid_status") == -1, artifact, "M254-E001-B002-08", "B002 invalid status drifted", failures)
    checks_total += 1
    checks_passed += require(after_success.get("registered_image_count") == 1 and after_success.get("next_expected_registration_order_ordinal") == 2, artifact, "M254-E001-B002-09", "B002 success snapshot drifted", failures)
    checks_total += 1
    checks_passed += require(after_duplicate.get("registered_image_count") == 1 and after_duplicate.get("last_registration_status") == -2, artifact, "M254-E001-B002-10", "B002 duplicate snapshot drifted", failures)
    checks_total += 1
    checks_passed += require(after_out_of_order.get("registered_image_count") == 1 and after_out_of_order.get("last_registration_status") == -3, artifact, "M254-E001-B002-11", "B002 out-of-order snapshot drifted", failures)
    checks_total += 1
    checks_passed += require(after_invalid.get("registered_image_count") == 1 and after_invalid.get("last_registration_status") == -1, artifact, "M254-E001-B002-12", "B002 invalid snapshot drifted", failures)

    distilled = {
        "ok": True,
        "success_status": probe.get("success_status"),
        "duplicate_status": probe.get("duplicate_status"),
        "out_of_order_status": probe.get("out_of_order_status"),
        "invalid_status": probe.get("invalid_status"),
        "registered_image_count": after_success.get("registered_image_count"),
        "next_expected_registration_order_ordinal": after_success.get("next_expected_registration_order_ordinal"),
    }
    return checks_total, checks_passed, distilled


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E001-C003-01", "C003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E001-C003-02", "C003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E001-C003-03", "C003 summary must report dynamic probes executed", failures)

    cases = get_case_map(payload)
    metadata = cases.get("metadata-library", {})
    category = cases.get("category-library", {})
    for case_id, case, descriptor_total, category_count in (
        ("metadata-library", metadata, 8, 0),
        ("category-library", category, 12, 2),
    ):
        probe = case.get("probe_payload", {}) if isinstance(case, dict) else {}
        sections = case.get("sections", []) if isinstance(case, dict) else []
        checks_total += 1
        checks_passed += require(isinstance(case, dict) and bool(case), artifact, f"M254-E001-C003-{case_id}-01", f"C003 case {case_id} is missing", failures)
        checks_total += 1
        checks_passed += require(case.get("compile_exit_code") == 0, artifact, f"M254-E001-C003-{case_id}-02", f"C003 case {case_id} must compile successfully", failures)
        checks_total += 1
        checks_passed += require(all(section in sections for section in [".CRT$XCU", "objc3.runtime.discovery_root", "objc3.runtime.linker_anchor"]), artifact, f"M254-E001-C003-{case_id}-03", f"C003 case {case_id} must preserve startup/linker sections", failures)
        checks_total += 1
        checks_passed += require(probe.get("copy_status") == 0 and probe.get("registered_image_count") == 1 and probe.get("next_expected_registration_order_ordinal") == 2, artifact, f"M254-E001-C003-{case_id}-04", f"C003 case {case_id} startup probe drifted", failures)
        checks_total += 1
        checks_passed += require(case.get("descriptor_total") == descriptor_total, artifact, f"M254-E001-C003-{case_id}-05", f"C003 case {case_id} descriptor total drifted", failures)
        checks_total += 1
        checks_passed += require(case.get("category_descriptor_count") == category_count, artifact, f"M254-E001-C003-{case_id}-06", f"C003 case {case_id} category descriptor count drifted", failures)

    distilled = {
        "ok": True,
        "case_ids": sorted(cases.keys()),
        "metadata_library_descriptor_total": metadata.get("descriptor_total"),
        "category_library_descriptor_total": category.get("descriptor_total"),
        "category_library_category_descriptor_count": category.get("category_descriptor_count"),
    }
    return checks_total, checks_passed, distilled


def validate_d003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E001-D003-01", "D003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E001-D003-02", "D003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E001-D003-03", "D003 summary must report dynamic probes executed", failures)

    cases = get_case_map(payload)
    metadata = cases.get("metadata-library", {})
    category = cases.get("category-library", {})
    for case_id, case in (("metadata-library", metadata), ("category-library", category)):
        probe = case.get("probe_payload", {}) if isinstance(case, dict) else {}
        checks_total += 1
        checks_passed += require(isinstance(case, dict) and bool(case), artifact, f"M254-E001-D003-{case_id}-01", f"D003 case {case_id} is missing", failures)
        checks_total += 1
        checks_passed += require(probe.get("post_reset_registered_image_count") == 0 and probe.get("post_reset_next_expected_registration_order_ordinal") == 1, artifact, f"M254-E001-D003-{case_id}-02", f"D003 case {case_id} reset snapshot drifted", failures)
        checks_total += 1
        checks_passed += require(probe.get("post_reset_last_reset_cleared_image_local_init_state_count") == 1, artifact, f"M254-E001-D003-{case_id}-03", f"D003 case {case_id} cleared init-state count drifted", failures)
        checks_total += 1
        checks_passed += require(probe.get("replay_status") == 0 and probe.get("post_replay_registered_image_count") == 1 and probe.get("post_replay_walked_image_count") == 1, artifact, f"M254-E001-D003-{case_id}-04", f"D003 case {case_id} replay snapshot drifted", failures)
        checks_total += 1
        checks_passed += require(probe.get("post_replay_last_registration_used_staged_table") == 1 and probe.get("post_replay_replay_generation") == 1, artifact, f"M254-E001-D003-{case_id}-05", f"D003 case {case_id} replay generation drifted", failures)

    distilled = {
        "ok": True,
        "case_ids": sorted(cases.keys()),
        "metadata_library_replay_status": metadata.get("probe_payload", {}).get("replay_status"),
        "category_library_replay_status": category.get("probe_payload", {}).get("replay_status"),
    }
    return checks_total, checks_passed, distilled


def validate_d004(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M254-E001-D004-01", "D004 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M254-E001-D004-02", "D004 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M254-E001-D004-03", "D004 summary must report dynamic probes executed", failures)
    checks_total += 1
    checks_passed += require(str(payload.get("compile_case", {}).get("registration_manifest", "")).endswith(REGISTRATION_MANIFEST_ARTIFACT), artifact, "M254-E001-D004-04", "D004 compile case must publish the canonical registration manifest", failures)
    checks_total += 1
    checks_passed += require(str(payload.get("proof_case", {}).get("digest", "")).endswith("digest.json"), artifact, "M254-E001-D004-05", "D004 proof case must publish digest.json", failures)
    checks_total += 1
    checks_passed += require("status: PASS" in str(payload.get("proof_case", {}).get("stdout", "")), artifact, "M254-E001-D004-06", "D004 proof case must report PASS", failures)

    smoke_summary_path = ROOT / str(payload.get("smoke_case", {}).get("summary", ""))
    checks_total += 1
    checks_passed += require(smoke_summary_path.exists(), artifact, "M254-E001-D004-07", "D004 smoke summary must exist", failures)
    smoke_summary = load_json(smoke_summary_path) if smoke_summary_path.exists() else {}
    results = smoke_summary.get("results", []) if isinstance(smoke_summary.get("results"), list) else []
    positive_results = [row for row in results if isinstance(row, dict) and row.get("kind") == "positive"]

    checks_total += 1
    checks_passed += require(smoke_summary.get("status") == "PASS", display_path(smoke_summary_path), "M254-E001-D004-08", "D004 smoke summary must report PASS", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("compile_wrapper") == "scripts/objc3c_native_compile.ps1", display_path(smoke_summary_path), "M254-E001-D004-09", "D004 smoke compile wrapper drifted", failures)
    checks_total += 1
    checks_passed += require(smoke_summary.get("runtime_launch_contract_script") == "scripts/objc3c_runtime_launch_contract.ps1", display_path(smoke_summary_path), "M254-E001-D004-10", "D004 smoke launch helper drifted", failures)
    checks_total += 1
    checks_passed += require(isinstance(results, list) and smoke_summary.get("passed") == smoke_summary.get("total") and int(smoke_summary.get("total", 0)) > 0, display_path(smoke_summary_path), "M254-E001-D004-11", "D004 smoke must report full passing coverage", failures)
    checks_total += 1
    checks_passed += require(len(positive_results) > 0, display_path(smoke_summary_path), "M254-E001-D004-12", "D004 smoke must include positive cases", failures)
    checks_total += 1
    checks_passed += require(all(row.get("launch_integration_contract_id") == D004_LAUNCH_CONTRACT_ID and row.get("registration_manifest") == REGISTRATION_MANIFEST_ARTIFACT for row in positive_results), display_path(smoke_summary_path), "M254-E001-D004-13", "D004 positive smoke cases must retain launch-contract fields", failures)
    checks_total += 1
    checks_passed += require(all(row.get("runtime_library_source") == "registration-manifest.runtime_support_library_archive_relative_path" for row in positive_results), display_path(smoke_summary_path), "M254-E001-D004-14", "D004 positive smoke cases must resolve the runtime archive from the registration manifest", failures)
    checks_total += 1
    checks_passed += require(all(isinstance(row.get("driver_linker_flags"), list) and len(row.get("driver_linker_flags")) > 0 for row in positive_results), display_path(smoke_summary_path), "M254-E001-D004-15", "D004 positive smoke cases must record emitted driver linker flags", failures)

    distilled = {
        "ok": True,
        "smoke_status": smoke_summary.get("status"),
        "positive_case_count": len(positive_results),
        "compile_wrapper": smoke_summary.get("compile_wrapper"),
        "runtime_launch_contract_script": smoke_summary.get("runtime_launch_contract_script"),
    }
    return checks_total, checks_passed, distilled


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_groups = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.frontend_artifacts_cpp, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in snippet_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_evidence: dict[str, Any] = {}
    for name, validator, path in (
        ("M254-A002", validate_a002, args.a002_summary),
        ("M254-B002", validate_b002, args.b002_summary),
        ("M254-C003", validate_c003, args.c003_summary),
        ("M254-D003", validate_d003, args.d003_summary),
        ("M254-D004", validate_d004, args.d004_summary),
    ):
        total, passed, distilled = validator(path, failures)
        checks_total += total
        checks_passed += passed
        upstream_evidence[name] = distilled

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": len(failures) == 0,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": True,
        "evidence_model": EVIDENCE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "upstream_evidence": upstream_evidence,
        "failures": [finding.__dict__ for finding in failures],
    }
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
