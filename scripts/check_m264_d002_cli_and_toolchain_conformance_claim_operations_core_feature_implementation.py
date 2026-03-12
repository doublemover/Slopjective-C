#!/usr/bin/env python3
"""Fail-closed checker for M264-D002 conformance-claim operations."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m264-d002-conformance-claim-operations-v1"
CONTRACT_ID = "objc3c-toolchain-conformance-claim-operations/m264-d002-v1"
SCHEMA_ID = "objc3c-driver-conformance-validation-v1"
EXPECTED_SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-D002" / "cli_and_toolchain_conformance_claim_operations_summary.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m264_cli_and_toolchain_conformance_claim_operations_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m264" / "m264_d002_cli_and_toolchain_conformance_claim_operations_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_CHECKLIST = ROOT / "spec" / "CONFORMANCE_PROFILE_CHECKLIST.md"
SPEC_DECISIONS = ROOT / "spec" / "DECISIONS_LOG.md"
PACKAGE_JSON = ROOT / "package.json"
CLI_OPTIONS_H = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
CLI_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
COMPILATION_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
NATIVE_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
MANIFEST_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
RUNTIME_BOOTSTRAP_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"


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
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M264-D002-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_validation_artifact(payload: dict[str, Any], artifact: str, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    expected = {
        "contract_id": CONTRACT_ID,
        "schema_id": SCHEMA_ID,
        "validation_model": "driver-validates-versioned-conformance-report-and-publication-sidecars-before-toolchain-consumption",
        "consumption_model": "validation-consumes-json-sidecars-only-and-keeps-unsupported-profiles-fail-closed",
        "format": "json",
        "report_schema_id": "objc3c-versioned-conformance-report-v1",
        "report_contract_id": "objc3c-versioned-conformance-report-lowering/m264-c001-v1",
        "runtime_capability_contract_id": "objc3c-runtime-capability-reporting/m264-c002-v1",
        "public_conformance_schema_id": "objc3-conformance-report/v1",
        "selected_profile": "core",
        "effective_compatibility_mode": "canonical",
        "publication_surface_kind": "native-cli",
    }
    for key, expected_value in expected.items():
        checks_total += 1
        checks_passed += require(payload.get(key) == expected_value, artifact, f"M264-D002-VAL-{key}", f"{key} mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("selected_profile_supported") is True, artifact, "M264-D002-VAL-selected_profile_supported", "selected_profile_supported mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("migration_assist_enabled") is False, artifact, "M264-D002-VAL-migration_assist_enabled", "migration_assist_enabled mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("supported_profile_ids") == ["core"], artifact, "M264-D002-VAL-supported_profile_ids", "supported_profile_ids mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("rejected_profile_ids") == ["strict", "strict-concurrency", "strict-system"], artifact, "M264-D002-VAL-rejected_profile_ids", "rejected_profile_ids mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("validated_report_artifact") == "module.objc3-conformance-report.json", artifact, "M264-D002-VAL-report-artifact", "validated_report_artifact mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("validated_publication_artifact") == "module.objc3-conformance-publication.json", artifact, "M264-D002-VAL-publication-artifact", "validated_publication_artifact mismatch", failures)
    checks_total += 1
    checks_passed += require(payload.get("ready") is True, artifact, "M264-D002-VAL-ready", "ready mismatch", failures)
    return checks_total, checks_passed


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    helper_summary = ROOT / "tmp" / "reports" / "m264" / "M264-D002" / "ensure_build_summary.json"
    build = run_command([
        "python",
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m264-d002",
        "--summary-out",
        str(helper_summary),
    ])
    checks_total += 1
    checks_passed += require(build.returncode == 0, "ensure_objc3c_native_build.py", "M264-D002-DYN-BUILD", f"fast build failed: {build.stderr or build.stdout}", failures)

    emit_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d002" / "emit-json"
    emit_dir.mkdir(parents=True, exist_ok=True)
    emit = run_command([
        str(args.native_exe),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(emit_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(emit.returncode == 0, "emit-json", "M264-D002-DYN-EMIT-RC", f"emit-json failed: {emit.stderr or emit.stdout}", failures)
    report_path = emit_dir / "module.objc3-conformance-report.json"
    publication_path = emit_dir / "module.objc3-conformance-publication.json"
    checks_total += 1
    checks_passed += require(report_path.exists(), display_path(report_path), "M264-D002-DYN-REPORT", "conformance report missing", failures)
    checks_total += 1
    checks_passed += require(publication_path.exists(), display_path(publication_path), "M264-D002-DYN-PUBLICATION", "conformance publication missing", failures)

    validate_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d002" / "validate-json"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate = run_command([
        str(args.native_exe),
        "--validate-objc3-conformance",
        str(report_path),
        "--out-dir",
        str(validate_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(validate.returncode == 0, "validate-json", "M264-D002-DYN-VALIDATE-RC", f"validate-json failed: {validate.stderr or validate.stdout}", failures)
    validation_path = validate_dir / "module.objc3-conformance-validation.json"
    checks_total += 1
    checks_passed += require(validation_path.exists(), display_path(validation_path), "M264-D002-DYN-VALIDATION", "validation artifact missing", failures)
    if validation_path.exists():
        payload = load_json(validation_path)
        total, passed = validate_validation_artifact(payload, display_path(validation_path), failures)
        checks_total += total
        checks_passed += passed

    yaml_emit = run_command([
        str(args.native_exe),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d002" / "emit-yaml"),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "yaml",
    ])
    checks_total += 1
    checks_passed += require(yaml_emit.returncode != 0, "emit-yaml", "M264-D002-DYN-YAML-EMIT-RC", "yaml emit unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require("unsupported --emit-objc3-conformance-format selection: yaml" in (yaml_emit.stderr + yaml_emit.stdout), "emit-yaml", "M264-D002-DYN-YAML-EMIT-DIAG", "yaml emit diagnostic missing", failures)

    yaml_validate = run_command([
        str(args.native_exe),
        "--validate-objc3-conformance",
        str(report_path),
        "--out-dir",
        str(ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d002" / "validate-yaml"),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "yaml",
    ])
    checks_total += 1
    checks_passed += require(yaml_validate.returncode != 0, "validate-yaml", "M264-D002-DYN-YAML-VALIDATE-RC", "yaml validate unexpectedly succeeded", failures)
    checks_total += 1
    checks_passed += require("unsupported --emit-objc3-conformance-format selection: yaml" in (yaml_validate.stderr + yaml_validate.stdout), "validate-yaml", "M264-D002-DYN-YAML-VALIDATE-DIAG", "yaml validate diagnostic missing", failures)

    malformed_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m264" / "d002" / "malformed"
    malformed_dir.mkdir(parents=True, exist_ok=True)
    malformed_report = malformed_dir / "module.objc3-conformance-report.json"
    malformed_publication = malformed_dir / "module.objc3-conformance-publication.json"
    malformed_text = report_path.read_text(encoding="utf-8").replace("objc3c-versioned-conformance-report-v1", "broken-schema-v1", 1)
    malformed_report.write_text(malformed_text, encoding="utf-8")
    malformed_publication.write_text(publication_path.read_text(encoding="utf-8"), encoding="utf-8")
    malformed_validate = run_command([
        str(args.native_exe),
        "--validate-objc3-conformance",
        str(malformed_report),
        "--out-dir",
        str(malformed_dir / "out"),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "json",
    ])
    checks_total += 1
    checks_passed += require(malformed_validate.returncode != 0, "malformed-validate", "M264-D002-DYN-MALFORMED-RC", "malformed report unexpectedly validated", failures)
    checks_total += 1
    checks_passed += require("invalid conformance report schema_id" in (malformed_validate.stderr + malformed_validate.stdout), "malformed-validate", "M264-D002-DYN-MALFORMED-DIAG", "malformed report diagnostic missing", failures)

    return checks_total, checks_passed, {
        "build_summary_path": display_path(helper_summary),
        "emit_report": display_path(report_path),
        "emit_publication": display_path(publication_path),
        "validation_artifact": display_path(validation_path),
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_snippets = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M264-D002-DOC-CONTRACT", CONTRACT_ID),
            SnippetCheck("M264-D002-DOC-VALIDATE", "--validate-objc3-conformance <report.json>"),
            SnippetCheck("M264-D002-DOC-ARTIFACT", "module.objc3-conformance-validation.json"),
        ],
        PACKET_DOC: [
            SnippetCheck("M264-D002-PACKET-ID", "Issue: `#7241`"),
            SnippetCheck("M264-D002-PACKET-SCOPE", "--validate-objc3-conformance <report.json>"),
        ],
        CLI_OPTIONS_H: [
            SnippetCheck("M264-D002-CLI-H-MODE", "enum class Objc3CliCommandMode"),
            SnippetCheck("M264-D002-CLI-H-VALIDATE", "validate_conformance_report_path"),
        ],
        CLI_OPTIONS_CPP: [
            SnippetCheck("M264-D002-CLI-CPP-EMIT", "--emit-objc3-conformance"),
            SnippetCheck("M264-D002-CLI-CPP-FORMAT", "--emit-objc3-conformance-format"),
            SnippetCheck("M264-D002-CLI-CPP-VALIDATE", "--validate-objc3-conformance"),
            SnippetCheck("M264-D002-CLI-CPP-MODE", "Objc3CliCommandMode::kValidateConformance"),
        ],
        COMPILATION_DRIVER_CPP: [
            SnippetCheck("M264-D002-COMP-VALIDATE", "RunObjc3ConformanceValidationPath"),
        ],
        NATIVE_DRIVER_CPP: [
            SnippetCheck("M264-D002-DRIVER-VALIDATE", "TryBuildObjc3ConformanceClaimValidationArtifact"),
            SnippetCheck("M264-D002-DRIVER-YAML", "unsupported --emit-objc3-conformance-format selection"),
            SnippetCheck("M264-D002-DRIVER-VALIDATE-FLAG", "RunObjc3ConformanceValidationPath"),
        ],
        MANIFEST_ARTIFACTS_H: [
            SnippetCheck("M264-D002-MANIFEST-H-PATH", "BuildConformanceValidationArtifactPath"),
            SnippetCheck("M264-D002-MANIFEST-H-WRITE", "WriteConformanceValidationArtifact"),
        ],
        MANIFEST_ARTIFACTS_CPP: [
            SnippetCheck("M264-D002-MANIFEST-CPP-SUFFIX", ".objc3-conformance-validation.json"),
        ],
        PROCESS_H: [
            SnippetCheck("M264-D002-PROCESS-H-INPUTS", "Objc3ConformanceClaimValidationArtifactInputs"),
            SnippetCheck("M264-D002-PROCESS-H-BUILDER", "TryBuildObjc3ConformanceClaimValidationArtifact"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M264-D002-PROCESS-CPP-CONTRACT", CONTRACT_ID),
            SnippetCheck("M264-D002-PROCESS-CPP-SCHEMA", SCHEMA_ID),
            SnippetCheck("M264-D002-PROCESS-CPP-MODEL", "validation-consumes-json-sidecars-only-and-keeps-unsupported-profiles-fail-closed"),
            SnippetCheck("M264-D002-PROCESS-CPP-REPORT", "invalid conformance report schema_id"),
        ],
        RUNTIME_CPP: [
            SnippetCheck("M264-D002-RUNTIME-CPP", "objc3c-toolchain-conformance-claim-operations/m264-d002-v1"),
        ],
        RUNTIME_BOOTSTRAP_H: [
            SnippetCheck("M264-D002-RUNTIME-H", "module.objc3-conformance-validation.json"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M264-D002-DOCSOURCE-TITLE", "## CLI/toolchain conformance-claim operations (M264-D002)"),
            SnippetCheck("M264-D002-DOCSOURCE-VALIDATE", "module.objc3-conformance-validation.json"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M264-D002-DOCNATIVE-TITLE", "CLI/toolchain conformance-claim operations (M264-D002)"),
            SnippetCheck("M264-D002-DOCNATIVE-FORMAT-JSON", "--emit-objc3-conformance-format json"),
            SnippetCheck("M264-D002-DOCNATIVE-FORMAT-YAML", "--emit-objc3-conformance-format yaml"),
        ],
        SPEC_CHECKLIST: [
            SnippetCheck("M264-D002-SPEC-CHECKLIST", "## M264 CLI/toolchain conformance-claim operations (implementation note)"),
            SnippetCheck("M264-D002-SPEC-CHECKLIST-VALIDATE", "module.objc3-conformance-validation.json"),
        ],
        SPEC_DECISIONS: [
            SnippetCheck("M264-D002-SPEC-DECISION", "## D-023: Emit/validate conformance operations consume the shipped JSON sidecars only"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M264-D002-PKG-CHECK", "check:objc3c:m264-d002-conformance-claim-operations"),
            SnippetCheck("M264-D002-PKG-READINESS", "check:objc3c:m264-d002-lane-d-readiness"),
        ],
    }

    for path, snippets in static_snippets.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_summary: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        total, passed, dynamic_summary = run_dynamic_probes(args, failures)
        checks_total += total
        checks_passed += passed

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": not args.skip_dynamic_probes,
        "ok": not failures,
        "failures": [finding.__dict__ for finding in failures],
        "cases": dynamic_summary,
    }
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if failures:
        json.dump(payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 1
    print(f"[ok] M264-D002 conformance-claim operation checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
