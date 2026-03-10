#!/usr/bin/env python3
"""Validate M256-E002 runnable class/protocol/category execution matrix."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m256-e002-runnable-class-protocol-category-execution-matrix-cross-lane-integration-sync-v1"
CONTRACT_ID = "objc3c-runnable-class-protocol-category-execution-matrix/m256-e002-v1"
EVIDENCE_MODEL = "a003-b004-c003-d004-e001-summary-chain-plus-live-inheritance-execution"
EXECUTION_MATRIX_MODEL = (
    "runnable-class-protocol-category-matrix-composes-upstream-summaries-with-live-inheritance-and-runtime-dispatch-proof"
)
FAILURE_MODEL = "fail-closed-on-runnable-object-matrix-drift-or-missing-live-runtime-proof"
NEXT_ISSUE = "M257-A001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m256" / "M256-E002" / "runnable_class_protocol_category_execution_matrix_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
CLANGXX_CANDIDATES = (
    "clang++",
    "clang++-21",
)
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m256" / "e002-runnable-execution-matrix"
INHERITANCE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m256_inheritance_override_realization_positive.objc3"
A003_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-A003" / "protocol_category_source_surface_completion_for_executable_runtime_summary.json"
B004_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-B004" / "inheritance_override_realization_legality_summary.json"
C003_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-C003" / "realization_records_summary.json"
D004_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-D004" / "canonical_runnable_object_sample_support_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-E001" / "class_protocol_category_conformance_gate_summary.json"
A003_CONTRACT_ID = "objc3c-executable-protocol-category-source-closure/m256-a003-v1"
B004_CONTRACT_ID = "objc3c-inheritance-override-realization-legality/m256-b004-v1"
C003_CONTRACT_ID = "objc3c-executable-realization-records/m256-c003-v1"
D004_CONTRACT_ID = "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1"
E001_CONTRACT_ID = "objc3c-executable-class-protocol-category-conformance-gate/m256-e001-v1"
EXPECTED_EXIT_CODE = 4

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m256_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync_e002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m256" / "m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m256_e002_lane_e_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py"


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
        SnippetCheck("M256-E002-DOC-EXP-01", "# M256 Runnable Class, Protocol, And Category Execution Matrix Cross-Lane Integration Sync Expectations (E002)"),
        SnippetCheck("M256-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M256-E002-DOC-EXP-03", "The linked executable must return exit code `4`."),
        SnippetCheck("M256-E002-DOC-EXP-04", "The matrix must explicitly hand off to `M257-A001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M256-E002-DOC-PKT-01", "# M256-E002 Runnable Class, Protocol, And Category Execution Matrix Cross-Lane Integration Sync Packet"),
        SnippetCheck("M256-E002-DOC-PKT-02", "Packet: `M256-E002`"),
        SnippetCheck("M256-E002-DOC-PKT-03", "Issue: `#7144`"),
        SnippetCheck("M256-E002-DOC-PKT-04", "- `M256-E001`"),
        SnippetCheck("M256-E002-DOC-PKT-05", "`M257-A001` is the explicit next handoff after this matrix closes."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M256-E002-NDOC-01", "## Runnable class, protocol, and category execution matrix (M256-E002)"),
        SnippetCheck("M256-E002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-E002-NDOC-03", f"`{EVIDENCE_MODEL}`"),
        SnippetCheck("M256-E002-NDOC-04", "tmp/reports/m256/M256-E002/runnable_class_protocol_category_execution_matrix_summary.json"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M256-E002-SPC-01", "## M256 runnable class/protocol/category execution matrix (E002)"),
        SnippetCheck("M256-E002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-E002-SPC-03", f"`{EXECUTION_MATRIX_MODEL}`"),
        SnippetCheck("M256-E002-SPC-04", "`M257-A001`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M256-E002-META-01", "## M256 runnable class/protocol/category execution matrix metadata anchors (E002)"),
        SnippetCheck("M256-E002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M256-E002-META-03", "`tmp/reports/m256/M256-E002/runnable_class_protocol_category_execution_matrix_summary.json`"),
        SnippetCheck("M256-E002-META-04", "`tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M256-E002-ARCH-01", "## M256 runnable class/protocol/category execution matrix (E002)"),
        SnippetCheck("M256-E002-ARCH-02", "`M256-E002` broadens the frozen `M256-E001` gate into the first live runnable execution matrix:"),
        SnippetCheck("M256-E002-ARCH-03", "check:objc3c:m256-e002-lane-e-readiness"),
    ),
    PARSER_CPP: (
        SnippetCheck("M256-E002-PARSE-01", "M256-E002 runnable class-protocol-category execution-matrix anchor"),
    ),
    SEMA_CPP: (
        SnippetCheck("M256-E002-SEMA-01", "M256-E002 runnable class-protocol-category execution-matrix anchor"),
    ),
    IR_CPP: (
        SnippetCheck("M256-E002-IR-01", "M256-E002 runnable class-protocol-category execution-matrix anchor"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M256-E002-PKG-01", '"check:objc3c:m256-e002-runnable-class-protocol-and-category-execution-matrix": "python scripts/check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py"'),
        SnippetCheck("M256-E002-PKG-02", '"test:tooling:m256-e002-runnable-class-protocol-and-category-execution-matrix": "python -m pytest tests/tooling/test_check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py -q"'),
        SnippetCheck("M256-E002-PKG-03", '"check:objc3c:m256-e002-lane-e-readiness": "python scripts/run_m256_e002_lane_e_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M256-E002-RUN-01", "check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py"),
        SnippetCheck("M256-E002-RUN-02", "check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py"),
        SnippetCheck("M256-E002-RUN-03", "check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py"),
        SnippetCheck("M256-E002-RUN-04", "check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py"),
        SnippetCheck("M256-E002-RUN-05", "check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py"),
        SnippetCheck("M256-E002-RUN-06", "check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M256-E002-TEST-01", "def test_checker_passes() -> None:"),
        SnippetCheck("M256-E002-TEST-02", CONTRACT_ID),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


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


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def validate_a003(payload: dict[str, Any], failures: list[Finding]) -> int:
    artifact = display_path(A003_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M256-E002-A003-01", "A003 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == A003_CONTRACT_ID, artifact, "M256-E002-A003-02", "A003 contract id drifted", failures)
    checks_total += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M256-E002-A003-03", "A003 summary must report full coverage", failures)
    dynamic_probe = payload.get("dynamic_probe")
    checks_total += require(isinstance(dynamic_probe, dict) and bool(dynamic_probe), artifact, "M256-E002-A003-04", "A003 dynamic probe must remain populated", failures)
    if isinstance(dynamic_probe, dict):
        checks_total += require(str(dynamic_probe.get("fixture", "")).endswith("m251_runtime_metadata_source_records_category_protocol_property.objc3"), artifact, "M256-E002-A003-05", "A003 fixture drifted", failures)
        checks_total += require(str(dynamic_probe.get("manifest_path", "")).endswith("module.manifest.json"), artifact, "M256-E002-A003-06", "A003 manifest path drifted", failures)
    return checks_total


def validate_b004(payload: dict[str, Any], failures: list[Finding], *, require_dynamic: bool) -> int:
    artifact = display_path(B004_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M256-E002-B004-01", "B004 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == B004_CONTRACT_ID, artifact, "M256-E002-B004-02", "B004 contract id drifted", failures)
    if require_dynamic:
        checks_total += require(payload.get("dynamic_probes_executed") is True, artifact, "M256-E002-B004-03", "B004 must replay dynamic probes for E002", failures)
        positive_case = payload.get("positive_case")
        checks_total += require(isinstance(positive_case, dict) and bool(positive_case), artifact, "M256-E002-B004-04", "B004 positive_case payload must remain populated", failures)
        if isinstance(positive_case, dict):
            checks_total += require(positive_case.get("returncode") == 0, artifact, "M256-E002-B004-05", "B004 positive inheritance case must compile successfully", failures)
            checks_total += require(positive_case.get("backend_text") == "llvm-direct", artifact, "M256-E002-B004-06", "B004 positive case must stay on llvm-direct", failures)
            checks_total += require(positive_case.get("manifest_exists") is True, artifact, "M256-E002-B004-07", "B004 positive manifest must exist", failures)
    return checks_total


def validate_c003(payload: dict[str, Any], failures: list[Finding], *, require_dynamic: bool) -> int:
    artifact = display_path(C003_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M256-E002-C003-01", "C003 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == C003_CONTRACT_ID, artifact, "M256-E002-C003-02", "C003 contract id drifted", failures)
    if require_dynamic:
        checks_total += require(payload.get("dynamic_probes_executed") is True, artifact, "M256-E002-C003-03", "C003 must replay dynamic probes for E002", failures)
        dynamic_cases = payload.get("dynamic_cases")
        checks_total += require(isinstance(dynamic_cases, list) and len(dynamic_cases) == 3, artifact, "M256-E002-C003-04", "C003 must preserve three dynamic realization cases", failures)
        if isinstance(dynamic_cases, list):
            section_names = {str(case.get("section_name", "")) for case in dynamic_cases if isinstance(case, dict)}
            checks_total += require(section_names == {"objc3.runtime.class_descriptors", "objc3.runtime.protocol_descriptors", "objc3.runtime.category_descriptors"}, artifact, "M256-E002-C003-05", "C003 section inventory drifted", failures)
    return checks_total


def validate_d004(payload: dict[str, Any], failures: list[Finding], *, require_dynamic: bool) -> int:
    artifact = display_path(D004_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M256-E002-D004-01", "D004 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == D004_CONTRACT_ID, artifact, "M256-E002-D004-02", "D004 contract id drifted", failures)
    if require_dynamic:
        checks_total += require(payload.get("dynamic_probes_executed") is True, artifact, "M256-E002-D004-03", "D004 must replay dynamic probes for E002", failures)
        dynamic = payload.get("dynamic")
        checks_total += require(isinstance(dynamic, dict) and dynamic.get("skipped") is False, artifact, "M256-E002-D004-04", "D004 dynamic payload must be live rather than skipped", failures)
        if isinstance(dynamic, dict):
            checks_total += require(dynamic.get("sample_backend") == "llvm-direct", artifact, "M256-E002-D004-05", "D004 sample backend must stay llvm-direct", failures)
            checks_total += require(dynamic.get("runtime_backend") == "llvm-direct", artifact, "M256-E002-D004-06", "D004 runtime backend must stay llvm-direct", failures)
            checks_total += require(dynamic.get("sample_exit_code") == 37, artifact, "M256-E002-D004-07", "D004 canonical sample exit code drifted", failures)
            probe_payload = dynamic.get("probe_payload")
            checks_total += require(isinstance(probe_payload, dict), artifact, "M256-E002-D004-08", "D004 probe payload must remain an object", failures)
            if isinstance(probe_payload, dict):
                graph_state = probe_payload.get("graph_state", {})
                worker_query = probe_payload.get("worker_query", {})
                tracer_query = probe_payload.get("tracer_query", {})
                checks_total += require(probe_payload.get("traced_value") == 13, artifact, "M256-E002-D004-09", "D004 traced category dispatch drifted", failures)
                checks_total += require(probe_payload.get("inherited_value") == 7, artifact, "M256-E002-D004-10", "D004 inherited dispatch drifted", failures)
                checks_total += require(probe_payload.get("class_value") == 11, artifact, "M256-E002-D004-11", "D004 class dispatch drifted", failures)
                checks_total += require(graph_state.get("attached_category_count") == 1, artifact, "M256-E002-D004-12", "D004 attached-category count drifted", failures)
                checks_total += require(graph_state.get("realized_class_count") == 2, artifact, "M256-E002-D004-13", "D004 realized-class count drifted", failures)
                checks_total += require(worker_query.get("conforms") == 1, artifact, "M256-E002-D004-14", "D004 Worker conformance drifted", failures)
                checks_total += require(tracer_query.get("conforms") == 1, artifact, "M256-E002-D004-15", "D004 Tracer conformance drifted", failures)
                checks_total += require(tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)", artifact, "M256-E002-D004-16", "D004 attachment owner drifted", failures)
    return checks_total


def validate_e001(payload: dict[str, Any], failures: list[Finding]) -> int:
    artifact = display_path(E001_SUMMARY)
    checks_total = 0
    checks_total += require(payload.get("ok") is True, artifact, "M256-E002-E001-01", "E001 summary must report ok=true", failures)
    checks_total += require(payload.get("contract_id") == E001_CONTRACT_ID, artifact, "M256-E002-E001-02", "E001 contract id drifted", failures)
    checks_total += require(payload.get("next_closeout_issue") == "M256-E002", artifact, "M256-E002-E001-03", "E001 must still hand off to M256-E002", failures)
    return checks_total


def resolve_clangxx() -> str | None:
    for name in CLANGXX_CANDIDATES:
        resolved = shutil.which(name)
        if resolved:
            return resolved
    if sys.platform == "win32":
        candidate = Path("C:/Program Files/LLVM/bin/clang++.exe")
        if candidate.exists():
            return str(candidate)
    return None


def run_matrix_case(failures: list[Finding]) -> tuple[int, dict[str, Any]]:
    checks_total = 0
    payload: dict[str, Any] = {}
    out_dir = PROBE_ROOT / "inheritance-positive"
    out_dir.mkdir(parents=True, exist_ok=True)

    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M256-E002-MATRIX-01", "native binary is missing", failures)
    checks_total += require(RUNTIME_LIBRARY.exists(), display_path(RUNTIME_LIBRARY), "M256-E002-MATRIX-02", "runtime library is missing", failures)
    checks_total += require(INHERITANCE_FIXTURE.exists(), display_path(INHERITANCE_FIXTURE), "M256-E002-MATRIX-03", "inheritance fixture is missing", failures)
    clangxx = resolve_clangxx()
    checks_total += require(clangxx is not None, "clang++", "M256-E002-MATRIX-04", "clang++ is unavailable", failures)
    if failures:
        return checks_total, payload

    compile_result = run_process([
        str(NATIVE_EXE),
        str(INHERITANCE_FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])
    manifest_path = out_dir / "module.manifest.json"
    runtime_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    exe_path = out_dir / "m256_e002_inheritance_positive.exe"

    checks_total += require(compile_result.returncode == 0, display_path(out_dir), "M256-E002-MATRIX-05", f"inheritance matrix compile failed: {compile_result.stdout}{compile_result.stderr}", failures)
    for check_id, path in (
        ("M256-E002-MATRIX-06", manifest_path),
        ("M256-E002-MATRIX-07", runtime_manifest_path),
        ("M256-E002-MATRIX-08", ir_path),
        ("M256-E002-MATRIX-09", obj_path),
        ("M256-E002-MATRIX-10", backend_path),
    ):
        checks_total += require(path.exists(), display_path(path), check_id, f"missing matrix artifact: {display_path(path)}", failures)
    if compile_result.returncode != 0 or not all(path.exists() for path in (manifest_path, runtime_manifest_path, ir_path, obj_path, backend_path)):
        return checks_total, {
            "out_dir": display_path(out_dir),
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
        }

    manifest = load_json(manifest_path)
    runtime_manifest = load_json(runtime_manifest_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M256-E002-MATRIX-11", "matrix case must stay on llvm-direct", failures)
    checks_total += require(len(manifest.get("interfaces", [])) == 2 and len(manifest.get("implementations", [])) == 2, display_path(manifest_path), "M256-E002-MATRIX-12", "inheritance matrix manifest must preserve two interfaces and two implementations", failures)
    checks_total += require(runtime_manifest.get("class_descriptor_count") == 4, display_path(runtime_manifest_path), "M256-E002-MATRIX-13", "matrix case must preserve four class descriptors", failures)

    link_result = run_process([
        str(clangxx),
        str(obj_path),
        str(RUNTIME_LIBRARY),
        "-o",
        str(exe_path),
    ])
    checks_total += require(link_result.returncode == 0, display_path(exe_path), "M256-E002-MATRIX-14", f"inheritance matrix link failed: {link_result.stdout}{link_result.stderr}", failures)
    if link_result.returncode != 0:
        return checks_total, {
            "out_dir": display_path(out_dir),
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
            "link_stdout": link_result.stdout,
            "link_stderr": link_result.stderr,
            "backend_text": backend_text,
        }

    run_result = run_process([str(exe_path)])
    checks_total += require(run_result.returncode == EXPECTED_EXIT_CODE, display_path(exe_path), "M256-E002-MATRIX-15", f"inheritance matrix executable must exit with {EXPECTED_EXIT_CODE}, saw {run_result.returncode}", failures)

    payload = {
        "fixture": display_path(INHERITANCE_FIXTURE),
        "out_dir": display_path(out_dir),
        "module_manifest": display_path(manifest_path),
        "module_runtime_registration_manifest": display_path(runtime_manifest_path),
        "module_ir": display_path(ir_path),
        "module_obj": display_path(obj_path),
        "linked_executable": display_path(exe_path),
        "backend_text": backend_text,
        "compile_returncode": compile_result.returncode,
        "link_returncode": link_result.returncode,
        "run_returncode": run_result.returncode,
        "manifest": manifest,
        "runtime_registration_manifest": runtime_manifest,
        "compile_stdout": compile_result.stdout,
        "compile_stderr": compile_result.stderr,
        "link_stdout": link_result.stdout,
        "link_stderr": link_result.stderr,
        "run_stdout": run_result.stdout,
        "run_stderr": run_result.stderr,
    }
    return checks_total, payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, snippets in STATIC_SNIPPETS.items():
        count, findings = check_static_contract(path, snippets)
        checks_total += count
        failures.extend(findings)

    upstream_payloads: dict[str, Any] = {}
    for path in (A003_SUMMARY, B004_SUMMARY, C003_SUMMARY, D004_SUMMARY, E001_SUMMARY):
        checks_total += require(path.exists(), display_path(path), f"M256-E002-SUMMARY-{path.stem}", f"required summary is missing: {display_path(path)}", failures)
    if not failures:
        upstream_payloads = {
            "m256_a003": load_json(A003_SUMMARY),
            "m256_b004": load_json(B004_SUMMARY),
            "m256_c003": load_json(C003_SUMMARY),
            "m256_d004": load_json(D004_SUMMARY),
            "m256_e001": load_json(E001_SUMMARY),
        }
        checks_total += validate_a003(upstream_payloads["m256_a003"], failures)
        require_dynamic = not args.skip_dynamic_probes
        checks_total += validate_b004(upstream_payloads["m256_b004"], failures, require_dynamic=require_dynamic)
        checks_total += validate_c003(upstream_payloads["m256_c003"], failures, require_dynamic=require_dynamic)
        checks_total += validate_d004(upstream_payloads["m256_d004"], failures, require_dynamic=require_dynamic)
        checks_total += validate_e001(upstream_payloads["m256_e001"], failures)

    matrix_case: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        matrix_checks, matrix_case = run_matrix_case(failures)
        checks_total += matrix_checks

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "evidence_model": EVIDENCE_MODEL,
        "execution_matrix_model": EXECUTION_MATRIX_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "upstream_evidence": {
            key: {
                subkey: value
                for subkey, value in payload.items()
                if subkey in {
                    "contract_id",
                    "previous_contract_id",
                    "dynamic_probes_executed",
                    "dynamic_probe",
                    "dynamic_cases",
                    "dynamic",
                    "next_closeout_issue",
                    "ok",
                    "checks_total",
                    "checks_passed",
                }
            }
            for key, payload in upstream_payloads.items()
        },
        "execution_matrix_case": matrix_case,
        "failures": [finding.__dict__ for finding in failures],
    }
    SUMMARY_OUT.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_OUT.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        json.dump(summary_payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1

    json.dump(summary_payload, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
