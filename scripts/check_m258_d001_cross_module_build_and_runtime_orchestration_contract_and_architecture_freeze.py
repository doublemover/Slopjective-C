#!/usr/bin/env python3
"""Fail-closed checker for M258-D001 cross-module build/runtime orchestration."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-d001-cross-module-build-runtime-orchestration-v1"
CONTRACT_ID = "objc3c-cross-module-build-runtime-orchestration/m258-d001-v1"
SOURCE_REUSE_CONTRACT_ID = "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1"
SOURCE_REGISTRATION_MANIFEST_CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_cross_module_build_runtime_orchestration_contract"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
AUTHORITY_MODEL = "serialized-runtime-import-surface-reuse-payload-plus-local-registration-manifest"
INPUT_MODEL = "filesystem-runtime-import-surface-artifact-path-list-plus-local-registration-manifest"
REGISTRATION_SCOPE_MODEL = "translation-unit-registration-manifests-remain-image-local-until-cross-module-registration-aggregation-lands"
PACKAGING_MODEL = "no-cross-module-link-plan-artifact-or-imported-registration-manifest-ingest-during-freeze"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-D001" / "cross_module_build_runtime_orchestration_contract_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "d001-cross-module-build-runtime-orchestration"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_d001_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_d001_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_d001_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze.py"
UPSTREAM_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
MID_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_imported_runtime_semantic_rules_consumer.objc3"
DOWNSTREAM_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
PAYLOAD_MEMBER = "serialized_runtime_metadata_reuse_payload"


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
        SnippetCheck("M258-D001-DOC-01", "# M258 Cross-Module Build And Runtime Orchestration Contract And Architecture Freeze Expectations (D001)"),
        SnippetCheck("M258-D001-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-D001-DOC-03", "Issue: `#7164`"),
        SnippetCheck("M258-D001-DOC-04", f"`{REGISTRATION_MANIFEST_ARTIFACT}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-D001-PKT-01", "# M258-D001 Cross-Module Build And Runtime Orchestration Contract And Architecture Freeze Packet"),
        SnippetCheck("M258-D001-PKT-02", "Packet: `M258-D001`"),
        SnippetCheck("M258-D001-PKT-03", "Dependencies: `M258-C002`, `M254-A002`"),
        SnippetCheck("M258-D001-PKT-04", "Next issue: `M258-D002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-D001-NDOC-01", "## Cross-module build and runtime orchestration freeze (M258-D001)"),
        SnippetCheck("M258-D001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-D001-NDOC-03", f"`{REGISTRATION_MANIFEST_ARTIFACT}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-D001-SPC-01", "## M258 cross-module build and runtime orchestration freeze (D001)"),
        SnippetCheck("M258-D001-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-D001-SPC-03", "cross-module link-plan artifacts are not landed"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-D001-META-01", "## M258 cross-module build and runtime orchestration anchors (D001)"),
        SnippetCheck("M258-D001-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-D001-META-03", f"`{REGISTRATION_MANIFEST_ARTIFACT}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-D001-ARCH-01", "## M258 cross-module build and runtime orchestration (D001)"),
        SnippetCheck("M258-D001-ARCH-02", "`M258-D002` is the next lane-D step:"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M258-D001-TYPE-01", "struct Objc3CrossModuleBuildRuntimeOrchestrationSummary {"),
        SnippetCheck("M258-D001-TYPE-02", "kObjc3CrossModuleBuildRuntimeOrchestrationContractId"),
        SnippetCheck("M258-D001-TYPE-03", "ready_for_packaging_and_runtime_registration_impl"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-D001-ART-01", "BuildCrossModuleBuildRuntimeOrchestrationSummary("),
        SnippetCheck("M258-D001-ART-02", "BuildCrossModuleBuildRuntimeOrchestrationSummaryJson("),
        SnippetCheck("M258-D001-ART-03", "objc_cross_module_build_runtime_orchestration_contract"),
        SnippetCheck("M258-D001-ART-04", '"O3S267"'),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-D001-IR-01", "M258-D001 cross-module build/runtime orchestration anchor"),
        SnippetCheck("M258-D001-IR-02", "aggregated runtime-registration"),
    ),
    API_H: (
        SnippetCheck("M258-D001-API-01", "M258-D001 cross-module build/runtime orchestration anchor"),
        SnippetCheck("M258-D001-API-02", "registration-manifest ingestion handle"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-D001-PKG-01", '"check:objc3c:m258-d001-cross-module-build-and-runtime-orchestration-contract"'),
        SnippetCheck("M258-D001-PKG-02", '"test:tooling:m258-d001-cross-module-build-and-runtime-orchestration-contract"'),
        SnippetCheck("M258-D001-PKG-03", '"check:objc3c:m258-d001-lane-d-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-D001-RUN-01", "M258-C002 + M258-D001"),
        SnippetCheck("M258-D001-RUN-02", "check_m258_d001_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-D001-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-D001-TEST-02", "def test_checker_passes_dynamic"),
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
        checks_total += require(True, display_path(NATIVE_EXE), "M258-D001-BIN-READY", "native binary present", failures)
        return checks_total
    build = run_process([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-D001-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-D001-NATIVE-EXISTS", "native binary missing after build", failures)
    return checks_total


def compile_fixture(*, fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", *extra_args])


def validate_manifest_surface(
    manifest: dict[str, Any],
    *,
    registration_manifest: dict[str, Any],
    expected_modules: list[str],
    expected_runtime_owned_declaration_count: int,
    expected_metadata_reference_count: int,
    failures: list[Finding],
) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    surface = semantic_surface.get("objc_cross_module_build_runtime_orchestration_contract") if isinstance(semantic_surface, dict) else None
    checks_total += require(isinstance(surface, dict), "downstream manifest", "M258-D001-SURFACE", "cross-module build/runtime orchestration surface missing", failures)
    if not isinstance(surface, dict):
        return checks_total, {}

    checks_total += require(surface.get("contract_id") == CONTRACT_ID, "downstream manifest", "M258-D001-CONTRACT", "unexpected D001 contract id", failures)
    checks_total += require(surface.get("source_serialized_runtime_metadata_artifact_reuse_contract_id") == SOURCE_REUSE_CONTRACT_ID, "downstream manifest", "M258-D001-SOURCE-REUSE", "unexpected source reuse contract id", failures)
    checks_total += require(surface.get("source_local_registration_manifest_contract_id") == SOURCE_REGISTRATION_MANIFEST_CONTRACT_ID, "downstream manifest", "M258-D001-SOURCE-MANIFEST", "unexpected source registration manifest contract id", failures)
    checks_total += require(surface.get("frontend_surface_path") == SURFACE_PATH, "downstream manifest", "M258-D001-PATH", "unexpected surface path", failures)
    checks_total += require(surface.get("import_artifact_relative_path") == IMPORT_ARTIFACT, "downstream manifest", "M258-D001-IMPORT-ARTIFACT", "unexpected import artifact path", failures)
    checks_total += require(surface.get("local_registration_manifest_artifact_relative_path") == REGISTRATION_MANIFEST_ARTIFACT, "downstream manifest", "M258-D001-REG-ARTIFACT", "unexpected registration manifest path", failures)
    checks_total += require(surface.get("authority_model") == AUTHORITY_MODEL, "downstream manifest", "M258-D001-AUTHORITY", "unexpected authority model", failures)
    checks_total += require(surface.get("input_model") == INPUT_MODEL, "downstream manifest", "M258-D001-INPUT", "unexpected input model", failures)
    checks_total += require(surface.get("registration_scope_model") == REGISTRATION_SCOPE_MODEL, "downstream manifest", "M258-D001-SCOPE", "unexpected registration scope model", failures)
    checks_total += require(surface.get("packaging_model") == PACKAGING_MODEL, "downstream manifest", "M258-D001-PACKAGING", "unexpected packaging model", failures)
    checks_total += require(surface.get("module_names_lexicographic") == expected_modules, "downstream manifest", "M258-D001-MODULES", "unexpected module-image set", failures)
    checks_total += require(surface.get("module_image_count") == len(expected_modules), "downstream manifest", "M258-D001-MODULE-COUNT", "unexpected module image count", failures)
    checks_total += require(surface.get("direct_import_input_count") == 1, "downstream manifest", "M258-D001-DIRECT-INPUTS", "unexpected direct imported-runtime-surface input count", failures)
    checks_total += require(surface.get("transitive_runtime_owned_declaration_count") == expected_runtime_owned_declaration_count, "downstream manifest", "M258-D001-DECL-COUNT", "unexpected transitive runtime-owned declaration count", failures)
    checks_total += require(surface.get("transitive_metadata_reference_count") == expected_metadata_reference_count, "downstream manifest", "M258-D001-REF-COUNT", "unexpected transitive metadata reference count", failures)
    checks_total += require(surface.get("local_class_descriptor_count") == registration_manifest.get("class_descriptor_count"), "downstream manifest", "M258-D001-CLASS-DESC", "local class descriptor count mismatch", failures)
    checks_total += require(surface.get("local_protocol_descriptor_count") == registration_manifest.get("protocol_descriptor_count"), "downstream manifest", "M258-D001-PROTO-DESC", "local protocol descriptor count mismatch", failures)
    checks_total += require(surface.get("local_category_descriptor_count") == registration_manifest.get("category_descriptor_count"), "downstream manifest", "M258-D001-CATEGORY-DESC", "local category descriptor count mismatch", failures)
    checks_total += require(surface.get("local_property_descriptor_count") == registration_manifest.get("property_descriptor_count"), "downstream manifest", "M258-D001-PROPERTY-DESC", "local property descriptor count mismatch", failures)
    checks_total += require(surface.get("local_ivar_descriptor_count") == registration_manifest.get("ivar_descriptor_count"), "downstream manifest", "M258-D001-IVAR-DESC", "local ivar descriptor count mismatch", failures)
    checks_total += require(surface.get("local_total_descriptor_count") == registration_manifest.get("total_descriptor_count"), "downstream manifest", "M258-D001-TOTAL-DESC", "local total descriptor count mismatch", failures)

    truthy_keys = (
        "ready",
        "fail_closed",
        "source_serialized_runtime_metadata_artifact_reuse_ready",
        "source_local_registration_manifest_ready",
        "semantic_surface_published",
        "local_registration_manifest_emitted",
    )
    falsy_keys = (
        "cross_module_link_plan_artifact_landed",
        "imported_registration_manifest_loading_landed",
        "runtime_archive_aggregation_landed",
        "cross_module_runtime_registration_landed",
        "cross_module_launch_orchestration_landed",
        "public_cross_module_orchestration_abi_landed",
        "ready_for_packaging_and_runtime_registration_impl",
    )
    for key in truthy_keys:
        checks_total += require(surface.get(key) is True, "downstream manifest", f"M258-D001-{key.upper()}", f"expected {key}=true", failures)
    for key in falsy_keys:
        checks_total += require(surface.get(key) is False, "downstream manifest", f"M258-D001-{key.upper()}", f"expected {key}=false", failures)
    checks_total += require(bool(surface.get("source_serialized_runtime_metadata_replay_key")), "downstream manifest", "M258-D001-REUSE-REPLAY", "source serialized-runtime-metadata replay key missing", failures)
    checks_total += require(bool(surface.get("source_local_registration_manifest_replay_key")), "downstream manifest", "M258-D001-MANIFEST-REPLAY", "source registration manifest replay key missing", failures)
    checks_total += require(bool(surface.get("replay_key")), "downstream manifest", "M258-D001-REPLAY", "surface replay key missing", failures)
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
            upstream_out = PROBE_ROOT / "upstream"
            mid_out = PROBE_ROOT / "mid"
            downstream_out = PROBE_ROOT / "downstream"

            upstream_completed = compile_fixture(fixture=UPSTREAM_FIXTURE, out_dir=upstream_out)
            upstream_artifact_path = upstream_out / IMPORT_ARTIFACT
            checks_total += require(upstream_completed.returncode == 0, display_path(UPSTREAM_FIXTURE), "M258-D001-UPSTREAM-COMPILE", upstream_completed.stderr or upstream_completed.stdout or "upstream compile failed", failures)
            checks_total += require(upstream_artifact_path.exists(), display_path(upstream_artifact_path), "M258-D001-UPSTREAM-ARTIFACT", "upstream import artifact missing", failures)

            if not failures:
                upstream_payload = load_json(upstream_artifact_path)
                upstream_module_name = str(upstream_payload.get("module_name", ""))

                mid_completed = compile_fixture(
                    fixture=MID_FIXTURE,
                    out_dir=mid_out,
                    extra_args=("--objc3-import-runtime-surface", str(upstream_artifact_path)),
                )
                mid_artifact_path = mid_out / IMPORT_ARTIFACT
                checks_total += require(mid_completed.returncode == 0, display_path(MID_FIXTURE), "M258-D001-MID-COMPILE", mid_completed.stderr or mid_completed.stdout or "mid compile failed", failures)
                checks_total += require(mid_artifact_path.exists(), display_path(mid_artifact_path), "M258-D001-MID-ARTIFACT", "mid import artifact missing", failures)

                if not failures:
                    mid_payload = load_json(mid_artifact_path)
                    mid_module_name = str(mid_payload.get("module_name", ""))

                    downstream_completed = compile_fixture(
                        fixture=DOWNSTREAM_FIXTURE,
                        out_dir=downstream_out,
                        extra_args=("--objc3-import-runtime-surface", str(mid_artifact_path)),
                    )
                    downstream_artifact_path = downstream_out / IMPORT_ARTIFACT
                    downstream_manifest_path = downstream_out / "module.manifest.json"
                    downstream_registration_manifest_path = downstream_out / REGISTRATION_MANIFEST_ARTIFACT
                    checks_total += require(downstream_completed.returncode == 0, display_path(DOWNSTREAM_FIXTURE), "M258-D001-DOWNSTREAM-COMPILE", downstream_completed.stderr or downstream_completed.stdout or "downstream compile failed", failures)
                    checks_total += require(downstream_artifact_path.exists(), display_path(downstream_artifact_path), "M258-D001-DOWNSTREAM-ARTIFACT", "downstream import artifact missing", failures)
                    checks_total += require(downstream_manifest_path.exists(), display_path(downstream_manifest_path), "M258-D001-DOWNSTREAM-MANIFEST", "downstream manifest missing", failures)
                    checks_total += require(downstream_registration_manifest_path.exists(), display_path(downstream_registration_manifest_path), "M258-D001-DOWNSTREAM-REGISTRATION-MANIFEST", "downstream runtime registration manifest missing", failures)

                    if downstream_artifact_path.exists() and downstream_manifest_path.exists() and downstream_registration_manifest_path.exists():
                        downstream_payload = load_json(downstream_artifact_path)
                        downstream_module_name = str(downstream_payload.get("module_name", ""))
                        reuse_payload = downstream_payload.get(PAYLOAD_MEMBER)
                        checks_total += require(isinstance(reuse_payload, dict), display_path(downstream_artifact_path), "M258-D001-REUSE-PAYLOAD", "downstream serialized reuse payload missing", failures)
                        if isinstance(reuse_payload, dict):
                            expected_modules = sorted({upstream_module_name, mid_module_name, downstream_module_name})
                            checks_total += require(reuse_payload.get("reused_module_names_lexicographic") == expected_modules, display_path(downstream_artifact_path), "M258-D001-REUSE-MODULES", "downstream serialized reuse payload module set mismatch", failures)
                            downstream_manifest = load_json(downstream_manifest_path)
                            downstream_registration_manifest = load_json(downstream_registration_manifest_path)
                            manifest_checks, manifest_surface = validate_manifest_surface(
                                downstream_manifest,
                                registration_manifest=downstream_registration_manifest,
                                expected_modules=expected_modules,
                                expected_runtime_owned_declaration_count=int(reuse_payload.get("runtime_owned_declaration_count", 0)),
                                expected_metadata_reference_count=int(reuse_payload.get("metadata_reference_count", 0)),
                                failures=failures,
                            )
                            checks_total += manifest_checks
                            dynamic_summary.update(
                                {
                                    "upstream": {"module_name": upstream_module_name},
                                    "mid": {"module_name": mid_module_name},
                                    "downstream": {
                                        "module_name": downstream_module_name,
                                        "reuse_payload": reuse_payload,
                                        "manifest_surface": manifest_surface,
                                        "registration_manifest": downstream_registration_manifest,
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
