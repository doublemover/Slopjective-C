#!/usr/bin/env python3
"""Fail-closed contract checker for M253-C005 selector/string pool emission."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m253-c005-selector-and-string-pool-emission-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-runtime-selector-string-pool-emission/m253-c005-v1"
BOUNDARY_COMMENT_PREFIX = (
    "; runtime_metadata_selector_string_pool_emission = contract="
    "objc3c-runtime-selector-string-pool-emission/m253-c005-v1"
)
NAMED_METADATA_LINE = '!objc3.objc_runtime_selector_string_pool_emission = !{!59}'
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m253_selector_and_string_pool_emission_core_feature_expansion_c005_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m253" / "m253_c005_selector_and_string_pool_emission_core_feature_expansion_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_METADATA_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
DEFAULT_MESSAGE_SEND_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive" / "message_send_runtime_shim.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m253" / "selector-string-pool"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m253/M253-C005/selector_string_pool_emission_summary.json")
DEFAULT_LLVM_READOBJ = "llvm-readobj"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class ProbeSpec:
    case_id: str
    fixture: Path
    probe_subdir: str
    expected_selector_pool_count: int
    expected_string_pool_count: int
    symbol_tokens: tuple[str, ...]
    boundary_tokens: tuple[str, ...]
    section_expectations: tuple[tuple[str, int, int], ...]


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-DOC-EXP-01", "# M253 Selector And String Pool Emission Core Feature Expansion Expectations (C005)"),
    SnippetCheck("M253-C005-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M253-C005-DOC-EXP-03", "`@__objc3_sec_selector_pool`"),
    SnippetCheck("M253-C005-DOC-EXP-04", "`@__objc3_str_pool_0000`"),
    SnippetCheck("M253-C005-DOC-EXP-05", "`@.objc3.sel.`"),
    SnippetCheck("M253-C005-DOC-EXP-06", "`tmp/reports/m253/M253-C005/selector_string_pool_emission_summary.json`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-DOC-PKT-01", "# M253-C005 Selector And String Pool Emission Core Feature Expansion Packet"),
    SnippetCheck("M253-C005-DOC-PKT-02", "Packet: `M253-C005`"),
    SnippetCheck("M253-C005-DOC-PKT-03", "- `M253-C004`"),
    SnippetCheck("M253-C005-DOC-PKT-04", "canonical-selector-cstring-pool-with-stable-ordinal-aggregate"),
    SnippetCheck("M253-C005-DOC-PKT-05", "tests/tooling/fixtures/native/execution/positive/message_send_runtime_shim.objc3"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-ARCH-01", "M253 lane-C C005 selector/string pool emission anchors explicit"),
    SnippetCheck("M253-C005-ARCH-02", "canonical selector and string pool sections"),
    SnippetCheck("M253-C005-ARCH-03", "descriptor bundles remain shape-stable"),
)

NATIVE_DOC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-NDOC-01", "## Selector and string pool emission (M253-C005)"),
    SnippetCheck("M253-C005-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C005-NDOC-03", "`!objc3.objc_runtime_selector_string_pool_emission`"),
    SnippetCheck("M253-C005-NDOC-04", "`@__objc3_sec_string_pool`"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-SPC-01", "## M253 selector/string pool emission (C005)"),
    SnippetCheck("M253-C005-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C005-SPC-03", "canonical-runtime-string-cstring-pool-with-stable-ordinal-aggregate"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-META-01", "## M253 selector/string pool metadata anchors (C005)"),
    SnippetCheck("M253-C005-META-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M253-C005-META-03", "`__objc3_sec_selector_pool`"),
    SnippetCheck("M253-C005-META-04", "`__objc3_str_pool_0000`"),
)

LOWERING_HEADER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-LHDR-01", "kObjc3RuntimeSelectorStringPoolEmissionContractId"),
    SnippetCheck("M253-C005-LHDR-02", "kObjc3RuntimeSelectorPoolEmissionPayloadModel"),
    SnippetCheck("M253-C005-LHDR-03", "kObjc3RuntimeStringPoolEmissionPayloadModel"),
    SnippetCheck("M253-C005-LHDR-04", "kObjc3RuntimeSelectorPoolLogicalSection"),
)

LOWERING_CPP_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-LCPP-01", "Objc3RuntimeMetadataSelectorStringPoolEmissionSummary()"),
    SnippetCheck("M253-C005-LCPP-02", "M253-C005 selector/string pool expansion anchor"),
    SnippetCheck("M253-C005-LCPP-03", "Objc3RuntimeMetadataHostSectionForLogicalName"),
)

IR_EMITTER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-IR-01", 'out << "!objc3.objc_runtime_selector_string_pool_emission = !{!59}\\n";'),
    SnippetCheck("M253-C005-IR-02", 'out << "; runtime_metadata_selector_string_pool_emission = "'),
    SnippetCheck("M253-C005-IR-03", 'emit_canonical_pool_section(selector_pool_globals_,'),
    SnippetCheck("M253-C005-IR-04", 'emit_canonical_pool_section(runtime_string_pool_globals_,'),
    SnippetCheck("M253-C005-IR-05", '"@__objc3_sec_selector_pool"'),
    SnippetCheck("M253-C005-IR-06", '"@__objc3_sec_string_pool"'),
)

PROCESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M253-C005-PROC-01", "M253-C005 selector/string pool expansion anchor"),
    SnippetCheck("M253-C005-PROC-02", "older selector-only global scheme"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M253-C005-PKG-01",
        '"check:objc3c:m253-c005-selector-and-string-pool-emission-core-feature-expansion": "python scripts/check_m253_c005_selector_and_string_pool_emission_core_feature_expansion.py"',
    ),
    SnippetCheck(
        "M253-C005-PKG-02",
        '"test:tooling:m253-c005-selector-and-string-pool-emission-core-feature-expansion": "python -m pytest tests/tooling/test_check_m253_c005_selector_and_string_pool_emission_core_feature_expansion.py -q"',
    ),
    SnippetCheck(
        "M253-C005-PKG-03",
        '"check:objc3c:m253-c005-lane-c-readiness": "npm run check:objc3c:m253-c004-lane-c-readiness && npm run build:objc3c-native && npm run check:objc3c:m253-c005-selector-and-string-pool-emission-core-feature-expansion && npm run test:tooling:m253-c005-selector-and-string-pool-emission-core-feature-expansion"',
    ),
)

SELECTOR_POOL_ENTRY_RE = re.compile(r"^@__objc3_sel_pool_\d{4} = private unnamed_addr constant \[", re.MULTILINE)
STRING_POOL_ENTRY_RE = re.compile(r"^@__objc3_str_pool_\d{4} = private unnamed_addr constant \[", re.MULTILINE)
LLVM_USED_RE = re.compile(r"^@llvm\.used = appending global \[(?P<count>\d+) x ptr\]", re.MULTILINE)


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
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--lowering-cpp", type=Path, default=DEFAULT_LOWERING_CPP)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--metadata-fixture", type=Path, default=DEFAULT_METADATA_FIXTURE)
    parser.add_argument("--message-send-fixture", type=Path, default=DEFAULT_MESSAGE_SEND_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--llvm-readobj", default=DEFAULT_LLVM_READOBJ)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


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


def run_command(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
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


def resolve_command(name: str) -> str | None:
    resolved = shutil.which(name)
    if resolved:
        return resolved
    return shutil.which(name + ".exe")


def parse_readobj_section(text: str, section_name: str) -> tuple[int | None, int | None]:
    marker = f"Name: {section_name}"
    idx = text.find(marker)
    if idx < 0:
        return None, None
    tail = text[idx:]
    block = tail.split("  Section {", 1)[0]
    raw_match = re.search(r"RawDataSize:\s*(\d+)", block)
    reloc_match = re.search(r"RelocationCount:\s*(\d+)", block)
    raw_size = int(raw_match.group(1)) if raw_match else None
    relocations = int(reloc_match.group(1)) if reloc_match else None
    return raw_size, relocations


def make_probe_specs(args: argparse.Namespace) -> tuple[ProbeSpec, ...]:
    return (
        ProbeSpec(
            case_id="M253-C005-CASE-METADATA",
            fixture=args.metadata_fixture,
            probe_subdir="metadata",
            expected_selector_pool_count=4,
            expected_string_pool_count=21,
            symbol_tokens=(
                "@__objc3_sel_pool_0000",
                "@__objc3_sel_pool_0003",
                "@__objc3_str_pool_0000",
                "@__objc3_str_pool_0020",
                "@__objc3_sec_selector_pool",
                "@__objc3_sec_string_pool",
            ),
            boundary_tokens=(
                "selector_pool_count=4",
                "string_pool_count=21",
                "selector_section=objc3.runtime.selector_pool",
                "string_section=objc3.runtime.string_pool",
            ),
            section_expectations=(
                ("objc3.runtime.selector_pool", 64, 4),
                ("objc3.runtime.string_pool", 512, 16),
            ),
        ),
        ProbeSpec(
            case_id="M253-C005-CASE-MESSAGE-SEND",
            fixture=args.message_send_fixture,
            probe_subdir="message-send",
            expected_selector_pool_count=1,
            expected_string_pool_count=0,
            symbol_tokens=(
                "@__objc3_sel_pool_0000",
                "@__objc3_sec_selector_pool",
                "@__objc3_sec_string_pool = internal global { i64 } { i64 0 }",
                "= getelementptr inbounds [10 x i8], ptr @__objc3_sel_pool_0000",
            ),
            boundary_tokens=(
                "selector_pool_count=1",
                "string_pool_count=0",
                "selector_section=objc3.runtime.selector_pool",
                "string_section=objc3.runtime.string_pool",
            ),
            section_expectations=(
                ("objc3.runtime.selector_pool", 24, 1),
                ("objc3.runtime.string_pool", 8, 0),
            ),
        ),
    )


def run_native_probe(args: argparse.Namespace, spec: ProbeSpec, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    probe_dir = args.probe_root.resolve() / spec.probe_subdir
    command = [
        str(args.native_exe.resolve()),
        str(spec.fixture.resolve()),
        "--out-dir",
        str(probe_dir),
        "--emit-prefix",
        "module",
    ]
    result = run_command(command, ROOT)

    ir_path = probe_dir / "module.ll"
    obj_path = probe_dir / "module.obj"
    manifest_path = probe_dir / "module.manifest.json"
    backend_path = probe_dir / "module.object-backend.txt"
    diagnostics_path = probe_dir / "module.diagnostics.txt"

    case: dict[str, Any] = {
        "case_id": spec.case_id,
        "command": command,
        "process_exit_code": result.returncode,
        "ir_path": display_path(ir_path),
        "object_path": display_path(obj_path),
        "manifest_path": display_path(manifest_path),
        "object_backend_path": display_path(backend_path),
        "diagnostics_path": display_path(diagnostics_path),
    }

    checks_total = 0
    checks_passed = 0
    for path, check_id, detail in (
        (args.native_exe, f"{spec.case_id}-EXE", "objc3c-native.exe is missing"),
        (spec.fixture, f"{spec.case_id}-FIXTURE", "fixture is missing"),
        (ir_path, f"{spec.case_id}-IR", "native probe must emit module.ll"),
        (obj_path, f"{spec.case_id}-OBJ", "native probe must emit module.obj"),
        (manifest_path, f"{spec.case_id}-MANIFEST", "native probe must emit module.manifest.json"),
        (backend_path, f"{spec.case_id}-BACKEND", "native probe must emit module.object-backend.txt"),
        (diagnostics_path, f"{spec.case_id}-DIAGNOSTICS", "native probe must emit module.diagnostics.txt"),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, detail, failures)

    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(spec.fixture), f"{spec.case_id}-STATUS", f"native probe exited with {result.returncode}: {result.stderr.strip()}", failures)

    if not all(path.exists() for path in (ir_path, obj_path, manifest_path, backend_path, diagnostics_path)):
        return checks_total, checks_passed, case

    ir_text = read_text(ir_path)
    manifest = json.loads(read_text(manifest_path))
    backend_text = read_text(backend_path).strip()
    diagnostics_text = read_text(diagnostics_path)
    boundary_line = next((line for line in ir_text.splitlines() if line.startswith(BOUNDARY_COMMENT_PREFIX)), "")
    llvm_used_line = next((line for line in ir_text.splitlines() if line.startswith("@llvm.used = appending global [")), "")
    selector_pool_count = len(SELECTOR_POOL_ENTRY_RE.findall(ir_text))
    string_pool_count = len(STRING_POOL_ENTRY_RE.findall(ir_text))
    case.update(
        {
            "backend": backend_text,
            "diagnostics_is_empty": diagnostics_text.strip() == "",
            "boundary_line": boundary_line,
            "llvm_used_line": llvm_used_line,
            "selector_pool_count": selector_pool_count,
            "string_pool_count": string_pool_count,
        }
    )

    semantic_surface = manifest["frontend"]["pipeline"]["semantic_surface"]
    debug_projection = semantic_surface["objc_executable_metadata_debug_projection"]
    binary_boundary = semantic_surface["objc_executable_metadata_runtime_ingest_binary_boundary"]

    for condition, artifact, check_id, detail in (
        (backend_text == "llvm-direct", display_path(backend_path), f"{spec.case_id}-BACKEND-TEXT", f"expected llvm-direct backend, saw {backend_text!r}"),
        (diagnostics_text.strip() == "", display_path(diagnostics_path), f"{spec.case_id}-DIAGNOSTICS-EMPTY", "module.diagnostics.txt must be empty"),
        (bool(boundary_line), display_path(ir_path), f"{spec.case_id}-BOUNDARY", "IR must publish the selector/string pool summary line"),
        (NAMED_METADATA_LINE in ir_text, display_path(ir_path), f"{spec.case_id}-NAMED-METADATA", "IR must publish !objc3.objc_runtime_selector_string_pool_emission"),
        ("@.objc3.sel." not in ir_text, display_path(ir_path), f"{spec.case_id}-NO-LEGACY-SELECTOR-GLOBALS", "legacy selector-only global naming must not remain in IR"),
        ("@__objc3_sec_selector_pool" in ir_text, display_path(ir_path), f"{spec.case_id}-SELECTOR-AGGREGATE", "IR must emit @__objc3_sec_selector_pool"),
        ("@__objc3_sec_string_pool" in ir_text, display_path(ir_path), f"{spec.case_id}-STRING-AGGREGATE", "IR must emit @__objc3_sec_string_pool"),
        ("@__objc3_sec_selector_pool" in llvm_used_line, display_path(ir_path), f"{spec.case_id}-USED-SELECTOR", "@llvm.used must retain @__objc3_sec_selector_pool"),
        ("@__objc3_sec_string_pool" in llvm_used_line, display_path(ir_path), f"{spec.case_id}-USED-STRING", "@llvm.used must retain @__objc3_sec_string_pool"),
        (debug_projection.get("ready") is True, display_path(manifest_path), f"{spec.case_id}-DEBUG-PROJECTION", "debug projection must remain ready"),
        (binary_boundary.get("ready") is True, display_path(manifest_path), f"{spec.case_id}-BINARY-BOUNDARY", "runtime ingest binary boundary must remain ready"),
    ):
        checks_total += 1
        checks_passed += require(condition, artifact, check_id, detail, failures)

    for token in spec.boundary_tokens:
        checks_total += 1
        checks_passed += require(token in boundary_line, display_path(ir_path), f"{spec.case_id}-BOUNDARY-{token}", f"boundary line must contain {token}", failures)

    for value, expected, check_id, detail in (
        (selector_pool_count, spec.expected_selector_pool_count, f"{spec.case_id}-SELECTOR-POOL-COUNT", "unexpected selector pool entry count"),
        (string_pool_count, spec.expected_string_pool_count, f"{spec.case_id}-STRING-POOL-COUNT", "unexpected string pool entry count"),
    ):
        checks_total += 1
        checks_passed += require(value == expected, display_path(ir_path), check_id, f"{detail}: expected {expected}, saw {value}", failures)

    for token in spec.symbol_tokens:
        checks_total += 1
        checks_passed += require(token in ir_text, display_path(ir_path), f"{spec.case_id}-IR-{token}", f"IR must mention {token}", failures)

    llvm_readobj = resolve_command(args.llvm_readobj)
    checks_total += 1
    checks_passed += require(llvm_readobj is not None, args.llvm_readobj, f"{spec.case_id}-LLVM-READOBJ", f"unable to resolve {args.llvm_readobj}", failures)

    readobj_text = ""
    if llvm_readobj is not None:
        readobj_result = run_command([llvm_readobj, "--sections", str(obj_path)], ROOT)
        case["llvm_readobj_exit_code"] = readobj_result.returncode
        readobj_text = readobj_result.stdout
        checks_total += 1
        checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), f"{spec.case_id}-READOBJ-STATUS", f"llvm-readobj failed: {readobj_result.stderr.strip()}", failures)

    for section_name, min_raw_size, min_relocations in spec.section_expectations:
        raw_size, relocations = parse_readobj_section(readobj_text, section_name)
        case[f"{section_name}_raw_size"] = raw_size
        case[f"{section_name}_relocations"] = relocations
        checks_total += 1
        checks_passed += require(section_name in readobj_text, display_path(obj_path), f"{spec.case_id}-SECTION-{section_name}", f"object must expose {section_name}", failures)
        checks_total += 1
        checks_passed += require(raw_size is not None and raw_size >= min_raw_size, display_path(obj_path), f"{spec.case_id}-SECTION-SIZE-{section_name}", f"{section_name} must have at least {min_raw_size} bytes, saw {raw_size}", failures)
        checks_total += 1
        checks_passed += require(relocations is not None and relocations >= min_relocations, display_path(obj_path), f"{spec.case_id}-SECTION-RELOCS-{section_name}", f"{section_name} must have at least {min_relocations} relocations, saw {relocations}", failures)

    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.lowering_cpp, LOWERING_CPP_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.process_cpp, PROCESS_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, Any]] = []
    dynamic_probes_executed = False
    if not args.skip_dynamic_probes:
        dynamic_probes_executed = True
        for spec in make_probe_specs(args):
            probe_total, probe_passed, case = run_native_probe(args, spec, failures)
            checks_total += probe_total
            checks_passed += probe_passed
            dynamic_cases.append(case)

    payload = {
        "mode": MODE,
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "evidence_path": str(args.summary_out).replace("\\", "/"),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
