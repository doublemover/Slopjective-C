#!/usr/bin/env python3
"""Fail-closed checker for M258-C002 serialized runtime-metadata artifact reuse."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-c002-serialized-runtime-metadata-artifact-reuse-v1"
CONTRACT_ID = "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1"
SOURCE_CONTRACT_ID = "objc3c-serialized-runtime-metadata-import-lowering/m258-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_artifact_reuse"
PAYLOAD_MEMBER = "serialized_runtime_metadata_reuse_payload"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-C002" / "module_metadata_artifact_reuse_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "c002-module-metadata-artifact-reuse"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_c002_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py"
UPSTREAM_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
MID_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_imported_runtime_semantic_rules_consumer.objc3"
DOWNSTREAM_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_category_protocol_property.objc3"
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
        SnippetCheck("M258-C002-DOC-01", "# M258 Module Metadata Serialization, Deserialization, And Artifact Reuse Core Feature Implementation Expectations (C002)"),
        SnippetCheck("M258-C002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-C002-DOC-03", "Issue: `#7163`"),
        SnippetCheck("M258-C002-DOC-04", f"`{PAYLOAD_MEMBER}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-C002-PKT-01", "# M258-C002 Module Metadata Serialization, Deserialization, And Artifact Reuse Core Feature Implementation Packet"),
        SnippetCheck("M258-C002-PKT-02", "Packet: `M258-C002`"),
        SnippetCheck("M258-C002-PKT-03", "Dependencies: `M258-C001`, `M258-B002`"),
        SnippetCheck("M258-C002-PKT-04", "Next issue: `M258-D001`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-C002-NDOC-01", "## Serialized metadata artifact reuse (M258-C002)"),
        SnippetCheck("M258-C002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-C002-NDOC-03", f"`{PAYLOAD_MEMBER}`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-C002-SPC-01", "## M258 module metadata serialization, deserialization, and artifact reuse (C002)"),
        SnippetCheck("M258-C002-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-C002-SPC-03", "downstream imports prefer that payload when present"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-C002-META-01", "## M258 serialized metadata artifact reuse anchors (C002)"),
        SnippetCheck("M258-C002-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-C002-META-03", f"`{PAYLOAD_MEMBER}`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-C002-ARCH-01", "`M258-C002` is the follow-on lane-C implementation step:"),
        SnippetCheck("M258-C002-ARCH-02", PAYLOAD_MEMBER),
        SnippetCheck("M258-C002-ARCH-03", "prefer the serialized reuse payload when"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M258-C002-TYPE-01", "struct Objc3SerializedRuntimeMetadataArtifactReuseSummary {"),
        SnippetCheck("M258-C002-TYPE-02", "kObjc3SerializedRuntimeMetadataArtifactReuseContractId"),
        SnippetCheck("M258-C002-TYPE-03", "downstream_module_consumption_ready"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-C002-ART-01", "BuildSerializedRuntimeMetadataArtifactReuseSummary("),
        SnippetCheck("M258-C002-ART-02", "BuildSerializedRuntimeMetadataReusePayloadJson("),
        SnippetCheck("M258-C002-ART-03", "BuildSerializedRuntimeMetadataReuseRecordSet("),
        SnippetCheck("M258-C002-ART-04", '"O3S266"'),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M258-C002-IMPH-01", "reused_module_names_lexicographic"),
        SnippetCheck("M258-C002-IMPH-02", "uses_serialized_runtime_metadata_payload"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M258-C002-IMP-01", "ParseSerializedRuntimeMetadataReusePayload("),
        SnippetCheck("M258-C002-IMP-02", "kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName"),
        SnippetCheck("M258-C002-IMP-03", "surface.uses_serialized_runtime_metadata_payload = true"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-C002-IR-01", "M258-C002 serialized metadata artifact reuse anchor"),
        SnippetCheck("M258-C002-IR-02", "runtime-import-surface artifacts may now carry a transitive serialized"),
    ),
    API_H: (
        SnippetCheck("M258-C002-API-01", "M258-C002 serialized metadata artifact reuse anchor"),
        SnippetCheck("M258-C002-API-02", "serialized-payload handles"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-C002-PKG-01", '"check:objc3c:m258-c002-module-metadata-serialization-deserialization-and-artifact-reuse"'),
        SnippetCheck("M258-C002-PKG-02", '"test:tooling:m258-c002-module-metadata-serialization-deserialization-and-artifact-reuse"'),
        SnippetCheck("M258-C002-PKG-03", '"check:objc3c:m258-c002-lane-c-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-C002-RUN-01", "M258-B002 + M258-C001 + M258-C002"),
        SnippetCheck("M258-C002-RUN-02", "check_m258_c002_module_metadata_serialization_deserialization_and_artifact_reuse_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-C002-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-C002-TEST-02", "def test_checker_passes_dynamic"),
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
        checks_total += require(True, display_path(NATIVE_EXE), "M258-C002-BIN-READY", "native binary present", failures)
        return checks_total
    build = run_process([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-C002-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-C002-NATIVE-EXISTS", "native binary missing after build", failures)
    return checks_total


def compile_fixture(*, fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", *extra_args])


def validate_reuse_payload(artifact_payload: dict[str, Any], *, expected_contract: str, failures: list[Finding], artifact_label: str) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload = artifact_payload.get(PAYLOAD_MEMBER)
    checks_total += require(isinstance(payload, dict), artifact_label, "M258-C002-PAYLOAD", "serialized reuse payload missing from import artifact", failures)
    if not isinstance(payload, dict):
        return checks_total, {}
    checks_total += require(payload.get("contract_id") == expected_contract, artifact_label, "M258-C002-PAYLOAD-CONTRACT", "unexpected serialized reuse payload contract id", failures)
    checks_total += require(payload.get("ready") is True, artifact_label, "M258-C002-PAYLOAD-READY", "serialized reuse payload should be ready", failures)
    checks_total += require(isinstance(payload.get("reused_module_names_lexicographic"), list) and bool(payload.get("reused_module_names_lexicographic")), artifact_label, "M258-C002-PAYLOAD-MODULES", "serialized reuse payload must list reused modules", failures)
    checks_total += require(isinstance(payload.get("runtime_owned_declarations"), dict), artifact_label, "M258-C002-PAYLOAD-DECLS", "serialized reuse payload declarations missing", failures)
    checks_total += require(isinstance(payload.get("metadata_references"), list), artifact_label, "M258-C002-PAYLOAD-REFS", "serialized reuse payload references missing", failures)
    return checks_total, payload


def validate_manifest_surface(manifest: dict[str, Any], *, expected_modules: list[str], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    surface = semantic_surface.get("objc_serialized_runtime_metadata_artifact_reuse") if isinstance(semantic_surface, dict) else None
    checks_total += require(isinstance(surface, dict), "downstream manifest", "M258-C002-SURFACE", "serialized runtime metadata artifact reuse surface missing", failures)
    if not isinstance(surface, dict):
        return checks_total, {}
    checks_total += require(surface.get("contract_id") == CONTRACT_ID, "downstream manifest", "M258-C002-CONTRACT", "unexpected C002 contract id", failures)
    checks_total += require(surface.get("source_serialized_import_lowering_contract_id") == SOURCE_CONTRACT_ID, "downstream manifest", "M258-C002-SOURCE-CONTRACT", "unexpected source contract id", failures)
    checks_total += require(surface.get("frontend_surface_path") == SURFACE_PATH, "downstream manifest", "M258-C002-SURFACE-PATH", "unexpected surface path", failures)
    checks_total += require(surface.get("payload_member_name") == PAYLOAD_MEMBER, "downstream manifest", "M258-C002-PAYLOAD-NAME", "unexpected payload member name", failures)
    checks_total += require(surface.get("reused_module_names_lexicographic") == expected_modules, "downstream manifest", "M258-C002-MODULES", "unexpected reused module names", failures)
    checks_total += require(surface.get("reused_module_count") == len(expected_modules), "downstream manifest", "M258-C002-MODULE-COUNT", "unexpected reused module count", failures)
    for key in (
        "ready",
        "fail_closed",
        "source_serialized_import_lowering_ready",
        "semantic_surface_published",
        "serialized_metadata_rehydration_landed",
        "artifact_reuse_landed",
        "downstream_module_consumption_ready",
    ):
        checks_total += require(surface.get(key) is True, "downstream manifest", f"M258-C002-{key.upper()}", f"expected {key}=true", failures)
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
            checks_total += require(upstream_completed.returncode == 0, display_path(UPSTREAM_FIXTURE), "M258-C002-UPSTREAM-COMPILE", upstream_completed.stderr or upstream_completed.stdout or "upstream compile failed", failures)
            checks_total += require(upstream_artifact_path.exists(), display_path(upstream_artifact_path), "M258-C002-UPSTREAM-ARTIFACT", "upstream import artifact missing", failures)

            if not failures:
                upstream_payload = load_json(upstream_artifact_path)
                upstream_module_name = str(upstream_payload.get("module_name", ""))

                mid_completed = compile_fixture(
                    fixture=MID_FIXTURE,
                    out_dir=mid_out,
                    extra_args=("--objc3-import-runtime-surface", str(upstream_artifact_path)),
                )
                mid_artifact_path = mid_out / IMPORT_ARTIFACT
                checks_total += require(mid_completed.returncode == 0, display_path(MID_FIXTURE), "M258-C002-MID-COMPILE", mid_completed.stderr or mid_completed.stdout or "mid compile failed", failures)
                checks_total += require(mid_artifact_path.exists(), display_path(mid_artifact_path), "M258-C002-MID-ARTIFACT", "mid import artifact missing", failures)

                if not failures:
                    mid_payload = load_json(mid_artifact_path)
                    mid_module_name = str(mid_payload.get("module_name", ""))
                    mid_checks, mid_reuse_payload = validate_reuse_payload(mid_payload, expected_contract=CONTRACT_ID, failures=failures, artifact_label=display_path(mid_artifact_path))
                    checks_total += mid_checks
                    expected_mid_modules = sorted({upstream_module_name, mid_module_name})
                    checks_total += require(mid_reuse_payload.get("reused_module_names_lexicographic") == expected_mid_modules, display_path(mid_artifact_path), "M258-C002-MID-MODULES", "mid serialized reuse payload does not carry the expected upstream+mid module set", failures)
                    checks_total += require(int(mid_reuse_payload.get("runtime_owned_declaration_count", 0)) > int(mid_payload.get("runtime_owned_declaration_count", 0)), display_path(mid_artifact_path), "M258-C002-MID-TRANSITIVE-COUNT", "mid serialized reuse payload did not grow beyond local-only declaration count", failures)

                    downstream_completed = compile_fixture(
                        fixture=DOWNSTREAM_FIXTURE,
                        out_dir=downstream_out,
                        extra_args=("--objc3-import-runtime-surface", str(mid_artifact_path)),
                    )
                    downstream_artifact_path = downstream_out / IMPORT_ARTIFACT
                    downstream_manifest_path = downstream_out / "module.manifest.json"
                    checks_total += require(downstream_completed.returncode == 0, display_path(DOWNSTREAM_FIXTURE), "M258-C002-DOWNSTREAM-COMPILE", downstream_completed.stderr or downstream_completed.stdout or "downstream compile failed", failures)
                    checks_total += require(downstream_artifact_path.exists(), display_path(downstream_artifact_path), "M258-C002-DOWNSTREAM-ARTIFACT", "downstream import artifact missing", failures)
                    checks_total += require(downstream_manifest_path.exists(), display_path(downstream_manifest_path), "M258-C002-DOWNSTREAM-MANIFEST", "downstream manifest missing", failures)

                    if downstream_artifact_path.exists() and downstream_manifest_path.exists():
                        downstream_payload = load_json(downstream_artifact_path)
                        downstream_module_name = str(downstream_payload.get("module_name", ""))
                        downstream_checks, downstream_reuse_payload = validate_reuse_payload(downstream_payload, expected_contract=CONTRACT_ID, failures=failures, artifact_label=display_path(downstream_artifact_path))
                        checks_total += downstream_checks
                        expected_downstream_modules = sorted({upstream_module_name, mid_module_name, downstream_module_name})
                        checks_total += require(downstream_reuse_payload.get("reused_module_names_lexicographic") == expected_downstream_modules, display_path(downstream_artifact_path), "M258-C002-DOWNSTREAM-PAYLOAD-MODULES", "downstream serialized reuse payload does not carry the full transitive module set", failures)
                        downstream_manifest = load_json(downstream_manifest_path)
                        manifest_checks, manifest_surface = validate_manifest_surface(downstream_manifest, expected_modules=expected_downstream_modules, failures=failures)
                        checks_total += manifest_checks
                        checks_total += require(manifest_surface.get("runtime_owned_declaration_count") == downstream_reuse_payload.get("runtime_owned_declaration_count"), display_path(downstream_manifest_path), "M258-C002-DOWNSTREAM-COUNT-MATCH", "manifest C002 surface count does not match emitted reuse payload count", failures)

                        dynamic_summary.update(
                            {
                                "upstream": {"module_name": upstream_module_name},
                                "mid": {
                                    "module_name": mid_module_name,
                                    "reuse_payload": mid_reuse_payload,
                                },
                                "downstream": {
                                    "module_name": downstream_module_name,
                                    "reuse_payload": downstream_reuse_payload,
                                    "manifest_surface": manifest_surface,
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
