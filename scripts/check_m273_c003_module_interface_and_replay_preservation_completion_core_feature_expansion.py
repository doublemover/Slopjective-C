#!/usr/bin/env python3
"""Checker for M273-C003 module/interface replay preservation completion."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-c003-part10-module-interface-replay-preservation-v1"
CONTRACT_ID = "objc3c-part10-module-interface-replay-preservation/m273-c003-v1"
SURFACE_KEY = "objc_part10_module_interface_and_replay_preservation"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m273" / "M273-C003" / "module_interface_replay_preservation_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "c003"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_module_interface_and_replay_preservation_completion_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion_packet.md"
DOC_ARTIFACTS = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m273_c003_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion.py"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_c003_part10_preservation_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_c003_part10_preservation_consumer.objc3"


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
        SnippetCheck("M273-C003-EXP-01", "# M273 Module, Interface, and Replay Preservation Completion Core Feature Expansion Expectations (C003)"),
        SnippetCheck("M273-C003-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M273-C003-EXP-03", "Issue: `#7355`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M273-C003-PKT-01", "# M273-C003 Packet: Module, Interface, and Replay Preservation Completion - Core Feature Expansion"),
        SnippetCheck("M273-C003-PKT-02", "Issue: `#7355`"),
        SnippetCheck("M273-C003-PKT-03", "Next issue: `M273-D001`"),
    ),
    DOC_ARTIFACTS: (
        SnippetCheck("M273-C003-ARTDOC-01", "## M273 module, interface, and replay preservation"),
        SnippetCheck("M273-C003-ARTDOC-02", SURFACE_KEY),
    ),
    DOC_NATIVE: (
        SnippetCheck("M273-C003-DOC-01", "## M273 module, interface, and replay preservation"),
        SnippetCheck("M273-C003-DOC-02", "part10_module_interface_replay_preservation"),
    ),
    SPEC_ATTR: (
        SnippetCheck("M273-C003-ATTR-01", "## M273 module, interface, and replay preservation completion (C003)"),
        SnippetCheck("M273-C003-ATTR-02", SURFACE_KEY),
    ),
    SPEC_METADATA: (
        SnippetCheck("M273-C003-META-01", "## M273 module/interface replay preservation note"),
        SnippetCheck("M273-C003-META-02", CONTRACT_ID),
    ),
    LOWERING_HEADER: (
        SnippetCheck("M273-C003-LH-01", "kObjc3Part10ModuleInterfaceReplayPreservationContractId"),
        SnippetCheck("M273-C003-LH-02", "Objc3Part10ModuleInterfaceReplayPreservationSummary"),
    ),
    LOWERING_CPP: (
        SnippetCheck("M273-C003-LCPP-01", "Objc3Part10ModuleInterfaceReplayPreservationSummary"),
        SnippetCheck("M273-C003-LCPP-02", ";next_issue=M273-D001"),
    ),
    IR_HEADER: (
        SnippetCheck("M273-C003-IRH-01", "lowering_part10_module_interface_replay_preservation_key"),
        SnippetCheck("M273-C003-IRH-02", "part10_module_replay_imported_module_count"),
    ),
    IR_CPP: (
        SnippetCheck("M273-C003-IRC-01", "part10_module_interface_replay_preservation = "),
        SnippetCheck("M273-C003-IRC-02", "!objc3.objc_part10_module_interface_and_replay_preservation = !{!106}"),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M273-C003-IMPH-01", "part10_module_interface_replay_preservation_present"),
        SnippetCheck("M273-C003-IMPH-02", "part10_local_derive_method_count"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M273-C003-IMPC-01", "PopulateImportedPart10ModuleInterfaceReplayPreservation("),
        SnippetCheck("M273-C003-IMPC-02", "unexpected Part 10 module/interface replay preservation contract id in import surface"),
    ),
    ARTIFACTS_CPP: (
        SnippetCheck("M273-C003-ART-01", "BuildPart10ModuleInterfaceReplayPreservationSummary("),
        SnippetCheck("M273-C003-ART-02", "BuildPart10ModuleInterfaceReplayPreservationSummaryJson("),
        SnippetCheck("M273-C003-ART-03", SURFACE_KEY),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M273-C003-PKG-01", '"check:objc3c:m273-c003-module-interface-and-replay-preservation-completion-core-feature-expansion"'),
        SnippetCheck("M273-C003-PKG-02", '"test:tooling:m273-c003-module-interface-and-replay-preservation-completion-core-feature-expansion"'),
        SnippetCheck("M273-C003-PKG-03", '"check:objc3c:m273-c003-lane-c-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M273-C003-RUN-01", "M273-C001 + M273-C002 + M273-C003"),
        SnippetCheck("M273-C003-RUN-02", "check_m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M273-C003-TEST-01", "def test_checker_passes_static() -> None:"),
        SnippetCheck("M273-C003-TEST-02", "def test_checker_passes_dynamic() -> None:"),
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


def ensure_binaries(failures: list[Finding]) -> tuple[int, int]:
    build = run_process(
        [
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m273-c003-check",
            "--summary-out",
            "tmp/reports/m273/M273-C003/ensure_objc3c_native_build_summary.json",
        ]
    )
    total = 1
    passed = require(build.returncode == 0, display_path(BUILD_HELPER), "M273-C003-BUILD", build.stderr or build.stdout or "native build failed", failures)
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


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


def validate_provider(*, out_dir: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any], dict[str, Any], dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / "module.runtime-import-surface.json"
    ir_path = out_dir / "module.ll"
    for check_id, path, detail in (
        ("M273-C003-PROV-MANIFEST", manifest_path, "provider manifest missing"),
        ("M273-C003-PROV-IMPORT", import_path, "provider import artifact missing"),
        ("M273-C003-PROV-IR", ir_path, "provider IR missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if not manifest_path.exists() or not import_path.exists() or not ir_path.exists():
        return checks_total, checks_passed, {}, {}, {}

    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    surface = semantic_surface(manifest)[SURFACE_KEY]
    import_surface = import_payload[SURFACE_KEY]
    ir_text = ir_path.read_text(encoding="utf-8")

    expected_counts = {
        "local_derive_method_count": 1,
        "local_macro_artifact_count": 1,
        "local_interface_property_behavior_artifact_count": 1,
        "local_implementation_property_behavior_artifact_count": 1,
        "local_runtime_method_list_count": 1,
        "imported_module_count": 0,
        "imported_derive_method_count": 0,
        "imported_macro_artifact_count": 0,
        "imported_interface_property_behavior_artifact_count": 0,
        "imported_implementation_property_behavior_artifact_count": 0,
        "imported_runtime_method_list_count": 0,
    }
    for field, expected in expected_counts.items():
        checks_total += 1
        checks_passed += require(surface.get(field) == expected, display_path(manifest_path), f"M273-C003-PROV-{field}", f"provider field {field} mismatch", failures)
        checks_total += 1
        checks_passed += require(import_surface.get(field) == expected, display_path(import_path), f"M273-C003-PROV-IMPORT-{field}", f"provider import field {field} mismatch", failures)

    for field in ("runtime_import_artifact_ready", "separate_compilation_preservation_ready", "deterministic"):
        checks_total += 1
        checks_passed += require(surface.get(field) is True, display_path(manifest_path), f"M273-C003-PROV-{field}", f"provider field {field} must be true", failures)
        checks_total += 1
        checks_passed += require(import_surface.get(field) is True, display_path(import_path), f"M273-C003-PROV-IMPORT-{field}", f"provider import field {field} must be true", failures)

    checks_total += 1
    checks_passed += require(surface.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M273-C003-PROV-CONTRACT", "provider contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(import_surface.get("contract_id") == CONTRACT_ID, display_path(import_path), "M273-C003-PROV-IMPORT-CONTRACT", "provider import contract id mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("replay_key") == import_surface.get("replay_key"), display_path(import_path), "M273-C003-PROV-REPLAY-MATCH", "provider replay key mismatch between manifest/import", failures)

    for check_id, snippet in (
        ("M273-C003-PROV-IR-01", "; part10_module_interface_replay_preservation = "),
        ("M273-C003-PROV-IR-02", "!objc3.objc_part10_module_interface_and_replay_preservation = !{!106}"),
        ("M273-C003-PROV-IR-03", '!106 = !{!"contract=objc3c-part10-module-interface-replay-preservation/m273-c003-v1'),
        ("M273-C003-PROV-IR-04", ", i64 1, i64 1, i64 1, i64 1, i64 1, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0, i1 1, i1 1, i1 1}"),
    ):
        checks_total += 1
        checks_passed += require(snippet in ir_text, display_path(ir_path), check_id, f"provider IR missing snippet: {snippet}", failures)

    return checks_total, checks_passed, manifest, import_payload, surface


def validate_consumer(*, out_dir: Path, provider_manifest: dict[str, Any], provider_import: dict[str, Any], failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    manifest_path = out_dir / "module.manifest.json"
    import_path = out_dir / "module.runtime-import-surface.json"
    ir_path = out_dir / "module.ll"
    for check_id, path, detail in (
        ("M273-C003-CONS-MANIFEST", manifest_path, "consumer manifest missing"),
        ("M273-C003-CONS-IMPORT", import_path, "consumer import artifact missing"),
        ("M273-C003-CONS-IR", ir_path, "consumer IR missing"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)
    if not manifest_path.exists() or not import_path.exists() or not ir_path.exists():
        return checks_total, checks_passed, {}

    manifest = load_json(manifest_path)
    import_payload = load_json(import_path)
    surface = semantic_surface(manifest)[SURFACE_KEY]
    import_surface = import_payload[SURFACE_KEY]
    provider_module = provider_manifest.get("module")
    provider_surface = semantic_surface(provider_manifest)[SURFACE_KEY]
    provider_import_surface = provider_import[SURFACE_KEY]
    ir_text = ir_path.read_text(encoding="utf-8")

    local_zero_fields = (
        "local_derive_method_count",
        "local_macro_artifact_count",
        "local_interface_property_behavior_artifact_count",
        "local_implementation_property_behavior_artifact_count",
        "local_runtime_method_list_count",
    )
    imported_one_fields = (
        "imported_module_count",
        "imported_derive_method_count",
        "imported_macro_artifact_count",
        "imported_interface_property_behavior_artifact_count",
        "imported_implementation_property_behavior_artifact_count",
        "imported_runtime_method_list_count",
    )
    for field in local_zero_fields:
        checks_total += 1
        checks_passed += require(surface.get(field) == 0, display_path(manifest_path), f"M273-C003-CONS-{field}", f"consumer field {field} should be zero", failures)
        checks_total += 1
        checks_passed += require(import_surface.get(field) == 0, display_path(import_path), f"M273-C003-CONS-IMPORT-{field}", f"consumer import field {field} should be zero", failures)
    for field in imported_one_fields:
        checks_total += 1
        checks_passed += require(surface.get(field) == 1, display_path(manifest_path), f"M273-C003-CONS-{field}", f"consumer field {field} should be one", failures)
        checks_total += 1
        checks_passed += require(import_surface.get(field) == 1, display_path(import_path), f"M273-C003-CONS-IMPORT-{field}", f"consumer import field {field} should be one", failures)

    for field in ("runtime_import_artifact_ready", "separate_compilation_preservation_ready", "deterministic"):
        checks_total += 1
        checks_passed += require(surface.get(field) is True, display_path(manifest_path), f"M273-C003-CONS-{field}", f"consumer field {field} must be true", failures)
        checks_total += 1
        checks_passed += require(import_surface.get(field) is True, display_path(import_path), f"M273-C003-CONS-IMPORT-{field}", f"consumer import field {field} must be true", failures)

    checks_total += 1
    checks_passed += require(provider_module in surface.get("imported_module_names_lexicographic", []), display_path(manifest_path), "M273-C003-CONS-IMPORTED-MODULE", "consumer manifest missing imported provider module name", failures)
    checks_total += 1
    checks_passed += require(provider_module in import_surface.get("imported_module_names_lexicographic", []), display_path(import_path), "M273-C003-CONS-IMPORT-IMPORTED-MODULE", "consumer import missing imported provider module name", failures)
    checks_total += 1
    checks_passed += require(provider_surface.get("local_derive_method_count") == surface.get("imported_derive_method_count"), display_path(manifest_path), "M273-C003-CONS-DERIVE-HANDOFF", "consumer imported derive count mismatch", failures)
    checks_total += 1
    checks_passed += require(provider_surface.get("local_macro_artifact_count") == surface.get("imported_macro_artifact_count"), display_path(manifest_path), "M273-C003-CONS-MACRO-HANDOFF", "consumer imported macro count mismatch", failures)
    checks_total += 1
    checks_passed += require(provider_surface.get("local_interface_property_behavior_artifact_count") == surface.get("imported_interface_property_behavior_artifact_count"), display_path(manifest_path), "M273-C003-CONS-IFACE-HANDOFF", "consumer imported interface property behavior count mismatch", failures)
    checks_total += 1
    checks_passed += require(provider_surface.get("local_implementation_property_behavior_artifact_count") == surface.get("imported_implementation_property_behavior_artifact_count"), display_path(manifest_path), "M273-C003-CONS-IMPL-HANDOFF", "consumer imported implementation property behavior count mismatch", failures)
    checks_total += 1
    checks_passed += require(provider_surface.get("local_runtime_method_list_count") == surface.get("imported_runtime_method_list_count"), display_path(manifest_path), "M273-C003-CONS-RUNTIME-LIST-HANDOFF", "consumer imported runtime method list count mismatch", failures)
    checks_total += 1
    checks_passed += require(provider_import_surface.get("replay_key") is not None, display_path(import_path), "M273-C003-CONS-PROVIDER-IMPORT-REPLAY", "provider import replay key missing", failures)

    for check_id, snippet in (
        ("M273-C003-CONS-IR-01", "; part10_module_interface_replay_preservation = "),
        ("M273-C003-CONS-IR-02", "!objc3.objc_part10_module_interface_and_replay_preservation = !{!106}"),
        ("M273-C003-CONS-IR-03", '!106 = !{!"contract=objc3c-part10-module-interface-replay-preservation/m273-c003-v1'),
        ("M273-C003-CONS-IR-04", ", i64 0, i64 0, i64 0, i64 0, i64 0, i64 1, i64 1, i64 1, i64 1, i64 1, i64 1, i1 1, i1 1, i1 1}"),
    ):
        checks_total += 1
        checks_passed += require(snippet in ir_text, display_path(ir_path), check_id, f"consumer IR missing snippet: {snippet}", failures)

    return checks_total, checks_passed, surface


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_total, path_passed, path_failures = check_static_contract(path, snippets)
        checks_total += path_total
        checks_passed += path_passed
        failures.extend(path_failures)
        static_summary[display_path(path)] = {"checks": path_total, "passed": path_passed, "ok": not path_failures}

    dynamic_summary: dict[str, object] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        build_total, build_passed = ensure_binaries(failures)
        checks_total += build_total
        checks_passed += build_passed
        if build_passed == build_total:
            provider_out = PROBE_ROOT / "provider"
            consumer_out = PROBE_ROOT / "consumer"
            provider_completed = compile_fixture(
                fixture=PROVIDER_FIXTURE,
                out_dir=provider_out,
                registration_order_ordinal=1,
            )
            checks_total += 1
            checks_passed += require(provider_completed.returncode == 0, display_path(PROVIDER_FIXTURE), "M273-C003-PROV-COMPILE", provider_completed.stderr or provider_completed.stdout or "provider compile failed", failures)
            if provider_completed.returncode == 0:
                provider_total, provider_passed, provider_manifest, provider_import, provider_surface = validate_provider(out_dir=provider_out, failures=failures)
                checks_total += provider_total
                checks_passed += provider_passed
                if provider_manifest:
                    consumer_completed = compile_fixture(
                        fixture=CONSUMER_FIXTURE,
                        out_dir=consumer_out,
                        registration_order_ordinal=2,
                        extra_args=("--objc3-import-runtime-surface", str(provider_out / "module.runtime-import-surface.json")),
                    )
                    checks_total += 1
                    checks_passed += require(consumer_completed.returncode == 0, display_path(CONSUMER_FIXTURE), "M273-C003-CONS-COMPILE", consumer_completed.stderr or consumer_completed.stdout or "consumer compile failed", failures)
                    if consumer_completed.returncode == 0:
                        consumer_total, consumer_passed, consumer_surface = validate_consumer(
                            out_dir=consumer_out,
                            provider_manifest=provider_manifest,
                            provider_import=provider_import,
                            failures=failures,
                        )
                        checks_total += consumer_total
                        checks_passed += consumer_passed
                        dynamic_summary.update(
                            {
                                "provider": {
                                    "module": provider_manifest.get("module"),
                                    "surface": provider_surface,
                                },
                                "consumer": {
                                    "module": "Part10PreservationConsumer",
                                    "surface": consumer_surface,
                                },
                            }
                        )

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    return payload, failures


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    payload, failures = build_summary(skip_dynamic_probes=args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        json.dump(payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
