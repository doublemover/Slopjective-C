#!/usr/bin/env python3
"""Checker for M272-C003 dispatch metadata/interface preservation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m272-c003-part9-dispatch-metadata-interface-preservation-v1"
CONTRACT_ID = "objc3c-part9-dispatch-metadata-interface-preservation/m272-c003-v1"
SOURCE_CONTRACT_ID = "objc3c-part9-dispatch-control-lowering-contract/m272-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part9_dispatch_metadata_and_interface_preservation"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
REUSE_PAYLOAD_KEY = "serialized_runtime_metadata_reuse_payload"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m272" / "M272-C003" / "dispatch_metadata_interface_preservation_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m272" / "c003"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m272_metadata_and_interface_preservation_for_dynamism_controls_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m272" / "m272_c003_metadata_and_interface_preservation_for_dynamism_controls_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m272_c003_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m272_c003_metadata_and_interface_preservation_for_dynamism_controls_core_feature_expansion.py"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_c003_dispatch_preservation_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m272_c003_dispatch_preservation_consumer.objc3"

EXPECTED_PROVIDER_PACKET = {
    "contract_id": CONTRACT_ID,
    "source_contract_id": SOURCE_CONTRACT_ID,
    "surface_path": SURFACE_PATH,
    "import_artifact_member_name": "objc_part9_dispatch_metadata_and_interface_preservation",
    "local_direct_callable_record_count": 6,
    "local_final_callable_record_count": 0,
    "local_final_container_record_count": 2,
    "local_sealed_container_record_count": 2,
    "imported_module_count": 0,
    "imported_direct_callable_record_count": 0,
    "imported_final_callable_record_count": 0,
    "imported_final_container_record_count": 0,
    "imported_sealed_container_record_count": 0,
    "runtime_import_artifact_ready": True,
    "separate_compilation_preservation_ready": True,
    "deterministic": True,
}
EXPECTED_CONSUMER_PACKET = {
    "contract_id": CONTRACT_ID,
    "source_contract_id": SOURCE_CONTRACT_ID,
    "surface_path": SURFACE_PATH,
    "import_artifact_member_name": "objc_part9_dispatch_metadata_and_interface_preservation",
    "local_direct_callable_record_count": 2,
    "local_final_callable_record_count": 0,
    "local_final_container_record_count": 2,
    "local_sealed_container_record_count": 2,
    "imported_module_count": 1,
    "imported_direct_callable_record_count": 6,
    "imported_final_callable_record_count": 0,
    "imported_final_container_record_count": 2,
    "imported_sealed_container_record_count": 2,
    "runtime_import_artifact_ready": True,
    "separate_compilation_preservation_ready": True,
    "deterministic": True,
}
PROVIDER_IR_SNIPPETS = [
    "; part9_dispatch_metadata_interface_preservation = ",
    "; frontend_objc_dispatch_metadata_interface_profile = local_direct_callable_record_count=6",
    "!objc3.objc_part9_dispatch_metadata_and_interface_preservation = !{!103}",
]
CONSUMER_IR_SNIPPETS = [
    "; part9_dispatch_metadata_interface_preservation = ",
    "; frontend_objc_dispatch_metadata_interface_profile = local_direct_callable_record_count=2",
    "imported_module_count=1",
    "!objc3.objc_part9_dispatch_metadata_and_interface_preservation = !{!103}",
]


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M272-C003-EXP-01", "# M272 Metadata And Interface Preservation For Dynamism Controls Expectations (C003)"),
        SnippetCheck("M272-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M272-C003-EXP-03", "module.runtime-import-surface.json"),
    ),
    PACKET_DOC: (
        SnippetCheck("M272-C003-PKT-01", "# M272-C003 Packet: Metadata And Interface Preservation For Dynamism Controls - Core Feature Expansion"),
        SnippetCheck("M272-C003-PKT-02", "Issue: `#7341`"),
        SnippetCheck("M272-C003-PKT-03", "Next issue: `M272-D001`"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M272-C003-DOCSRC-01", "## M272 dispatch metadata and interface preservation"),
        SnippetCheck("M272-C003-DOCSRC-02", "objc_part9_dispatch_metadata_and_interface_preservation"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M272-C003-DOC-01", "## M272 dispatch metadata and interface preservation"),
        SnippetCheck("M272-C003-DOC-02", "objc_part9_dispatch_metadata_and_interface_preservation"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M272-C003-ATTR-01", "## M272 dispatch metadata and interface preservation (C003)"),
        SnippetCheck("M272-C003-ATTR-02", "objc_part9_dispatch_metadata_and_interface_preservation"),
    ),
    SPEC_DECISIONS: (
        SnippetCheck("M272-C003-DEC-01", "## D-031: Part 9 dispatch intent must survive runtime metadata and import-surface replay after live lowering lands"),
        SnippetCheck("M272-C003-DEC-02", "module.runtime-import-surface.json"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M272-C003-LH-01", "kObjc3Part9DispatchMetadataInterfacePreservationContractId"),
        SnippetCheck("M272-C003-LH-02", "kObjc3Part9DispatchMetadataInterfacePreservationImportArtifactMemberName"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M272-C003-LC-01", "Objc3Part9DispatchMetadataInterfacePreservationSummary()"),
        SnippetCheck("M272-C003-LC-02", ";next_issue=M272-D001"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M272-C003-TYP-01", "bool objc_final_declared = false;"),
        SnippetCheck("M272-C003-TYP-02", "bool effective_direct_dispatch = false;"),
    ),
    FRONTEND_PIPELINE: (
        SnippetCheck("M272-C003-PIP-01", "record.objc_final_declared = interface_decl.objc_final_declared;"),
        SnippetCheck("M272-C003-PIP-02", "method_record.effective_direct_dispatch ="),
        SnippetCheck("M272-C003-PIP-03", "interface_record.effective_direct_dispatch =="),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M272-C003-ART-01", "BuildPart9DispatchMetadataInterfacePreservationSummary("),
        SnippetCheck("M272-C003-ART-02", "objc_part9_dispatch_metadata_and_interface_preservation"),
        SnippetCheck("M272-C003-ART-03", "part9_dispatch_metadata_interface_preservation_summary"),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M272-C003-IMH-01", "part9_dispatch_metadata_interface_preservation_present"),
        SnippetCheck("M272-C003-IMH-02", "part9_local_direct_callable_record_count"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M272-C003-IMC-01", "PopulateImportedPart9DispatchMetadataInterfacePreservation("),
        SnippetCheck("M272-C003-IMC-02", "unexpected Part 9 dispatch metadata/interface preservation contract id in import surface"),
    ),
    IR_HEADER: (
        SnippetCheck("M272-C003-IRH-01", "lowering_part9_dispatch_metadata_interface_preservation_key"),
        SnippetCheck("M272-C003-IRH-02", "part9_dispatch_metadata_imported_module_count"),
    ),
    IR_CPP: (
        SnippetCheck("M272-C003-IRC-01", "; part9_dispatch_metadata_interface_preservation = "),
        SnippetCheck("M272-C003-IRC-02", "!objc3.objc_part9_dispatch_metadata_and_interface_preservation = !{!103}"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M272-C003-PKG-01", '"check:objc3c:m272-c003-metadata-and-interface-preservation-for-dynamism-controls-core-feature-expansion"'),
        SnippetCheck("M272-C003-PKG-02", '"test:tooling:m272-c003-metadata-and-interface-preservation-for-dynamism-controls-core-feature-expansion"'),
        SnippetCheck("M272-C003-PKG-03", '"check:objc3c:m272-c003-lane-c-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M272-C003-RUN-01", "M272-B003 + M272-C001 + M272-C002 + M272-C003"),
    ),
    TEST_FILE: (
        SnippetCheck("M272-C003-TEST-01", "def test_checker_passes_in_static_mode(tmp_path: Path) -> None:"),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M272-C003-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = path.read_text(encoding="utf-8")
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def compile_fixture(*, fixture: Path, out_dir: Path, ordinal: int, import_surface: Path | None = None) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        str(ordinal),
    ]
    if import_surface is not None:
        command.extend(["--objc3-import-runtime-surface", str(import_surface)])
    return run_process(command)


def provider_class_records(runtime_owned_declarations: dict[str, Any], name: str) -> list[dict[str, Any]]:
    classes = runtime_owned_declarations.get("classes")
    if not isinstance(classes, list):
        return []
    return [record for record in classes if isinstance(record, dict) and record.get("name") == name]


def provider_method_records(runtime_owned_declarations: dict[str, Any], owner_name: str, selector: str) -> list[dict[str, Any]]:
    methods = runtime_owned_declarations.get("methods")
    if not isinstance(methods, list):
        return []
    return [record for record in methods if isinstance(record, dict) and record.get("owner_name") == owner_name and record.get("selector") == selector]


def validate_packet(packet: dict[str, Any], expected: dict[str, Any], artifact: str, failures: list[Finding], prefix: str) -> tuple[int, int]:
    total = 0
    passed = 0
    for index, (field, expected_value) in enumerate(expected.items(), start=1):
        total += 1
        passed += require(packet.get(field) == expected_value, artifact, f"{prefix}-PKT-{index:02d}", f"{field} mismatch", failures)
    total += 1
    passed += require(bool(packet.get("replay_key")), artifact, f"{prefix}-PKT-30", "replay_key missing", failures)
    total += 1
    passed += require(bool(packet.get("lowering_replay_key")), artifact, f"{prefix}-PKT-31", "lowering_replay_key missing", failures)
    return total, passed


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    ensure_build = run_process([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m272-c003-readiness",
        "--summary-out",
        "tmp/reports/m272/M272-C003/ensure_objc3c_native_build_summary.json",
    ])
    total += 1
    passed += require(ensure_build.returncode == 0, display_path(BUILD_HELPER), "M272-C003-DYN-01", ensure_build.stderr or ensure_build.stdout or "fast build failed", failures)
    total += 1
    passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M272-C003-DYN-02", "native compiler missing after build", failures)
    if ensure_build.returncode != 0 or not NATIVE_EXE.exists():
        return total, passed, {"build_output": (ensure_build.stdout or "") + (ensure_build.stderr or "")}

    provider_out = PROBE_ROOT / "provider"
    consumer_out = PROBE_ROOT / "consumer"
    provider = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, ordinal=1)
    provider_output = (provider.stdout or "") + (provider.stderr or "")
    provider_import = provider_out / IMPORT_ARTIFACT
    provider_manifest = provider_out / "module.manifest.json"
    provider_ir = provider_out / "module.ll"
    provider_obj = provider_out / "module.obj"
    total += 4
    passed += require(provider.returncode == 0, display_path(PROVIDER_FIXTURE), "M272-C003-DYN-03", f"provider compile failed: {provider_output}", failures)
    passed += require(provider_import.exists(), display_path(provider_import), "M272-C003-DYN-04", "provider import artifact missing", failures)
    passed += require(provider_ir.exists(), display_path(provider_ir), "M272-C003-DYN-05", "provider IR missing", failures)
    passed += require(provider_obj.exists(), display_path(provider_obj), "M272-C003-DYN-06", "provider object missing", failures)
    if provider.returncode != 0 or not provider_import.exists():
        return total, passed, {"provider_output": provider_output.strip()}

    provider_payload = load_json(provider_import)
    provider_packet = provider_payload.get("objc_part9_dispatch_metadata_and_interface_preservation")
    total += 1
    passed += require(isinstance(provider_packet, dict), display_path(provider_import), "M272-C003-DYN-07", "provider preservation packet missing", failures)
    if isinstance(provider_packet, dict):
        sub_total, sub_passed = validate_packet(provider_packet, EXPECTED_PROVIDER_PACKET, display_path(provider_import), failures, "M272-C003-PROVIDER")
        total += sub_total
        passed += sub_passed
    provider_runtime_owned = provider_payload.get("runtime_owned_declarations")
    total += 1
    passed += require(isinstance(provider_runtime_owned, dict), display_path(provider_import), "M272-C003-DYN-08", "provider runtime_owned_declarations missing", failures)
    if isinstance(provider_runtime_owned, dict):
        class_records = provider_class_records(provider_runtime_owned, "PolicyBox")
        total += 2
        passed += require(len(class_records) == 2, display_path(provider_import), "M272-C003-DYN-09", "expected interface and implementation PolicyBox class records", failures)
        passed += require(all(record.get("objc_final_declared") is True and record.get("objc_sealed_declared") is True for record in class_records), display_path(provider_import), "M272-C003-DYN-10", "PolicyBox class records must preserve final/sealed flags", failures)
        direct_records = provider_method_records(provider_runtime_owned, "PolicyBox", "implicitDirect")
        dynamic_records = provider_method_records(provider_runtime_owned, "PolicyBox", "dynamicEscape")
        total += 2
        passed += require(any(record.get("effective_direct_dispatch") is True for record in direct_records), display_path(provider_import), "M272-C003-DYN-11", "implicitDirect records must preserve effective direct dispatch", failures)
        passed += require(any(record.get("effective_direct_dispatch") is False for record in dynamic_records), display_path(provider_import), "M272-C003-DYN-12", "dynamicEscape records must preserve dynamic opt-out", failures)
    if provider_ir.exists():
        provider_ir_text = provider_ir.read_text(encoding="utf-8")
        for index, snippet in enumerate(PROVIDER_IR_SNIPPETS, start=20):
            total += 1
            passed += require(snippet in provider_ir_text, display_path(provider_ir), f"M272-C003-DYN-{index}", f"missing provider IR snippet: {snippet}", failures)

    consumer = compile_fixture(fixture=CONSUMER_FIXTURE, out_dir=consumer_out, ordinal=2, import_surface=provider_import)
    consumer_output = (consumer.stdout or "") + (consumer.stderr or "")
    consumer_import = consumer_out / IMPORT_ARTIFACT
    consumer_ir = consumer_out / "module.ll"
    consumer_obj = consumer_out / "module.obj"
    total += 4
    passed += require(consumer.returncode == 0, display_path(CONSUMER_FIXTURE), "M272-C003-DYN-40", f"consumer compile failed: {consumer_output}", failures)
    passed += require(consumer_import.exists(), display_path(consumer_import), "M272-C003-DYN-41", "consumer import artifact missing", failures)
    passed += require(consumer_ir.exists(), display_path(consumer_ir), "M272-C003-DYN-42", "consumer IR missing", failures)
    passed += require(consumer_obj.exists(), display_path(consumer_obj), "M272-C003-DYN-43", "consumer object missing", failures)
    if consumer.returncode != 0 or not consumer_import.exists():
        return total, passed, {"provider_output": provider_output.strip(), "consumer_output": consumer_output.strip()}

    consumer_payload = load_json(consumer_import)
    consumer_packet = consumer_payload.get("objc_part9_dispatch_metadata_and_interface_preservation")
    total += 1
    passed += require(isinstance(consumer_packet, dict), display_path(consumer_import), "M272-C003-DYN-44", "consumer preservation packet missing", failures)
    if isinstance(consumer_packet, dict):
        sub_total, sub_passed = validate_packet(consumer_packet, EXPECTED_CONSUMER_PACKET, display_path(consumer_import), failures, "M272-C003-CONSUMER")
        total += sub_total
        passed += sub_passed
    reuse_payload = consumer_payload.get(REUSE_PAYLOAD_KEY)
    total += 1
    passed += require(isinstance(reuse_payload, dict), display_path(consumer_import), "M272-C003-DYN-45", "consumer serialized reuse payload missing", failures)
    if isinstance(reuse_payload, dict):
        runtime_owned = reuse_payload.get("runtime_owned_declarations")
        total += 1
        passed += require(isinstance(runtime_owned, dict), display_path(consumer_import), "M272-C003-DYN-46", "consumer reuse runtime_owned_declarations missing", failures)
        if isinstance(runtime_owned, dict):
            reused_classes = provider_class_records(runtime_owned, "PolicyBox")
            reused_methods = provider_method_records(runtime_owned, "PolicyBox", "explicitDirect")
            total += 2
            passed += require(any(record.get("objc_final_declared") is True and record.get("objc_sealed_declared") is True for record in reused_classes), display_path(consumer_import), "M272-C003-DYN-47", "reused provider class records must preserve final/sealed flags", failures)
            passed += require(any(record.get("effective_direct_dispatch") is True for record in reused_methods), display_path(consumer_import), "M272-C003-DYN-48", "reused provider method records must preserve effective direct dispatch", failures)
    if consumer_ir.exists():
        consumer_ir_text = consumer_ir.read_text(encoding="utf-8")
        for index, snippet in enumerate(CONSUMER_IR_SNIPPETS, start=60):
            total += 1
            passed += require(snippet in consumer_ir_text, display_path(consumer_ir), f"M272-C003-DYN-{index}", f"missing consumer IR snippet: {snippet}", failures)

    return total, passed, {
        "provider_import_artifact": display_path(provider_import),
        "consumer_import_artifact": display_path(consumer_import),
        "provider_ir": display_path(provider_ir),
        "consumer_ir": display_path(consumer_ir),
        "provider_packet": provider_packet,
        "consumer_packet": consumer_packet,
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    args = parser.parse_args(argv or sys.argv[1:])

    failures: list[Finding] = []
    total = 0
    passed = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total += len(snippets)
        passed += ensure_snippets(path, snippets, failures)

    dynamic_payload: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dyn_total, dyn_passed, dynamic_payload = run_dynamic_probes(failures)
        total += dyn_total
        passed += dyn_passed

    ok = not failures
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "total_checks": total,
        "passed_checks": passed,
        "failed_checks": total - passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic": dynamic_payload,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M272-C003 checker passed ({passed}/{total} checks)")
        return 0
    print(json.dumps(payload, indent=2))
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
