#!/usr/bin/env python3
"""Validate sanitizer runtime evidence before any support promotion.

This checker is intentionally read-only for generated evidence: it does not
build packages or run probes.  It only verifies that the source-owned promotion
fixture and the already-generated runtime evidence agree and remain
fail-closed.
"""

from __future__ import annotations

import sys
import hashlib
from pathlib import Path
from typing import Any, Iterable

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS_ROOT = ROOT / "scripts"
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import validate_json_schema
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_package_channels.sanitizer_contracts import (
    runtime_package_variant_contract,
)


SCHEMA_PATH = (
    ROOT
    / "schemas"
    / "objc3c-sanitizer-runtime-promotion-evidence-v1.schema.json"
)
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "sanitizer_runtime_promotion_evidence_contract.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "security-hardening"
    / "sanitizer-runtime-promotion-evidence-summary.json"
)
GENERATED_REPORT_ROOT = ROOT / "tmp" / "reports" / "sanitizer-runtime-evidence"
PACKAGE_ROOTS = {
    "address": ROOT
    / "tmp"
    / "sre"
    / "pkg"
    / "check-sanitizer-runtime-evidence-asan"
    / "address"
    / "package",
    "undefined": ROOT
    / "tmp"
    / "sre"
    / "pkg"
    / "check-sanitizer-runtime-evidence-ubsan"
    / "undefined"
    / "package",
}

CONTRACT_ID = "objc3c.security.hardening.sanitizer.runtime-promotion-evidence.contract.v1"
SUMMARY_CONTRACT_ID = (
    "objc3c.security.hardening.sanitizer.runtime-promotion-evidence.summary.v1"
)
TARGET_PLATFORM_ID = "windows-x64"
REQUIRED_VARIANTS = ("address", "undefined")
PACKAGE_MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"


def fail(message: str) -> int:
    print(f"objc3c-sanitizer-runtime-promotion-evidence: {message}", file=sys.stderr)
    return 1


def display_path(path: Path) -> str:
    try:
        return repo_rel(path)
    except ValueError:
        return path.resolve().as_posix()


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def require_object(value: Any, label: str) -> dict[str, Any]:
    expect(isinstance(value, dict), f"{label} must be an object")
    return value


def require_list(value: Any, label: str) -> list[Any]:
    expect(isinstance(value, list), f"{label} must be a list")
    return value


def require_nonempty_string(value: Any, label: str) -> str:
    expect(isinstance(value, str) and bool(value), f"{label} must be a non-empty string")
    return value


def require_false(value: Any, label: str) -> None:
    expect(value is False, f"{label} must remain false")


def require_file(path: Path, label: str) -> Path:
    expect(path.is_file(), f"{label} missing: {display_path(path)}")
    return path


def require_repo_relative_path(raw_path: Any, label: str) -> str:
    value = require_nonempty_string(raw_path, label).replace("\\", "/")
    expect(not Path(value).is_absolute(), f"{label} must be repo-relative")
    expect(not value.startswith("../"), f"{label} must stay inside the repository")
    return value


def load_required_json(path: Path, label: str) -> dict[str, Any]:
    require_file(path, label)
    return load_json(path)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return "sha256:" + digest.hexdigest()


def variant_from_payload(payload: dict[str, Any], label: str) -> str:
    candidates = (
        payload.get("sanitizer_variant"),
        payload.get("sanitizer"),
        payload.get("variant"),
        payload.get("expected_detection"),
    )
    for candidate in candidates:
        if candidate in REQUIRED_VARIANTS:
            return str(candidate)
    variant_id = str(payload.get("variant_id", ""))
    for variant in REQUIRED_VARIANTS:
        if variant in variant_id:
            return variant
    raise RuntimeError(f"{label} missing sanitizer variant")


def first_string(payload: dict[str, Any], field_names: Iterable[str], label: str) -> str:
    for field_name in field_names:
        value = payload.get(field_name)
        if isinstance(value, str) and value:
            return value
    raise RuntimeError(f"{label} missing {', '.join(field_names)}")


def first_bool(payload: dict[str, Any], field_names: Iterable[str]) -> bool | None:
    for field_name in field_names:
        value = payload.get(field_name)
        if isinstance(value, bool):
            return value
    return None


def nested_object(payload: dict[str, Any], field_names: Iterable[str]) -> dict[str, Any] | None:
    for field_name in field_names:
        value = payload.get(field_name)
        if isinstance(value, dict):
            return value
    return None


def contract_rows(contract: dict[str, Any]) -> dict[str, dict[str, Any]]:
    rows: list[Any] = []
    for field_name in (
        "positive_contract_fixtures",
        "runtime_promotion_variants",
        "promotion_variants",
        "variants",
        "sanitizer_variants",
    ):
        value = contract.get(field_name)
        if isinstance(value, list):
            rows.extend(value)

    by_variant: dict[str, dict[str, Any]] = {}
    for index, raw_row in enumerate(rows):
        row = require_object(raw_row, f"source variant row {index}")
        variant = variant_from_payload(row, f"source variant row {index}")
        expect(variant not in by_variant, f"duplicate source fixture row for {variant}")
        by_variant[variant] = row

    missing = sorted(set(REQUIRED_VARIANTS) - set(by_variant))
    expect(not missing, f"source fixture missing sanitizer variants: {', '.join(missing)}")
    return by_variant


def expected_package_info(row: dict[str, Any], variant: str) -> dict[str, str]:
    package = nested_object(row, ("package", "package_contract", "package_manifest")) or row
    platform = first_string(
        row | package,
        ("target_platform_id", "platform_id", "platform"),
        f"{variant} source fixture",
    )
    package_id = first_string(
        row | package,
        ("package_id", "expected_package_id"),
        f"{variant} source fixture",
    )
    channel = first_string(
        row | package,
        ("package_channel_id", "channel_id", "expected_package_channel_id"),
        f"{variant} source fixture",
    )
    expect(platform == TARGET_PLATFORM_ID, f"{variant} source fixture platform drifted")
    contract = runtime_package_variant_contract(variant)
    expect(package_id == contract.package_id, f"{variant} source fixture package id drifted")
    expect(channel == contract.package_channel_id, f"{variant} source fixture package channel drifted")
    return {
        "sanitizer_variant": variant,
        "target_platform_id": platform,
        "package_id": package_id,
        "package_channel_id": channel,
    }


def negative_cases(contract: dict[str, Any]) -> list[dict[str, Any]]:
    raw_cases: Any = None
    for field_name in (
        "negative_contract_fixtures",
        "negative_contracts",
        "negative_cases",
        "fail_closed_negative_cases",
    ):
        value = contract.get(field_name)
        if isinstance(value, list):
            raw_cases = value
            break
    expect(raw_cases is not None, "source fixture negative cases missing")
    cases = [require_object(case, "source fixture negative case") for case in require_list(raw_cases, "source fixture negative cases")]
    expect(cases, "source fixture negative cases must be non-empty")

    variants_with_cases: set[str] = set()
    for index, case in enumerate(cases):
        variant = variant_from_payload(case, f"source fixture negative case {index}")
        variants_with_cases.add(variant)
        behavior = str(case.get("required_behavior", case.get("behavior", "")))
        expect(
            behavior.startswith("fail-closed"),
            f"{variant} negative case {index} must require fail-closed behavior",
        )
        for field_name in (
            "support_truth",
            "native_execution_claimed",
            "support_promotion_allowed",
            "release_runtime_mixing_allowed",
        ):
            if field_name in case:
                require_false(case.get(field_name), f"{variant} negative case {field_name}")

    missing = sorted(set(REQUIRED_VARIANTS) - variants_with_cases)
    expect(not missing, f"source fixture negative cases missing variants: {', '.join(missing)}")
    return cases


def validate_schema_surface(contract: dict[str, Any]) -> dict[str, str]:
    schema = load_required_json(SCHEMA_PATH, "sanitizer runtime promotion schema")
    try:
        validate_json_schema(contract, schema, label=display_path(CONTRACT_PATH))
    except Exception as exc:
        raise RuntimeError(f"source fixture schema validation failed: {exc}") from exc

    schema_contract_id = (
        schema.get("properties", {}).get("contract_id", {}).get("const")
        if isinstance(schema.get("properties"), dict)
        else None
    )
    if isinstance(schema_contract_id, str):
        expect(
            contract.get("contract_id") == schema_contract_id,
            "source fixture contract_id drifted from schema",
        )
    elif "contract_id" in contract:
        expect(contract.get("contract_id") == CONTRACT_ID, "source fixture contract_id drifted")

    return {
        "schema": display_path(SCHEMA_PATH),
        "source_contract": display_path(CONTRACT_PATH),
    }


def variant_row_from_summary(summary: dict[str, Any], variant: str) -> dict[str, Any]:
    rows = require_list(summary.get("variants"), f"{variant} generated summary variants")
    matches = [
        require_object(row, f"{variant} generated summary variant row")
        for row in rows
        if isinstance(row, dict) and row.get("sanitizer_variant") == variant
    ]
    expect(len(matches) == 1, f"{variant} generated summary must contain exactly one matching variant row")
    return matches[0]


def assert_status_pass(payload: dict[str, Any], label: str) -> None:
    expect(payload.get("status") == "PASS", f"{label} status must be PASS")


def assert_common_identity(
    payload: dict[str, Any],
    expected: dict[str, str],
    label: str,
    *,
    package_fields_required: bool = True,
) -> None:
    variant = expected["sanitizer_variant"]
    if "sanitizer_variant" in payload or "sanitizer" in payload or "variant" in payload:
        expect(variant_from_payload(payload, label) == variant, f"{label} sanitizer variant drifted")
    if "target_platform_id" in payload:
        expect(payload.get("target_platform_id") == expected["target_platform_id"], f"{label} platform drifted")
    elif package_fields_required:
        raise RuntimeError(f"{label} missing target platform")
    if "package_id" in payload:
        expect(payload.get("package_id") == expected["package_id"], f"{label} package id drifted")
    elif package_fields_required:
        raise RuntimeError(f"{label} missing package id")
    if "package_channel_id" in payload:
        expect(
            payload.get("package_channel_id") == expected["package_channel_id"],
            f"{label} package channel drifted",
        )
    elif package_fields_required:
        raise RuntimeError(f"{label} missing package channel")


def assert_no_promotion(payload: dict[str, Any], label: str) -> None:
    for field_name in ("support_truth", "native_execution_claimed", "support_promotion_allowed"):
        if field_name in payload:
            require_false(payload.get(field_name), f"{label}.{field_name}")


def artifact_set(payload: dict[str, Any], label: str) -> set[str]:
    artifacts = require_list(payload.get("runtime_library_artifacts"), f"{label} runtime_library_artifacts")
    values: set[str] = set()
    for index, raw_artifact in enumerate(artifacts):
        artifact = require_object(raw_artifact, f"{label} runtime_library_artifacts[{index}]")
        values.add(require_repo_relative_path(artifact.get("artifact"), f"{label} artifact path"))
        expect(str(artifact.get("sha256", "")), f"{label} artifact digest missing")
        expect(artifact.get("install_required") is True, f"{label} artifact must be install-required")
    return values


def validate_packaged_smoke(smoke: dict[str, Any], variant: str, label: str) -> dict[str, Any]:
    expect(smoke.get("exit_code") == 0, f"{label} packaged smoke exit code must be zero")
    expect(smoke.get("summary_status") == "PASS", f"{label} packaged smoke must be PASS")
    summary_rel = require_repo_relative_path(
        smoke.get("summary_path"),
        f"{variant} packaged smoke summary path",
    )
    smoke_summary = load_required_json(ROOT / summary_rel, f"{variant} packaged smoke summary")
    expect(smoke_summary.get("status") == "PASS", f"{variant} packaged smoke summary must be PASS")
    expect(smoke_summary.get("sanitizer_variant") == variant, f"{variant} packaged smoke summary variant drifted")
    return {
        "summary_status": "PASS",
        "exit_code": 0,
        "summary_path": summary_rel,
    }


def expected_generated_paths(variant: str) -> dict[str, str]:
    contract = runtime_package_variant_contract(variant)
    report_dir = GENERATED_REPORT_ROOT / variant
    package_root = PACKAGE_ROOTS[variant]
    run_id = f"sre-{variant}-{variant}"
    return {
        "summary_path": display_path(report_dir / "summary.json"),
        "probe_path": display_path(report_dir / "probe.json"),
        "diagnostics_path": display_path(report_dir / "diagnostics.json"),
        "package_manifest_path": display_path(package_root / PACKAGE_MANIFEST_RELATIVE_PATH),
        "runtime_manifest_path": display_path(package_root / str(contract.runtime_library_manifest_path)),
        "detection_stderr_path": display_path(
            report_dir / "probe-work" / f"{variant}_detection.stderr.txt",
        ),
        "packaged_smoke_path": display_path(
            package_root
            / "tmp"
            / "artifacts"
            / "objc3c-native"
            / "execution-smoke"
            / run_id
            / "summary.json",
        ),
    }


def validate_source_generated_paths(source_row: dict[str, Any], variant: str) -> dict[str, str]:
    paths = require_object(
        source_row.get("generated_evidence_paths"),
        f"{variant} source generated_evidence_paths",
    )
    expected_paths = expected_generated_paths(variant)
    for field_name, expected_path in expected_paths.items():
        actual_path = require_repo_relative_path(
            paths.get(field_name),
            f"{variant} generated evidence {field_name}",
        )
        expect(
            actual_path == expected_path,
            f"{variant} generated evidence {field_name} drifted: {actual_path} != {expected_path}",
        )
    return expected_paths


def validate_detection(
    detection: dict[str, Any],
    variant: str,
    stderr_path: Path,
    *,
    fixture_trap_or_recover_mode: str | None,
    source_expected_detection: dict[str, Any],
) -> dict[str, Any]:
    expect(detection.get("expected_detection") == variant, f"{variant} expected detection drifted")
    expect(int(detection.get("compile_exit_code", -1)) == 0, f"{variant} detection compile failed")
    run_exit_code = int(detection.get("run_exit_code", 0))
    expect(run_exit_code != 0, f"{variant} detection run exit must be nonzero")

    reported_stderr = require_repo_relative_path(
        detection.get("stderr"),
        f"{variant} detection stderr path",
    )
    expect(reported_stderr == display_path(stderr_path), f"{variant} detection stderr path drifted")
    require_file(stderr_path, f"{variant} detection stderr")

    mode = str(detection.get("trap_or_recover_mode", fixture_trap_or_recover_mode or ""))
    if fixture_trap_or_recover_mode is not None:
        expect(mode == fixture_trap_or_recover_mode, f"{variant} trap/recover mode drifted")

    stderr_text = stderr_path.read_text(encoding="utf-8", errors="replace")
    source_token = str(source_expected_detection.get("required_stderr_token", ""))
    if variant == "address":
        expect("AddressSanitizer" in stderr_text, "ASan detection stderr missing AddressSanitizer")
        if source_token:
            expect(source_token in stderr_text, f"{variant} detection stderr missing source fixture token")
    elif mode != "trap":
        expect(
            "UndefinedBehaviorSanitizer" in stderr_text or "runtime error" in stderr_text,
            "UBSan recover detection stderr missing sanitizer marker",
        )
        if source_token:
            expect(source_token in stderr_text, f"{variant} detection stderr missing source fixture token")

    return {
        "run_exit_code": run_exit_code,
        "expected_detection": variant,
        "trap_or_recover_mode": mode or None,
        "stderr": display_path(stderr_path),
    }


def validate_package_manifest(
    manifest: dict[str, Any],
    runtime_manifest: dict[str, Any],
    expected: dict[str, str],
    variant: str,
) -> dict[str, Any]:
    contract = runtime_package_variant_contract(variant)
    package_variant = require_object(
        manifest.get("sanitizer_package_variant"),
        f"{variant} package manifest sanitizer_package_variant",
    )
    assert_common_identity(package_variant, expected, f"{variant} package manifest")
    expect(
        manifest.get("runtime_variant") == contract.runtime_variant,
        f"{variant} package manifest runtime variant drifted",
    )
    expect(
        package_variant.get("runtime_library_manifest_path") == contract.runtime_library_manifest_path,
        f"{variant} package manifest runtime manifest path drifted",
    )
    require_false(package_variant.get("support_truth"), f"{variant} package manifest support_truth")
    require_false(
        package_variant.get("native_execution_claimed"),
        f"{variant} package manifest native_execution_claimed",
    )
    require_false(
        package_variant.get("release_runtime_mixing_allowed"),
        f"{variant} package manifest release_runtime_mixing_allowed",
    )
    require_false(
        package_variant.get("default_release_channel_allowed"),
        f"{variant} package manifest default_release_channel_allowed",
    )
    if contract.trap_or_recover_mode is not None:
        expect(
            package_variant.get("trap_or_recover_mode") == contract.trap_or_recover_mode,
            f"{variant} package manifest trap/recover mode drifted",
        )

    manifest_artifacts = artifact_set(package_variant, f"{variant} package manifest")
    runtime_artifacts = artifact_set(runtime_manifest, f"{variant} runtime manifest")
    expected_artifacts = {
        artifact
        for artifact in contract.runtime_library_payload_entries
        if artifact != str(contract.runtime_library_manifest_path)
    }
    expect(manifest_artifacts == expected_artifacts, f"{variant} package manifest artifact set drifted")
    expect(runtime_artifacts == expected_artifacts, f"{variant} runtime manifest artifact set drifted")
    expect(runtime_artifacts == manifest_artifacts, f"{variant} runtime/package artifact sets disagree")

    return {
        "manifest_path": PACKAGE_MANIFEST_RELATIVE_PATH,
        "runtime_manifest_path": str(contract.runtime_library_manifest_path),
        "artifact_count": len(runtime_artifacts),
        "release_runtime_mixing_allowed": False,
        "support_truth": False,
        "native_execution_claimed": False,
    }


def validate_runtime_manifest(
    manifest: dict[str, Any],
    expected: dict[str, str],
    variant: str,
) -> None:
    expect(manifest.get("contract_id") == "objc3c.sanitizer.runtime-library-manifest.v1", f"{variant} runtime manifest contract drifted")
    expect(manifest.get("sanitizer") == variant, f"{variant} runtime manifest sanitizer drifted")
    expect(manifest.get("target_platform_id") == expected["target_platform_id"], f"{variant} runtime manifest platform drifted")
    require_false(manifest.get("support_truth"), f"{variant} runtime manifest support_truth")
    require_false(manifest.get("native_execution_claimed"), f"{variant} runtime manifest native_execution_claimed")


def validate_generated_variant(variant: str, source_row: dict[str, Any]) -> dict[str, Any]:
    expected = expected_package_info(source_row, variant)
    contract = runtime_package_variant_contract(variant)
    source_paths = validate_source_generated_paths(source_row, variant)
    report_dir = GENERATED_REPORT_ROOT / variant
    package_root = PACKAGE_ROOTS[variant]
    summary_path = report_dir / "summary.json"
    probe_path = report_dir / "probe.json"
    diagnostics_path = report_dir / "diagnostics.json"
    stderr_path = report_dir / "probe-work" / f"{variant}_detection.stderr.txt"
    package_manifest_path = package_root / PACKAGE_MANIFEST_RELATIVE_PATH
    runtime_manifest_path = package_root / str(contract.runtime_library_manifest_path).replace("/", "/")

    summary = load_required_json(summary_path, f"{variant} generated summary")
    probe = load_required_json(probe_path, f"{variant} generated probe")
    diagnostics = load_required_json(diagnostics_path, f"{variant} generated diagnostics")
    package_manifest = load_required_json(package_manifest_path, f"{variant} package manifest")
    runtime_manifest = load_required_json(runtime_manifest_path, f"{variant} runtime manifest")
    require_file(stderr_path, f"{variant} detection stderr")

    assert_status_pass(summary, f"{variant} generated summary")
    assert_status_pass(probe, f"{variant} generated probe")
    assert_status_pass(diagnostics, f"{variant} generated diagnostics")
    assert_no_promotion(summary, f"{variant} generated summary")
    assert_no_promotion(probe, f"{variant} generated probe")
    assert_no_promotion(diagnostics, f"{variant} generated diagnostics")
    expect(summary.get("target_platform_id") == expected["target_platform_id"], f"{variant} generated summary platform drifted")

    summary_variant = variant_row_from_summary(summary, variant)
    expect(summary_variant.get("target_platform_id") == expected["target_platform_id"], f"{variant} generated summary row platform drifted")
    summary_package = require_object(summary_variant.get("package"), f"{variant} generated summary package")
    assert_common_identity(
        summary_package,
        expected,
        f"{variant} generated summary package",
        package_fields_required=False,
    )
    expect(
        summary_package.get("package_root") == display_path(package_root),
        f"{variant} generated summary package root drifted",
    )
    expect(
        summary_package.get("manifest_path") == display_path(package_manifest_path),
        f"{variant} generated summary package manifest path drifted",
    )

    runtime_probe = require_object(summary_variant.get("runtime_probe"), f"{variant} generated summary runtime_probe")
    expect(runtime_probe.get("probe_summary") == display_path(probe_path), f"{variant} summary probe path drifted")
    expect(runtime_probe.get("diagnostics") == display_path(diagnostics_path), f"{variant} summary diagnostics path drifted")
    embedded_probe = require_object(runtime_probe.get("probe"), f"{variant} generated summary embedded probe")
    expect(embedded_probe == probe, f"{variant} summary embedded probe disagrees with probe.json")

    assert_common_identity(probe, expected, f"{variant} generated probe")
    expect(probe.get("package_root") == display_path(package_root), f"{variant} probe package root drifted")
    expect(probe.get("package_manifest") == display_path(package_manifest_path), f"{variant} probe package manifest path drifted")
    expect(probe.get("runtime_library_manifest") == display_path(runtime_manifest_path), f"{variant} probe runtime manifest path drifted")
    probe_artifacts = artifact_set(probe, f"{variant} generated probe")

    assert_common_identity(diagnostics, expected, f"{variant} generated diagnostics", package_fields_required=False)
    expect(
        diagnostics.get("packaged_execution_smoke") == probe.get("packaged_execution_smoke"),
        f"{variant} diagnostics packaged smoke disagrees with probe",
    )
    expect(
        diagnostics.get("expected_detection_record") == probe.get("expected_detection_record"),
        f"{variant} diagnostics detection record disagrees with probe",
    )

    smoke = require_object(probe.get("packaged_execution_smoke"), f"{variant} packaged smoke")
    smoke_summary = validate_packaged_smoke(smoke, variant, f"{variant} probe")
    expect(
        smoke_summary["summary_path"] == source_paths["packaged_smoke_path"],
        f"{variant} source packaged smoke path drifted",
    )
    detection = require_object(probe.get("expected_detection_record"), f"{variant} expected detection record")
    fixture_mode = (
        str(source_row["trap_or_recover_mode"])
        if "trap_or_recover_mode" in source_row
        else None
    )
    source_expected_detection = require_object(
        source_row.get("expected_detection"),
        f"{variant} source expected detection",
    )
    detection_summary = validate_detection(
        detection,
        variant,
        stderr_path,
        fixture_trap_or_recover_mode=fixture_mode,
        source_expected_detection=source_expected_detection,
    )

    validate_runtime_manifest(runtime_manifest, expected, variant)
    runtime_manifest_digest = sha256_file(runtime_manifest_path)
    expect(
        probe.get("runtime_library_manifest_digest") == runtime_manifest_digest,
        f"{variant} probe runtime manifest digest drifted",
    )
    package_variant = require_object(
        package_manifest.get("sanitizer_package_variant"),
        f"{variant} package manifest sanitizer_package_variant",
    )
    expect(
        package_variant.get("runtime_library_manifest_digest") == runtime_manifest_digest,
        f"{variant} package manifest runtime manifest digest drifted",
    )
    package_summary = validate_package_manifest(
        package_manifest,
        runtime_manifest,
        expected,
        variant,
    )
    expect(
        probe_artifacts
        == {
            artifact
            for artifact in contract.runtime_library_payload_entries
            if artifact != str(contract.runtime_library_manifest_path)
        },
        f"{variant} probe runtime artifact set drifted",
    )

    return {
        "sanitizer_variant": variant,
        "target_platform_id": expected["target_platform_id"],
        "package_id": expected["package_id"],
        "package_channel_id": expected["package_channel_id"],
        "generated_summary": display_path(summary_path),
        "probe": display_path(probe_path),
        "diagnostics": display_path(diagnostics_path),
        "package_root": display_path(package_root),
        "package": package_summary,
        "digests": {
            "source_contract": sha256_file(CONTRACT_PATH),
            "summary": sha256_file(summary_path),
            "probe": sha256_file(probe_path),
            "diagnostics": sha256_file(diagnostics_path),
            "package_manifest": sha256_file(package_manifest_path),
            "runtime_manifest": runtime_manifest_digest,
            "detection_stderr": sha256_file(stderr_path),
            "packaged_smoke": sha256_file(ROOT / smoke_summary["summary_path"]),
        },
        "detection": detection_summary,
        "packaged_smoke": smoke_summary,
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
    }


def load_sanitizer_runtime_promotion_evidence_contract() -> dict[str, Any]:
    return load_required_json(CONTRACT_PATH, "sanitizer runtime promotion source fixture")


def validate_sanitizer_runtime_promotion_evidence_contract(
    contract: dict[str, Any],
) -> dict[str, Any]:
    schema_surface = validate_schema_surface(contract)
    rows = contract_rows(contract)
    negatives = negative_cases(contract)
    variants = [
        validate_generated_variant(variant, rows[variant])
        for variant in REQUIRED_VARIANTS
    ]
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_contract_id": contract.get("contract_id", CONTRACT_ID),
        "source_contract": display_path(CONTRACT_PATH),
        "schema_surface": schema_surface,
        "generated_report_root": display_path(GENERATED_REPORT_ROOT),
        "variants": variants,
        "negative_contract_fixtures": {
            "case_count": len(negatives),
            "variants": list(REQUIRED_VARIANTS),
        },
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
        "release_runtime_mixing_allowed": False,
        "summary_path": display_path(SUMMARY_PATH),
    }


def main() -> int:
    try:
        contract = load_sanitizer_runtime_promotion_evidence_contract()
        summary = validate_sanitizer_runtime_promotion_evidence_contract(contract)
    except RuntimeError as exc:
        return fail(str(exc))

    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {display_path(SUMMARY_PATH)}")
    print("objc3c-sanitizer-runtime-promotion-evidence: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
