#!/usr/bin/env python3
"""Fail-closed contract checker for M255-C001 dispatch lowering ABI freeze."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-c001-dispatch-lowering-abi-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1"
PREVIOUS_CONTRACT_ID = "objc3c-super-dynamic-method-family/m255-b003-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_dispatch_lowering_abi_contract"
CANONICAL_RUNTIME_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_SYMBOL = "objc3_msgsend_i32"
SELECTOR_LOOKUP_SYMBOL = "objc3_runtime_lookup_selector"
SELECTOR_HANDLE_TYPE = "objc3_runtime_selector_handle"
BOUNDARY_MODEL = "compatibility-bridge-default-target-before-live-runtime-dispatch-cutover"
SELECTOR_OPERAND_MODEL = "selector-cstring-pointer-remains-lowered-operand-until-m255-c002"
SELECTOR_HANDLE_MODEL = "runtime-lookup-produces-selector-handle-before-live-dispatch"
ARGUMENT_PADDING_MODEL = "zero-pad-to-fixed-runtime-arg-slot-count"
DEFAULT_TARGET_MODEL = "default-lowering-target-remains-compatibility-bridge-until-m255-c002"
COMPATIBILITY_ROLE_MODEL = "compatibility-bridge-remains-test-and-backcompat-surface-not-canonical-runtime-abi"
DEFERRED_CASES_MODEL = "super-nil-direct-runtime-entrypoint-cutover-deferred-until-m255-c003"
BOUNDARY_COMMENT_PREFIX = "; runtime_dispatch_lowering_abi_boundary = contract=objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-C001" / "dispatch_lowering_abi_contract_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m255_super_dynamic_method_family_edges.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "c001-dispatch-lowering-abi"


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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def nested(payload: dict[str, Any], path: str) -> Any:
    current: Any = payload
    for part in path.split("."):
        if not isinstance(current, dict) or part not in current:
            raise KeyError(path)
        current = current[part]
    return current


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M255-C001-DOC-EXP-01", "# M255 Dispatch Lowering ABI Contract and Architecture Freeze Expectations (C001)"),
    SnippetCheck("M255-C001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C001-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M255-C001-DOC-EXP-04", f"`{CANONICAL_RUNTIME_SYMBOL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-C001-PKT-01", "# M255-C001 Dispatch Lowering ABI Contract and Architecture Freeze Packet"),
    SnippetCheck("M255-C001-PKT-02", "Packet: `M255-C001`"),
    SnippetCheck("M255-C001-PKT-03", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-C001-PKT-04", "No live call emission cutover to `objc3_runtime_dispatch_i32` lands yet."),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-C001-NDOC-01", "## Dispatch lowering ABI freeze (M255-C001)"),
    SnippetCheck("M255-C001-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M255-C001-NDOC-03", f"`{CANONICAL_RUNTIME_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-C001-SPC-01", "## M255 dispatch lowering ABI freeze (C001)"),
    SnippetCheck("M255-C001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-C001-SPC-03", "fixed argument marshalling remains `4` `i32` slots with zero padding"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-C001-META-01", "## M255 dispatch lowering ABI metadata anchors (C001)"),
    SnippetCheck("M255-C001-META-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M255-C001-META-03", f"`{COMPATIBILITY_SYMBOL}`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-C001-HDR-01", "kObjc3RuntimeDispatchLoweringAbiContractId"),
    SnippetCheck("M255-C001-HDR-02", "struct Objc3RuntimeDispatchLoweringAbiContract"),
    SnippetCheck("M255-C001-HDR-03", "kObjc3RuntimeDispatchLoweringDeferredCasesModel"),
)
LOWERING_CPP_SNIPPETS = (
    SnippetCheck("M255-C001-CPP-01", "M255-C001 dispatch lowering ABI freeze anchor"),
    SnippetCheck("M255-C001-CPP-02", "IsValidObjc3RuntimeDispatchLoweringAbiContract("),
    SnippetCheck("M255-C001-CPP-03", "Objc3RuntimeDispatchLoweringAbiBoundarySummary("),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M255-C001-ART-01", "BuildRuntimeDispatchLoweringAbiContract("),
    SnippetCheck("M255-C001-ART-02", '<< ",\\\"objc_runtime_dispatch_lowering_abi_contract\\\":'),
    SnippetCheck("M255-C001-ART-03", "runtime_dispatch_lowering_abi_boundary = "),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-C001-PARSE-01", "M255-C001 dispatch lowering ABI freeze anchor"),
)
IR_SNIPPETS = (
    SnippetCheck("M255-C001-IR-01", "M255-C001 dispatch lowering ABI freeze anchor"),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-C001-SHIM-01", "M255-C001 dispatch lowering ABI freeze"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-C001-PKG-01", '"check:objc3c:m255-c001-dispatch-lowering-abi-contract-and-architecture-freeze": "python scripts/check_m255_c001_dispatch_lowering_abi_contract_and_architecture_freeze.py"'),
    SnippetCheck("M255-C001-PKG-02", '"test:tooling:m255-c001-dispatch-lowering-abi-contract-and-architecture-freeze": "python -m pytest tests/tooling/test_check_m255_c001_dispatch_lowering_abi_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M255-C001-PKG-03", '"check:objc3c:m255-c001-lane-c-readiness": "npm run build:objc3c-native && npm run check:objc3c:m255-c001-dispatch-lowering-abi-contract-and-architecture-freeze && npm run test:tooling:m255-c001-dispatch-lowering-abi-contract-and-architecture-freeze"'),
)


def run_dynamic_probe(failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    out_dir = PROBE_ROOT / "hello"
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    result = run_command(command)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"

    case: dict[str, object] = {
        "fixture": display_path(FIXTURE),
        "out_dir": display_path(out_dir),
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "manifest_exists": manifest_path.exists(),
        "ir_exists": ir_path.exists(),
    }

    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(out_dir), "M255-C001-DYN-01", "native probe failed", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), "M255-C001-DYN-02", "manifest missing", failures)
    checks_total += 1
    checks_passed += require(ir_path.exists(), display_path(ir_path), "M255-C001-DYN-03", "IR missing", failures)
    if result.returncode != 0 or not manifest_path.exists() or not ir_path.exists():
        return checks_total, checks_passed, case

    manifest = load_json(manifest_path)
    surface = nested(manifest, SURFACE_PATH)
    if not isinstance(surface, dict):
        failures.append(Finding(display_path(manifest_path), "M255-C001-DYN-04", "semantic surface is not an object"))
        return checks_total + 1, checks_passed, case

    case["surface"] = surface
    checks = [
        (surface.get("lowering_boundary_model") == BOUNDARY_MODEL, "M255-C001-DYN-04", "lowering boundary model mismatch"),
        (surface.get("canonical_runtime_dispatch_symbol") == CANONICAL_RUNTIME_SYMBOL, "M255-C001-DYN-05", "canonical runtime symbol mismatch"),
        (surface.get("compatibility_runtime_dispatch_symbol") == COMPATIBILITY_SYMBOL, "M255-C001-DYN-06", "compatibility symbol mismatch"),
        (surface.get("default_lowering_target_symbol") == COMPATIBILITY_SYMBOL, "M255-C001-DYN-07", "default lowering target symbol mismatch"),
        (surface.get("selector_lookup_symbol") == SELECTOR_LOOKUP_SYMBOL, "M255-C001-DYN-08", "selector lookup symbol mismatch"),
        (surface.get("selector_handle_type") == SELECTOR_HANDLE_TYPE, "M255-C001-DYN-09", "selector handle type mismatch"),
        (surface.get("receiver_abi_type") == "i32", "M255-C001-DYN-10", "receiver ABI mismatch"),
        (surface.get("selector_abi_type") == "ptr", "M255-C001-DYN-11", "selector ABI mismatch"),
        (surface.get("argument_abi_type") == "i32", "M255-C001-DYN-12", "argument ABI mismatch"),
        (surface.get("result_abi_type") == "i32", "M255-C001-DYN-13", "result ABI mismatch"),
        (surface.get("fixed_argument_slot_count") == 4, "M255-C001-DYN-14", "fixed argument slot count mismatch"),
        (surface.get("runtime_dispatch_parameter_count") == 6, "M255-C001-DYN-15", "runtime dispatch parameter count mismatch"),
        (surface.get("selector_operand_model") == SELECTOR_OPERAND_MODEL, "M255-C001-DYN-16", "selector operand model mismatch"),
        (surface.get("selector_handle_model") == SELECTOR_HANDLE_MODEL, "M255-C001-DYN-17", "selector handle model mismatch"),
        (surface.get("argument_padding_model") == ARGUMENT_PADDING_MODEL, "M255-C001-DYN-18", "argument padding model mismatch"),
        (surface.get("default_lowering_target_model") == DEFAULT_TARGET_MODEL, "M255-C001-DYN-19", "default target model mismatch"),
        (surface.get("compatibility_bridge_role_model") == COMPATIBILITY_ROLE_MODEL, "M255-C001-DYN-20", "compatibility role model mismatch"),
        (surface.get("deferred_cases_model") == DEFERRED_CASES_MODEL, "M255-C001-DYN-21", "deferred cases model mismatch"),
        (surface.get("message_send_sites", 0) >= 1, "M255-C001-DYN-22", "expected at least one message-send site"),
        (surface.get("fail_closed") is True, "M255-C001-DYN-23", "fail_closed flag mismatch"),
    ]
    checks_total += len(checks)
    for ok, check_id, detail in checks:
        checks_passed += require(ok, display_path(manifest_path), check_id, detail, failures)

    lowering = manifest.get("lowering", {})
    checks_total += 1
    checks_passed += require(
        isinstance(lowering, dict) and lowering.get("runtime_dispatch_symbol") == COMPATIBILITY_SYMBOL,
        display_path(manifest_path),
        "M255-C001-DYN-24",
        "manifest lowering.runtime_dispatch_symbol mismatch",
        failures,
    )

    ir_text = read_text(ir_path)
    case["ir_boundary_comment_present"] = BOUNDARY_COMMENT_PREFIX in ir_text
    checks_total += 1
    checks_passed += require(BOUNDARY_COMMENT_PREFIX in ir_text, display_path(ir_path), "M255-C001-DYN-25", "missing runtime dispatch lowering ABI boundary comment", failures)
    checks_total += 1
    checks_passed += require("@objc3_msgsend_i32" in ir_text, display_path(ir_path), "M255-C001-DYN-26", "expected compatibility dispatch symbol in current IR", failures)

    return checks_total, checks_passed, case


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    files_and_snippets = [
        (ROOT / "docs" / "contracts" / "m255_dispatch_lowering_abi_contract_and_architecture_freeze_c001_expectations.md", EXPECTATIONS_SNIPPETS),
        (ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_c001_dispatch_lowering_abi_contract_and_architecture_freeze_packet.md", PACKET_SNIPPETS),
        (ROOT / "docs" / "objc3c-native.md", NATIVE_DOC_SNIPPETS),
        (ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md", LOWERING_SPEC_SNIPPETS),
        (ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md", METADATA_SPEC_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h", LOWERING_HEADER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp", LOWERING_CPP_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp", FRONTEND_ARTIFACTS_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp", PARSER_SNIPPETS),
        (ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp", IR_SNIPPETS),
        (ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c", SHIM_SNIPPETS),
        (ROOT / "package.json", PACKAGE_SNIPPETS),
    ]
    for path, snippets in files_and_snippets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_probes: dict[str, object]
    if args.skip_dynamic_probes:
        dynamic_probes = {"skipped": True}
    else:
        dyn_total, dyn_passed, probe_case = run_dynamic_probe(failures)
        checks_total += dyn_total
        checks_passed += dyn_passed
        dynamic_probes = {"skipped": False, "positive": probe_case}

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "previous_contract_id": PREVIOUS_CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "findings": [finding.__dict__ for finding in failures],
        "dynamic_probes": dynamic_probes,
        "frozen_surface": {
            "canonical_runtime_dispatch_symbol": CANONICAL_RUNTIME_SYMBOL,
            "compatibility_runtime_dispatch_symbol": COMPATIBILITY_SYMBOL,
            "selector_lookup_symbol": SELECTOR_LOOKUP_SYMBOL,
            "selector_handle_type": SELECTOR_HANDLE_TYPE,
            "receiver_abi_type": "i32",
            "selector_abi_type": "ptr",
            "argument_abi_type": "i32",
            "result_abi_type": "i32",
            "fixed_argument_slot_count": 4,
        },
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    json.dump(summary, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
