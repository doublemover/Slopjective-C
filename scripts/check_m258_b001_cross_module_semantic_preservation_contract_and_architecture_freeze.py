#!/usr/bin/env python3
"""Fail-closed checker for M258-B001 cross-module semantic preservation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-b001-cross-module-runtime-metadata-semantic-preservation-v1"
CONTRACT_ID = "objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1"
SOURCE_FRONTEND_CLOSURE_CONTRACT_ID = "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_cross_module_runtime_metadata_semantic_preservation_contract"
SOURCE_SURFACE_KEY = "objc_runtime_aware_import_module_frontend_closure"
SURFACE_KEY = "objc_cross_module_runtime_metadata_semantic_preservation_contract"
SOURCE_ARTIFACT_RELATIVE_PATH = "module.runtime-import-surface.json"
AUTHORITY_MODEL = "semantic-preservation-freeze-derived-from-runtime-import-surface-and-runtime-metadata-source-records"
CONFORMANCE_SHAPE_MODEL = "superclass-protocol-and-category-attachment-shape"
DISPATCH_TRAIT_MODEL = "selector-classness-accessor-ivar-binding-and-body-availability"
EFFECT_TRAIT_MODEL = "property-attribute-and-ownership-effect-profiles"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-B001" / "cross_module_runtime_metadata_semantic_preservation_contract_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "b001-cross-module-semantic-preservation-contract"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_cross_module_semantic_preservation_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_b001_cross_module_semantic_preservation_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_b001_lane_b_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_b001_cross_module_semantic_preservation_contract_and_architecture_freeze.py"
CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"


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
        SnippetCheck("M258-B001-DOC-01", "# M258 Cross-Module Semantic Preservation Contract And Architecture Freeze Expectations (B001)"),
        SnippetCheck("M258-B001-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-B001-DOC-03", "Issue: `#7160`"),
        SnippetCheck("M258-B001-DOC-04", f"`{SURFACE_PATH}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-B001-PKT-01", "# M258-B001 Cross-Module Semantic Preservation Contract And Architecture Freeze Packet"),
        SnippetCheck("M258-B001-PKT-02", "Packet: `M258-B001`"),
        SnippetCheck("M258-B001-PKT-03", "Dependencies: none"),
        SnippetCheck("M258-B001-PKT-04", "Next issue: `M258-B002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-B001-NDOC-01", "## Cross-module semantic preservation (M258-B001)"),
        SnippetCheck("M258-B001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-B001-NDOC-03", f"`{SOURCE_ARTIFACT_RELATIVE_PATH}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-B001-SPC-01", "## M258 cross-module semantic preservation freeze (B001)"),
        SnippetCheck("M258-B001-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-B001-SPC-03", "imported runtime metadata semantics are not lowered into IR yet"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-B001-META-01", "## M258 cross-module semantic preservation anchors (B001)"),
        SnippetCheck("M258-B001-META-02", "property-attribute and ownership-effect profiles"),
        SnippetCheck("M258-B001-META-03", f"`{SOURCE_ARTIFACT_RELATIVE_PATH}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-B001-ARCH-01", "## M258 cross-module semantic preservation (B001)"),
        SnippetCheck("M258-B001-ARCH-02", "imported runtime metadata semantics are still exposed only through filesystem artifacts"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M258-B001-TYPE-01", "struct Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary {"),
        SnippetCheck("M258-B001-TYPE-02", "imported_conformance_shape_landed"),
        SnippetCheck("M258-B001-TYPE-03", "ready_for_imported_metadata_semantic_rules"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-B001-ART-01", "BuildCrossModuleRuntimeMetadataSemanticPreservationSummary("),
        SnippetCheck("M258-B001-ART-02", "BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson("),
        SnippetCheck("M258-B001-ART-03", "objc_cross_module_runtime_metadata_semantic_preservation_contract"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-B001-IR-01", "M258-B001 cross-module semantic preservation anchor"),
        SnippetCheck("M258-B001-IR-02", "imported runtime metadata semantics are not lowered into IR"),
    ),
    API_H: (
        SnippetCheck("M258-B001-API-01", "M258-B001 cross-module semantic preservation anchor"),
        SnippetCheck("M258-B001-API-02", "filesystem-artifact only"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-B001-PKG-01", '"check:objc3c:m258-b001-cross-module-semantic-preservation-contract"'),
        SnippetCheck("M258-B001-PKG-02", '"test:tooling:m258-b001-cross-module-semantic-preservation-contract"'),
        SnippetCheck("M258-B001-PKG-03", '"check:objc3c:m258-b001-lane-b-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-B001-RUN-01", "M258-B001"),
        SnippetCheck("M258-B001-RUN-02", "check_m258_b001_cross_module_semantic_preservation_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-B001-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-B001-TEST-02", "def test_checker_passes_dynamic"),
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
        checks_total += require(True, display_path(NATIVE_EXE), "M258-B001-BIN-READY", "native binary present", failures)
        return checks_total
    build = run_process([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-B001-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-B001-NATIVE-EXISTS", "native binary missing after build", failures)
    return checks_total


def compute_semantic_counts(
    source_artifact_payload: dict[str, Any],
    runtime_metadata_source_records: dict[str, Any],
) -> dict[str, int]:
    declarations = source_artifact_payload.get("runtime_owned_declarations")
    if not isinstance(declarations, dict):
        raise TypeError("runtime_owned_declarations is incomplete")
    classes = declarations.get("classes")
    protocols = declarations.get("protocols")
    categories = declarations.get("categories")
    if not all(isinstance(value, list) for value in (classes, protocols, categories)):
        raise TypeError("runtime-owned class/protocol/category inventories are incomplete")
    properties = runtime_metadata_source_records.get("properties") if isinstance(runtime_metadata_source_records, dict) else None
    methods = runtime_metadata_source_records.get("methods") if isinstance(runtime_metadata_source_records, dict) else None
    ivars = runtime_metadata_source_records.get("ivars") if isinstance(runtime_metadata_source_records, dict) else None
    if not all(isinstance(value, list) for value in (properties, methods, ivars)):
        raise TypeError("runtime metadata source record projections are incomplete")

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
        sum(1 for record in properties if record.get("executable_synthesized_binding_kind") not in (None, "", "none"))
        + sum(
            1
            for record in ivars
            if record.get("executable_synthesized_binding_kind") not in (None, "", "none")
            or record.get("executable_ivar_layout_symbol")
        )
    )

    return {
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


def validate_surface(
    *,
    manifest: dict[str, Any],
    source_artifact_payload: dict[str, Any],
    artifact_label: str,
    expected_module: str,
    expect_class_records: bool,
    expect_category_records: bool,
    failures: list[Finding],
) -> tuple[int, dict[str, object]]:
    checks_total = 0
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    source_surface = semantic_surface.get(SOURCE_SURFACE_KEY) if isinstance(semantic_surface, dict) else None
    surface = semantic_surface.get(SURFACE_KEY) if isinstance(semantic_surface, dict) else None
    runtime_metadata_source_records = manifest.get("runtime_metadata_source_records")

    checks_total += require(isinstance(frontend, dict), artifact_label, "M258-B001-FRONTEND", "frontend payload missing", failures)
    checks_total += require(isinstance(source_surface, dict), artifact_label, "M258-B001-SOURCE-SURFACE", "A002 source surface missing", failures)
    checks_total += require(isinstance(surface, dict), artifact_label, "M258-B001-SURFACE", "B001 surface missing", failures)
    checks_total += require(isinstance(runtime_metadata_source_records, dict), artifact_label, "M258-B001-RECORDS", "runtime metadata source records missing", failures)
    if failures:
        return checks_total, {}

    counts = compute_semantic_counts(source_artifact_payload, runtime_metadata_source_records)

    checks_total += require(source_surface.get("contract_id") == SOURCE_FRONTEND_CLOSURE_CONTRACT_ID, artifact_label, "M258-B001-SOURCE-CONTRACT", "source closure contract id mismatch", failures)
    checks_total += require(source_surface.get("ready") is True, artifact_label, "M258-B001-SOURCE-READY", "source closure must be ready", failures)
    checks_total += require(surface.get("contract_id") == CONTRACT_ID, artifact_label, "M258-B001-CONTRACT", "surface contract id mismatch", failures)
    checks_total += require(surface.get("source_frontend_closure_contract_id") == SOURCE_FRONTEND_CLOSURE_CONTRACT_ID, artifact_label, "M258-B001-SOURCE-CLOSURE-CONTRACT", "source frontend closure contract mismatch", failures)
    checks_total += require(surface.get("frontend_surface_path") == SURFACE_PATH, artifact_label, "M258-B001-PATH", "surface path mismatch", failures)
    checks_total += require(surface.get("source_artifact_relative_path") == SOURCE_ARTIFACT_RELATIVE_PATH, artifact_label, "M258-B001-ARTIFACT", "source artifact path mismatch", failures)
    checks_total += require(surface.get("authority_model") == AUTHORITY_MODEL, artifact_label, "M258-B001-AUTHORITY", "authority model mismatch", failures)
    checks_total += require(surface.get("conformance_shape_model") == CONFORMANCE_SHAPE_MODEL, artifact_label, "M258-B001-CONFORMANCE-MODEL", "conformance model mismatch", failures)
    checks_total += require(surface.get("dispatch_trait_model") == DISPATCH_TRAIT_MODEL, artifact_label, "M258-B001-DISPATCH-MODEL", "dispatch model mismatch", failures)
    checks_total += require(surface.get("effect_trait_model") == EFFECT_TRAIT_MODEL, artifact_label, "M258-B001-EFFECT-MODEL", "effect model mismatch", failures)
    checks_total += require(surface.get("module_name") == expected_module, artifact_label, "M258-B001-MODULE", "module name mismatch", failures)
    for key, value in counts.items():
        checks_total += require(surface.get(key) == value, artifact_label, f"M258-B001-{key.upper()}", f"{key} mismatch", failures)
    checks_total += require(surface.get("fail_closed") is True, artifact_label, "M258-B001-FAIL-CLOSED", "surface must remain fail-closed", failures)
    checks_total += require(surface.get("source_frontend_closure_ready") is True, artifact_label, "M258-B001-SOURCE-READY-FLAG", "source closure ready flag must be true", failures)
    checks_total += require(surface.get("runtime_metadata_source_records_ready") is True, artifact_label, "M258-B001-RECORDS-READY", "runtime metadata source records ready flag must be true", failures)
    checks_total += require(surface.get("semantic_surface_published") is True, artifact_label, "M258-B001-PUBLISHED", "semantic surface must be published", failures)
    checks_total += require(surface.get("imported_conformance_shape_landed") is False, artifact_label, "M258-B001-CONFORMANCE-LANDED", "imported conformance shape must remain unlanded", failures)
    checks_total += require(surface.get("imported_dispatch_traits_landed") is False, artifact_label, "M258-B001-DISPATCH-LANDED", "imported dispatch traits must remain unlanded", failures)
    checks_total += require(surface.get("imported_effect_traits_landed") is False, artifact_label, "M258-B001-EFFECT-LANDED", "imported effect traits must remain unlanded", failures)
    checks_total += require(surface.get("imported_runtime_metadata_semantics_landed") is False, artifact_label, "M258-B001-SEMANTICS-LANDED", "imported runtime metadata semantics must remain unlanded", failures)
    checks_total += require(surface.get("ready_for_imported_metadata_semantic_rules") is False, artifact_label, "M258-B001-READY-RULES", "ready_for_imported_metadata_semantic_rules must remain false", failures)
    checks_total += require(surface.get("ready_for_cross_module_dispatch_equivalence") is False, artifact_label, "M258-B001-READY-DISPATCH", "ready_for_cross_module_dispatch_equivalence must remain false", failures)
    checks_total += require(surface.get("source_frontend_closure_replay_key") == source_surface.get("replay_key"), artifact_label, "M258-B001-SOURCE-REPLAY", "source replay key mismatch", failures)
    checks_total += require(bool(surface.get("replay_key")), artifact_label, "M258-B001-REPLAY", "surface replay key must be non-empty", failures)
    checks_total += require(surface.get("failure_reason") == "", artifact_label, "M258-B001-FAILURE", "failure reason must be empty", failures)
    checks_total += require((counts["class_record_count"] > 0) is expect_class_records, artifact_label, "M258-B001-EXPECT-CLASS", "class inventory expectation mismatch", failures)
    checks_total += require((counts["category_record_count"] > 0) is expect_category_records, artifact_label, "M258-B001-EXPECT-CATEGORY", "category inventory expectation mismatch", failures)
    checks_total += require(runtime_metadata_source_records.get("deterministic") is True, artifact_label, "M258-B001-RECORDS-DETERMINISTIC", "runtime metadata source records must be deterministic", failures)
    checks_total += require(len(runtime_metadata_source_records.get("properties", [])) == counts["property_record_count"], artifact_label, "M258-B001-RECORDS-PROPERTY-COUNT", "manifest property record projection mismatch", failures)
    checks_total += require(len(runtime_metadata_source_records.get("methods", [])) == counts["method_record_count"], artifact_label, "M258-B001-RECORDS-METHOD-COUNT", "manifest method record projection mismatch", failures)
    checks_total += require(len(runtime_metadata_source_records.get("ivars", [])) == counts["ivar_record_count"], artifact_label, "M258-B001-RECORDS-IVAR-COUNT", "manifest ivar record projection mismatch", failures)

    return checks_total, {
        "module_name": expected_module,
        **counts,
        "replay_key": surface.get("replay_key"),
    }


def compile_fixture(
    *,
    label: str,
    fixture: Path,
    out_dir: Path,
    expected_module: str,
    expect_class_records: bool,
    expect_category_records: bool,
    failures: list[Finding],
) -> tuple[int, dict[str, object]]:
    checks_total = 0
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])
    manifest_path = out_dir / "module.manifest.json"
    artifact_path = out_dir / SOURCE_ARTIFACT_RELATIVE_PATH
    backend_path = out_dir / "module.object-backend.txt"
    checks_total += require(completed.returncode == 0, label, "M258-B001-COMPILE", completed.stderr or completed.stdout or "compile failed", failures)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M258-B001-MANIFEST", "manifest missing", failures)
    checks_total += require(artifact_path.exists(), display_path(artifact_path), "M258-B001-SOURCE-ARTIFACT", "A002 source artifact missing", failures)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M258-B001-BACKEND", "backend marker missing", failures)
    if backend_path.exists():
        backend_text = backend_path.read_text(encoding="utf-8").strip()
        checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M258-B001-BACKEND-TEXT", "backend marker must remain llvm-direct", failures)
    if failures:
        return checks_total, {}
    manifest = load_json(manifest_path)
    source_artifact_payload = load_json(artifact_path)
    surface_checks, summary = validate_surface(
        manifest=manifest,
        source_artifact_payload=source_artifact_payload,
        artifact_label=display_path(manifest_path),
        expected_module=expected_module,
        expect_class_records=expect_class_records,
        expect_category_records=expect_category_records,
        failures=failures,
    )
    checks_total += surface_checks
    return checks_total, summary


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
            class_checks, class_summary = compile_fixture(
                label="class-fixture",
                fixture=CLASS_FIXTURE,
                out_dir=PROBE_ROOT / "class_fixture",
                expected_module="runtimeMetadataClassRecords",
                expect_class_records=True,
                expect_category_records=False,
                failures=failures,
            )
            checks_total += class_checks
            category_checks, category_summary = compile_fixture(
                label="category-fixture",
                fixture=CATEGORY_FIXTURE,
                out_dir=PROBE_ROOT / "category_fixture",
                expected_module="runtimeMetadataCategoryRecords",
                expect_class_records=False,
                expect_category_records=True,
                failures=failures,
            )
            checks_total += category_checks
            dynamic_summary.update({
                "class_fixture": class_summary,
                "category_fixture": category_summary,
            })

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total,
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    if failures:
        payload["checks_passed"] = checks_total - len(failures)
    return payload, failures, checks_total


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true", help="only run static contract checks")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT, help="where to write the JSON summary")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    payload, failures, checks_total = build_summary(args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    checks_passed = payload["checks_passed"]
    if failures:
        print(f"[error] {MODE} ({checks_passed}/{checks_total} checks passed)", file=sys.stderr)
        for failure in failures:
            print(f"- {failure.check_id} [{failure.artifact}] {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE} ({checks_passed}/{checks_total} checks passed)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
