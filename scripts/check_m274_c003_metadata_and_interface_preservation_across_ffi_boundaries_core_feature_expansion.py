#!/usr/bin/env python3
"""Checker for M274-C003 metadata and interface preservation across FFI boundaries."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-c003-part11-ffi-metadata-interface-preservation-v1"
CONTRACT_ID = "objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part11_ffi_metadata_and_interface_preservation"
IMPORT_MEMBER = "objc_part11_ffi_metadata_and_interface_preservation"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-C003" / "ffi_metadata_interface_preservation_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "c003"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_metadata_and_interface_preservation_across_ffi_boundaries_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_c003_metadata_and_interface_preservation_across_ffi_boundaries_core_feature_expansion_packet.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_c003_ffi_metadata_interface_preservation_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_c003_ffi_metadata_interface_preservation_consumer.objc3"
IMPORT_ARTIFACT_NAME = "module.runtime-import-surface.json"


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
        SnippetCheck("M274-C003-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-C003-EXP-02", SURFACE_PATH),
        SnippetCheck("M274-C003-EXP-03", f"`{IMPORT_MEMBER}`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-C003-PKT-01", "# M274-C003 Packet: Metadata And Interface Preservation Across FFI Boundaries - Core Feature Expansion"),
        SnippetCheck("M274-C003-PKT-02", "Dependencies: `M274-A003`"),
        SnippetCheck("M274-C003-PKT-03", "`M274-C002`"),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M274-C003-LH-01", CONTRACT_ID),
        SnippetCheck("M274-C003-LH-02", IMPORT_MEMBER),
    ),
    LOWERING_CPP: (
        SnippetCheck("M274-C003-LCPP-01", "Objc3Part11FfiMetadataInterfacePreservationReplayKey"),
    ),
    FRONTEND_ARTIFACTS_CPP: (
        SnippetCheck("M274-C003-FAC-01", "BuildPart11FfiMetadataInterfacePreservationContract("),
        SnippetCheck("M274-C003-FAC-02", IMPORT_MEMBER),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M274-C003-IMPH-01", "part11_ffi_metadata_interface_preservation_present"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M274-C003-IMPC-01", "PopulateImportedPart11FfiMetadataInterfacePreservation("),
        SnippetCheck("M274-C003-IMPC-02", "unexpected Part 11 ffi metadata/interface preservation contract id in import surface"),
    ),
    IR_HEADER: (
        SnippetCheck("M274-C003-IRH-01", "lowering_part11_ffi_metadata_interface_preservation_key"),
    ),
    IR_CPP: (
        SnippetCheck("M274-C003-IRC-01", "; part11_ffi_metadata_interface_preservation = "),
        SnippetCheck("M274-C003-IRC-02", "!objc3.objc_part11_ffi_metadata_and_interface_preservation = !{!110}"),
    ),
    PROVIDER_FIXTURE: (
        SnippetCheck("M274-C003-FIXP-01", "module m274_c003_ffi_metadata_interface_preservation_provider;"),
        SnippetCheck("M274-C003-FIXP-02", "objc_foreign"),
        SnippetCheck("M274-C003-FIXP-03", "objc_import_module(named(\"ForeignKit\"))"),
    ),
    CONSUMER_FIXTURE: (
        SnippetCheck("M274-C003-FIXC-01", "module m274_c003_ffi_metadata_interface_preservation_consumer;"),
        SnippetCheck("M274-C003-FIXC-02", "objc_import_module(named(\"ConsumerKit\"))"),
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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, failures
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def run_process(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def ensure_binaries(failures: list[Finding]) -> int:
    build = run_process([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m274-c003-check",
        "--summary-out",
        "tmp/reports/m274/M274-C003/ensure_objc3c_native_build_summary.json",
    ])
    return require(build.returncode == 0, display_path(BUILD_HELPER), "M274-C003-BUILD", build.stderr or build.stdout or "native build failed", failures)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


def compile_fixture(*, fixture: Path, out_dir: Path, registration_order_ordinal: int, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_process([
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir", str(out_dir),
        "--emit-prefix", "module",
        "--objc3-bootstrap-registration-order-ordinal", str(registration_order_ordinal),
        *extra_args,
    ])


def validate_provider(out_dir: Path, failures: list[Finding]) -> tuple[int, dict[str, Any], dict[str, Any], dict[str, Any]]:
    checks_total = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / IMPORT_ARTIFACT_NAME
    ir_path = out_dir / "module.ll"
    for check_id, path in (("PROV-MANIFEST", manifest_path), ("PROV-IMPORT", import_path), ("PROV-IR", ir_path)):
        checks_total += require(path.exists(), display_path(path), f"M274-C003-{check_id}", f"missing {display_path(path)}", failures)
    if failures:
        return checks_total, {}, {}, {}
    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    surface = semantic_surface(manifest).get("objc_part11_ffi_metadata_and_interface_preservation")
    checks_total += require(isinstance(surface, dict), display_path(manifest_path), "M274-C003-PROV-SURFACE", "provider surface missing", failures)
    nested = import_payload.get(IMPORT_MEMBER)
    checks_total += require(isinstance(nested, dict), display_path(import_path), "M274-C003-PROV-NESTED", "provider import surface missing nested part11 packet", failures)
    if isinstance(surface, dict):
        checks_total += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M274-C003-PROV-CONTRACT", "provider contract mismatch", failures)
        checks_total += require(surface.get("runtime_import_artifact_ready") is True, display_path(manifest_path), "M274-C003-PROV-IMPORT-READY", "provider import readiness must be true", failures)
        checks_total += require(surface.get("separate_compilation_preservation_ready") is True, display_path(manifest_path), "M274-C003-PROV-SEPARATE", "provider separate-compilation readiness must be true", failures)
        checks_total += require(surface.get("deterministic") is True, display_path(manifest_path), "M274-C003-PROV-DETERMINISTIC", "provider must be deterministic", failures)
    if isinstance(surface, dict) and isinstance(nested, dict):
        checks_total += require(nested.get("replay_key") == surface.get("replay_key"), display_path(import_path), "M274-C003-PROV-REPLAY", "provider nested replay key mismatch", failures)
    ir_text = ir_path.read_text(encoding="utf-8")
    checks_total += require("; part11_ffi_metadata_interface_preservation = " in ir_text, display_path(ir_path), "M274-C003-PROV-IR-COMMENT", "provider IR comment missing", failures)
    checks_total += require("!objc3.objc_part11_ffi_metadata_and_interface_preservation = !{!110}" in ir_text, display_path(ir_path), "M274-C003-PROV-IR-META", "provider IR metadata missing", failures)
    return checks_total, manifest, import_payload, surface if isinstance(surface, dict) else {}


def validate_consumer(out_dir: Path, provider_surface: dict[str, Any], failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / IMPORT_ARTIFACT_NAME
    ir_path = out_dir / "module.ll"
    for check_id, path in (("CONS-MANIFEST", manifest_path), ("CONS-IMPORT", import_path), ("CONS-IR", ir_path)):
        checks_total += require(path.exists(), display_path(path), f"M274-C003-{check_id}", f"missing {display_path(path)}", failures)
    if failures:
        return checks_total, {}
    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    surface = semantic_surface(manifest).get("objc_part11_ffi_metadata_and_interface_preservation")
    nested = import_payload.get(IMPORT_MEMBER)
    checks_total += require(isinstance(surface, dict), display_path(manifest_path), "M274-C003-CONS-SURFACE", "consumer surface missing", failures)
    checks_total += require(isinstance(nested, dict), display_path(import_path), "M274-C003-CONS-NESTED", "consumer import surface missing nested part11 packet", failures)
    if isinstance(surface, dict):
        checks_total += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M274-C003-CONS-CONTRACT", "consumer contract mismatch", failures)
        checks_total += require(surface.get("imported_module_count") == 1, display_path(manifest_path), "M274-C003-CONS-IMPORTED-MODULES", "consumer imported module count mismatch", failures)
        checks_total += require(surface.get("imported_foreign_callable_count") == provider_surface.get("local_foreign_callable_count"), display_path(manifest_path), "M274-C003-CONS-IMPORTED-FOREIGN", "consumer imported foreign callable count mismatch", failures)
        checks_total += require(surface.get("imported_metadata_preservation_sites") == provider_surface.get("local_metadata_preservation_sites"), display_path(manifest_path), "M274-C003-CONS-IMPORTED-META", "consumer imported metadata count mismatch", failures)
        checks_total += require(surface.get("imported_interface_annotation_sites") == provider_surface.get("local_interface_annotation_sites"), display_path(manifest_path), "M274-C003-CONS-IMPORTED-ANNOT", "consumer imported interface annotation count mismatch", failures)
        checks_total += require(surface.get("runtime_import_artifact_ready") is True, display_path(manifest_path), "M274-C003-CONS-IMPORT-READY", "consumer import readiness must be true", failures)
        checks_total += require(surface.get("separate_compilation_preservation_ready") is True, display_path(manifest_path), "M274-C003-CONS-SEPARATE", "consumer separate-compilation readiness must be true", failures)
        checks_total += require(surface.get("deterministic") is True, display_path(manifest_path), "M274-C003-CONS-DETERMINISTIC", "consumer must be deterministic", failures)
    if isinstance(surface, dict) and isinstance(nested, dict):
        checks_total += require(nested.get("replay_key") == surface.get("replay_key"), display_path(import_path), "M274-C003-CONS-REPLAY", "consumer nested replay key mismatch", failures)
    ir_text = ir_path.read_text(encoding="utf-8")
    checks_total += require("; part11_ffi_metadata_interface_preservation = " in ir_text, display_path(ir_path), "M274-C003-CONS-IR-COMMENT", "consumer IR comment missing", failures)
    checks_total += require("!objc3.objc_part11_ffi_metadata_and_interface_preservation = !{!110}" in ir_text, display_path(ir_path), "M274-C003-CONS-IR-META", "consumer IR metadata missing", failures)
    return checks_total, surface if isinstance(surface, dict) else {}


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
            provider_out = PROBE_ROOT / "provider"
            consumer_out = PROBE_ROOT / "consumer"
            provider_completed = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, registration_order_ordinal=1)
            checks_total += require(provider_completed.returncode == 0, display_path(PROVIDER_FIXTURE), "M274-C003-PROV-COMPILE", provider_completed.stderr or provider_completed.stdout or "provider compile failed", failures)
            if not failures:
                provider_checks, provider_manifest, provider_import, provider_surface = validate_provider(provider_out, failures)
                checks_total += provider_checks
                if not failures:
                    provider_import_path = provider_out / IMPORT_ARTIFACT_NAME
                    consumer_completed = compile_fixture(
                        fixture=CONSUMER_FIXTURE,
                        out_dir=consumer_out,
                        registration_order_ordinal=2,
                        extra_args=("--objc3-import-runtime-surface", str(provider_import_path)),
                    )
                    checks_total += require(consumer_completed.returncode == 0, display_path(CONSUMER_FIXTURE), "M274-C003-CONS-COMPILE", consumer_completed.stderr or consumer_completed.stdout or "consumer compile failed", failures)
                    if not failures:
                        consumer_checks, consumer_surface = validate_consumer(consumer_out, provider_surface, failures)
                        checks_total += consumer_checks
                        dynamic_summary = {
                            "provider": {
                                "module": provider_manifest.get("module"),
                                "surface": provider_surface,
                                "import_module": provider_import.get("module_name"),
                            },
                            "consumer": {"surface": consumer_surface},
                        }

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_path": SURFACE_PATH,
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
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
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
