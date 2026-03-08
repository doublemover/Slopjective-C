#!/usr/bin/env python3
"""Fail-closed contract checker for M252-B004 property and ivar export legality."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m252-b004-property-ivar-export-legality-synthesis-preconditions-v1"
CONTRACT_ID = "objc3c-property-ivar-export-legality-synthesis-preconditions/m252-b004-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m252_property_ivar_export_legality_synthesis_preconditions_b004_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m252" / "m252_b004_property_ivar_export_legality_synthesis_preconditions_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
DEFAULT_SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_CLASS_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_class_property_synthesis_ready.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_category_property_export_only.objc3"
DEFAULT_MISSING_INTERFACE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_missing_interface_property.objc3"
DEFAULT_INCOMPATIBLE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m252_b004_incompatible_property_signature.objc3"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m252" / "b004-property-ivar-export-legality"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json")
LANE_CONTRACT = "m154-property-synthesis-ivar-binding-v1"


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
    SnippetCheck("M252-B004-DOC-EXP-01", "# M252 Property Ivar Export Legality Synthesis Preconditions Expectations (B004)"),
    SnippetCheck("M252-B004-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M252-B004-DOC-EXP-03", "lowering_property_synthesis_ivar_binding_replay_key"),
    SnippetCheck("M252-B004-DOC-EXP-04", "`tests/tooling/fixtures/native/m252_b004_class_property_synthesis_ready.objc3`"),
    SnippetCheck("M252-B004-DOC-EXP-05", "`O3S206`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M252-B004-DOC-PKT-01", "# M252-B004 Property Ivar Export Legality Synthesis Preconditions Packet"),
    SnippetCheck("M252-B004-DOC-PKT-02", "Packet: `M252-B004`"),
    SnippetCheck("M252-B004-DOC-PKT-03", "- `M252-B003`"),
    SnippetCheck("M252-B004-DOC-PKT-04", "Category-only property export keeps the property-synthesis/ivar binding counts"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M252-B004-ARCH-01", "M252 lane-B B004 property and ivar export legality"),
    SnippetCheck("M252-B004-ARCH-02", "manifest counters and replay keys stay derived"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M252-B004-NDOC-01", "## Property ivar export legality and synthesis preconditions (M252-B004)"),
    SnippetCheck("M252-B004-NDOC-02", "`frontend.pipeline.sema_pass_manager.lowering_property_synthesis_ivar_binding_replay_key`"),
    SnippetCheck("M252-B004-NDOC-03", "`tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M252-B004-SPC-01", "## M252 property ivar export legality and synthesis preconditions (B004)"),
    SnippetCheck("M252-B004-SPC-02", "`Objc3SemaParityContractSurface`"),
    SnippetCheck("M252-B004-SPC-03", "category-only property export keep"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M252-B004-META-01", "## M252 property ivar export legality metadata anchors (B004)"),
    SnippetCheck("M252-B004-META-02", "lowering replay key derived from the same canonical sema"),
    SnippetCheck("M252-B004-META-03", "`tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json`"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M252-B004-PARSE-01", "M252-B004 export-legality anchor: property attribute spelling must stay"),
)
SEMA_CONTRACT_SNIPPETS = (
    SnippetCheck("M252-B004-SEMA-01", "M252-B004 export-legality anchor: these counts are the canonical sema"),
)
SEMA_PASSES_SNIPPETS = (
    SnippetCheck("M252-B004-PASS-01", "M252-B004 export-legality anchor: class implementation property synthesis"),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M252-B004-ART-01", "Objc3PropertySynthesisIvarBindingContract BuildPropertySynthesisIvarBindingContract("),
    SnippetCheck("M252-B004-ART-02", "const Objc3SemaParityContractSurface &sema_parity_surface"),
    SnippetCheck("M252-B004-ART-03", "M252-B004 export-legality anchor: manifest sema surfaces must publish the"),
)
CLASS_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B004-FIX-CLASS-01", "module m252ClassPropertySynthesisReady;"),
    SnippetCheck("M252-B004-FIX-CLASS-02", "@property (readonly, getter=value) id token;"),
    SnippetCheck("M252-B004-FIX-CLASS-03", "- (id) value {"),
)
CATEGORY_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B004-FIX-CATEGORY-01", "module m252CategoryPropertyExportOnly;"),
    SnippetCheck("M252-B004-FIX-CATEGORY-02", "@interface Root (Debug)"),
    SnippetCheck("M252-B004-FIX-CATEGORY-03", "@property (readonly) id shadow;"),
)
MISSING_INTERFACE_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B004-FIX-MISSING-01", "// Expected diagnostic code(s): O3S206."),
    SnippetCheck("M252-B004-FIX-MISSING-02", "module m252MissingInterfaceProperty;"),
    SnippetCheck("M252-B004-FIX-MISSING-03", "@property (readonly) id value;"),
)
INCOMPATIBLE_FIXTURE_SNIPPETS = (
    SnippetCheck("M252-B004-FIX-CONFLICT-01", "// Expected diagnostic code(s): O3S206."),
    SnippetCheck("M252-B004-FIX-CONFLICT-02", "module m252IncompatiblePropertySignature;"),
    SnippetCheck("M252-B004-FIX-CONFLICT-03", "@property (readonly) i32 value;"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M252-B004-PKG-01", '"check:objc3c:m252-b004-property-ivar-export-legality-synthesis-preconditions": "python scripts/check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py"'),
    SnippetCheck("M252-B004-PKG-02", '"test:tooling:m252-b004-property-ivar-export-legality-synthesis-preconditions": "python -m pytest tests/tooling/test_check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py -q"'),
    SnippetCheck("M252-B004-PKG-03", '"check:objc3c:m252-b004-lane-b-readiness": "npm run check:objc3c:m252-b003-lane-b-readiness && npm run build:objc3c-native && npm run check:objc3c:m252-b004-property-ivar-export-legality-synthesis-preconditions && npm run test:tooling:m252-b004-property-ivar-export-legality-synthesis-preconditions"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--sema-contract", dest="sema_contract", type=Path, default=DEFAULT_SEMA_CONTRACT)
    parser.add_argument("--sema-passes", type=Path, default=DEFAULT_SEMA_PASSES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--class-fixture", type=Path, default=DEFAULT_CLASS_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--missing-interface-fixture", type=Path, default=DEFAULT_MISSING_INTERFACE_FIXTURE)
    parser.add_argument("--incompatible-fixture", type=Path, default=DEFAULT_INCOMPATIBLE_FIXTURE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--runner-exe", type=Path, default=DEFAULT_RUNNER_EXE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-runner-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def expected_replay_key(counts: dict[str, int]) -> str:
    return (
        f"property_synthesis_sites={counts['property_synthesis_sites']};"
        f"property_synthesis_explicit_ivar_bindings={counts['property_synthesis_explicit_ivar_bindings']};"
        f"property_synthesis_default_ivar_bindings={counts['property_synthesis_default_ivar_bindings']};"
        f"ivar_binding_sites={counts['ivar_binding_sites']};"
        f"ivar_binding_resolved={counts['ivar_binding_resolved']};"
        f"ivar_binding_missing={counts['ivar_binding_missing']};"
        f"ivar_binding_conflicts={counts['ivar_binding_conflicts']};"
        "deterministic=true;"
        f"lane_contract={LANE_CONTRACT}"
    )


def run_success_case(
    *,
    runner_exe: Path,
    fixture_path: Path,
    out_dir: Path,
    expected_counts: dict[str, int],
) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-B004-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(runner_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    manifest_path = out_dir / "module.manifest.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-B004-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), "M252-B004-RUNNER-DIAGNOSTICS", "frontend C API runner did not write module.diagnostics.json", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M252-B004-RUNNER-MANIFEST", "frontend C API runner did not write module.manifest.json", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    diagnostics_payload = load_json(diagnostics_path)
    manifest_payload = load_json(manifest_path)
    diagnostics = diagnostics_payload.get("diagnostics")
    checks_total += require(isinstance(diagnostics, list), display_path(diagnostics_path), "M252-B004-DIAGNOSTICS-LIST", "diagnostics payload must contain a diagnostics list", findings)
    if not isinstance(diagnostics, list):
        return checks_total, findings, None

    observed_codes = [entry.get("code") for entry in diagnostics if isinstance(entry, dict)]
    checks_total += require(summary_payload.get("success") is True, display_path(summary_path), "M252-B004-RUNNER-SUCCESS", f"runner success mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(summary_payload.get("status") == 0, display_path(summary_path), "M252-B004-RUNNER-STATUS", f"runner status mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-B004-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)
    checks_total += require(completed.returncode == 0, display_path(summary_path), "M252-B004-PROCESS-EXIT", f"runner process exit mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(observed_codes == [], display_path(diagnostics_path), "M252-B004-DIAGNOSTIC-CODES", f"expected no diagnostics, observed {observed_codes}", findings)

    frontend = manifest_payload.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    sema = pipeline.get("sema_pass_manager", {}) if isinstance(pipeline, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    property_surface = semantic_surface.get("objc_property_synthesis_ivar_binding_surface", {}) if isinstance(semantic_surface, dict) else {}
    checks_total += require(isinstance(sema, dict), display_path(manifest_path), "M252-B004-SEMA-PAYLOAD", "manifest sema_pass_manager payload is missing", findings)
    checks_total += require(isinstance(property_surface, dict), display_path(manifest_path), "M252-B004-SURFACE-PAYLOAD", "manifest property synthesis semantic surface is missing", findings)
    if not isinstance(sema, dict) or not isinstance(property_surface, dict):
        return checks_total, findings, None

    replay_key = expected_replay_key(expected_counts)
    for key, expected_value in expected_counts.items():
        checks_total += require(sema.get(key) == expected_value, display_path(manifest_path), f"M252-B004-SEMA-{key.upper()}", f"sema field {key} mismatch: expected {expected_value}, observed {sema.get(key)}", findings)
        checks_total += require(property_surface.get(key) == expected_value, display_path(manifest_path), f"M252-B004-SURFACE-{key.upper()}", f"surface field {key} mismatch: expected {expected_value}, observed {property_surface.get(key)}", findings)
    checks_total += require(sema.get("deterministic_property_synthesis_ivar_binding_handoff") is True, display_path(manifest_path), "M252-B004-SEMA-DETERMINISTIC", "sema deterministic handoff flag must be true", findings)
    checks_total += require(property_surface.get("deterministic_handoff") is True, display_path(manifest_path), "M252-B004-SURFACE-DETERMINISTIC", "surface deterministic handoff flag must be true", findings)
    checks_total += require(sema.get("lowering_property_synthesis_ivar_binding_replay_key") == replay_key, display_path(manifest_path), "M252-B004-SEMA-REPLAY-KEY", f"sema replay key mismatch: expected {replay_key}, observed {sema.get('lowering_property_synthesis_ivar_binding_replay_key')}", findings)
    checks_total += require(property_surface.get("replay_key") == replay_key, display_path(manifest_path), "M252-B004-SURFACE-REPLAY-KEY", f"surface replay key mismatch: expected {replay_key}, observed {property_surface.get('replay_key')}", findings)
    checks_total += require(sema.get("lowering_property_synthesis_ivar_binding_replay_key") == property_surface.get("replay_key"), display_path(manifest_path), "M252-B004-REPLAY-KEY-MATCH", "sema and semantic surface replay keys must match", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "diagnostics_path": display_path(diagnostics_path),
        "manifest_path": display_path(manifest_path),
        "observed_codes": observed_codes,
        "expected_counts": expected_counts,
        "observed_replay_key": property_surface.get("replay_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run_failure_case(
    *,
    runner_exe: Path,
    fixture_path: Path,
    out_dir: Path,
    expected_message_substring: str,
) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M252-B004-FIXTURE-EXISTS", "fixture is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(runner_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ]
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    summary_path = out_dir / "module.c_api_summary.json"
    diagnostics_path = out_dir / "module.diagnostics.json"

    checks_total += require(summary_path.exists(), display_path(summary_path), "M252-B004-RUNNER-SUMMARY", "frontend C API runner did not write module.c_api_summary.json", findings)
    checks_total += require(diagnostics_path.exists(), display_path(diagnostics_path), "M252-B004-RUNNER-DIAGNOSTICS", "frontend C API runner did not write module.diagnostics.json", findings)
    if findings:
        return checks_total, findings, None

    summary_payload = load_json(summary_path)
    diagnostics_payload = load_json(diagnostics_path)
    diagnostics = diagnostics_payload.get("diagnostics")
    checks_total += require(isinstance(diagnostics, list), display_path(diagnostics_path), "M252-B004-DIAGNOSTICS-LIST", "diagnostics payload must contain a diagnostics list", findings)
    if not isinstance(diagnostics, list):
        return checks_total, findings, None

    observed_codes = [entry.get("code") for entry in diagnostics if isinstance(entry, dict)]
    observed_messages = [entry.get("message") for entry in diagnostics if isinstance(entry, dict)]
    checks_total += require(summary_payload.get("success") is False, display_path(summary_path), "M252-B004-RUNNER-SUCCESS", f"runner success mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(summary_payload.get("status") == 1, display_path(summary_path), "M252-B004-RUNNER-STATUS", f"runner status mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(summary_payload.get("semantic_skipped") is False, display_path(summary_path), "M252-B004-SEMANTIC-SKIPPED", "semantic stage unexpectedly skipped", findings)
    checks_total += require(completed.returncode == 1, display_path(summary_path), "M252-B004-PROCESS-EXIT", f"runner process exit mismatch for {display_path(fixture_path)}", findings)
    checks_total += require(observed_codes == ["O3S206"], display_path(diagnostics_path), "M252-B004-DIAGNOSTIC-CODES", f"expected ['O3S206'], observed {observed_codes}", findings)
    checks_total += require(any(isinstance(message, str) and expected_message_substring in message for message in observed_messages), display_path(diagnostics_path), "M252-B004-DIAGNOSTIC-MESSAGE", f"diagnostic messages did not contain {expected_message_substring!r}", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "summary_path": display_path(summary_path),
        "diagnostics_path": display_path(diagnostics_path),
        "observed_codes": observed_codes,
        "observed_messages": observed_messages,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M252-B004-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M252-B004-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M252-B004-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M252-B004-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M252-B004-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M252-B004-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.parser_cpp, "M252-B004-PARSE-EXISTS", PARSER_SNIPPETS),
        (args.sema_contract, "M252-B004-SEMA-EXISTS", SEMA_CONTRACT_SNIPPETS),
        (args.sema_passes, "M252-B004-PASS-EXISTS", SEMA_PASSES_SNIPPETS),
        (args.frontend_artifacts, "M252-B004-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.class_fixture, "M252-B004-FIX-CLASS-EXISTS", CLASS_FIXTURE_SNIPPETS),
        (args.category_fixture, "M252-B004-FIX-CATEGORY-EXISTS", CATEGORY_FIXTURE_SNIPPETS),
        (args.missing_interface_fixture, "M252-B004-FIX-MISSING-EXISTS", MISSING_INTERFACE_FIXTURE_SNIPPETS),
        (args.incompatible_fixture, "M252-B004-FIX-CONFLICT-EXISTS", INCOMPATIBLE_FIXTURE_SNIPPETS),
        (args.package_json, "M252-B004-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        added_checks, added_failures = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += added_checks
        failures.extend(added_failures)

    runner_cases: dict[str, object] = {}
    runner_probes_executed = False
    if not args.skip_runner_probes:
        checks_total += require(args.runner_exe.exists(), display_path(args.runner_exe), "M252-B004-RUNNER-EXE", "native frontend C API runner binary is missing", failures)
        if args.runner_exe.exists():
            runner_probes_executed = True
            args.probe_root.mkdir(parents=True, exist_ok=True)
            class_checks, class_failures, class_payload = run_success_case(
                runner_exe=args.runner_exe,
                fixture_path=args.class_fixture,
                out_dir=args.probe_root / "class_property_synthesis_ready",
                expected_counts={
                    "property_synthesis_sites": 1,
                    "property_synthesis_explicit_ivar_bindings": 0,
                    "property_synthesis_default_ivar_bindings": 1,
                    "ivar_binding_sites": 1,
                    "ivar_binding_resolved": 1,
                    "ivar_binding_missing": 0,
                    "ivar_binding_conflicts": 0,
                },
            )
            checks_total += class_checks
            failures.extend(class_failures)
            if class_payload is not None:
                runner_cases["class_property_synthesis_ready"] = class_payload

            category_checks, category_failures, category_payload = run_success_case(
                runner_exe=args.runner_exe,
                fixture_path=args.category_fixture,
                out_dir=args.probe_root / "category_property_export_only",
                expected_counts={
                    "property_synthesis_sites": 0,
                    "property_synthesis_explicit_ivar_bindings": 0,
                    "property_synthesis_default_ivar_bindings": 0,
                    "ivar_binding_sites": 0,
                    "ivar_binding_resolved": 0,
                    "ivar_binding_missing": 0,
                    "ivar_binding_conflicts": 0,
                },
            )
            checks_total += category_checks
            failures.extend(category_failures)
            if category_payload is not None:
                runner_cases["category_property_export_only"] = category_payload

            missing_checks, missing_failures, missing_payload = run_failure_case(
                runner_exe=args.runner_exe,
                fixture_path=args.missing_interface_fixture,
                out_dir=args.probe_root / "missing_interface_property",
                expected_message_substring="is not declared in interface",
            )
            checks_total += missing_checks
            failures.extend(missing_failures)
            if missing_payload is not None:
                runner_cases["missing_interface_property"] = missing_payload

            incompatible_checks, incompatible_failures, incompatible_payload = run_failure_case(
                runner_exe=args.runner_exe,
                fixture_path=args.incompatible_fixture,
                out_dir=args.probe_root / "incompatible_property_signature",
                expected_message_substring="incompatible property signature",
            )
            checks_total += incompatible_checks
            failures.extend(incompatible_failures)
            if incompatible_payload is not None:
                runner_cases["incompatible_property_signature"] = incompatible_payload

    ok = not failures
    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [finding.__dict__ for finding in failures],
        "runner_probes_executed": runner_probes_executed,
        "runner_cases": runner_cases,
    }
    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if ok:
        print(f"[m252-b004] PASS {checks_total}/{checks_total} -> {display_path(summary_path)}")
        return 0

    print(f"[m252-b004] FAIL {checks_total - len(failures)}/{checks_total} -> {display_path(summary_path)}", file=sys.stderr)
    for finding in failures:
        print(f" - {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
