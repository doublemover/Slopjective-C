#!/usr/bin/env python3
"""Fail-closed contract checker for M263-E001 bootstrap completion conformance gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-e001-bootstrap-completion-conformance-gate-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-completion-gate/m263-e001-v1"
EVIDENCE_MODEL = "a002-b003-c003-d003-summary-chain"
FAILURE_MODEL = "fail-closed-on-bootstrap-completion-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M263-E002"
A002_CONTRACT_ID = "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1"
B003_CONTRACT_ID = "objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1"
C003_CONTRACT_ID = "objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1"
D003_CONTRACT_ID = "objc3c-runtime-live-restart-hardening/m263-d003-v1"
D002_CONTRACT_ID = "objc3c-runtime-live-registration-discovery-replay/m263-d002-v1"
M254_D003_CONTRACT_ID = "objc3c-runtime-bootstrap-reset-replay/m254-d003-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-E001" / "bootstrap_completion_conformance_gate_summary.json"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_bootstrap_completion_conformance_gate_contract_and_architecture_freeze_e001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m263_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A002_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-A002" / "registration_manifest_and_descriptor_frontend_closure_summary.json"
DEFAULT_B003_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-B003" / "bootstrap_failure_restart_semantics_summary.json"
DEFAULT_C003_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-C003" / "archive_static_link_bootstrap_replay_corpus_summary.json"
DEFAULT_D003_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-D003" / "live_restart_hardening_summary.json"


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
    SnippetCheck("M263-E001-DOC-EXP-01", "# M263 Bootstrap Completion Conformance Gate Contract And Architecture Freeze Expectations (E001)"),
    SnippetCheck("M263-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-E001-DOC-EXP-03", "tmp/reports/m263/M263-C003/archive_static_link_bootstrap_replay_corpus_summary.json"),
    SnippetCheck("M263-E001-DOC-EXP-04", "tmp/reports/m263/M263-D003/live_restart_hardening_summary.json"),
    SnippetCheck("M263-E001-DOC-EXP-05", "The gate must explicitly hand off to `M263-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck("M263-E001-DOC-PKT-01", "# M263-E001 Bootstrap Completion Conformance Gate Contract And Architecture Freeze Packet"),
    SnippetCheck("M263-E001-DOC-PKT-02", "Packet: `M263-E001`"),
    SnippetCheck("M263-E001-DOC-PKT-03", "- `M263-C003`"),
    SnippetCheck("M263-E001-DOC-PKT-04", "- `M263-D003`"),
    SnippetCheck("M263-E001-DOC-PKT-05", "`check:objc3c:m263-e001-lane-e-readiness`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M263-E001-NDOC-01", "## Bootstrap completion conformance gate (M263-E001)"),
    SnippetCheck("M263-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck("M263-E001-NDOC-04", "tmp/reports/m263/M263-E001/bootstrap_completion_conformance_gate_summary.json"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M263-E001-SPC-01", "## M263 bootstrap completion conformance gate (E001)"),
    SnippetCheck("M263-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-E001-SPC-03", f"`{FAILURE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M263-E001-META-01", "## M263 bootstrap completion conformance gate metadata anchors (E001)"),
    SnippetCheck("M263-E001-META-02", "`module.runtime-registration-descriptor.json`"),
    SnippetCheck("M263-E001-META-03", "single-image and multi-image retained-link bootstrap evidence"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M263-E001-DRV-01", "M263-E001 bootstrap-completion gate anchor"),
    SnippetCheck("M263-E001-DRV-02", "A002/B003/C003/D003 proof chain"),
)
MANIFEST_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M263-E001-MAN-01", "M263-E001 bootstrap-completion gate anchor"),
    SnippetCheck("M263-E001-MAN-02", "canonical emitted artifact pair consumed by the lane-E bootstrap completion gate"),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M263-E001-FAPI-01", "M263-E001 bootstrap-completion gate anchor"),
    SnippetCheck("M263-E001-FAPI-02", "frontend C API publishes the same manifest/descriptor artifact pair as the CLI path"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M263-E001-RUN-01", "check:objc3c:m263-a002-lane-a-readiness"),
    SnippetCheck("M263-E001-RUN-02", "check:objc3c:m263-b003-lane-b-readiness"),
    SnippetCheck("M263-E001-RUN-03", "check:objc3c:m263-c003-lane-c-readiness"),
    SnippetCheck("M263-E001-RUN-04", "check:objc3c:m263-d003-lane-d-readiness"),
    SnippetCheck("M263-E001-RUN-05", "tests/tooling/test_check_m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M263-E001-PKG-01", '"check:objc3c:m263-e001-bootstrap-completion-conformance-gate": "python scripts/check_m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze.py"'),
    SnippetCheck("M263-E001-PKG-02", '"test:tooling:m263-e001-bootstrap-completion-conformance-gate": "python -m pytest tests/tooling/test_check_m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M263-E001-PKG-03", '"check:objc3c:m263-e001-lane-e-readiness": "python scripts/run_m263_e001_lane_e_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--manifest-artifacts-cpp", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=DEFAULT_B003_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
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
        failures.append(Finding(display_path(path), "M263-E001-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def validate_a002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    dynamic = payload.get("dynamic_probes", {})
    explicit = dynamic.get("explicit", {}) if isinstance(dynamic, dict) else {}
    default = dynamic.get("default", {}) if isinstance(dynamic, dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E001-A002-01", "A002 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == A002_CONTRACT_ID, artifact, "M263-E001-A002-02", "A002 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(isinstance(dynamic, dict) and dynamic.get("skipped") is False, artifact, "M263-E001-A002-03", "A002 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(explicit.get("backend") == "llvm-direct" and default.get("backend") == "llvm-direct", artifact, "M263-E001-A002-04", "A002 backends must remain llvm-direct", failures)
    checks_total += 1
    checks_passed += require(explicit.get("artifact") == "module.runtime-registration-descriptor.json" and default.get("artifact") == "module.runtime-registration-descriptor.json", artifact, "M263-E001-A002-05", "A002 descriptor artifact path drifted", failures)
    checks_total += 1
    checks_passed += require(explicit.get("registration_descriptor_identity_source") == "source-pragma", artifact, "M263-E001-A002-06", "A002 explicit descriptor identity source drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("registration_descriptor_identity_source") == "module-derived-default", artifact, "M263-E001-A002-07", "A002 default descriptor identity source drifted", failures)

    distilled = {
        "ok": True,
        "explicit_backend": explicit.get("backend"),
        "default_backend": default.get("backend"),
        "artifact": explicit.get("artifact"),
        "explicit_registration_descriptor_identity_source": explicit.get("registration_descriptor_identity_source"),
        "default_registration_descriptor_identity_source": default.get("registration_descriptor_identity_source"),
    }
    return checks_total, checks_passed, distilled


def validate_b003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    dynamic = payload.get("dynamic_probes", {})
    default = dynamic.get("default", {}) if isinstance(dynamic, dict) else {}
    explicit = dynamic.get("explicit", {}) if isinstance(dynamic, dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E001-B003-01", "B003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == B003_CONTRACT_ID, artifact, "M263-E001-B003-02", "B003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M263-E001-B003-03", "B003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M263-E001-B003-04", "B003 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(default.get("startup_registered_image_count") == 1 and explicit.get("startup_registered_image_count") == 1, artifact, "M263-E001-B003-05", "B003 startup registered image count drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("unsupported_replay_status") == -1 and explicit.get("unsupported_replay_status") == -1, artifact, "M263-E001-B003-06", "B003 unsupported replay status drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("post_reset_registered_image_count") == 0 and explicit.get("post_reset_registered_image_count") == 0, artifact, "M263-E001-B003-07", "B003 reset registered image count drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("first_restart_status") == 0 and explicit.get("first_restart_status") == 0, artifact, "M263-E001-B003-08", "B003 first restart status drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("second_restart_status") == 0 and explicit.get("second_restart_status") == 0, artifact, "M263-E001-B003-09", "B003 second restart status drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("second_restart_replay_generation") == 2 and explicit.get("second_restart_replay_generation") == 2, artifact, "M263-E001-B003-10", "B003 second restart replay generation drifted", failures)

    distilled = {
        "ok": True,
        "default_startup_registered_image_count": default.get("startup_registered_image_count"),
        "default_unsupported_replay_status": default.get("unsupported_replay_status"),
        "default_second_restart_replay_generation": default.get("second_restart_replay_generation"),
        "explicit_startup_registered_image_count": explicit.get("startup_registered_image_count"),
    }
    return checks_total, checks_passed, distilled


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    dynamic = payload.get("dynamic_summary", {})
    plain = dynamic.get("plain_probe", {}) if isinstance(dynamic, dict) else {}
    single = dynamic.get("single_probe", {}) if isinstance(dynamic, dict) else {}
    merged = dynamic.get("merged_probe", {}) if isinstance(dynamic, dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E001-C003-01", "C003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == C003_CONTRACT_ID, artifact, "M263-E001-C003-02", "C003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M263-E001-C003-03", "C003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M263-E001-C003-04", "C003 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(dynamic.get("case_id") == "M263-C003-CORPUS", artifact, "M263-E001-C003-05", "C003 corpus case id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("bootstrap_failure_restart_contract_id") == B003_CONTRACT_ID, artifact, "M263-E001-C003-06", "C003 upstream B003 contract drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("registration_descriptor_lowering_contract_id") == "objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1", artifact, "M263-E001-C003-07", "C003 upstream C002 contract drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("object_format") == "coff", artifact, "M263-E001-C003-08", "C003 object format drifted", failures)
    checks_total += 1
    checks_passed += require(len(payload.get("dynamic_summary", {}).get("translation_unit_identity_keys", [])) == 2, artifact, "M263-E001-C003-09", "C003 translation unit identity key count drifted", failures)
    checks_total += 1
    checks_passed += require(len(payload.get("dynamic_summary", {}).get("merged_driver_linker_flags", [])) == 2, artifact, "M263-E001-C003-10", "C003 merged linker flag count drifted", failures)
    checks_total += 1
    checks_passed += require(plain.get("startup_registered_image_count") == 0, artifact, "M263-E001-C003-11", "C003 plain link startup registered image count drifted", failures)
    checks_total += 1
    checks_passed += require(single.get("startup_registered_image_count") == 1 and single.get("post_replay_registered_image_count") == 1, artifact, "M263-E001-C003-12", "C003 single retained replay evidence drifted", failures)
    checks_total += 1
    checks_passed += require(merged.get("startup_registered_image_count") == 2 and merged.get("post_replay_registered_image_count") == 2, artifact, "M263-E001-C003-13", "C003 merged retained replay evidence drifted", failures)
    checks_total += 1
    checks_passed += require(merged.get("startup_next_expected_registration_order_ordinal") == 3 and merged.get("post_replay_next_expected_registration_order_ordinal") == 3, artifact, "M263-E001-C003-14", "C003 merged ordinal continuity drifted", failures)

    distilled = {
        "ok": True,
        "object_format": payload.get("object_format"),
        "plain_startup_registered_image_count": plain.get("startup_registered_image_count"),
        "single_startup_registered_image_count": single.get("startup_registered_image_count"),
        "single_post_replay_registered_image_count": single.get("post_replay_registered_image_count"),
        "merged_startup_registered_image_count": merged.get("startup_registered_image_count"),
        "merged_post_replay_registered_image_count": merged.get("post_replay_registered_image_count"),
        "merged_driver_linker_flag_count": len(payload.get("dynamic_summary", {}).get("merged_driver_linker_flags", [])),
    }
    return checks_total, checks_passed, distilled


def validate_d003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    cases = payload.get("dynamic_cases", []) if isinstance(payload.get("dynamic_cases"), list) else []
    case_map = {str(case.get("case_id")): case for case in cases if isinstance(case, dict)}
    default = case_map.get("default", {})
    explicit = case_map.get("explicit", {})
    default_probe = default.get("probe_payload", {}) if isinstance(default, dict) else {}
    explicit_probe = explicit.get("probe_payload", {}) if isinstance(explicit, dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E001-D003-01", "D003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == D003_CONTRACT_ID, artifact, "M263-E001-D003-02", "D003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M263-E001-D003-03", "D003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_skipped") is False and payload.get("dynamic_case_count") == 2, artifact, "M263-E001-D003-04", "D003 dynamic probe coverage drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("upstream_live_registration_contract_id") == D002_CONTRACT_ID, artifact, "M263-E001-D003-05", "D003 upstream D002 contract drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("upstream_failure_restart_contract_id") == B003_CONTRACT_ID, artifact, "M263-E001-D003-06", "D003 upstream B003 contract drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("upstream_reset_replay_contract_id") == M254_D003_CONTRACT_ID, artifact, "M263-E001-D003-07", "D003 upstream M254-D003 contract drifted", failures)
    checks_total += 1
    checks_passed += require(default_probe.get("startup_registered_image_count") == 1 and explicit_probe.get("startup_registered_image_count") == 1, artifact, "M263-E001-D003-08", "D003 startup registered image count drifted", failures)
    checks_total += 1
    checks_passed += require(default_probe.get("unsupported_replay_status") == -1 and explicit_probe.get("unsupported_replay_status") == -1, artifact, "M263-E001-D003-09", "D003 unsupported replay status drifted", failures)
    checks_total += 1
    checks_passed += require(default_probe.get("post_reset_registered_image_count") == 0 and explicit_probe.get("post_reset_registered_image_count") == 0, artifact, "M263-E001-D003-10", "D003 reset registered image count drifted", failures)
    checks_total += 1
    checks_passed += require(default_probe.get("first_restart_status") == 0 and explicit_probe.get("first_restart_status") == 0, artifact, "M263-E001-D003-11", "D003 first restart status drifted", failures)
    checks_total += 1
    checks_passed += require(default_probe.get("second_restart_status") == 0 and explicit_probe.get("second_restart_status") == 0, artifact, "M263-E001-D003-12", "D003 second restart status drifted", failures)
    checks_total += 1
    checks_passed += require(default_probe.get("second_restart_replay_generation") == 2 and explicit_probe.get("second_restart_replay_generation") == 2, artifact, "M263-E001-D003-13", "D003 second restart replay generation drifted", failures)

    distilled = {
        "ok": True,
        "default_startup_registered_image_count": default_probe.get("startup_registered_image_count"),
        "default_post_reset_registered_image_count": default_probe.get("post_reset_registered_image_count"),
        "default_second_restart_replay_generation": default_probe.get("second_restart_replay_generation"),
        "explicit_startup_registered_image_count": explicit_probe.get("startup_registered_image_count"),
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
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.manifest_artifacts_cpp, MANIFEST_ARTIFACTS_SNIPPETS),
        (args.frontend_anchor_cpp, FRONTEND_ANCHOR_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in snippet_groups:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_evidence: dict[str, Any] = {}
    for name, validator, path in (
        ("M263-A002", validate_a002, args.a002_summary),
        ("M263-B003", validate_b003, args.b003_summary),
        ("M263-C003", validate_c003, args.c003_summary),
        ("M263-D003", validate_d003, args.d003_summary),
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
