#!/usr/bin/env python3
"""Validate M259-A002 canonical runnable sample set."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m259-a002-canonical-runnable-sample-set-v1"
CONTRACT_ID = "objc3c-canonical-runnable-sample-set/m259-a002-v1"
EVIDENCE_MODEL = "a001-freeze-plus-live-integrated-runnable-object-property-category-protocol-sample"
SAMPLE_SET_MODEL = "integrated-runnable-sample-set-unifies-alloc-init-protocol-category-and-property-behavior"
FAILURE_MODEL = "fail-closed-on-integrated-runnable-sample-drift-or-missing-live-proof"
NEXT_ISSUE = "M259-B001"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m259" / "M259-A002" / "canonical_runnable_sample_set_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m259" / "a002-canonical-runnable-sample-set"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
CLANGXX_CANDIDATES = ("clang++", "clang++-21")
EXPECTED_BACKEND = "llvm-direct"

A001_SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-A001" / "runnable_sample_surface_contract_summary.json"
A001_CONTRACT_ID = "objc3c-runnable-sample-surface/m259-a001-v1"

FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_a002_canonical_runnable_sample_set.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m259_a002_canonical_runnable_sample_set_probe.cpp"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_a002_canonical_runnable_sample_set_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m259" / "m259_a002_canonical_runnable_sample_set_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
EXECUTION_SMOKE = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
EXECUTION_REPLAY = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m259_a002_lane_a_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m259_a002_canonical_runnable_sample_set.py"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


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
        SnippetCheck("M259-A002-DOC-EXP-01", "# M259 A002 Canonical Runnable Sample Set Expectations"),
        SnippetCheck("M259-A002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M259-A002-DOC-EXP-03", "`Widget -> Tracer` conforms through attachment owner"),
        SnippetCheck("M259-A002-DOC-EXP-04", "The implementation must explicitly hand off to `M259-B001`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M259-A002-DOC-PKT-01", "# M259-A002 Canonical Runnable Sample Set Packet"),
        SnippetCheck("M259-A002-DOC-PKT-02", "Packet: `M259-A002`"),
        SnippetCheck("M259-A002-DOC-PKT-03", "Issue: `#7209`"),
        SnippetCheck("M259-A002-DOC-PKT-04", "Next issue: `M259-B001`."),
    ),
    NATIVE_DOC: (
        SnippetCheck("M259-A002-NDOC-01", "## Canonical runnable sample set (M259-A002)"),
        SnippetCheck("M259-A002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-A002-NDOC-03", "`tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`"),
        SnippetCheck("M259-A002-NDOC-04", "`tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M259-A002-SPC-01", "## M259 canonical runnable sample set (A002)"),
        SnippetCheck("M259-A002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-A002-SPC-03", f"`{SAMPLE_SET_MODEL}`"),
        SnippetCheck("M259-A002-SPC-04", "`M259-B001`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M259-A002-META-01", "## M259 canonical runnable sample set metadata anchors (A002)"),
        SnippetCheck("M259-A002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M259-A002-META-03", "`tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`"),
        SnippetCheck("M259-A002-META-04", "`tmp/reports/m259/M259-A002/canonical_runnable_sample_set_summary.json`"),
    ),
    EXECUTION_SMOKE: (
        SnippetCheck("M259-A002-SMOKE-01", "M259-A002 canonical-runnable-sample-set anchor:"),
        SnippetCheck("M259-A002-SMOKE-02", "integrated object/property/category/protocol sample exists as a dedicated proof asset"),
    ),
    EXECUTION_REPLAY: (
        SnippetCheck("M259-A002-REPLAY-01", "M259-A002 canonical-runnable-sample-set anchor:"),
        SnippetCheck("M259-A002-REPLAY-02", "dedicated canonical sample-set proof remains separate from scalar replay"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M259-A002-PKG-01", '"check:objc3c:m259-a002-canonical-runnable-sample-set"'),
        SnippetCheck("M259-A002-PKG-02", '"test:tooling:m259-a002-canonical-runnable-sample-set"'),
        SnippetCheck("M259-A002-PKG-03", '"check:objc3c:m259-a002-lane-a-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M259-A002-RUN-01", "check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"),
        SnippetCheck("M259-A002-RUN-02", "check_m259_a002_canonical_runnable_sample_set.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M259-A002-TEST-01", "def test_checker_passes_static(tmp_path: Path) -> None:"),
        SnippetCheck("M259-A002-TEST-02", CONTRACT_ID),
    ),
    FIXTURE: (
        SnippetCheck("M259-A002-FIXTURE-01", "module canonicalRunnableSampleSet;"),
        SnippetCheck("M259-A002-FIXTURE-02", "@property (assign, setter=setCount:) i32 count;"),
        SnippetCheck("M259-A002-FIXTURE-03", "@interface Widget (Tracing) <Tracer>"),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M259-A002-PROBE-01", 'objc3_runtime_copy_realized_class_entry_for_testing("Widget"'),
        SnippetCheck("M259-A002-PROBE-02", 'objc3_runtime_copy_protocol_conformance_query_for_testing('),
        SnippetCheck("M259-A002-PROBE-03", 'objc3_runtime_copy_property_entry_for_testing("Widget", "count"'),
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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def resolve_tool(candidates: Sequence[str]) -> str | None:
    for candidate in candidates:
        direct = shutil.which(candidate)
        if direct:
            return direct
        if sys.platform == "win32":
            llvm_bin = Path("C:/Program Files/LLVM/bin")
            candidate_path = llvm_bin / candidate
            if candidate_path.exists():
                return str(candidate_path)
            if not candidate.endswith(".exe"):
                exe_candidate = llvm_bin / f"{candidate}.exe"
                if exe_candidate.exists():
                    return str(exe_candidate)
    return None


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def ensure_binaries(failures: list[Finding]) -> int:
    checks_total = 0
    if NATIVE_EXE.exists() and RUNTIME_LIBRARY.exists():
        checks_total += require(True, display_path(NATIVE_EXE), "M259-A002-BIN-READY", "native binary present", failures)
        checks_total += require(True, display_path(RUNTIME_LIBRARY), "M259-A002-LIB-READY", "runtime library present", failures)
        return checks_total
    build = run_command([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    output = (build.stdout or "") + (build.stderr or "")
    checks_total += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M259-A002-BUILD", output.strip() or "native build failed", failures)
    checks_total += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M259-A002-NATIVE-EXISTS", "native binary missing after build", failures)
    checks_total += require(RUNTIME_LIBRARY.exists(), display_path(RUNTIME_LIBRARY), "M259-A002-LIB-EXISTS", "runtime library missing after build", failures)
    return checks_total


def compile_fixture(out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(NATIVE_EXE),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    return run_command(command)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--skip-dynamic-probes",
        action="store_true",
        help="Validate only static contract/doc wiring.",
    )
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=SUMMARY_OUT,
        help="Summary output path.",
    )
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    static_contracts: dict[str, dict[str, Any]] = {}

    for path, snippets in STATIC_SNIPPETS.items():
        total, findings = check_static_contract(path, snippets)
        checks_total += total
        static_contracts[display_path(path)] = {"checks": total, "ok": len(findings) == 0}
        failures.extend(findings)

    a001_summary = load_json(A001_SUMMARY)
    checks_total += require(a001_summary.get("contract_id") == A001_CONTRACT_ID, display_path(A001_SUMMARY), "M259-A002-A001-CONTRACT", "M259-A001 contract id drift", failures)
    checks_total += require(a001_summary.get("ok") is True, display_path(A001_SUMMARY), "M259-A002-A001-OK", "M259-A001 summary must remain green", failures)

    dynamic_probes: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        checks_total += ensure_binaries(failures)
        clangxx = resolve_tool(CLANGXX_CANDIDATES)
        checks_total += require(clangxx is not None, "clang++", "M259-A002-CLANGXX", f"unable to resolve any clang++ candidate from {CLANGXX_CANDIDATES}", failures)

        probe_dir = PROBE_ROOT / f"probe-{uuid.uuid4().hex}"
        compile_result = compile_fixture(probe_dir)
        module_obj = probe_dir / "module.obj"
        module_ir = probe_dir / "module.ll"
        module_backend = probe_dir / "module.object-backend.txt"
        runtime_manifest = probe_dir / "module.runtime-registration-manifest.json"

        checks_total += require(compile_result.returncode == 0, display_path(FIXTURE), "M259-A002-COMPILE", f"fixture compile failed: {compile_result.stdout}{compile_result.stderr}", failures)
        checks_total += require(module_obj.exists(), display_path(module_obj), "M259-A002-OBJ", "missing emitted object", failures)
        checks_total += require(module_ir.exists(), display_path(module_ir), "M259-A002-IR", "missing emitted LLVM IR", failures)
        checks_total += require(module_backend.exists(), display_path(module_backend), "M259-A002-BACKEND-FILE", "missing backend marker", failures)
        checks_total += require(runtime_manifest.exists(), display_path(runtime_manifest), "M259-A002-MANIFEST", "missing runtime registration manifest", failures)

        probe_payload: dict[str, Any] = {"skipped": True}
        runtime_manifest_payload: dict[str, Any] = {}
        if compile_result.returncode == 0 and module_obj.exists() and module_ir.exists() and module_backend.exists() and runtime_manifest.exists() and clangxx is not None:
            backend_text = module_backend.read_text(encoding="utf-8").strip()
            checks_total += require(backend_text == EXPECTED_BACKEND, display_path(module_backend), "M259-A002-BACKEND", f"backend must remain {EXPECTED_BACKEND!r}, saw {backend_text!r}", failures)

            runtime_manifest_payload = load_json(runtime_manifest)
            checks_total += require(runtime_manifest_payload.get("class_descriptor_count") == 4, display_path(runtime_manifest), "M259-A002-MANIFEST-CLASS", "runtime manifest must preserve four class descriptors", failures)
            checks_total += require(runtime_manifest_payload.get("protocol_descriptor_count") == 2, display_path(runtime_manifest), "M259-A002-MANIFEST-PROTOCOL", "runtime manifest must preserve two protocol descriptors", failures)
            checks_total += require(runtime_manifest_payload.get("category_descriptor_count") == 2, display_path(runtime_manifest), "M259-A002-MANIFEST-CATEGORY", "runtime manifest must preserve two category descriptors", failures)
            checks_total += require(runtime_manifest_payload.get("property_descriptor_count") == 8, display_path(runtime_manifest), "M259-A002-MANIFEST-PROPERTY", "runtime manifest must preserve eight property descriptors", failures)
            checks_total += require(runtime_manifest_payload.get("ivar_descriptor_count") == 4, display_path(runtime_manifest), "M259-A002-MANIFEST-IVAR", "runtime manifest must preserve four ivar descriptors", failures)

            probe_exe = probe_dir / "m259_a002_canonical_runnable_sample_set_probe.exe"
            probe_compile = run_command([
                str(clangxx),
                "-std=c++20",
                "-I",
                str(RUNTIME_INCLUDE_ROOT),
                str(RUNTIME_PROBE),
                str(module_obj),
                str(RUNTIME_LIBRARY),
                "-o",
                str(probe_exe),
            ])
            checks_total += require(probe_compile.returncode == 0, display_path(RUNTIME_PROBE), "M259-A002-PROBE-COMPILE", f"probe compile failed: {probe_compile.stdout}{probe_compile.stderr}", failures)

            if probe_compile.returncode == 0:
                probe_run = run_command([str(probe_exe)])
                checks_total += require(probe_run.returncode == 0, display_path(probe_exe), "M259-A002-PROBE-RUN", f"probe execution failed: {probe_run.stdout}{probe_run.stderr}", failures)
                if probe_run.returncode == 0:
                    probe_payload = json.loads(probe_run.stdout)
                    widget_entry = probe_payload.get("widget_entry", {})
                    worker_query = probe_payload.get("worker_query", {})
                    tracer_query = probe_payload.get("tracer_query", {})
                    count_property = probe_payload.get("count_property", {})
                    value_property = probe_payload.get("value_property", {})
                    token_property = probe_payload.get("token_property", {})

                    checks_total += require(widget_entry.get("found") == 1, display_path(RUNTIME_PROBE), "M259-A002-PAY-01", "Widget must realize successfully", failures)
                    checks_total += require(widget_entry.get("base_identity") == 1041, display_path(RUNTIME_PROBE), "M259-A002-PAY-02", "Widget base identity must remain 1041 for the canonical sample", failures)
                    checks_total += require(widget_entry.get("runtime_property_accessor_count") == 4, display_path(RUNTIME_PROBE), "M259-A002-PAY-03", "Widget must expose four runtime property accessors", failures)
                    checks_total += require(widget_entry.get("runtime_instance_size_bytes") == 24, display_path(RUNTIME_PROBE), "M259-A002-PAY-04", "Widget instance size must remain 24 bytes", failures)
                    checks_total += require(widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)", display_path(RUNTIME_PROBE), "M259-A002-PAY-05", "Widget must preserve the attached Tracing category owner", failures)

                    checks_total += require(probe_payload.get("init_value", 0) != 0, display_path(RUNTIME_PROBE), "M259-A002-PAY-06", "alloc/init must return a non-zero instance receiver", failures)
                    checks_total += require(probe_payload.get("traced_value") == 13, display_path(RUNTIME_PROBE), "M259-A002-PAY-07", "tracedValue must return 13", failures)
                    checks_total += require(probe_payload.get("inherited_value") == 7, display_path(RUNTIME_PROBE), "M259-A002-PAY-08", "inheritedValue must return 7", failures)
                    checks_total += require(probe_payload.get("class_value") == 11, display_path(RUNTIME_PROBE), "M259-A002-PAY-09", "classValue must return 11", failures)
                    checks_total += require(probe_payload.get("shared_value") == 19, display_path(RUNTIME_PROBE), "M259-A002-PAY-10", "shared must return 19", failures)
                    checks_total += require(probe_payload.get("count_value") == 37, display_path(RUNTIME_PROBE), "M259-A002-PAY-11", "count must reload 37", failures)
                    checks_total += require(probe_payload.get("enabled_value") == 1, display_path(RUNTIME_PROBE), "M259-A002-PAY-12", "enabled must reload 1", failures)
                    checks_total += require(probe_payload.get("current_value") == 55, display_path(RUNTIME_PROBE), "M259-A002-PAY-13", "currentValue must reload 55", failures)
                    checks_total += require(probe_payload.get("token_value") == 0, display_path(RUNTIME_PROBE), "M259-A002-PAY-14", "tokenValue must remain 0", failures)

                    checks_total += require(worker_query.get("conforms") == 1, display_path(RUNTIME_PROBE), "M259-A002-PAY-15", "Widget -> Worker must conform", failures)
                    checks_total += require(worker_query.get("matched_protocol_owner_identity") in {"protocol:Worker", "protocol:Tracer"}, display_path(RUNTIME_PROBE), "M259-A002-PAY-16", "Worker query must resolve through Worker or inherited Tracer", failures)
                    checks_total += require(worker_query.get("matched_attachment_owner_identity") is None, display_path(RUNTIME_PROBE), "M259-A002-PAY-17", "Worker query must not require attachment owner match", failures)
                    checks_total += require(tracer_query.get("conforms") == 1, display_path(RUNTIME_PROBE), "M259-A002-PAY-18", "Widget -> Tracer must conform", failures)
                    checks_total += require(tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", display_path(RUNTIME_PROBE), "M259-A002-PAY-19", "Tracer query must resolve through protocol:Tracer", failures)
                    checks_total += require(tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)", display_path(RUNTIME_PROBE), "M259-A002-PAY-20", "Tracer query must resolve through the attached category owner", failures)

                    checks_total += require(count_property.get("found") == 1 and count_property.get("slot_index") == 0 and count_property.get("offset_bytes") == 0 and count_property.get("size_bytes") == 4 and count_property.get("getter_owner_identity") == "implementation:Widget::instance_method:count" and count_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCount:", display_path(RUNTIME_PROBE), "M259-A002-PAY-21", "count property reflection must preserve slot/layout/accessor facts", failures)
                    checks_total += require(value_property.get("found") == 1 and value_property.get("slot_index") == 2 and value_property.get("offset_bytes") == 8 and value_property.get("size_bytes") == 8 and value_property.get("getter_owner_identity") == "implementation:Widget::instance_method:currentValue" and value_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCurrentValue:", display_path(RUNTIME_PROBE), "M259-A002-PAY-22", "value property reflection must preserve slot/layout/accessor facts", failures)
                    checks_total += require(token_property.get("found") == 1 and token_property.get("slot_index") == 3 and token_property.get("offset_bytes") == 16 and token_property.get("setter_available") == 0 and token_property.get("getter_owner_identity") == "implementation:Widget::instance_method:tokenValue" and token_property.get("setter_owner_identity") is None, display_path(RUNTIME_PROBE), "M259-A002-PAY-23", "token property reflection must preserve readonly slot/layout/accessor facts", failures)

        dynamic_probes = {
            "skipped": False,
            "probe_dir": display_path(probe_dir),
            "compile_stdout": compile_result.stdout,
            "compile_stderr": compile_result.stderr,
            "runtime_manifest": runtime_manifest_payload,
            "probe_payload": probe_payload,
        }

    checks_passed = checks_total - len(failures)
    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "evidence_model": EVIDENCE_MODEL,
        "sample_set_model": SAMPLE_SET_MODEL,
        "failure_model": FAILURE_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "failures": [finding.__dict__ for finding in failures],
        "static_contracts": static_contracts,
        "dependency": {
            "M259-A001": {
                "summary": display_path(A001_SUMMARY),
                "contract_id": a001_summary.get("contract_id"),
                "ok": a001_summary.get("ok"),
            }
        },
        "dynamic_probes": dynamic_probes,
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
