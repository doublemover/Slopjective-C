#!/usr/bin/env python3
"""Checker for M265-C002 optional chaining lowering."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_LINEAGE = "objc3c-part3-optional-keypath-lowering/m265-c001-v1"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-C002" / "optional_chaining_binding_and_coalescing_lowering_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_c002_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART3 = ROOT / "spec" / "PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md"
AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
LOWERING_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_PIPELINE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_runtime_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_non_objc_negative.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "c002"


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
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M265-C002-MISSING", f"missing artifact: {display_path(path)}"))
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


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-C002-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-C002-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def resolve_clang() -> str:
    candidates = (
        shutil.which("clang"),
        shutil.which("clang.exe"),
        r"C:\Program Files\LLVM\bin\clang.exe",
    )
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang"


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = load_json(registration_manifest_path)
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    result = run_command([
        resolve_clang(),
        str(obj_path),
        str(runtime_library_path),
        f"@{rsp_path.resolve()}",
        "-o",
        str(exe_path),
    ], cwd=out_dir)
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m265-c002-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-C002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-C002-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M265-C002-DYN-native", "native driver missing after build", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-C002-DYN-runner", "frontend runner missing after build", failures)

    positive_out = PROBE_ROOT / "positive"
    positive_out.mkdir(parents=True, exist_ok=True)
    positive = run_command([str(args.native_exe), str(POSITIVE_FIXTURE), "--out-dir", str(positive_out), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(positive.returncode == 0, display_path(POSITIVE_FIXTURE), "M265-C002-DYN-pos-rc", f"positive compile failed: {positive.stderr or positive.stdout}", failures)

    positive_manifest = positive_out / "module.manifest.json"
    positive_ir = positive_out / "module.ll"
    positive_obj = positive_out / "module.obj"
    positive_backend = positive_out / "module.object-backend.txt"
    positive_diag = positive_out / "module.diagnostics.json"
    positive_rsp = positive_out / "module.runtime-metadata-linker-options.rsp"
    positive_registration = positive_out / "module.runtime-registration-manifest.json"
    for check_id, path in (
        ("M265-C002-DYN-pos-manifest", positive_manifest),
        ("M265-C002-DYN-pos-ir", positive_ir),
        ("M265-C002-DYN-pos-obj", positive_obj),
        ("M265-C002-DYN-pos-backend", positive_backend),
        ("M265-C002-DYN-pos-diag", positive_diag),
        ("M265-C002-DYN-pos-rsp", positive_rsp),
        ("M265-C002-DYN-pos-registration", positive_registration),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, f"missing artifact: {display_path(path)}", failures)

    if positive_diag.exists():
        sub_total, sub_passed = validate_empty_diagnostics(positive_diag, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    source_packet: dict[str, Any] = {}
    lowering_packet: dict[str, Any] = {}
    if positive_manifest.exists():
        manifest = load_json(positive_manifest)
        semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
        if isinstance(semantic_surface, dict):
            source_packet = semantic_surface.get("objc_part3_type_source_closure", {})
            lowering_packet = semantic_surface.get("objc_part3_optional_keypath_lowering_contract", {})
        checks_total += 1
        checks_passed += require(isinstance(source_packet, dict), display_path(positive_manifest), "M265-C002-DYN-source-packet", "missing source-closure packet", failures)
        checks_total += 1
        checks_passed += require(isinstance(lowering_packet, dict), display_path(positive_manifest), "M265-C002-DYN-lowering-packet", "missing lowering packet", failures)
        if isinstance(source_packet, dict):
            for key, expected in {
                "contract_id": "objc3c-part3-type-source-closure/m265-a002-v1",
                "optional_member_access_fail_closed": False,
            }.items():
                checks_total += 1
                checks_passed += require(source_packet.get(key) == expected, display_path(positive_manifest), f"M265-C002-DYN-source-{key}", f"{key} mismatch", failures)
            checks_total += 1
            checks_passed += require(source_packet.get("unsupported_claim_ids") == [], display_path(positive_manifest), "M265-C002-DYN-source-unsupported", "source packet unsupported claims must be empty", failures)
            checks_total += 1
            checks_passed += require(int(source_packet.get("optional_member_access_sites", 0)) >= 3, display_path(positive_manifest), "M265-C002-DYN-source-sites", "expected optional_member_access_sites >= 3", failures)
        if isinstance(lowering_packet, dict):
            for key, expected in {
                "contract_id": CONTRACT_LINEAGE,
                "optional_send_sites": 4,
                "live_optional_lowering_sites": 4,
                "single_evaluation_nil_short_circuit_sites": 4,
                "typed_keypath_literal_sites": 0,
                "deferred_typed_keypath_sites": 0,
            }.items():
                checks_total += 1
                checks_passed += require(lowering_packet.get(key) == expected, display_path(positive_manifest), f"M265-C002-DYN-lowering-{key}", f"{key} mismatch", failures)
            checks_total += 1
            checks_passed += require("optional-member-access" in str(lowering_packet.get("optional_model", "")), display_path(positive_manifest), "M265-C002-DYN-lowering-model", "optional model must mention optional-member-access", failures)

    if positive_backend.exists():
        checks_total += 1
        checks_passed += require(positive_backend.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(positive_backend), "M265-C002-DYN-backend", "expected llvm-direct object backend", failures)

    if positive_ir.exists():
        ir_text = read_text(positive_ir)
        checks_total += 1
        checks_passed += require("part3_optional_keypath_lowering = contract_id=objc3c-part3-optional-keypath-lowering/m265-c001-v1" in ir_text, display_path(positive_ir), "M265-C002-DYN-ir-marker", "IR marker missing", failures)
        checks_total += 1
        checks_passed += require(ir_text.count("opt_send_nil_") >= 3, display_path(positive_ir), "M265-C002-DYN-ir-nil", "expected optional-send nil blocks", failures)
        checks_total += 1
        checks_passed += require(ir_text.count("phi i32 [0") >= 3, display_path(positive_ir), "M265-C002-DYN-ir-phi", "expected nil-short-circuit phi nodes", failures)

    exe_path, link_result = link_executable(positive_out)
    checks_total += 1
    checks_passed += require(exe_path is not None, display_path(positive_out), "M265-C002-DYN-link", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}", failures)
    if exe_path is not None:
        run = run_command([str(exe_path)], cwd=positive_out)
        checks_total += 1
        checks_passed += require(run.returncode == 9, display_path(exe_path), "M265-C002-DYN-run", f"expected exit code 9, got {run.returncode}", failures)
        evidence["positive_exit_code"] = run.returncode
        if link_result is not None:
            evidence["link_warnings_tail"] = ((link_result.stdout or "") + (link_result.stderr or ""))[-500:]

    negative_out = PROBE_ROOT / "non-objc"
    negative_out.mkdir(parents=True, exist_ok=True)
    negative = run_command([
        str(args.runner_exe),
        str(NEGATIVE_FIXTURE),
        "--out-dir",
        str(negative_out),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    negative_text = (negative.stdout or "") + (negative.stderr or "")
    checks_total += 1
    checks_passed += require(negative.returncode != 0, display_path(NEGATIVE_FIXTURE), "M265-C002-DYN-neg-rc", "negative fixture unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require("O3S206" in negative_text, display_path(NEGATIVE_FIXTURE), "M265-C002-DYN-neg-o3s206", "expected O3S206 diagnostic", failures)

    evidence["positive_fixture"] = display_path(POSITIVE_FIXTURE)
    evidence["positive_manifest"] = display_path(positive_manifest)
    evidence["source_packet"] = source_packet
    evidence["lowering_packet"] = lowering_packet
    evidence["negative_fixture"] = display_path(NEGATIVE_FIXTURE)
    evidence["negative_output"] = negative_text.strip()
    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-C002-EXP-01", "# M265-C002 Expectations"),
            SnippetCheck("M265-C002-EXP-02", CONTRACT_LINEAGE),
            SnippetCheck("M265-C002-EXP-03", "optional-member access now lowers through the same nil-short-circuit path"),
            SnippetCheck("M265-C002-EXP-04", "non-ObjC-reference receivers still fail closed with deterministic diagnostics"),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-C002-PKT-01", "# M265-C002 Packet"),
            SnippetCheck("M265-C002-PKT-02", "implement real optional chaining lowering for `?.`"),
            SnippetCheck("M265-C002-PKT-03", "explicit sugar tracking"),
            SnippetCheck("M265-C002-PKT-04", "the positive fixture proves chained optional-member access compiles, links, runs, and returns `9`"),
        ],
        AST_HEADER: [
            SnippetCheck("M265-C002-AST-01", "optional_member_access_enabled"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M265-C002-TOK-01", "QuestionDot"),
            SnippetCheck("M265-C002-TOK-02", "optional bindings/sends/member-access/coalescing"),
        ],
        LEXER_CPP: [
            SnippetCheck("M265-C002-LEX-01", 'TokenKind::QuestionDot'),
            SnippetCheck("M265-C002-LEX-02", '"?."'),
        ],
        PARSER_CPP: [
            SnippetCheck("M265-C002-PARSE-01", "Match(TokenKind::QuestionDot)"),
            SnippetCheck("M265-C002-PARSE-02", "optional_member_access_enabled = true"),
            SnippetCheck("M265-C002-PARSE-03", "expected identifier after '?.'"),
        ],
        LOWERING_H: [
            SnippetCheck("M265-C002-H-01", "optional binding/send/optional-member-access/"),
            SnippetCheck("M265-C002-H-02", "optional-bindings-sends-optional-member-access-and-coalescing"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M265-C002-CPP-01", "M265-C002 optional chaining lowering anchor"),
            SnippetCheck("M265-C002-CPP-02", "optional-member access"),
        ],
        IR_CPP: [
            SnippetCheck("M265-C002-IR-01", "Objc3Part3OptionalKeypathLoweringSummary()"),
            SnippetCheck("M265-C002-IR-02", "part3_optional_keypath_lowering = "),
        ],
        FRONTEND_PIPELINE_CPP: [
            SnippetCheck("M265-C002-FP-01", "summary.optional_member_access_sites"),
            SnippetCheck("M265-C002-FP-02", "optional_member_access_enabled"),
            SnippetCheck("M265-C002-FP-03", "summary.optional_member_access_fail_closed = false"),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M265-C002-FA-01", "kObjc3Part3OptionalKeypathLoweringOptionalModel"),
            SnippetCheck("M265-C002-FA-02", "contract.optional_send_sites"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M265-C002-DOCSRC-01", "`?.` optional-member access now lowers natively"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-C002-DOCN-01", "`?.` optional-member access now lowers natively"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-C002-AM-01", "Optional-member access written as `?.` now lowers"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-C002-ATTR-01", "optional-member access `?.` now lowers natively"),
        ],
        SPEC_PART3: [
            SnippetCheck("M265-C002-P3-01", "optional-member access now lowers natively"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-C002-PKG-01", '"check:objc3c:m265-c002-optional-chaining-binding-and-coalescing-lowering-core-feature-implementation"'),
            SnippetCheck("M265-C002-PKG-02", '"test:tooling:m265-c002-optional-chaining-binding-and-coalescing-lowering-core-feature-implementation"'),
            SnippetCheck("M265-C002-PKG-03", '"check:objc3c:m265-c002-lane-c-readiness"'),
        ],
    }

    for path, required in snippets.items():
        checks_total += len(required)
        checks_passed += ensure_snippets(path, required, failures)

    dynamic: dict[str, Any] = {}
    if args.skip_dynamic_probes:
        dynamic_executed = False
    else:
        dynamic_executed = True
        dyn_total, dyn_passed, dynamic = run_dynamic_probes(args, failures)
        checks_total += dyn_total
        checks_passed += dyn_passed

    summary = {
        "issue": "M265-C002",
        "title": "Optional chaining, binding, and coalescing lowering - Core feature implementation",
        "contract_lineage": CONTRACT_LINEAGE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [asdict(failure) for failure in failures],
        "evidence": dynamic,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if failures:
        print(json.dumps(summary, indent=2))
        return 1
    print(f"[ok] M265-C002 validated ({checks_passed}/{checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
