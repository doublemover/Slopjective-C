#!/usr/bin/env python3
"""Checker for M274-C001 interop lowering and ABI contract freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-c001-part11-interop-lowering-and-abi-contract-v1"
CONTRACT_ID = "objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part11_interop_lowering_and_abi_contract"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m274" / "M274-C001" / "interop_lowering_and_abi_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_interop_lowering_and_abi_contract_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_c001_interop_lowering_and_abi_contract_contract_and_architecture_freeze_packet.md"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_c001_interop_lowering_abi_contract_positive.objc3"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
        failures.append(Finding(display_path(path), "STATIC-MISSING", f"missing artifact: {display_path(path)}"))
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


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M274-C001-EXP-01", "# M274 Interop Lowering And ABI Contract And Architecture Freeze Expectations (C001)"),
        SnippetCheck("M274-C001-EXP-02", "Issue: `#7367`"),
        SnippetCheck("M274-C001-EXP-03", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-C001-EXP-04", SURFACE_PATH),
        SnippetCheck("M274-C001-EXP-05", "ABI lowering and runtime bridge generation remain deferred to later milestone work"),
        SnippetCheck("M274-C001-EXP-06", "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"),
        SnippetCheck("M274-C001-EXP-07", "objc3c-part11-foreign-declaration-import-source-closure/m274-a001-v1"),
        SnippetCheck("M274-C001-EXP-08", "objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-C001-PKT-01", "# M274-C001 Packet: Interop Lowering And ABI Contract - Contract And Architecture Freeze"),
        SnippetCheck("M274-C001-PKT-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-C001-PKT-03", SURFACE_PATH),
        SnippetCheck("M274-C001-PKT-04", "foreign-callable surfaces"),
        SnippetCheck("M274-C001-PKT-05", "module/interface preservation"),
        SnippetCheck("M274-C001-PKT-06", "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"),
        SnippetCheck("M274-C001-PKT-07", "objc3c-part11-foreign-declaration-import-source-closure/m274-a001-v1"),
        SnippetCheck("M274-C001-PKT-08", "objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1"),
    ),
    FIXTURE: (
        SnippetCheck("M274-C001-FIX-01", "module Part11InteropLoweringAbiContractPositive;"),
        SnippetCheck("M274-C001-FIX-02", "objc_foreign"),
        SnippetCheck("M274-C001-FIX-03", "objc_import_module(named(\"SampleKit\"))"),
        SnippetCheck("M274-C001-FIX-04", "objc_cxx_name(named(\"CppBridge\"))"),
        SnippetCheck("M274-C001-FIX-05", "objc_header_name(named(\"Bridge.hpp\"))"),
        SnippetCheck("M274-C001-FIX-06", "objc_swift_name(named(\"SwiftBridge\"))"),
        SnippetCheck("M274-C001-FIX-07", "objc_swift_private"),
        SnippetCheck("M274-C001-FIX-08", "@protocol RuntimeParityProtocol"),
        SnippetCheck("M274-C001-FIX-09", "@interface RuntimeParityBridge"),
        SnippetCheck("M274-C001-FIX-10", "@implementation RuntimeParityBridge"),
    ),
}


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M274-C001-DYN-01", "frontend runner missing", failures)

    out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "c001" / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([
        str(args.runner_exe),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--no-emit-object",
    ])
    output = (completed.stdout or "") + (completed.stderr or "")
    checks_total += 1
    checks_passed += require(completed.returncode == 0, display_path(FIXTURE), "M274-C001-DYN-02", f"positive fixture failed: {output}", failures)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M274-C001-DYN-03", "positive manifest missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M274-C001-DYN-04", "positive IR missing", failures)

    surface_payload: dict[str, Any] = {}
    manifest: dict[str, Any] = {}
    if manifest_path.exists():
        manifest = load_json(manifest_path)
        surface = semantic_surface(manifest)
        surface_payload = surface.get("objc_part11_interop_lowering_and_abi_contract", {})
        checks_total += 1
        checks_passed += require(isinstance(surface_payload, dict), display_path(manifest_path), "M274-C001-DYN-05", "missing objc_part11_interop_lowering_and_abi_contract", failures)
        if isinstance(surface_payload, dict):
            checks_total += 1
            checks_passed += require(
                surface_payload.get("contract_id") == CONTRACT_ID,
                display_path(manifest_path),
                "M274-C001-DYN-06",
                "interop lowering contract id mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("semantic_contract_id") == "objc3c-part11-interop-semantic-model/m274-b001-v1",
                display_path(manifest_path),
                "M274-C001-DYN-07",
                "interop semantic dependency contract mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("runtime_parity_contract_id") == "objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1",
                display_path(manifest_path),
                "M274-C001-DYN-08",
                "runtime parity dependency contract mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("cpp_interaction_contract_id") == "objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1",
                display_path(manifest_path),
                "M274-C001-DYN-09",
                "cpp interaction dependency contract mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("swift_isolation_contract_id") == "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1",
                display_path(manifest_path),
                "M274-C001-DYN-10",
                "swift isolation dependency contract mismatch",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("preservation_contract_id") == "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1",
                display_path(manifest_path),
                "M274-C001-DYN-11",
                "preservation dependency contract mismatch",
                failures,
            )
            expected_counts = {
                "foreign_callable_sites": 4,
                "c_foreign_callable_sites": 2,
                "objc_runtime_parity_callable_sites": 3,
                "ownership_bridge_callable_sites": 0,
                "error_surface_sites": 0,
                "async_boundary_sites": 0,
                "swift_concurrency_metadata_sites": 0,
                "interface_preserved_foreign_callable_sites": 4,
                "interface_preserved_metadata_annotation_sites": 13,
                "guard_blocked_sites": 0,
                "contract_violation_sites": 0,
            }
            for index, (field, expected_value) in enumerate(expected_counts.items(), start=12):
                checks_total += 1
                checks_passed += require(
                    surface_payload.get(field) == expected_value,
                    display_path(manifest_path),
                    f"M274-C001-DYN-{index:02d}",
                    f"{field} mismatch",
                    failures,
                )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("deterministic_handoff") is True,
                display_path(manifest_path),
                "M274-C001-DYN-23",
                "interop lowering handoff is not deterministic",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                surface_payload.get("ready_for_ir_emission") is True,
                display_path(manifest_path),
                "M274-C001-DYN-24",
                "interop lowering contract is not ready for IR emission",
                failures,
            )

        for key, expected_contract_id, check_id in [
            ("objc_part11_foreign_declaration_and_import_source_closure", "objc3c-part11-foreign-declaration-import-source-closure/m274-a001-v1", "M274-C001-DYN-28"),
            ("objc_part11_cpp_and_swift_interop_annotation_source_completion", "objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1", "M274-C001-DYN-29"),
            ("objc_part11_interop_semantic_model", "objc3c-part11-interop-semantic-model/m274-b001-v1", "M274-C001-DYN-30"),
            ("objc_part11_c_and_objc_runtime_parity_semantics", "objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1", "M274-C001-DYN-31"),
            ("objc_part11_cpp_ownership_throws_and_async_interactions", "objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1", "M274-C001-DYN-32"),
            ("objc_part11_swift_metadata_and_isolation_mapping", "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1", "M274-C001-DYN-33"),
            ("objc_part11_interop_lowering_and_abi_contract", CONTRACT_ID, "M274-C001-DYN-34"),
        ]:
            payload = surface.get(key) if isinstance(surface.get(key), dict) else {}
            checks_total += 1
            checks_passed += require(isinstance(payload, dict), display_path(manifest_path), check_id, f"missing manifest surface: {key}", failures)
            if isinstance(payload, dict):
                checks_total += 1
                checks_passed += require(
                    payload.get("contract_id") == expected_contract_id,
                    display_path(manifest_path),
                    check_id + "-CID",
                    f"{key} contract id mismatch",
                    failures,
                )

        interop_semantic = surface.get("objc_part11_interop_semantic_model")
        runtime_parity = surface.get("objc_part11_c_and_objc_runtime_parity_semantics")
        cpp_interop = surface.get("objc_part11_cpp_ownership_throws_and_async_interactions")
        swift_interop = surface.get("objc_part11_swift_metadata_and_isolation_mapping")
        if isinstance(interop_semantic, dict):
            checks_total += 1
            checks_passed += require(
                interop_semantic.get("ready_for_semantic_expansion") is True,
                display_path(manifest_path),
                "M274-C001-DYN-46",
                "interop semantic model not ready for semantic expansion",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                interop_semantic.get("ffi_abi_lowering_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-35",
                "interop semantic model must defer ffi abi lowering",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                interop_semantic.get("runtime_bridge_generation_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-36",
                "interop semantic model must defer runtime bridge generation",
                failures,
            )
        if isinstance(runtime_parity, dict):
            checks_total += 1
            checks_passed += require(
                runtime_parity.get("ready_for_lowering_and_runtime") is True,
                display_path(manifest_path),
                "M274-C001-DYN-37",
                "runtime parity model not ready for lowering/runtime",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                runtime_parity.get("ffi_abi_lowering_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-38",
                "runtime parity model must defer ffi abi lowering",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                runtime_parity.get("runtime_bridge_generation_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-39",
                "runtime parity model must defer runtime bridge generation",
                failures,
            )
        if isinstance(cpp_interop, dict):
            checks_total += 1
            checks_passed += require(
                cpp_interop.get("ready_for_lowering_and_runtime") is True,
                display_path(manifest_path),
                "M274-C001-DYN-40",
                "cpp interop model not ready for lowering/runtime",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                cpp_interop.get("ffi_abi_lowering_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-41",
                "cpp interop model must defer ffi abi lowering",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                cpp_interop.get("runtime_bridge_generation_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-42",
                "cpp interop model must defer runtime bridge generation",
                failures,
            )
        if isinstance(swift_interop, dict):
            checks_total += 1
            checks_passed += require(
                swift_interop.get("ready_for_lowering_and_runtime") is True,
                display_path(manifest_path),
                "M274-C001-DYN-43",
                "swift interop model not ready for lowering/runtime",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                swift_interop.get("ffi_abi_lowering_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-44",
                "swift interop model must defer ffi abi lowering",
                failures,
            )
            checks_total += 1
            checks_passed += require(
                swift_interop.get("runtime_bridge_generation_deferred") is True,
                display_path(manifest_path),
                "M274-C001-DYN-45",
                "swift interop model must defer runtime bridge generation",
                failures,
            )

    ir_text = ir_path.read_text(encoding="utf-8") if ir_path.exists() else ""
    for check_id, snippet in [
        ("M274-C001-DYN-47", "; lowering_ir_boundary = "),
        ("M274-C001-DYN-48", "; runtime_dispatch_decl = "),
        ("M274-C001-DYN-49", "; part11_interop_lowering_abi_contract = "),
        ("M274-C001-DYN-50", "; part10_module_interface_replay_preservation = "),
        ("M274-C001-DYN-51", "!objc3.objc_part11_interop_lowering_and_abi_contract = !{!"),
        ("M274-C001-DYN-52", "!objc3.objc_part10_module_interface_and_replay_preservation = !{!"),
        ("M274-C001-DYN-53", "!objc3.objc_part9_dispatch_metadata_and_interface_preservation = !{!"),
    ]:
        checks_total += 1
        checks_passed += require(snippet in ir_text, display_path(ir_path), check_id, f"IR missing snippet: {snippet}", failures)

    return checks_total, checks_passed, {
        "runner": display_path(args.runner_exe),
        "positive_fixture": display_path(FIXTURE),
        "positive_returncode": completed.returncode,
        "positive_output": output.strip(),
        "positive_manifest": display_path(manifest_path),
        "positive_ir": display_path(ir_path),
        "interop_lowering_contract": surface_payload,
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_summary: dict[str, Any] = {"skipped": True}
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
        "surface_path": SURFACE_PATH,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "dynamic_summary": dynamic_summary,
        "failures": [failure.__dict__ for failure in failures],
        "ok": checks_total > 0 and checks_passed == checks_total and not failures,
    }
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
