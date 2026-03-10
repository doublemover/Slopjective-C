#!/usr/bin/env python3
"""Fail-closed checker for M258-C001 serialized metadata import/lowering freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m258-c001-serialized-runtime-metadata-import-lowering-freeze-v1"
CONTRACT_ID = "objc3c-serialized-runtime-metadata-import-lowering/m258-c001-v1"
SOURCE_CONTRACT_ID = "objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_import_lowering_contract"
INPUT_MODEL = "filesystem-runtime-import-surface-artifact-path-list"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-C001" / "serialized_metadata_import_and_lowering_contract_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "c001-serialized-metadata-import-lowering"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_serialized_metadata_import_and_lowering_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_c001_serialized_metadata_import_and_lowering_contract_and_architecture_freeze_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_c001_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_c001_serialized_metadata_import_and_lowering_contract_and_architecture_freeze.py"
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
        SnippetCheck("M258-C001-DOC-01", "# M258 Serialized Metadata Import And Lowering Contract And Architecture Freeze Expectations (C001)"),
        SnippetCheck("M258-C001-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-C001-DOC-03", "Issue: `#7162`"),
        SnippetCheck("M258-C001-DOC-04", f"`{SURFACE_PATH}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-C001-PKT-01", "# M258-C001 Serialized Metadata Import And Lowering Contract And Architecture Freeze Packet"),
        SnippetCheck("M258-C001-PKT-02", "Packet: `M258-C001`"),
        SnippetCheck("M258-C001-PKT-03", "Dependencies: `M258-B002`"),
        SnippetCheck("M258-C001-PKT-04", "Next issue: `M258-C002`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-C001-NDOC-01", "## Serialized metadata import and lowering freeze (M258-C001)"),
        SnippetCheck("M258-C001-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-C001-NDOC-03", "serialized imported metadata payloads are not rehydrated into live"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-C001-SPC-01", "## M258 serialized metadata import and lowering freeze (C001)"),
        SnippetCheck("M258-C001-SPC-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-C001-SPC-03", "imported metadata payloads are not lowered into IR in this lane"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-C001-META-01", "## M258 serialized metadata import/lowering anchors (C001)"),
        SnippetCheck("M258-C001-META-02", f"`{SURFACE_PATH}`"),
        SnippetCheck("M258-C001-META-03", "serialized metadata rehydration is not landed"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-C001-ARCH-01", "`M258-C001` is the next lane-C step:"),
        SnippetCheck("M258-C001-ARCH-02", "objc_serialized_runtime_metadata_import_lowering_contract"),
        SnippetCheck("M258-C001-ARCH-03", "serialized imported metadata payloads are not rehydrated yet"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M258-C001-TYPE-01", "struct Objc3SerializedRuntimeMetadataImportLoweringSummary {"),
        SnippetCheck("M258-C001-TYPE-02", "kObjc3SerializedRuntimeMetadataImportLoweringContractId"),
        SnippetCheck("M258-C001-TYPE-03", "ready_for_serialized_metadata_lowering_impl"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-C001-ART-01", "BuildSerializedRuntimeMetadataImportLoweringSummary("),
        SnippetCheck("M258-C001-ART-02", "BuildSerializedRuntimeMetadataImportLoweringSummaryJson("),
        SnippetCheck("M258-C001-ART-03", "objc_serialized_runtime_metadata_import_lowering_contract"),
        SnippetCheck("M258-C001-ART-04", '"O3S265"'),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-C001-IR-01", "M258-C001 serialized metadata import/lowering anchor"),
        SnippetCheck("M258-C001-IR-02", "serialized imported metadata payloads still are not rehydrated"),
    ),
    API_H: (
        SnippetCheck("M258-C001-API-01", "M258-C001 serialized metadata import/lowering anchor"),
        SnippetCheck("M258-C001-API-02", "serialized imported payload handles"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-C001-PKG-01", '"check:objc3c:m258-c001-serialized-metadata-import-and-lowering-contract"'),
        SnippetCheck("M258-C001-PKG-02", '"test:tooling:m258-c001-serialized-metadata-import-and-lowering-contract"'),
        SnippetCheck("M258-C001-PKG-03", '"check:objc3c:m258-c001-lane-c-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-C001-RUN-01", "M258-B002 + M258-C001"),
        SnippetCheck("M258-C001-RUN-02", "check_m258_c001_serialized_metadata_import_and_lowering_contract_and_architecture_freeze.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-C001-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-C001-TEST-02", "def test_checker_passes_dynamic"),
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
        checks_total += require(True, display_path(NATIVE_EXE), "M258-C001-BIN-READY", "native binary present", failures)
        return checks_total
    build = run_process([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-C001-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-C001-NATIVE-EXISTS", "native binary missing after build", failures)
    return checks_total


def compile_fixture(*, fixture: Path, out_dir: Path, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", *extra_args])


def validate_consumer_manifest(manifest: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, object]]:
    checks_total = 0
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    surface = semantic_surface.get("objc_serialized_runtime_metadata_import_lowering_contract") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(surface, dict), "consumer manifest", "M258-C001-SURFACE", "serialized runtime metadata import/lowering surface missing", failures)
    if not isinstance(surface, dict):
        return checks_total, {}

    checks_total += require(surface.get("contract_id") == CONTRACT_ID, "consumer manifest", "M258-C001-CONTRACT", "unexpected contract id", failures)
    checks_total += require(surface.get("source_imported_semantic_rules_contract_id") == SOURCE_CONTRACT_ID, "consumer manifest", "M258-C001-SOURCE-CONTRACT", "unexpected source contract id", failures)
    checks_total += require(surface.get("frontend_surface_path") == SURFACE_PATH, "consumer manifest", "M258-C001-SURFACE-PATH", "unexpected surface path", failures)
    checks_total += require(surface.get("input_model") == INPUT_MODEL, "consumer manifest", "M258-C001-INPUT-MODEL", "unexpected input model", failures)
    checks_total += require(surface.get("ready") is True, "consumer manifest", "M258-C001-READY", "expected ready=true", failures)
    checks_total += require(surface.get("fail_closed") is True, "consumer manifest", "M258-C001-FAIL-CLOSED", "expected fail_closed=true", failures)
    checks_total += require(surface.get("source_imported_semantic_rules_ready") is True, "consumer manifest", "M258-C001-SOURCE-READY", "expected source_imported_semantic_rules_ready=true", failures)
    checks_total += require(surface.get("semantic_surface_published") is True, "consumer manifest", "M258-C001-SURFACE-PUBLISHED", "expected semantic_surface_published=true", failures)
    checks_total += require(surface.get("imported_surface_ingest_landed") is True, "consumer manifest", "M258-C001-INGEST-LANDED", "expected imported_surface_ingest_landed=true", failures)
    checks_total += require(surface.get("imported_input_path_count") == 2, "consumer manifest", "M258-C001-INPUT-COUNT", "expected imported_input_path_count=2", failures)
    checks_total += require(surface.get("imported_module_count") == 2, "consumer manifest", "M258-C001-MODULE-COUNT", "expected imported_module_count=2", failures)

    for key in (
        "serialized_metadata_rehydration_landed",
        "incremental_reuse_landed",
        "imported_metadata_ir_lowering_landed",
        "public_live_imported_payload_abi_landed",
        "ready_for_serialized_metadata_lowering_impl",
        "ready_for_incremental_reuse_impl",
    ):
        checks_total += require(surface.get(key) is False, "consumer manifest", f"M258-C001-{key.upper()}", f"expected {key}=false", failures)

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

            class_completed = compile_fixture(fixture=CLASS_FIXTURE, out_dir=class_out)
            category_completed = compile_fixture(fixture=CATEGORY_FIXTURE, out_dir=category_out)
            class_artifact = class_out / IMPORT_ARTIFACT
            category_artifact = category_out / IMPORT_ARTIFACT
            checks_total += require(class_completed.returncode == 0, display_path(CLASS_FIXTURE), "M258-C001-CLASS-COMPILE", class_completed.stderr or class_completed.stdout or "class producer compile failed", failures)
            checks_total += require(category_completed.returncode == 0, display_path(CATEGORY_FIXTURE), "M258-C001-CATEGORY-COMPILE", category_completed.stderr or category_completed.stdout or "category producer compile failed", failures)
            checks_total += require(class_artifact.exists(), display_path(class_artifact), "M258-C001-CLASS-ARTIFACT", "class import artifact missing", failures)
            checks_total += require(category_artifact.exists(), display_path(category_artifact), "M258-C001-CATEGORY-ARTIFACT", "category import artifact missing", failures)

            if not failures:
                consumer_completed = compile_fixture(
                    fixture=CONSUMER_FIXTURE,
                    out_dir=consumer_out,
                    extra_args=(
                        "--objc3-import-runtime-surface",
                        str(class_artifact),
                        "--objc3-import-runtime-surface",
                        str(category_artifact),
                    ),
                )
                consumer_manifest_path = consumer_out / "module.manifest.json"
                checks_total += require(consumer_completed.returncode == 0, display_path(CONSUMER_FIXTURE), "M258-C001-CONSUMER-COMPILE", consumer_completed.stderr or consumer_completed.stdout or "consumer compile failed", failures)
                checks_total += require(consumer_manifest_path.exists(), display_path(consumer_manifest_path), "M258-C001-CONSUMER-MANIFEST", "consumer manifest missing", failures)
                surface_summary: dict[str, object] = {}
                if consumer_manifest_path.exists():
                    consumer_manifest = load_json(consumer_manifest_path)
                    surface_checks, surface_summary = validate_consumer_manifest(consumer_manifest, failures)
                    checks_total += surface_checks
                dynamic_summary.update({"consumer_surface": surface_summary})

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
