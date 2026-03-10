#!/usr/bin/env python3
"""Fail-closed checker for M264-A002 frontend truth-surface wiring."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-a002-frontend-feature-claim-and-strictness-truth-surface-v1"
CONTRACT_ID = "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
A001_CONTRACT_ID = "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
DRIVER_SURFACE_MODEL = "language-version-and-compatibility-live-strictness-and-feature-macro-fail-closed"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-A002" / "frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
FIXTURE_HELLO = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
FIXTURE_METADATA = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
SUPPORTED_SELECTION_SURFACES = [
    "selection:language-version",
    "selection:compatibility-mode",
    "selection:migration-assist",
]
UNSUPPORTED_SELECTION_SURFACES = [
    "selection:strictness",
    "selection:strict-concurrency",
]
SUPPRESSED_MACRO_CLAIMS = [
    "macro-claim:__OBJC3_STRICTNESS_LEVEL__",
    "macro-claim:__OBJC3_CONCURRENCY_MODE__",
    "macro-claim:__OBJC3_CONCURRENCY_STRICT__",
]
SUPPORTED_COMPATIBILITY_MODES = ["canonical", "legacy"]

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_a002_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation_packet.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
TOKEN_HEADER = ROOT / "native" / "objc3c" / "src" / "token" / "objc3_token_contract.h"
LEXER_CPP = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.cpp"
PARSER_CPP = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
PACKAGE_JSON = ROOT / "package.json"


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
    parser.add_argument("--summary-out", type=Path, default=EXPECTED_SUMMARY)
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-A002-MISSING", f"missing artifact: {display_path(path)}"))
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
    return subprocess.run(
        list(command),
        cwd=str(ROOT),
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


def truth_surface_from_manifest(manifest_path: Path) -> tuple[dict[str, Any], dict[str, Any]]:
    manifest = load_json(manifest_path)
    frontend = manifest.get("frontend", {})
    if not isinstance(frontend, dict):
        raise TypeError(f"missing frontend block in {display_path(manifest_path)}")
    pipeline = frontend.get("pipeline", {})
    if not isinstance(pipeline, dict):
        raise TypeError(f"missing frontend.pipeline block in {display_path(manifest_path)}")
    semantic_surface = pipeline.get("semantic_surface", {})
    if not isinstance(semantic_surface, dict):
        raise TypeError(f"missing frontend.pipeline.semantic_surface block in {display_path(manifest_path)}")
    surface = semantic_surface.get("objc_feature_claim_and_strictness_truth_surface")
    if not isinstance(surface, dict):
        raise TypeError(f"missing truth surface packet in {display_path(manifest_path)}")
    return frontend, surface


def validate_frontend_block(
    frontend: dict[str, Any],
    artifact: str,
    expected_mode: str,
    expected_migration_assist: bool,
    failures: list[Finding],
) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(frontend.get("compatibility_mode") == expected_mode, artifact, "M264-A002-FRONTEND-01", "compatibility mode mismatch", failures)
    checks_total += 1
    checks_passed += require(frontend.get("default_compatibility_mode") == "canonical", artifact, "M264-A002-FRONTEND-02", "default compatibility mode drifted", failures)
    checks_total += 1
    checks_passed += require(frontend.get("migration_assist") is expected_migration_assist, artifact, "M264-A002-FRONTEND-03", "migration assist mismatch", failures)
    checks_total += 1
    checks_passed += require(frontend.get("language_version_selection_supported") is True, artifact, "M264-A002-FRONTEND-04", "language-version selection must stay supported", failures)
    checks_total += 1
    checks_passed += require(frontend.get("compatibility_selection_supported") is True, artifact, "M264-A002-FRONTEND-05", "compatibility selection must stay supported", failures)
    checks_total += 1
    checks_passed += require(frontend.get("migration_assist_selection_supported") is True, artifact, "M264-A002-FRONTEND-06", "migration assist selection must stay supported", failures)
    checks_total += 1
    checks_passed += require(frontend.get("strictness_selection_supported") is False, artifact, "M264-A002-FRONTEND-07", "strictness selection must remain unsupported", failures)
    checks_total += 1
    checks_passed += require(frontend.get("strict_concurrency_selection_supported") is False, artifact, "M264-A002-FRONTEND-08", "strict concurrency selection must remain unsupported", failures)
    checks_total += 1
    checks_passed += require(frontend.get("feature_macro_surface_supported") is False, artifact, "M264-A002-FRONTEND-09", "feature macro surface must remain unsupported", failures)
    checks_total += 1
    checks_passed += require(frontend.get("feature_claim_truth_surface_contract_id") == CONTRACT_ID, artifact, "M264-A002-FRONTEND-10", "truth-surface contract id drifted", failures)
    return checks_total, checks_passed


def validate_truth_surface(
    surface: dict[str, Any],
    artifact: str,
    expected_mode: str,
    expected_migration_assist: bool,
    failures: list[Finding],
) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(surface.get("contract_id") == CONTRACT_ID, artifact, "M264-A002-SURFACE-01", "contract id drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("runnable_feature_claim_inventory_contract_id") == A001_CONTRACT_ID, artifact, "M264-A002-SURFACE-02", "A001 contract link drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("language_mode") == "objc3-v1-native-subset", artifact, "M264-A002-SURFACE-03", "language mode drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("language_version") == 3, artifact, "M264-A002-SURFACE-04", "language version drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("effective_compatibility_mode") == expected_mode, artifact, "M264-A002-SURFACE-05", "effective compatibility mode mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("default_compatibility_mode") == "canonical", artifact, "M264-A002-SURFACE-06", "default compatibility mode drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("migration_assist_enabled") is expected_migration_assist, artifact, "M264-A002-SURFACE-07", "migration assist truth mismatch", failures)
    checks_total += 1
    checks_passed += require(surface.get("driver_surface_model") == DRIVER_SURFACE_MODEL, artifact, "M264-A002-SURFACE-08", "driver surface model drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("language_version_selection_supported") is True, artifact, "M264-A002-SURFACE-09", "language-version support drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("compatibility_selection_supported") is True, artifact, "M264-A002-SURFACE-10", "compatibility support drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("migration_assist_selection_supported") is True, artifact, "M264-A002-SURFACE-11", "migration-assist support drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("strictness_selection_supported") is False, artifact, "M264-A002-SURFACE-12", "strictness support must remain false", failures)
    checks_total += 1
    checks_passed += require(surface.get("strict_concurrency_selection_supported") is False, artifact, "M264-A002-SURFACE-13", "strict concurrency support must remain false", failures)
    checks_total += 1
    checks_passed += require(surface.get("feature_macro_surface_supported") is False, artifact, "M264-A002-SURFACE-14", "feature macro support must remain false", failures)
    checks_total += 1
    checks_passed += require(surface.get("claim_truth_fail_closed") is True, artifact, "M264-A002-SURFACE-15", "claim-truth policy must stay fail-closed", failures)
    checks_total += 1
    checks_passed += require(surface.get("supported_compatibility_modes") == SUPPORTED_COMPATIBILITY_MODES, artifact, "M264-A002-SURFACE-16", "supported compatibility modes drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("supported_selection_surface_ids") == SUPPORTED_SELECTION_SURFACES, artifact, "M264-A002-SURFACE-17", "supported selection surfaces drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("unsupported_selection_surface_ids") == UNSUPPORTED_SELECTION_SURFACES, artifact, "M264-A002-SURFACE-18", "unsupported selection surfaces drifted", failures)
    checks_total += 1
    checks_passed += require(surface.get("suppressed_macro_claim_ids") == SUPPRESSED_MACRO_CLAIMS, artifact, "M264-A002-SURFACE-19", "suppressed macro claims drifted", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    build = run_command(["npm.cmd", "run", "build:objc3c-native"])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "npm run build:objc3c-native", "M264-A002-DYN-01", f"native build failed: {build.stderr or build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M264-A002-DYN-02", "frontend runner missing after build", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M264-A002-DYN-03", "native executable missing after build", failures)

    case_specs = [
        {
            "case_id": "hello-canonical-native",
            "fixture": FIXTURE_HELLO,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
            "tool": "native",
        },
        {
            "case_id": "hello-legacy-migration-runner",
            "fixture": FIXTURE_HELLO,
            "extra_args": ["--objc3-compat-mode", "legacy", "--objc3-migration-assist"],
            "expected_mode": "legacy",
            "expected_migration_assist": True,
            "tool": "runner",
        },
        {
            "case_id": "metadata-canonical-runner",
            "fixture": FIXTURE_METADATA,
            "extra_args": [],
            "expected_mode": "canonical",
            "expected_migration_assist": False,
            "tool": "runner",
        },
    ]
    cases: list[dict[str, Any]] = []
    for spec in case_specs:
        out_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "a002" / spec["case_id"]
        out_dir.mkdir(parents=True, exist_ok=True)
        if spec["tool"] == "runner":
            command = [
                str(args.runner_exe),
                str(spec["fixture"]),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                "--no-emit-ir",
                "--no-emit-object",
                *spec["extra_args"],
            ]
        else:
            command = [
                str(args.native_exe),
                str(spec["fixture"]),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *spec["extra_args"],
            ]
        run = run_command(command)
        artifact = spec["case_id"]
        checks_total += 1
        checks_passed += require(run.returncode == 0, artifact, "M264-A002-DYN-04", f"compile failed: {run.stderr or run.stdout}", failures)
        manifest_path = out_dir / "module.manifest.json"
        checks_total += 1
        checks_passed += require(manifest_path.exists(), artifact, "M264-A002-DYN-05", "manifest missing", failures)
        if manifest_path.exists():
            frontend, surface = truth_surface_from_manifest(manifest_path)
            sub_total, sub_passed = validate_frontend_block(frontend, artifact, spec["expected_mode"], spec["expected_migration_assist"], failures)
            checks_total += sub_total
            checks_passed += sub_passed
            sub_total, sub_passed = validate_truth_surface(surface, artifact, spec["expected_mode"], spec["expected_migration_assist"], failures)
            checks_total += sub_total
            checks_passed += sub_passed
            if spec["tool"] == "native":
                checks_total += 1
                checks_passed += require((out_dir / "module.ll").exists(), artifact, "M264-A002-DYN-06", "native runnable probe must emit IR", failures)
                checks_total += 1
                checks_passed += require((out_dir / "module.obj").exists(), artifact, "M264-A002-DYN-07", "native runnable probe must emit object output", failures)
            if spec["case_id"] == "metadata-canonical-runner":
                checks_total += 1
                checks_passed += require(surface.get("declared_protocol_count", 0) > 0, artifact, "M264-A002-DYN-08", "metadata fixture must prove protocol source coverage", failures)
                checks_total += 1
                checks_passed += require(surface.get("declared_interface_count", 0) > 0, artifact, "M264-A002-DYN-09", "metadata fixture must prove interface source coverage", failures)
                checks_total += 1
                checks_passed += require(surface.get("declared_implementation_count", 0) > 0, artifact, "M264-A002-DYN-10", "metadata fixture must prove implementation source coverage", failures)
            cases.append(
                {
                    "case_id": spec["case_id"],
                    "tool": spec["tool"],
                    "fixture": display_path(spec["fixture"]),
                    "manifest_path": display_path(manifest_path),
                    "effective_compatibility_mode": surface.get("effective_compatibility_mode"),
                    "migration_assist_enabled": surface.get("migration_assist_enabled"),
                    "supported_selection_surface_ids": surface.get("supported_selection_surface_ids"),
                    "unsupported_selection_surface_ids": surface.get("unsupported_selection_surface_ids"),
                    "suppressed_macro_claim_ids": surface.get("suppressed_macro_claim_ids"),
                }
            )
    return checks_total, checks_passed, {"cases": cases}


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-A002-DOC-01", "# M264 Frontend Feature-Claim And Strictness Truthfulness Wiring Core Feature Implementation Expectations (A002)"),
            SnippetCheck("M264-A002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
            SnippetCheck("M264-A002-DOC-03", "frontend.pipeline.semantic_surface.objc_feature_claim_and_strictness_truth_surface"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-A002-PKT-01", "# M264-A002 Frontend Feature-Claim And Strictness Truthfulness Wiring Core Feature Implementation Packet"),
            SnippetCheck("M264-A002-PKT-02", "Issue: `#7234`"),
            SnippetCheck("M264-A002-PKT-03", "frontend.feature_claim_truth_surface_contract_id"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M264-A002-NDOC-01", "## Feature-claim and strictness truth surface (M264-A002)"),
            SnippetCheck("M264-A002-NDOC-02", "frontend.pipeline.semantic_surface.objc_feature_claim_and_strictness_truth_surface"),
            SnippetCheck("M264-A002-NDOC-03", "strictness selection: not yet supported"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M264-A002-SPC-01", "## M264 frontend selection truth surface (implementation note)"),
            SnippetCheck("M264-A002-SPC-02", "frontend.feature_claim_truth_surface_contract_id"),
            SnippetCheck("M264-A002-SPC-03", "strictness / strict-concurrency selection remain unsupported"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M264-A002-DEC-01", "## D-017: Unsupported strictness and macro-claim surfaces stay unadvertised until executable"),
            SnippetCheck("M264-A002-DEC-02", "Strictness selection, strict concurrency selection, and feature-macro claim"),
        ],
        TOKEN_HEADER: [
            SnippetCheck("M264-A002-TOK-01", "kObjc3FeatureClaimStrictnessTruthSurfaceContractId"),
            SnippetCheck("M264-A002-TOK-02", "kObjc3SupportedSelectionSurfaceLanguageVersion"),
            SnippetCheck("M264-A002-TOK-03", "kObjc3SuppressedMacroClaimStrictnessLevel"),
        ],
        LEXER_CPP: [
            SnippetCheck("M264-A002-LEX-01", "M264-A002 truth-surface wiring anchor"),
            SnippetCheck("M264-A002-LEX-02", "strictness / strict concurrency do"),
        ],
        PARSER_CPP: [
            SnippetCheck("M264-A002-PARSE-01", "M264-A002 truth-surface wiring anchor"),
            SnippetCheck("M264-A002-PARSE-02", "unsupported selection surfaces stay"),
        ],
        ARTIFACTS_CPP: [
            SnippetCheck("M264-A002-ART-01", "BuildFeatureClaimStrictnessTruthSurfaceJson"),
            SnippetCheck("M264-A002-ART-02", "feature_claim_truth_surface_contract_id"),
            SnippetCheck("M264-A002-ART-03", "objc_feature_claim_and_strictness_truth_surface"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M264-A002-PKG-01", '"check:objc3c:m264-a002-frontend-feature-claim-and-strictness-truthfulness-wiring"'),
            SnippetCheck("M264-A002-PKG-02", '"test:tooling:m264-a002-frontend-feature-claim-and-strictness-truthfulness-wiring"'),
            SnippetCheck("M264-A002-PKG-03", '"check:objc3c:m264-a002-lane-a-readiness"'),
        ],
    }
    for path, path_snippets in snippets.items():
        checks_total += len(path_snippets)
        checks_passed += ensure_snippets(path, path_snippets, failures)

    dynamic_summary: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if args.skip_dynamic_probes:
        checks_total += 1
        checks_passed += 1
    else:
        sub_total, sub_passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in failures],
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "driver_surface_model": DRIVER_SURFACE_MODEL,
        "dynamic_summary": dynamic_summary,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[FAIL] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
