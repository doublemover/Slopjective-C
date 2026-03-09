#!/usr/bin/env python3
"""Fail-closed checker for M255-E001 live dispatch gate."""

from __future__ import annotations

import argparse
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m255-e001-live-dispatch-gate-contract-and-architecture-freeze-v1"
CONTRACT_ID = "objc3c-runtime-live-dispatch-gate/m255-e001-v1"
EVIDENCE_MODEL = "a002-b003-c004-d004-summary-chain"
SHIM_BOUNDARY_MODEL = "live-runtime-dispatch-required-compatibility-shim-evidence-only"
FAILURE_MODEL = "fail-closed-on-live-dispatch-evidence-drift"
NEXT_CLOSEOUT_ISSUE = "M255-E002"
CANONICAL_SYMBOL = "objc3_runtime_dispatch_i32"
COMPATIBILITY_SYMBOL = "objc3_msgsend_i32"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m255_live_dispatch_gate_contract_and_architecture_freeze_e001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_e001_live_dispatch_gate_contract_and_architecture_freeze_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_SHIM_C = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m255_e001_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_A002_SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-A002" / "dispatch_site_modeling_summary.json"
DEFAULT_B003_SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-B003" / "super_direct_dynamic_method_family_summary.json"
DEFAULT_C004_SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-C004" / "live_dispatch_cutover_summary.json"
DEFAULT_D004_SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-D004" / "protocol_category_method_resolution_summary.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m255" / "M255-E001" / "live_dispatch_gate_summary.json"


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
    SnippetCheck("M255-E001-DOC-EXP-01", "# M255 Live Dispatch Gate Contract And Architecture Freeze Expectations (E001)"),
    SnippetCheck("M255-E001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-E001-DOC-EXP-03", "tmp/reports/m255/M255-C004/live_dispatch_cutover_summary.json"),
    SnippetCheck("M255-E001-DOC-EXP-04", "tmp/reports/m255/M255-D004/protocol_category_method_resolution_summary.json"),
    SnippetCheck("M255-E001-DOC-EXP-05", "The gate must explicitly hand off to `M255-E002`."),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-E001-DOC-PKT-01", "# M255-E001 Live Dispatch Gate Contract And Architecture Freeze Packet"),
    SnippetCheck("M255-E001-DOC-PKT-02", "Packet: `M255-E001`"),
    SnippetCheck("M255-E001-DOC-PKT-03", "`live-runtime-dispatch-required-compatibility-shim-evidence-only`"),
    SnippetCheck("M255-E001-DOC-PKT-04", "`M255-E002` is the explicit next handoff"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-E001-NDOC-01", "## Live dispatch gate (M255-E001)"),
    SnippetCheck("M255-E001-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-E001-NDOC-03", f"`{EVIDENCE_MODEL}`"),
    SnippetCheck("M255-E001-NDOC-04", "tmp/reports/m255/M255-E001/live_dispatch_gate_summary.json"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-E001-SPC-01", "## M255 live dispatch gate (E001)"),
    SnippetCheck("M255-E001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-E001-SPC-03", "`objc3_msgsend_i32` remains exported only as compatibility/test evidence"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-E001-META-01", "## M255 live dispatch gate metadata anchors (E001)"),
    SnippetCheck("M255-E001-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-E001-META-03", "tmp/reports/m255/M255-E001/live_dispatch_gate_summary.json"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M255-E001-ARCH-01", "## M255 live dispatch gate (E001)"),
    SnippetCheck("M255-E001-ARCH-02", "`M255-C004` proves live IR emits `objc3_runtime_dispatch_i32`"),
    SnippetCheck("M255-E001-ARCH-03", "`M255-E002` is the first issue allowed to replace shim-based smoke"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-E001-HDR-01", "kObjc3RuntimeLiveDispatchGateContractId"),
    SnippetCheck("M255-E001-HDR-02", f'"{CONTRACT_ID}"'),
    SnippetCheck("M255-E001-HDR-03", "kObjc3RuntimeLiveDispatchGateEvidenceModel"),
    SnippetCheck("M255-E001-HDR-04", "kObjc3RuntimeLiveDispatchGateFailureModel"),
)
IR_SNIPPETS = (
    SnippetCheck("M255-E001-IR-01", "M255-E001 live-dispatch gate anchor"),
    SnippetCheck("M255-E001-IR-02", "supported live sends must"),
    SnippetCheck("M255-E001-IR-03", "compatibility shim remains exported only as evidence/test surface"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-E001-PARSE-01", "M255-E001 live-dispatch gate anchor"),
    SnippetCheck("M255-E001-PARSE-02", "execute through the live runtime path rather than the compatibility shim"),
    SnippetCheck("M255-E001-PARSE-03", "smoke/closeout replacement"),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-E001-SHIM-01", "M255-E001 live-dispatch gate"),
    SnippetCheck("M255-E001-SHIM-02", "authoritative live-dispatch proof."),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M255-E001-RUN-01", "check:objc3c:m255-a002-lane-a-readiness"),
    SnippetCheck("M255-E001-RUN-02", "check:objc3c:m255-b003-lane-b-readiness"),
    SnippetCheck("M255-E001-RUN-03", "check:objc3c:m255-c004-lane-c-readiness"),
    SnippetCheck("M255-E001-RUN-04", "check:objc3c:m255-d004-lane-d-readiness"),
    SnippetCheck("M255-E001-RUN-05", "tests/tooling/test_check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-E001-PKG-01", '"check:objc3c:m255-e001-live-dispatch-gate": "python scripts/check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py"'),
    SnippetCheck("M255-E001-PKG-02", '"test:tooling:m255-e001-live-dispatch-gate": "python -m pytest tests/tooling/test_check_m255_e001_live_dispatch_gate_contract_and_architecture_freeze.py -q"'),
    SnippetCheck("M255-E001-PKG-03", '"check:objc3c:m255-e001-lane-e-readiness": "python scripts/run_m255_e001_lane_e_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--ir-cpp", type=Path, default=DEFAULT_IR_CPP)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER_CPP)
    parser.add_argument("--shim-c", type=Path, default=DEFAULT_SHIM_C)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--a002-summary", type=Path, default=DEFAULT_A002_SUMMARY)
    parser.add_argument("--b003-summary", type=Path, default=DEFAULT_B003_SUMMARY)
    parser.add_argument("--c004-summary", type=Path, default=DEFAULT_C004_SUMMARY)
    parser.add_argument("--d004-summary", type=Path, default=DEFAULT_D004_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M255-E001-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def validate_a002(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M255-E001-A002-01", "A002 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M255-E001-A002-02", "A002 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M255-E001-A002-03", "A002 summary must report dynamic probes executed", failures)

    cases = payload.get("dynamic_cases", [])
    native_case = next((case for case in cases if isinstance(case, dict) and case.get("mode") == "native-llvm-direct"), {})
    surface = native_case.get("surface", {}) if isinstance(native_case, dict) else {}
    checks_total += 1
    checks_passed += require(bool(native_case), artifact, "M255-E001-A002-04", "A002 native-llvm-direct case is missing", failures)
    checks_total += 1
    checks_passed += require(native_case.get("backend") == "llvm-direct", artifact, "M255-E001-A002-05", "A002 native case must preserve llvm-direct", failures)
    checks_total += 1
    checks_passed += require(surface.get("dynamic_dispatch_sites") == 1, artifact, "M255-E001-A002-06", "A002 dynamic dispatch site count drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("direct_dispatch_sites") == 0, artifact, "M255-E001-A002-07", "A002 direct dispatch site count drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("deterministic_handoff") is True, artifact, "M255-E001-A002-08", "A002 deterministic handoff drifted", failures)

    distilled = {
        "ok": True,
        "backend": native_case.get("backend"),
        "dynamic_dispatch_sites": surface.get("dynamic_dispatch_sites"),
        "direct_dispatch_sites": surface.get("direct_dispatch_sites"),
        "dynamic_entrypoint_family": surface.get("dynamic_entrypoint_family"),
    }
    return checks_total, checks_passed, distilled


def validate_b003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M255-E001-B003-01", "B003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("findings") == [], artifact, "M255-E001-B003-02", "B003 findings must remain empty", failures)

    probes = payload.get("dynamic_probes", {})
    positive = probes.get("positive", {}) if isinstance(probes, dict) else {}
    dispatch_surface = positive.get("dispatch_surface", {}) if isinstance(positive, dict) else {}
    runtime_shim_surface = positive.get("runtime_shim_surface", {}) if isinstance(positive, dict) else {}
    checks_total += 1
    checks_passed += require(probes.get("skipped") is False, artifact, "M255-E001-B003-03", "B003 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(dispatch_surface.get("super_dispatch_sites") == 4, artifact, "M255-E001-B003-04", "B003 super dispatch site count drifted", failures)
    checks_total += 1
    checks_passed += require(dispatch_surface.get("dynamic_dispatch_sites") == 3, artifact, "M255-E001-B003-05", "B003 dynamic dispatch site count drifted", failures)
    checks_total += 1
    checks_passed += require(runtime_shim_surface.get("runtime_dispatch_symbol") in {COMPATIBILITY_SYMBOL, CANONICAL_SYMBOL}, artifact, "M255-E001-B003-06", "B003 preserved dispatch symbol drifted outside the shim/live boundary", failures)
    checks_total += 1
    checks_passed += require(runtime_shim_surface.get("runtime_shim_required_sites") == 7, artifact, "M255-E001-B003-07", "B003 shim-era required-site count drifted", failures)

    distilled = {
        "ok": True,
        "super_dispatch_sites": dispatch_surface.get("super_dispatch_sites"),
        "dynamic_dispatch_sites": dispatch_surface.get("dynamic_dispatch_sites"),
        "runtime_dispatch_symbol": runtime_shim_surface.get("runtime_dispatch_symbol"),
        "shim_required_sites": runtime_shim_surface.get("runtime_shim_required_sites"),
    }
    return checks_total, checks_passed, distilled


def validate_c004(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M255-E001-C004-01", "C004 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M255-E001-C004-02", "C004 summary must report full check coverage", failures)
    models = payload.get("models", {})
    checks_total += 1
    checks_passed += require(models.get("canonical_symbol") == CANONICAL_SYMBOL, artifact, "M255-E001-C004-03", "C004 canonical symbol drifted", failures)
    checks_total += 1
    checks_passed += require(models.get("compatibility_symbol") == COMPATIBILITY_SYMBOL, artifact, "M255-E001-C004-04", "C004 compatibility symbol drifted", failures)

    dynamic_probes = payload.get("dynamic_probes", {})
    nil_case = dynamic_probes.get("nil", {})
    dynamic_case = dynamic_probes.get("dynamic", {})
    super_dynamic_case = dynamic_probes.get("super_dynamic", {})
    for prefix, case, expected_calls in (
        ("NIL", nil_case, 1),
        ("DYN", dynamic_case, 1),
        ("SUP", super_dynamic_case, 7),
    ):
        checks_total += 1
        checks_passed += require(case.get("canonical_call_count") == expected_calls, artifact, f"M255-E001-C004-{prefix}-05", f"C004 {prefix.lower()} canonical call count drifted", failures)
        checks_total += 1
        checks_passed += require(case.get("compatibility_call_count") == 0, artifact, f"M255-E001-C004-{prefix}-06", f"C004 {prefix.lower()} compatibility call count drifted", failures)
        checks_total += 1
        checks_passed += require(case.get("compatibility_decl_emitted") is False, artifact, f"M255-E001-C004-{prefix}-07", f"C004 {prefix.lower()} compatibility declaration drifted", failures)
    checks_total += 1
    checks_passed += require(dynamic_case.get("program_exit_code") == dynamic_case.get("expected_exit_code"), artifact, "M255-E001-C004-08", "C004 dynamic execution result drifted", failures)

    distilled = {
        "ok": True,
        "canonical_symbol": models.get("canonical_symbol"),
        "compatibility_symbol": models.get("compatibility_symbol"),
        "nil_canonical_call_count": nil_case.get("canonical_call_count"),
        "dynamic_canonical_call_count": dynamic_case.get("canonical_call_count"),
        "super_dynamic_canonical_call_count": super_dynamic_case.get("canonical_call_count"),
        "compatibility_call_count_zero": all(case.get("compatibility_call_count") == 0 for case in (nil_case, dynamic_case, super_dynamic_case)),
    }
    return checks_total, checks_passed, distilled


def validate_d004(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M255-E001-D004-01", "D004 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M255-E001-D004-02", "D004 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == "objc3c-runtime-protocol-category-method-resolution/m255-d004-v1", artifact, "M255-E001-D004-03", "D004 contract id drifted", failures)

    dynamic_case = payload.get("dynamic_case", {})
    probe = dynamic_case.get("probe_payload", {}) if isinstance(dynamic_case, dict) else {}
    instance_first_state = probe.get("instance_first_state", {}) if isinstance(probe, dict) else {}
    class_self_state = probe.get("class_self_state", {}) if isinstance(probe, dict) else {}
    fallback_first_state = probe.get("fallback_first_state", {}) if isinstance(probe, dict) else {}
    fallback_second_state = probe.get("fallback_second_state", {}) if isinstance(probe, dict) else {}
    checks_total += 1
    checks_passed += require(dynamic_case.get("skipped") is False, artifact, "M255-E001-D004-04", "D004 dynamic case must execute", failures)
    checks_total += 1
    checks_passed += require(probe.get("instance_first") == 33 and probe.get("instance_second") == 33, artifact, "M255-E001-D004-05", "D004 instance category dispatch drifted", failures)
    checks_total += 1
    checks_passed += require(probe.get("class_self") == 44 and probe.get("known_class") == 44, artifact, "M255-E001-D004-06", "D004 class category dispatch drifted", failures)
    checks_total += 1
    checks_passed += require(probe.get("fallback_first") == probe.get("fallback_expected") == probe.get("fallback_second"), artifact, "M255-E001-D004-07", "D004 fallback proof drifted", failures)
    checks_total += 1
    checks_passed += require(instance_first_state.get("last_category_probe_count") == 1 and instance_first_state.get("last_protocol_probe_count") == 0, artifact, "M255-E001-D004-08", "D004 instance probe counts drifted", failures)
    checks_total += 1
    checks_passed += require(class_self_state.get("last_category_probe_count") == 1 and class_self_state.get("last_protocol_probe_count") == 0, artifact, "M255-E001-D004-09", "D004 class probe counts drifted", failures)
    checks_total += 1
    checks_passed += require(fallback_first_state.get("last_category_probe_count") == 1 and fallback_first_state.get("last_protocol_probe_count") == 2, artifact, "M255-E001-D004-10", "D004 fallback probe counts drifted", failures)
    checks_total += 1
    checks_passed += require(fallback_second_state.get("last_dispatch_used_cache") == 1 and fallback_second_state.get("last_dispatch_fell_back") == 1, artifact, "M255-E001-D004-11", "D004 negative cache reuse drifted", failures)

    distilled = {
        "ok": True,
        "instance_first": probe.get("instance_first"),
        "class_self": probe.get("class_self"),
        "fallback_expected": probe.get("fallback_expected"),
        "instance_category_probe_count": instance_first_state.get("last_category_probe_count"),
        "fallback_protocol_probe_count": fallback_first_state.get("last_protocol_probe_count"),
        "fallback_cached": fallback_second_state.get("last_dispatch_used_cache"),
    }
    return checks_total, checks_passed, distilled


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or [])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    doc_checks: tuple[tuple[Path, tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.ir_cpp, IR_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.shim_c, SHIM_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in doc_checks:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    a002_total, a002_passed, a002_summary = validate_a002(args.a002_summary, failures)
    b003_total, b003_passed, b003_summary = validate_b003(args.b003_summary, failures)
    c004_total, c004_passed, c004_summary = validate_c004(args.c004_summary, failures)
    d004_total, d004_passed, d004_summary = validate_d004(args.d004_summary, failures)
    checks_total += a002_total + b003_total + c004_total + d004_total
    checks_passed += a002_passed + b003_passed + c004_passed + d004_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "shim_boundary_model": SHIM_BOUNDARY_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_closeout_issue": NEXT_CLOSEOUT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "findings": [finding.__dict__ for finding in failures],
        "upstream_evidence": {
            "m255_a002": a002_summary,
            "m255_b003": b003_summary,
            "m255_c004": c004_summary,
            "m255_d004": d004_summary,
        },
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
