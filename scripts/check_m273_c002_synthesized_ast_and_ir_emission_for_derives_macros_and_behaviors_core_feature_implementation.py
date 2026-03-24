#!/usr/bin/env python3
"""Checker for M273-C002 synthesized AST/IR emission."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m273-c002-part10-synthesized-ast-ir-emission-v1"
CONTRACT_ID = "objc3c-part10-synthesized-ast-ir-emission/m273-c002-v1"
SURFACE_KEY = "objc_part10_synthesized_ast_and_ir_emission"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m273" / "M273-C002" / "synthesized_ast_ir_emission_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_synthesized_ast_and_ir_emission_for_derives_macros_and_behaviors_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_c002_synthesized_ast_and_ir_emission_for_derives_macros_and_behaviors_core_feature_implementation_packet.md"
DOC_ARTIFACTS = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_METADATA = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
LLVM_NM = Path(r"C:\Program Files\LLVM\bin\llvm-nm.exe")
OBJECT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_c002_synthesized_ast_ir_positive.objc3"
MACRO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_c002_synthesized_ast_ir_macro_positive.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M273-C002-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m273-c002-readiness",
        "--summary-out",
        "tmp/reports/m273/M273-C002/ensure_build_runtime_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M273-C002-DYN-01", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M273-C002-DYN-02", "frontend runner missing after build", failures)

    object_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "c002" / "positive"
    object_out_dir.mkdir(parents=True, exist_ok=True)
    object_completed = run_command([
        str(args.runner_exe),
        str(OBJECT_FIXTURE),
        "--out-dir",
        str(object_out_dir),
        "--emit-prefix",
        "module",
    ])
    object_output = (object_completed.stdout or "") + (object_completed.stderr or "")
    checks_total += 1
    checks_passed += require(object_completed.returncode == 0, display_path(OBJECT_FIXTURE), "M273-C002-DYN-03", f"object fixture failed: {object_output}", failures)

    object_manifest_path = object_out_dir / "module.manifest.json"
    object_ir_path = object_out_dir / "module.ll"
    object_path = object_out_dir / "module.obj"
    for index, path in enumerate((object_manifest_path, object_ir_path, object_path), start=4):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M273-C002-DYN-{index:02d}", f"missing object-path artifact: {display_path(path)}", failures)

    object_packet: dict[str, object] = {}
    if object_manifest_path.exists():
        object_manifest = json.loads(object_manifest_path.read_text(encoding="utf-8"))
        object_packet = object_manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    object_expected = {
        "derive_inventory_sites": 1,
        "emitted_derive_method_sites": 1,
        "emitted_macro_artifact_sites": 0,
        "emitted_property_behavior_artifact_sites": 2,
        "emitted_global_artifact_sites": 3,
        "emitted_runtime_method_list_sites": 1,
        "guard_blocked_sites": 0,
        "contract_violation_sites": 0,
    }
    for index, (field, expected_value) in enumerate(object_expected.items(), start=7):
        checks_total += 1
        checks_passed += require(object_packet.get(field) == expected_value, display_path(object_manifest_path), f"M273-C002-DYN-{index:02d}", f"object packet field {field} mismatch", failures)

    for field, expected_value, index in (("deterministic_handoff", True, 15), ("ready_for_ir_emission", True, 16)):
        checks_total += 1
        checks_passed += require(object_packet.get(field) is expected_value, display_path(object_manifest_path), f"M273-C002-DYN-{index:02d}", f"object packet field {field} mismatch", failures)

    object_ir_text = object_ir_path.read_text(encoding="utf-8") if object_ir_path.exists() else ""
    object_ir_checks = [
        ("M273-C002-DYN-17", "; part10_synthesized_ast_ir_emission = derive_inventory_sites=1;emitted_derive_method_sites=1;emitted_macro_artifact_sites=0;emitted_property_behavior_artifact_sites=2;emitted_global_artifact_sites=3;emitted_runtime_method_list_sites=1"),
        ("M273-C002-DYN-18", "; frontend_objc_part10_synthesized_emission_profile = emitted_derive_method_sites=1, emitted_macro_artifact_sites=0, emitted_property_behavior_artifact_sites=2, emitted_global_artifact_sites=3, emitted_runtime_method_list_sites=1"),
        ("M273-C002-DYN-19", "define i32 @objc3_method_Widget_instance_isEqual_(i32 %arg0)"),
        ("M273-C002-DYN-20", "@objc3_part10_property_behavior_class_interface_Widget_value_Observed = private constant"),
        ("M273-C002-DYN-21", "@objc3_part10_property_behavior_class_implementation_Widget_value_Observed = private constant"),
        ("M273-C002-DYN-22", "!objc3.objc_part10_synthesized_ast_and_ir_emission = !{!105}"),
    ]
    for check_id, snippet in object_ir_checks:
        checks_total += 1
        checks_passed += require(snippet in object_ir_text, display_path(object_ir_path), check_id, f"object-path IR missing snippet: {snippet}", failures)

    object_nm_output = ""
    if object_path.exists() and LLVM_NM.exists():
        nm = run_command([str(LLVM_NM), str(object_path)])
        object_nm_output = (nm.stdout or "") + (nm.stderr or "")
        checks_total += 1
        checks_passed += require(nm.returncode == 0, display_path(object_path), "M273-C002-DYN-23", f"llvm-nm failed: {object_nm_output}", failures)
        checks_total += 1
        checks_passed += require(
            "objc3_method_Widget_instance_isEqual_" in object_nm_output,
            display_path(object_path),
            "M273-C002-DYN-24",
            "object missing exported derived method symbol: objc3_method_Widget_instance_isEqual_",
            failures,
        )

    macro_out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "c002" / "macro"
    macro_out_dir.mkdir(parents=True, exist_ok=True)
    macro_completed = run_command([
        str(args.runner_exe),
        str(MACRO_FIXTURE),
        "--out-dir",
        str(macro_out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-object",
    ])
    macro_output = (macro_completed.stdout or "") + (macro_completed.stderr or "")
    checks_total += 1
    checks_passed += require(macro_completed.returncode == 0, display_path(MACRO_FIXTURE), "M273-C002-DYN-27", f"macro fixture failed: {macro_output}", failures)

    macro_manifest_path = macro_out_dir / "module.manifest.json"
    macro_ir_path = macro_out_dir / "module.ll"
    for index, path in enumerate((macro_manifest_path, macro_ir_path), start=28):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M273-C002-DYN-{index:02d}", f"missing macro-path artifact: {display_path(path)}", failures)

    macro_packet: dict[str, object] = {}
    if macro_manifest_path.exists():
        macro_manifest = json.loads(macro_manifest_path.read_text(encoding="utf-8"))
        macro_packet = macro_manifest["frontend"]["pipeline"]["semantic_surface"][SURFACE_KEY]

    macro_expected = {
        "derive_inventory_sites": 1,
        "emitted_derive_method_sites": 1,
        "emitted_macro_artifact_sites": 1,
        "emitted_property_behavior_artifact_sites": 2,
        "emitted_global_artifact_sites": 4,
        "emitted_runtime_method_list_sites": 1,
        "guard_blocked_sites": 0,
        "contract_violation_sites": 0,
    }
    for index, (field, expected_value) in enumerate(macro_expected.items(), start=30):
        checks_total += 1
        checks_passed += require(macro_packet.get(field) == expected_value, display_path(macro_manifest_path), f"M273-C002-DYN-{index:02d}", f"macro packet field {field} mismatch", failures)

    for field, expected_value, index in (("deterministic_handoff", True, 38), ("ready_for_ir_emission", True, 39)):
        checks_total += 1
        checks_passed += require(macro_packet.get(field) is expected_value, display_path(macro_manifest_path), f"M273-C002-DYN-{index:02d}", f"macro packet field {field} mismatch", failures)

    macro_ir_text = macro_ir_path.read_text(encoding="utf-8") if macro_ir_path.exists() else ""
    macro_ir_checks = [
        ("M273-C002-DYN-40", "; part10_synthesized_ast_ir_emission = derive_inventory_sites=1;emitted_derive_method_sites=1;emitted_macro_artifact_sites=1;emitted_property_behavior_artifact_sites=2;emitted_global_artifact_sites=4;emitted_runtime_method_list_sites=1"),
        ("M273-C002-DYN-41", "; frontend_objc_part10_synthesized_emission_profile = emitted_derive_method_sites=1, emitted_macro_artifact_sites=1, emitted_property_behavior_artifact_sites=2, emitted_global_artifact_sites=4, emitted_runtime_method_list_sites=1"),
        ("M273-C002-DYN-42", "define i32 @objc3_method_Widget_instance_isEqual_(i32 %arg0)"),
        ("M273-C002-DYN-43", "@objc3_part10_macro_artifact_traced__Trace_ = private constant"),
        ("M273-C002-DYN-44", "@objc3_part10_property_behavior_class_interface_Widget_value_Observed = private constant"),
        ("M273-C002-DYN-45", "!objc3.objc_part10_synthesized_ast_and_ir_emission = !{!105}"),
    ]
    for check_id, snippet in macro_ir_checks:
        checks_total += 1
        checks_passed += require(snippet in macro_ir_text, display_path(macro_ir_path), check_id, f"macro-path IR missing snippet: {snippet}", failures)

    return checks_total, checks_passed, {
        "object_fixture": display_path(OBJECT_FIXTURE),
        "object_returncode": object_completed.returncode,
        "object_output": object_output.strip(),
        "object_manifest": display_path(object_manifest_path),
        "object_ir": display_path(object_ir_path),
        "object_file": display_path(object_path),
        "object_packet": object_packet,
        "object_llvm_nm_output": object_nm_output.strip(),
        "macro_fixture": display_path(MACRO_FIXTURE),
        "macro_returncode": macro_completed.returncode,
        "macro_output": macro_output.strip(),
        "macro_manifest": display_path(macro_manifest_path),
        "macro_ir": display_path(macro_ir_path),
        "macro_packet": macro_packet,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M273-C002-EXP-01", "# M273 Synthesized AST and IR Emission for Derives, Macros, and Behaviors Expectations (C002)"),
            SnippetCheck("M273-C002-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M273-C002-PKT-01", "# M273-C002 Packet: Synthesized AST and IR Emission for Derives, Macros, and Behaviors - Core Feature Implementation"),
            SnippetCheck("M273-C002-PKT-02", SURFACE_KEY),
        ],
        DOC_ARTIFACTS: [
            SnippetCheck("M273-C002-ARTDOC-01", "## M273 synthesized AST and IR emission"),
            SnippetCheck("M273-C002-ARTDOC-02", SURFACE_KEY),
        ],
        DOC_NATIVE: [
            SnippetCheck("M273-C002-DOC-01", "## M273 synthesized AST and IR emission"),
            SnippetCheck("M273-C002-DOC-02", "part10_synthesized_ast_ir_emission"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M273-C002-ATTR-01", "## M273 synthesized AST and IR emission (C002)"),
            SnippetCheck("M273-C002-ATTR-02", SURFACE_KEY),
        ],
        SPEC_METADATA: [
            SnippetCheck("M273-C002-META-01", "## M273 synthesized AST/IR metadata note"),
            SnippetCheck("M273-C002-META-02", CONTRACT_ID),
        ],
        LOWERING_HEADER: [
            SnippetCheck("M273-C002-HDR-01", "kObjc3Part10SynthesizedArtifactEmissionContractId"),
            SnippetCheck("M273-C002-HDR-02", "struct Objc3Part10SynthesizedArtifactEmissionContract"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M273-C002-CPP-01", "IsValidObjc3Part10SynthesizedArtifactEmissionContract"),
            SnippetCheck("M273-C002-CPP-02", "Objc3Part10SynthesizedArtifactEmissionReplayKey"),
        ],
        IR_HEADER: [
            SnippetCheck("M273-C002-IRH-01", "struct Objc3IRPart10DerivedMethodBundle"),
            SnippetCheck("M273-C002-IRH-02", "part10_derived_method_bundles_lexicographic"),
            SnippetCheck("M273-C002-IRH-03", "lowering_part10_synthesized_emission_replay_key"),
        ],
        IR_CPP: [
            SnippetCheck("M273-C002-IRC-01", "part10_synthesized_ast_ir_emission = "),
            SnippetCheck("M273-C002-IRC-02", "frontend_objc_part10_synthesized_emission_profile = emitted_derive_method_sites="),
            SnippetCheck("M273-C002-IRC-03", "!objc3.objc_part10_synthesized_ast_and_ir_emission = !{!105}"),
        ],
        ARTIFACTS_CPP: [
            SnippetCheck("M273-C002-ART-01", "BuildPart10DerivedMethodBundles"),
            SnippetCheck("M273-C002-ART-02", "BuildPart10SynthesizedArtifactEmissionContract"),
            SnippetCheck("M273-C002-ART-03", "BuildPart10SynthesizedArtifactEmissionContractJson"),
            SnippetCheck("M273-C002-ART-04", SURFACE_KEY),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M273-C002-PKG-01", '"check:objc3c:m273-c002-synthesized-ast-and-ir-emission-for-derives-macros-and-behaviors-core-feature-implementation"'),
            SnippetCheck("M273-C002-PKG-02", '"check:objc3c:m273-c002-lane-c-readiness"'),
        ],
    }

    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, object] = {"skipped": True}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "surface_key": SURFACE_KEY,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_executed": dynamic_executed,
        "dynamic_summary": dynamic_summary,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        json.dump(payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1
    json.dump(payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
