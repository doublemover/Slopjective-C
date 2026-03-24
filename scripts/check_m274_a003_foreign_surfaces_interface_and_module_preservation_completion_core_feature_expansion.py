#!/usr/bin/env python3
"""Checker for M274-A003 Part 11 foreign surface preservation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-a003-part11-foreign-surface-interface-preservation-v1"
CONTRACT_ID = "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"
SURFACE_KEY = "objc_part11_foreign_surface_interface_and_module_preservation"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m274" / "M274-A003" / "foreign_surfaces_interface_module_preservation_summary.json"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_foreign_surfaces_interface_and_module_preservation_completion_a003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion_packet.md"
DOC_ARTIFACTS = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
TOKEN_CONTRACT = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_a003_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion.py"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_a003_foreign_surfaces_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_a003_foreign_surfaces_consumer.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "a003"


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
        SnippetCheck("M274-A003-EXP-01", "# M274 Foreign Surfaces Interface And Module Preservation Completion Expectations (A003)"),
        SnippetCheck("M274-A003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-A003-EXP-03", "Issue: `#7362`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-A003-PKT-01", "# M274-A003 Packet: Part 11 Foreign Surfaces Interface And Module Preservation - Core Feature Expansion"),
        SnippetCheck("M274-A003-PKT-02", f"semantic surface `frontend.pipeline.semantic_surface.{SURFACE_KEY}`"),
        SnippetCheck("M274-A003-PKT-03", "Dependencies: `M274-A001`, `M274-A002`"),
        SnippetCheck("M274-A003-PKT-04", "Next issue: `M274-B001`"),
    ),
    DOC_ARTIFACTS: (
        SnippetCheck("M274-A003-ARTDOC-01", "## M274 foreign surfaces interface and module preservation"),
        SnippetCheck("M274-A003-ARTDOC-02", SURFACE_KEY),
    ),
    DOC_NATIVE: (
        SnippetCheck("M274-A003-DOC-01", "## M274 foreign surfaces interface and module preservation"),
        SnippetCheck("M274-A003-DOC-02", SURFACE_KEY),
    ),
    SPEC_ATTR: (
        SnippetCheck("M274-A003-ATTR-01", "## M274 foreign surfaces interface and module preservation completion (A003)"),
        SnippetCheck("M274-A003-ATTR-02", SURFACE_KEY),
    ),
    SPEC_METADATA: (
        SnippetCheck("M274-A003-META-01", "## M274 foreign surface preservation note"),
        SnippetCheck("M274-A003-META-02", CONTRACT_ID),
    ),
    TOKEN_CONTRACT: (
        SnippetCheck("M274-A003-TOK-01", "kObjc3Part11ForeignSurfaceInterfacePreservationContractId"),
        SnippetCheck("M274-A003-TOK-02", "m274-a003-v1"),
    ),
    FRONTEND_TYPES: (
        SnippetCheck("M274-A003-TYP-01", "kObjc3Part11ForeignSurfaceInterfacePreservationSurfacePath"),
        SnippetCheck("M274-A003-TYP-02", "Objc3Part11ForeignSurfaceInterfacePreservationSummary"),
    ),
    ARTIFACTS_CPP: (
        SnippetCheck("M274-A003-ART-01", "BuildPart11ForeignSurfaceInterfacePreservationSummary("),
        SnippetCheck("M274-A003-ART-02", "BuildPart11ForeignSurfaceInterfacePreservationSummaryJson("),
        SnippetCheck("M274-A003-ART-03", SURFACE_KEY),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M274-A003-IMPH-01", "part11_foreign_surface_interface_preservation_present"),
        SnippetCheck("M274-A003-IMPH-02", "part11_local_foreign_callable_count"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M274-A003-IMPC-01", "PopulateImportedPart11ForeignSurfaceInterfacePreservation("),
        SnippetCheck("M274-A003-IMPC-02", "unexpected Part 11 foreign surface interface preservation contract id in import surface"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M274-A003-ARCH-01", "## M274 foreign surfaces interface and module preservation completion (A003)"),
        SnippetCheck("M274-A003-ARCH-02", SURFACE_KEY),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-A003-PKG-01", '"check:objc3c:m274-a003-foreign-surfaces-interface-and-module-preservation-completion-core-feature-expansion"'),
        SnippetCheck("M274-A003-PKG-02", '"test:tooling:m274-a003-foreign-surfaces-interface-and-module-preservation-completion-core-feature-expansion"'),
        SnippetCheck("M274-A003-PKG-03", '"check:objc3c:m274-a003-lane-a-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-A003-RUN-01", "build_objc3c_native_docs.py"),
        SnippetCheck("M274-A003-RUN-02", "check_m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M274-A003-TEST-01", "def test_m274_a003_checker_passes_static() -> None:"),
        SnippetCheck("M274-A003-TEST-02", "def test_m274_a003_checker_emits_summary() -> None:"),
    ),
    PROVIDER_FIXTURE: (
        SnippetCheck("M274-A003-PROV-01", "module Part11ForeignSurfacesProvider;"),
        SnippetCheck("M274-A003-PROV-02", "objc_import_module(named(\"SampleKit\"))"),
        SnippetCheck("M274-A003-PROV-03", "objc_swift_name(named(\"SwiftBridge\"))"),
        SnippetCheck("M274-A003-PROV-04", "objc_header_name(named(\"HeaderBridge\"))"),
    ),
    CONSUMER_FIXTURE: (
        SnippetCheck("M274-A003-CONS-01", "module Part11ForeignSurfacesConsumer;"),
        SnippetCheck("M274-A003-CONS-02", "fn main() -> i32 {"),
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
    return 0


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, int, list[Finding]]:
    failures: list[Finding] = []
    checks_total = len(snippets) + 1
    checks_passed = 0
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, checks_passed, failures
    checks_passed += 1
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet in text:
            checks_passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, checks_passed, failures


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


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


def ensure_binaries(failures: list[Finding]) -> tuple[int, int]:
    build = run_process(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m274-a003-check",
            "--summary-out",
            "tmp/reports/m274/M274-A003/ensure_objc3c_native_build_summary.json",
        ]
    )
    total = 1
    passed = require(build.returncode == 0, display_path(BUILD_HELPER), "M274-A003-BUILD", build.stderr or build.stdout or "native build failed", failures)
    return total, passed


def compile_fixture(*, fixture: Path, out_dir: Path, registration_order_ordinal: int, extra_args: Sequence[str] = ()) -> subprocess.CompletedProcess[str]:
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


def nested_part11_import_surface(payload: dict[str, Any]) -> dict[str, Any]:
    value = payload.get(SURFACE_KEY)
    if not isinstance(value, dict):
        raise TypeError(f"import payload missing {SURFACE_KEY}")
    return value


def validate_provider(*, out_dir: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any], dict[str, Any], dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / "module.runtime-import-surface.json"
    for check_id, path, detail in (
        ("M274-A003-PROV-MANIFEST", manifest_path, "provider manifest missing"),
        ("M274-A003-PROV-IMPORT", import_path, "provider import artifact missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if not manifest_path.exists() or not import_path.exists():
        return checks_total, checks_passed, {}, {}, {}

    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    surface = semantic_surface(manifest)[SURFACE_KEY]
    import_surface = nested_part11_import_surface(import_payload)

    expected_counts = {
        "local_foreign_callable_count": 2,
        "local_import_module_annotation_count": 1,
        "local_imported_module_name_count": 1,
        "local_swift_name_annotation_count": 1,
        "local_swift_private_annotation_count": 1,
        "local_cpp_name_annotation_count": 1,
        "local_header_name_annotation_count": 1,
        "local_named_annotation_payload_count": 3,
        "imported_module_count": 0,
        "imported_foreign_callable_count": 0,
        "imported_import_module_annotation_count": 0,
        "imported_imported_module_name_count": 0,
        "imported_swift_name_annotation_count": 0,
        "imported_swift_private_annotation_count": 0,
        "imported_cpp_name_annotation_count": 0,
        "imported_header_name_annotation_count": 0,
        "imported_named_annotation_payload_count": 0,
    }
    for field, expected in expected_counts.items():
        checks_total += 2
        checks_passed += require(surface.get(field) == expected, display_path(manifest_path), f"M274-A003-PROV-{field}-MANIFEST", f"provider manifest field {field} mismatch", failures)
        checks_passed += require(import_surface.get(field) == expected, display_path(import_path), f"M274-A003-PROV-{field}-IMPORT", f"provider import field {field} mismatch", failures)

    for field in ("runtime_import_artifact_ready", "separate_compilation_preservation_ready", "deterministic"):
        checks_total += 2
        checks_passed += require(surface.get(field) is True, display_path(manifest_path), f"M274-A003-PROV-{field}-MANIFEST", f"provider manifest field {field} must be true", failures)
        checks_passed += require(import_surface.get(field) is True, display_path(import_path), f"M274-A003-PROV-{field}-IMPORT", f"provider import field {field} must be true", failures)

    checks_total += 4
    checks_passed += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M274-A003-PROV-CONTRACT-MANIFEST", "provider manifest contract id mismatch", failures)
    checks_passed += require(import_surface.get("contract_id") == CONTRACT_ID, display_path(import_path), "M274-A003-PROV-CONTRACT-IMPORT", "provider import contract id mismatch", failures)
    checks_passed += require(surface.get("local_import_module_names_lexicographic") == ['\"SampleKit\"'], display_path(manifest_path), "M274-A003-PROV-NAMES-MANIFEST", "provider manifest local import module names mismatch", failures)
    checks_passed += require(import_surface.get("local_import_module_names_lexicographic") == ['\"SampleKit\"'], display_path(import_path), "M274-A003-PROV-NAMES-IMPORT", "provider import local import module names mismatch", failures)

    checks_total += 2
    checks_passed += require(surface.get("imported_provider_module_names_lexicographic") == [], display_path(manifest_path), "M274-A003-PROV-IMPORTED-NAMES-MANIFEST", "provider manifest imported provider names must be empty", failures)
    checks_passed += require(import_surface.get("imported_provider_module_names_lexicographic") == [], display_path(import_path), "M274-A003-PROV-IMPORTED-NAMES-IMPORT", "provider import imported provider names must be empty", failures)

    return checks_total, checks_passed, manifest, import_payload, surface


def validate_consumer(*, out_dir: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any], dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / "module.runtime-import-surface.json"
    for check_id, path, detail in (
        ("M274-A003-CONS-MANIFEST", manifest_path, "consumer manifest missing"),
        ("M274-A003-CONS-IMPORT", import_path, "consumer import artifact missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if not manifest_path.exists() or not import_path.exists():
        return checks_total, checks_passed, {}, {}

    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    surface = semantic_surface(manifest)[SURFACE_KEY]
    import_surface = nested_part11_import_surface(import_payload)

    expected_local_zero = {
        "local_foreign_callable_count": 0,
        "local_import_module_annotation_count": 0,
        "local_imported_module_name_count": 0,
        "local_swift_name_annotation_count": 0,
        "local_swift_private_annotation_count": 0,
        "local_cpp_name_annotation_count": 0,
        "local_header_name_annotation_count": 0,
        "local_named_annotation_payload_count": 0,
    }
    expected_imported = {
        "imported_module_count": 1,
        "imported_foreign_callable_count": 2,
        "imported_import_module_annotation_count": 1,
        "imported_imported_module_name_count": 1,
        "imported_swift_name_annotation_count": 1,
        "imported_swift_private_annotation_count": 1,
        "imported_cpp_name_annotation_count": 1,
        "imported_header_name_annotation_count": 1,
        "imported_named_annotation_payload_count": 3,
    }
    for field, expected in {**expected_local_zero, **expected_imported}.items():
        checks_total += 2
        checks_passed += require(surface.get(field) == expected, display_path(manifest_path), f"M274-A003-CONS-{field}-MANIFEST", f"consumer manifest field {field} mismatch", failures)
        checks_passed += require(import_surface.get(field) == expected, display_path(import_path), f"M274-A003-CONS-{field}-IMPORT", f"consumer import field {field} mismatch", failures)

    for field in ("runtime_import_artifact_ready", "separate_compilation_preservation_ready", "deterministic"):
        checks_total += 2
        checks_passed += require(surface.get(field) is True, display_path(manifest_path), f"M274-A003-CONS-{field}-MANIFEST", f"consumer manifest field {field} must be true", failures)
        checks_passed += require(import_surface.get(field) is True, display_path(import_path), f"M274-A003-CONS-{field}-IMPORT", f"consumer import field {field} must be true", failures)

    checks_total += 2
    checks_passed += require(surface.get("imported_provider_module_names_lexicographic") == ["Part11ForeignSurfacesProvider"], display_path(manifest_path), "M274-A003-CONS-IMPORTED-NAMES-MANIFEST", "consumer manifest imported provider names mismatch", failures)
    checks_passed += require(import_surface.get("imported_provider_module_names_lexicographic") == ["Part11ForeignSurfacesProvider"], display_path(import_path), "M274-A003-CONS-IMPORTED-NAMES-IMPORT", "consumer import imported provider names mismatch", failures)

    return checks_total, checks_passed, manifest, import_surface


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    provider_out = PROBE_ROOT / "provider"
    consumer_out = PROBE_ROOT / "consumer"

    provider = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, registration_order_ordinal=1)
    checks_total += 1
    checks_passed += require(provider.returncode == 0, display_path(PROVIDER_FIXTURE), "M274-A003-PROV-COMPILE", provider.stderr or provider.stdout or "provider compile failed", failures)
    if provider.returncode != 0:
        return checks_total, checks_passed, {"provider_returncode": provider.returncode}

    provider_total, provider_passed, provider_manifest, _provider_import_payload, provider_surface = validate_provider(out_dir=provider_out, failures=failures)
    checks_total += provider_total
    checks_passed += provider_passed

    consumer = compile_fixture(
        fixture=CONSUMER_FIXTURE,
        out_dir=consumer_out,
        registration_order_ordinal=2,
        extra_args=("--objc3-import-runtime-surface", str(provider_out / "module.runtime-import-surface.json")),
    )
    checks_total += 1
    checks_passed += require(consumer.returncode == 0, display_path(CONSUMER_FIXTURE), "M274-A003-CONS-COMPILE", consumer.stderr or consumer.stdout or "consumer compile failed", failures)
    if consumer.returncode != 0:
        return checks_total, checks_passed, {"provider_surface": provider_surface, "consumer_returncode": consumer.returncode}

    consumer_total, consumer_passed, consumer_manifest, consumer_import_surface = validate_consumer(out_dir=consumer_out, failures=failures)
    checks_total += consumer_total
    checks_passed += consumer_passed

    return checks_total, checks_passed, {
        "provider_module": provider_manifest.get("module"),
        "consumer_module": consumer_manifest.get("module"),
        "provider_surface": provider_surface,
        "consumer_import_surface": consumer_import_surface,
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    args = parser.parse_args(argv)

    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_contract: dict[str, Any] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        total, passed, local_failures = check_static_contract(path, snippets)
        checks_total += total
        checks_passed += passed
        failures.extend(local_failures)
        static_contract[display_path(path)] = {
            "checks_total": total,
            "checks_passed": passed,
        }

    dynamic_summary: dict[str, Any]
    if args.skip_dynamic_probes:
        dynamic_summary = {"skipped": True}
    else:
        build_total, build_passed = ensure_binaries(failures)
        checks_total += build_total
        checks_passed += build_passed
        dynamic_total, dynamic_passed, dynamic_summary = run_dynamic_checks(failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed
        dynamic_summary["skipped"] = False

    ok = not failures and checks_total == checks_passed
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "static_contract": static_contract,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "dynamic_summary": dynamic_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if ok:
        print(f"[ok] M274-A003 checker passed ({checks_passed}/{checks_total} checks)")
        print(f"[ok] summary: {display_path(args.summary_out)}")
        return 0

    print(f"[fail] M274-A003 checker failed ({checks_passed}/{checks_total} checks)", file=sys.stderr)
    print(f"[fail] summary: {display_path(args.summary_out)}", file=sys.stderr)
    for finding in failures:
        print(f"[fail] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
