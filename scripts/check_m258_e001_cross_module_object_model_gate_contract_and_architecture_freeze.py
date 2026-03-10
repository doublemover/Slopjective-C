#!/usr/bin/env python3
"""Fail-closed checker for M258-E001 cross-module object-model gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-e001-cross-module-object-model-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-cross-module-object-model-gate/m258-e001-v1"
EVIDENCE_MODEL = "a002-b002-c002-d002-summary-chain"
EXECUTION_GATE_MODEL = (
    "cross-module-runnable-object-model-evidence-consumes-import-sema-reuse-and-runtime-packaging-proofs"
)
FAILURE_MODEL = "fail-closed-on-cross-module-object-model-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M258-E002"

M258_A002_CONTRACT_ID = "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"
M258_B002_CONTRACT_ID = "objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1"
M258_C002_CONTRACT_ID = "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1"
M258_D002_CONTRACT_ID = "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"

EXPECTED_IMPORTED_MODULES = ["runtimeMetadataCategoryRecords", "runtimeMetadataClassRecords"]
EXPECTED_REUSED_MODULES = [
    "runtimeImportedSemanticRulesConsumer",
    "runtimeMetadataCategoryRecords",
    "runtimeMetadataClassRecords",
]
EXPECTED_PACKAGED_MODULES = ["runtimePackagingConsumer", "runtimePackagingProvider"]

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m258_cross_module_object_model_gate_contract_and_architecture_freeze_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m258"
    / "m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze_packet.md"
)
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m258_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m258" / "M258-A002" / "runtime_aware_import_module_frontend_closure_summary.json"
)
DEFAULT_B002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m258" / "M258-B002" / "imported_runtime_metadata_semantic_rules_summary.json"
)
DEFAULT_C002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m258" / "M258-C002" / "module_metadata_artifact_reuse_summary.json"
)
DEFAULT_D002_SUMMARY = (
    ROOT / "tmp" / "reports" / "m258" / "M258-D002" / "cross_module_runtime_packaging_summary.json"
)
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-E001" / "cross_module_object_model_gate_summary.json"


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
    SnippetCheck(
        "M258-E001-DOC-EXP-01",
        "# M258 Cross-Module Object-Model Gate Contract And Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck("M258-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck(
        "M258-E001-DOC-EXP-03",
        "tmp/reports/m258/M258-D002/cross_module_runtime_packaging_summary.json",
    ),
    SnippetCheck("M258-E001-DOC-EXP-04", "The gate must explicitly hand off to `M258-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M258-E001-DOC-PKT-01",
        "# M258-E001 Cross-Module Object-Model Gate Contract And Architecture Freeze Packet",
    ),
    SnippetCheck("M258-E001-DOC-PKT-02", "Packet: `M258-E001`"),
    SnippetCheck("M258-E001-DOC-PKT-03", "- `M258-D002`"),
    SnippetCheck("M258-E001-DOC-PKT-04", "Next issue: `M258-E002`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M258-E001-NDOC-01", "## Cross-module object-model gate (M258-E001)"),
    SnippetCheck("M258-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M258-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck(
        "M258-E001-NDOC-04",
        "tmp/reports/m258/M258-E001/cross_module_object_model_gate_summary.json",
    ),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M258-E001-SPC-01", "## M258 cross-module object-model gate (E001)"),
    SnippetCheck("M258-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M258-E001-SPC-03", f"`{FAILURE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M258-E001-META-01", "## M258 cross-module object-model gate metadata anchors (E001)"),
    SnippetCheck("M258-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M258-E001-META-03",
        "tmp/reports/m258/M258-E001/cross_module_object_model_gate_summary.json",
    ),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M258-E001-ARCH-01", "## M258 cross-module object-model gate (E001)"),
    SnippetCheck(
        "M258-E001-ARCH-02",
        "`M258-E001` freezes the first lane-E proof gate for runnable cross-module",
    ),
    SnippetCheck(
        "M258-E001-ARCH-03",
        "`M258-E002` is the first issue allowed to broaden this freeze into a",
    ),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M258-E001-ART-01", "M258-E001 cross-module object-model gate anchor"),
    SnippetCheck(
        "M258-E001-ART-02",
        "the A002/B002/C002/D002 summary chain and freezes the current",
    ),
)
IR_SNIPPETS = (
    SnippetCheck("M258-E001-IR-01", "M258-E001 cross-module object-model gate anchor"),
    SnippetCheck(
        "M258-E001-IR-02",
        "still lives in the emitted evidence chain and not in any new",
    ),
)
API_SNIPPETS = (
    SnippetCheck("M258-E001-API-01", "M258-E001 cross-module object-model gate anchor"),
    SnippetCheck(
        "M258-E001-API-02",
        "no public in-memory cross-module object-model gate handle",
    ),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M258-E001-RUN-01", "check:objc3c:m258-a002-lane-a-readiness"),
    SnippetCheck(
        "M258-E001-RUN-02",
        "scripts/check_m258_b002_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation.py",
    ),
    SnippetCheck(
        "M258-E001-RUN-03",
        "scripts/check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py",
    ),
    SnippetCheck(
        "M258-E001-RUN-04",
        "scripts/check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py",
    ),
    SnippetCheck(
        "M258-E001-RUN-05",
        "tests/tooling/test_check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py",
    ),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M258-E001-PKG-01",
        '"check:objc3c:m258-e001-cross-module-object-model-gate": "python scripts/check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py"',
    ),
    SnippetCheck(
        "M258-E001-PKG-02",
        '"test:tooling:m258-e001-cross-module-object-model-gate": "python -m pytest tests/tooling/test_check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py -q"',
    ),
    SnippetCheck(
        "M258-E001-PKG-03",
        '"check:objc3c:m258-e001-lane-e-readiness": "python scripts/run_m258_e001_lane_e_readiness.py"',
    ),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--frontend-artifacts-cpp", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_CPP)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--api-h", type=Path, default=DEFAULT_API_H)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b002-summary", type=Path, default=DEFAULT_B002_SUMMARY)
    parser.add_argument("--c002-summary", type=Path, default=DEFAULT_C002_SUMMARY)
    parser.add_argument("--d002-summary", type=Path, default=DEFAULT_D002_SUMMARY)
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
        failures.append(Finding(display_path(path), "M258-E001-MISSING", f"required artifact is missing: {display_path(path)}"))
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
    total = 0
    passed = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    native_class = dynamic.get("native_class") if isinstance(dynamic, dict) else {}
    native_category = dynamic.get("native_category") if isinstance(dynamic, dict) else {}
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M258-E001-A002-01", "A002 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M258_A002_CONTRACT_ID, artifact, "M258-E001-A002-02", "A002 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E001-A002-03", "A002 summary must report full check coverage", failures)
    total += 1
    passed += require(dynamic.get("parity") is True, artifact, "M258-E001-A002-04", "A002 parity proof must remain true", failures)
    total += 1
    passed += require(native_class.get("module_name") == "runtimeMetadataClassRecords", artifact, "M258-E001-A002-05", "A002 native class module drifted", failures)
    total += 1
    passed += require(native_category.get("module_name") == "runtimeMetadataCategoryRecords", artifact, "M258-E001-A002-06", "A002 native category module drifted", failures)
    distilled = {
        "contract_id": payload.get("contract_id"),
        "parity": dynamic.get("parity"),
        "native_class_module": native_class.get("module_name"),
        "native_category_module": native_category.get("module_name"),
    }
    return total, passed, distilled


def validate_b002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    consumer = dynamic.get("consumer_surface") if isinstance(dynamic, dict) else {}
    duplicate = dynamic.get("duplicate_path_failure") if isinstance(dynamic, dict) else {}
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M258-E001-B002-01", "B002 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M258_B002_CONTRACT_ID, artifact, "M258-E001-B002-02", "B002 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E001-B002-03", "B002 summary must report full check coverage", failures)
    total += 1
    passed += require(consumer.get("ready") is True, artifact, "M258-E001-B002-04", "B002 consumer surface must remain ready", failures)
    total += 1
    passed += require(
        consumer.get("ready_for_cross_module_dispatch_equivalence") is True,
        artifact,
        "M258-E001-B002-05",
        "B002 dispatch-equivalence readiness drifted",
        failures,
    )
    total += 1
    passed += require(
        consumer.get("imported_module_names_lexicographic") == EXPECTED_IMPORTED_MODULES,
        artifact,
        "M258-E001-B002-06",
        "B002 imported module inventory drifted",
        failures,
    )
    total += 1
    passed += require(
        int(duplicate.get("returncode", 0)) != 0 and duplicate.get("diagnostic_contains_o3s264") is True,
        artifact,
        "M258-E001-B002-07",
        "B002 duplicate-path fail-closed proof drifted",
        failures,
    )
    distilled = {
        "contract_id": payload.get("contract_id"),
        "consumer_ready": consumer.get("ready"),
        "dispatch_equivalence_ready": consumer.get("ready_for_cross_module_dispatch_equivalence"),
        "imported_modules": consumer.get("imported_module_names_lexicographic"),
        "duplicate_path_failure": duplicate,
    }
    return total, passed, distilled


def validate_c002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    downstream = dynamic.get("downstream") if isinstance(dynamic, dict) else {}
    manifest_surface = downstream.get("manifest_surface") if isinstance(downstream, dict) else {}
    reuse_payload = downstream.get("reuse_payload") if isinstance(downstream, dict) else {}
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M258-E001-C002-01", "C002 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M258_C002_CONTRACT_ID, artifact, "M258-E001-C002-02", "C002 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E001-C002-03", "C002 summary must report full check coverage", failures)
    total += 1
    passed += require(manifest_surface.get("ready") is True, artifact, "M258-E001-C002-04", "C002 manifest surface must remain ready", failures)
    total += 1
    passed += require(manifest_surface.get("serialized_metadata_rehydration_landed") is True, artifact, "M258-E001-C002-05", "C002 serialized metadata rehydration drifted", failures)
    total += 1
    passed += require(manifest_surface.get("artifact_reuse_landed") is True, artifact, "M258-E001-C002-06", "C002 artifact reuse drifted", failures)
    total += 1
    passed += require(manifest_surface.get("downstream_module_consumption_ready") is True, artifact, "M258-E001-C002-07", "C002 downstream module consumption drifted", failures)
    total += 1
    passed += require(
        reuse_payload.get("reused_module_names_lexicographic") == EXPECTED_REUSED_MODULES,
        artifact,
        "M258-E001-C002-08",
        "C002 reused module set drifted",
        failures,
    )
    distilled = {
        "contract_id": payload.get("contract_id"),
        "manifest_ready": manifest_surface.get("ready"),
        "artifact_reuse_landed": manifest_surface.get("artifact_reuse_landed"),
        "reused_modules": reuse_payload.get("reused_module_names_lexicographic"),
    }
    return total, passed, distilled


def validate_d002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    total = 0
    passed = 0
    dynamic = payload.get("dynamic_probes") if isinstance(payload.get("dynamic_probes"), dict) else {}
    consumer = dynamic.get("consumer") if isinstance(dynamic, dict) else {}
    probe_summary = dynamic.get("probe") if isinstance(dynamic, dict) else {}
    plan = consumer.get("cross_module_link_plan") if isinstance(consumer, dict) else {}
    probe = probe_summary.get("probe_payload") if isinstance(probe_summary, dict) else {}
    total += 1
    passed += require(payload.get("ok") is True, artifact, "M258-E001-D002-01", "D002 summary must report ok=true", failures)
    total += 1
    passed += require(payload.get("contract_id") == M258_D002_CONTRACT_ID, artifact, "M258-E001-D002-02", "D002 contract id drifted", failures)
    total += 1
    passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M258-E001-D002-03", "D002 summary must report full check coverage", failures)
    total += 1
    passed += require(plan.get("ready") is True, artifact, "M258-E001-D002-04", "D002 link plan must remain ready", failures)
    total += 1
    passed += require(
        plan.get("module_names_lexicographic") == EXPECTED_PACKAGED_MODULES,
        artifact,
        "M258-E001-D002-05",
        "D002 packaged module set drifted",
        failures,
    )
    total += 1
    passed += require(int(probe_summary.get("link_returncode", -1)) == 0, artifact, "M258-E001-D002-06", "D002 probe link must remain successful", failures)
    total += 1
    passed += require(int(probe.get("imported_worker_conforms", 0)) == 1, artifact, "M258-E001-D002-07", "D002 imported protocol conformance proof drifted", failures)
    total += 1
    passed += require(int(probe.get("startup_registered_image_count", 0)) == 2, artifact, "M258-E001-D002-08", "D002 startup image count drifted", failures)
    total += 1
    passed += require(int(probe.get("post_replay_registered_image_count", 0)) == 2, artifact, "M258-E001-D002-09", "D002 replay image count drifted", failures)
    total += 1
    passed += require(int(probe.get("post_reset_retained_bootstrap_image_count", 0)) == 2, artifact, "M258-E001-D002-10", "D002 retained bootstrap catalog drifted", failures)
    total += 1
    passed += require(
        probe.get("post_replay_imported_provider_protocol_value") == probe.get("imported_provider_protocol_value")
        and probe.get("post_replay_local_consumer_class_value") == probe.get("local_consumer_class_value"),
        artifact,
        "M258-E001-D002-11",
        "D002 replay-stable cross-module dispatch proof drifted",
        failures,
    )
    distilled = {
        "contract_id": payload.get("contract_id"),
        "link_plan_ready": plan.get("ready"),
        "packaged_modules": plan.get("module_names_lexicographic"),
        "startup_registered_image_count": probe.get("startup_registered_image_count"),
        "post_replay_registered_image_count": probe.get("post_replay_registered_image_count"),
        "imported_worker_conforms": probe.get("imported_worker_conforms"),
    }
    return total, passed, distilled


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_matrix = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.frontend_artifacts_cpp, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.api_h, API_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in snippet_matrix:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    upstream_summaries: dict[str, Any] = {}
    for label, validator, path in (
        ("M258-A002", validate_a002, args.a002_summary),
        ("M258-B002", validate_b002, args.b002_summary),
        ("M258-C002", validate_c002, args.c002_summary),
        ("M258-D002", validate_d002, args.d002_summary),
    ):
        total, passed, distilled = validator(path, failures)
        checks_total += total
        checks_passed += passed
        upstream_summaries[label] = distilled

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "execution_gate_model": EXECUTION_GATE_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "failures": [failure.__dict__ for failure in failures],
        "upstream_summaries": upstream_summaries,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.artifact} :: {failure.check_id} :: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} validated")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
