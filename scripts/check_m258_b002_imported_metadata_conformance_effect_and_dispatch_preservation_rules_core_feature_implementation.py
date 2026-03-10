#!/usr/bin/env python3
"""Fail-closed checker for M258-B002 imported runtime metadata semantic rules."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-b002-imported-runtime-metadata-semantic-rules-v1"
CONTRACT_ID = "objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1"
SOURCE_CONTRACT_ID = "objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_imported_runtime_metadata_semantic_rules"
INPUT_MODEL = "filesystem-runtime-import-surface-artifact-path-list"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-B002" / "imported_runtime_metadata_semantic_rules_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "b002-imported-runtime-metadata-semantic-rules"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation_b002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_b002_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
CLI_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
CLI_OPTIONS_H = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
FRONTEND_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_frontend_options.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_b002_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_b002_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation.py"
CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_imported_runtime_semantic_rules_consumer.objc3"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"


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
        SnippetCheck("M258-B002-DOC-01", "# M258 Imported Metadata Conformance, Effect, And Dispatch Preservation Rules Core Feature Implementation Expectations (B002)"),
        SnippetCheck("M258-B002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-B002-DOC-03", "Issue: `#7161`"),
        SnippetCheck("M258-B002-DOC-04", f"`{SURFACE_PATH}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-B002-PKT-01", "# M258-B002 Imported Metadata Conformance, Effect, And Dispatch Preservation Rules Core Feature Implementation Packet"),
        SnippetCheck("M258-B002-PKT-02", "Packet: `M258-B002`"),
        SnippetCheck("M258-B002-PKT-03", "Dependencies: `M258-A002`, `M258-B001`"),
        SnippetCheck("M258-B002-PKT-04", "Next issue: `M258-C001`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-B002-NDOC-01", "## Imported metadata semantic rules (M258-B002)"),
        SnippetCheck("M258-B002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-B002-NDOC-03", "--objc3-import-runtime-surface <path>"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-B002-SPC-01", "## M258 imported metadata conformance, effect, and dispatch preservation rules (B002)"),
        SnippetCheck("M258-B002-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-B002-SPC-03", "imported runtime metadata payloads still are not lowered into IR in this lane"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-B002-META-01", "## M258 imported metadata semantic-rule anchors (B002)"),
        SnippetCheck("M258-B002-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-B002-META-03", f"`{INPUT_MODEL}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-B002-ARCH-01", "`M258-B002` is the next lane-B step:"),
        SnippetCheck("M258-B002-ARCH-02", "--objc3-import-runtime-surface <path>"),
        SnippetCheck("M258-B002-ARCH-03", "objc_imported_runtime_metadata_semantic_rules"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M258-B002-TYPE-01", "struct Objc3ImportedRuntimeMetadataSemanticRulesSummary {"),
        SnippetCheck("M258-B002-TYPE-02", "imported_module_names_lexicographic"),
        SnippetCheck("M258-B002-TYPE-03", "ready_for_imported_metadata_semantic_rules"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-B002-ART-01", "BuildImportedRuntimeMetadataSemanticRulesSummary("),
        SnippetCheck("M258-B002-ART-02", "BuildImportedRuntimeMetadataSemanticRulesSummaryJson("),
        SnippetCheck("M258-B002-ART-03", "objc_imported_runtime_metadata_semantic_rules"),
        SnippetCheck("M258-B002-ART-04", '"O3S264"'),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M258-B002-IMP-01", "TryLoadObjc3ImportedRuntimeModuleSurface("),
        SnippetCheck("M258-B002-IMP-02", "runtime-owned declaration count does not match imported record inventory"),
        SnippetCheck("M258-B002-IMP-03", "unexpected import-surface contract id"),
    ),
    CLI_OPTIONS_H: (
        SnippetCheck("M258-B002-CLIH-01", "imported_runtime_surface_paths"),
    ),
    CLI_OPTIONS_CPP: (
        SnippetCheck("M258-B002-CLI-01", "--objc3-import-runtime-surface"),
        SnippetCheck("M258-B002-CLI-02", "options.imported_runtime_surface_paths.push_back"),
    ),
    FRONTEND_OPTIONS_CPP: (
        SnippetCheck("M258-B002-OPTS-01", "options.imported_runtime_surface_paths.push_back"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-B002-IR-01", "M258-B002 imported metadata semantic rules anchor"),
        SnippetCheck("M258-B002-IR-02", "are not lowered into IR in this lane."),
    ),
    API_H: (
        SnippetCheck("M258-B002-API-01", "M258-B002 imported metadata semantic rules anchor"),
        SnippetCheck("M258-B002-API-02", "does not expose live imported"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M258-B002-BLD-01", "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-B002-PKG-01", '"check:objc3c:m258-b002-imported-metadata-conformance-effect-and-dispatch-preservation-rules"'),
        SnippetCheck("M258-B002-PKG-02", '"test:tooling:m258-b002-imported-metadata-conformance-effect-and-dispatch-preservation-rules"'),
        SnippetCheck("M258-B002-PKG-03", '"check:objc3c:m258-b002-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-B002-RUN-01", "M258-A002 + M258-B001 + M258-B002"),
        SnippetCheck("M258-B002-RUN-02", "check_m258_b002_imported_metadata_conformance_effect_and_dispatch_preservation_rules_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-B002-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-B002-TEST-02", "def test_checker_passes_dynamic"),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
    return 1


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, failures
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def ensure_binaries(failures: list[Finding]) -> int:
    checks_total = 0
    if NATIVE_EXE.exists():
        checks_total += require(True, display_path(NATIVE_EXE), "M258-B002-BIN-READY", "native binary present", failures)
        return checks_total
    build = run_process([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-B002-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-B002-NATIVE-EXISTS", "native binary missing after build", failures)
    return checks_total


def compile_fixture(
    *,
    fixture: Path,
    out_dir: Path,
    registration_order_ordinal: int,
    extra_args: Sequence[str] = (),
) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            "--objc3-bootstrap-registration-order-ordinal",
            str(registration_order_ordinal),
            *extra_args,
        ]
    )


def compute_counts_from_artifact(artifact_payload: dict[str, Any]) -> dict[str, int | list[str]]:
    declarations = artifact_payload.get("runtime_owned_declarations")
    if not isinstance(declarations, dict):
        raise TypeError("runtime_owned_declarations must be an object")
    classes = declarations.get("classes")
    protocols = declarations.get("protocols")
    categories = declarations.get("categories")
    properties = declarations.get("properties")
    methods = declarations.get("methods")
    ivars = declarations.get("ivars")
    if not all(isinstance(value, list) for value in (classes, protocols, categories, properties, methods, ivars)):
        raise TypeError("runtime-owned declaration inventories are incomplete")

    superclass_edge_count = sum(1 for record in classes if record.get("has_super") and record.get("super_name"))
    protocol_conformance_edge_count = (
        sum(len(record.get("adopted_protocols", [])) for record in classes)
        + sum(len(record.get("inherited_protocols", [])) for record in protocols)
        + sum(len(record.get("adopted_protocols", [])) for record in categories)
    )
    property_accessor_trait_count = sum(1 for record in properties if record.get("effective_getter_selector")) + sum(
        1 for record in properties if record.get("effective_setter_available") and record.get("effective_setter_selector")
    )
    property_ivar_binding_trait_count = sum(1 for record in properties if record.get("ivar_binding_symbol"))
    method_selector_trait_count = sum(1 for record in methods if record.get("selector"))
    class_method_trait_count = sum(1 for record in methods if record.get("is_class_method") is True)
    instance_method_trait_count = sum(1 for record in methods if record.get("is_class_method") is not True)
    implemented_method_count = sum(1 for record in methods if record.get("has_body") is True)
    declaration_only_method_count = sum(1 for record in methods if record.get("has_body") is not True)
    property_attribute_profile_count = sum(1 for record in properties if record.get("property_attribute_profile"))
    ownership_effect_profile_count = sum(
        1
        for record in properties
        if record.get("ownership_lifetime_profile")
        or record.get("ownership_runtime_hook_profile")
        or record.get("accessor_ownership_profile")
    )
    executable_binding_trait_count = (
        sum(
            1
            for record in properties
            if record.get("executable_synthesized_binding_kind") not in (None, "", "none")
            or record.get("executable_synthesized_binding_symbol")
            or record.get("executable_ivar_layout_symbol")
        )
        + sum(
            1
            for record in ivars
            if record.get("executable_synthesized_binding_kind") not in (None, "", "none")
            or record.get("executable_synthesized_binding_symbol")
            or record.get("executable_ivar_layout_symbol")
        )
    )

    return {
        "module_name": artifact_payload.get("module_name", ""),
        "class_record_count": len(classes),
        "protocol_record_count": len(protocols),
        "category_record_count": len(categories),
        "property_record_count": len(properties),
        "method_record_count": len(methods),
        "ivar_record_count": len(ivars),
        "runtime_owned_declaration_count": len(classes) + len(protocols) + len(categories) + len(properties) + len(methods) + len(ivars),
        "superclass_edge_count": superclass_edge_count,
        "protocol_conformance_edge_count": protocol_conformance_edge_count,
        "category_attachment_count": len(categories),
        "property_accessor_trait_count": property_accessor_trait_count,
        "property_ivar_binding_trait_count": property_ivar_binding_trait_count,
        "method_selector_trait_count": method_selector_trait_count,
        "class_method_trait_count": class_method_trait_count,
        "instance_method_trait_count": instance_method_trait_count,
        "implemented_method_count": implemented_method_count,
        "declaration_only_method_count": declaration_only_method_count,
        "property_attribute_profile_count": property_attribute_profile_count,
        "ownership_effect_profile_count": ownership_effect_profile_count,
        "executable_binding_trait_count": executable_binding_trait_count,
    }


def aggregate_counts(*payloads: dict[str, Any]) -> dict[str, int | list[str]]:
    totals: dict[str, int | list[str]] = {
        "module_names": [],
        "class_record_count": 0,
        "protocol_record_count": 0,
        "category_record_count": 0,
        "property_record_count": 0,
        "method_record_count": 0,
        "ivar_record_count": 0,
        "runtime_owned_declaration_count": 0,
        "superclass_edge_count": 0,
        "protocol_conformance_edge_count": 0,
        "category_attachment_count": 0,
        "property_accessor_trait_count": 0,
        "property_ivar_binding_trait_count": 0,
        "method_selector_trait_count": 0,
        "class_method_trait_count": 0,
        "instance_method_trait_count": 0,
        "implemented_method_count": 0,
        "declaration_only_method_count": 0,
        "property_attribute_profile_count": 0,
        "ownership_effect_profile_count": 0,
        "executable_binding_trait_count": 0,
    }
    for payload in payloads:
        counts = compute_counts_from_artifact(payload)
        module_names = totals["module_names"]
        assert isinstance(module_names, list)
        module_names.append(str(counts["module_name"]))
        for key, value in counts.items():
            if key == "module_name":
                continue
            totals[key] = int(totals[key]) + int(value)
    module_names = totals["module_names"]
    assert isinstance(module_names, list)
    module_names.sort()
    return totals


def validate_import_artifact_extensions(payload: dict[str, Any], label: str, failures: list[Finding]) -> int:
    checks_total = 0
    declarations = payload.get("runtime_owned_declarations")
    properties = declarations.get("properties") if isinstance(declarations, dict) else None
    ivars = declarations.get("ivars") if isinstance(declarations, dict) else None
    checks_total += require(isinstance(properties, list) and bool(properties), label, "M258-B002-ARTIFACT-PROPERTIES", "property inventory missing from import artifact", failures)
    checks_total += require(isinstance(ivars, list) and bool(ivars), label, "M258-B002-ARTIFACT-IVARS", "ivar inventory missing from import artifact", failures)
    if not isinstance(properties, list) or not isinstance(ivars, list):
        return checks_total
    property_keys = {
        "property_attribute_profile",
        "ownership_lifetime_profile",
        "ownership_runtime_hook_profile",
        "accessor_ownership_profile",
        "executable_ivar_layout_symbol",
        "executable_ivar_layout_slot_index",
        "executable_ivar_layout_size_bytes",
        "executable_ivar_layout_alignment_bytes",
    }
    ivar_keys = {
        "executable_ivar_layout_slot_index",
        "executable_ivar_layout_size_bytes",
        "executable_ivar_layout_alignment_bytes",
        "source_model",
    }
    checks_total += require(all(property_keys.issubset(record.keys()) for record in properties if isinstance(record, dict)), label, "M258-B002-ARTIFACT-PROPERTY-KEYS", "import artifact property entries are missing preserved effect/binding fields", failures)
    checks_total += require(all(ivar_keys.issubset(record.keys()) for record in ivars if isinstance(record, dict)), label, "M258-B002-ARTIFACT-IVAR-KEYS", "import artifact ivar entries are missing preserved layout/source-model fields", failures)
    return checks_total


def validate_consumer_manifest(manifest: dict[str, Any], aggregate: dict[str, int | list[str]], failures: list[Finding]) -> tuple[int, dict[str, object]]:
    checks_total = 0
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    surface = semantic_surface.get("objc_imported_runtime_metadata_semantic_rules") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(surface, dict), "consumer manifest", "M258-B002-SURFACE", "imported runtime metadata semantic surface missing", failures)
    if not isinstance(surface, dict):
        return checks_total, {}

    expected_module_names = aggregate["module_names"]
    assert isinstance(expected_module_names, list)
    checks_total += require(surface.get("contract_id") == CONTRACT_ID, "consumer manifest", "M258-B002-CONTRACT", "unexpected B002 contract id", failures)
    checks_total += require(surface.get("source_semantic_preservation_contract_id") == SOURCE_CONTRACT_ID, "consumer manifest", "M258-B002-SOURCE-CONTRACT", "unexpected source semantic preservation contract id", failures)
    checks_total += require(surface.get("frontend_surface_path") == SURFACE_PATH, "consumer manifest", "M258-B002-SURFACE-PATH", "unexpected surface path", failures)
    checks_total += require(surface.get("input_model") == INPUT_MODEL, "consumer manifest", "M258-B002-INPUT-MODEL", "unexpected input model", failures)
    checks_total += require(surface.get("imported_module_names_lexicographic") == expected_module_names, "consumer manifest", "M258-B002-MODULE-NAMES", "imported module names are not deterministic", failures)
    checks_total += require(surface.get("imported_input_path_count") == len(expected_module_names), "consumer manifest", "M258-B002-INPUT-COUNT", "unexpected imported input path count", failures)
    checks_total += require(surface.get("imported_module_count") == len(expected_module_names), "consumer manifest", "M258-B002-MODULE-COUNT", "unexpected imported module count", failures)

    for key in (
        "class_record_count",
        "protocol_record_count",
        "category_record_count",
        "property_record_count",
        "method_record_count",
        "ivar_record_count",
        "runtime_owned_declaration_count",
        "superclass_edge_count",
        "protocol_conformance_edge_count",
        "category_attachment_count",
        "property_accessor_trait_count",
        "property_ivar_binding_trait_count",
        "method_selector_trait_count",
        "class_method_trait_count",
        "instance_method_trait_count",
        "implemented_method_count",
        "declaration_only_method_count",
        "property_attribute_profile_count",
        "ownership_effect_profile_count",
        "executable_binding_trait_count",
    ):
        checks_total += require(surface.get(key) == aggregate[key], "consumer manifest", f"M258-B002-{key.upper()}", f"semantic count mismatch for {key}", failures)

    for key in (
        "ready",
        "fail_closed",
        "source_semantic_preservation_contract_ready",
        "semantic_surface_published",
        "imported_runtime_surface_inputs_present",
        "imported_runtime_surface_inputs_loaded",
        "imported_conformance_shape_landed",
        "imported_dispatch_traits_landed",
        "imported_effect_traits_landed",
        "imported_runtime_metadata_semantics_landed",
        "ready_for_imported_metadata_semantic_rules",
        "ready_for_cross_module_dispatch_equivalence",
    ):
        checks_total += require(surface.get(key) is True, "consumer manifest", f"M258-B002-{key.upper()}", f"expected {key}=true", failures)

    checks_total += require(surface.get("property_attribute_profile_count", 0) > 0, "consumer manifest", "M258-B002-EFFECTS", "property attribute profiles were not preserved", failures)
    checks_total += require(surface.get("ownership_effect_profile_count", 0) > 0, "consumer manifest", "M258-B002-OWNERSHIP", "ownership effect profiles were not preserved", failures)
    checks_total += require(surface.get("method_selector_trait_count", 0) > 0, "consumer manifest", "M258-B002-DISPATCH", "dispatch traits were not preserved", failures)
    return checks_total, surface


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding], int]:
    failures: list[Finding] = []
    checks_total = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_checks, path_failures = check_static_contract(path, snippets)
        checks_total += path_checks
        failures.extend(path_failures)
        static_summary[display_path(path)] = {"checks": path_checks, "ok": not path_failures}

    dynamic_summary: dict[str, object] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        checks_total += ensure_binaries(failures)
        if not failures:
            class_out = PROBE_ROOT / "producer_class"
            category_out = PROBE_ROOT / "producer_category"
            consumer_out = PROBE_ROOT / "consumer_positive"
            duplicate_out = PROBE_ROOT / "consumer_duplicate_path"

            class_completed = compile_fixture(fixture=CLASS_FIXTURE, out_dir=class_out, registration_order_ordinal=1)
            category_completed = compile_fixture(fixture=CATEGORY_FIXTURE, out_dir=category_out, registration_order_ordinal=2)
            class_manifest = class_out / "module.manifest.json"
            category_manifest = category_out / "module.manifest.json"
            class_artifact = class_out / IMPORT_ARTIFACT
            category_artifact = category_out / IMPORT_ARTIFACT

            checks_total += require(class_completed.returncode == 0, display_path(CLASS_FIXTURE), "M258-B002-CLASS-COMPILE", class_completed.stderr or class_completed.stdout or "class producer compile failed", failures)
            checks_total += require(category_completed.returncode == 0, display_path(CATEGORY_FIXTURE), "M258-B002-CATEGORY-COMPILE", category_completed.stderr or category_completed.stdout or "category producer compile failed", failures)
            checks_total += require(class_manifest.exists(), display_path(class_manifest), "M258-B002-CLASS-MANIFEST", "class producer manifest missing", failures)
            checks_total += require(category_manifest.exists(), display_path(category_manifest), "M258-B002-CATEGORY-MANIFEST", "category producer manifest missing", failures)
            checks_total += require(class_artifact.exists(), display_path(class_artifact), "M258-B002-CLASS-ARTIFACT", "class import artifact missing", failures)
            checks_total += require(category_artifact.exists(), display_path(category_artifact), "M258-B002-CATEGORY-ARTIFACT", "category import artifact missing", failures)

            if not failures:
                class_payload = load_json(class_artifact)
                category_payload = load_json(category_artifact)
                checks_total += validate_import_artifact_extensions(class_payload, display_path(class_artifact), failures)
                checks_total += validate_import_artifact_extensions(category_payload, display_path(category_artifact), failures)
                aggregate = aggregate_counts(class_payload, category_payload)

                consumer_completed = compile_fixture(
                    fixture=CONSUMER_FIXTURE,
                    out_dir=consumer_out,
                    registration_order_ordinal=3,
                    extra_args=(
                        "--objc3-import-runtime-surface",
                        str(class_artifact),
                        "--objc3-import-runtime-surface",
                        str(category_artifact),
                    ),
                )
                consumer_manifest_path = consumer_out / "module.manifest.json"
                checks_total += require(consumer_completed.returncode == 0, display_path(CONSUMER_FIXTURE), "M258-B002-CONSUMER-COMPILE", consumer_completed.stderr or consumer_completed.stdout or "consumer compile failed", failures)
                checks_total += require(consumer_manifest_path.exists(), display_path(consumer_manifest_path), "M258-B002-CONSUMER-MANIFEST", "consumer manifest missing", failures)
                surface_summary: dict[str, object] = {}
                if consumer_manifest_path.exists():
                    consumer_manifest = load_json(consumer_manifest_path)
                    surface_checks, surface_summary = validate_consumer_manifest(consumer_manifest, aggregate, failures)
                    checks_total += surface_checks

                duplicate_completed = compile_fixture(
                    fixture=CONSUMER_FIXTURE,
                    out_dir=duplicate_out,
                    registration_order_ordinal=4,
                    extra_args=(
                        "--objc3-import-runtime-surface",
                        str(class_artifact),
                        "--objc3-import-runtime-surface",
                        str(class_artifact),
                    ),
                )
                duplicate_output = (duplicate_completed.stderr or "") + "\n" + (duplicate_completed.stdout or "")
                duplicate_diag_path = duplicate_out / "module.diagnostics.txt"
                duplicate_diag_text = duplicate_diag_path.read_text(encoding="utf-8") if duplicate_diag_path.exists() else ""
                duplicate_combined_output = duplicate_output + "\n" + duplicate_diag_text
                checks_total += require(duplicate_completed.returncode != 0, display_path(CONSUMER_FIXTURE), "M258-B002-DUPLICATE-PATH-FAIL", "duplicate import paths should fail closed", failures)
                checks_total += require("O3S264" in duplicate_combined_output and "provided more than once" in duplicate_combined_output, display_path(CONSUMER_FIXTURE), "M258-B002-DUPLICATE-PATH-DIAG", "duplicate import path diagnostic missing", failures)

                dynamic_summary.update(
                    {
                        "producer_class": compute_counts_from_artifact(class_payload),
                        "producer_category": compute_counts_from_artifact(category_payload),
                        "aggregate_imported_semantics": aggregate,
                        "consumer_surface": surface_summary,
                        "duplicate_path_failure": {
                            "returncode": duplicate_completed.returncode,
                            "diagnostic_contains_o3s264": "O3S264" in duplicate_combined_output,
                        },
                    }
                )

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total if not failures else checks_total - len(failures),
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    return payload, failures, checks_total


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    payload, failures, _ = build_summary(skip_dynamic_probes=args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.artifact} :: {failure.check_id} :: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} validated")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
