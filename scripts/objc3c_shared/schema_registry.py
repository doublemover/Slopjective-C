"""Canonical checked-in schema path registry for tooling reports."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_any, load_json_object, validate_json_schema

ROOT = Path(__file__).resolve().parents[2]

SCHEMA_PATHS: dict[str, Path] = {
    "objc3-conformance-dashboard-status-v1": ROOT
    / "schemas"
    / "objc3-conformance-dashboard-status-v1.schema.json",
    "objc3-conformance-evidence-bundle-v1": ROOT
    / "schemas"
    / "objc3-conformance-evidence-bundle-v1.schema.json",
    "objc3-runtime-2025Q4-manifest": ROOT
    / "schemas"
    / "objc3-runtime-2025Q4.manifest.schema.json",
    "objc3-abi-2025Q4": ROOT / "schemas" / "objc3-abi-2025Q4.schema.json",
    "objc3c-application-architecture-evidence-summary-v1": ROOT
    / "schemas"
    / "objc3c-application-architecture-evidence-summary-v1.schema.json",
    "objc3c-artifact-authenticity-v1": ROOT
    / "schemas"
    / "objc3c-artifact-authenticity-v1.schema.json",
    "objc3c-abi-api-governance-v1": ROOT
    / "schemas"
    / "objc3c-abi-api-governance-v1.schema.json",
    "source-hygiene-hard-cutover-report-v1": ROOT
    / "schemas"
    / "source-hygiene-hard-cutover-report-v1.schema.json",
    "objc3c-governance-anti-regression-summary-v1": ROOT
    / "schemas"
    / "objc3c-governance-anti-regression-summary-v1.schema.json",
    "objc3c-governance-budget-summary-v1": ROOT
    / "schemas"
    / "objc3c-governance-budget-summary-v1.schema.json",
    "objc3c-governance-sustainability-evidence-v1": ROOT
    / "schemas"
    / "objc3c-governance-sustainability-evidence-v1.schema.json",
    "objc3c-adoption-legibility-evidence-v1": ROOT
    / "schemas"
    / "objc3c-adoption-legibility-evidence-v1.schema.json",
    "objc3c-long-horizon-operations-evidence-v1": ROOT
    / "schemas"
    / "objc3c-long-horizon-operations-evidence-v1.schema.json",
    "objc3c-upgrade-support-report-v1": ROOT
    / "schemas"
    / "objc3c-upgrade-support-report-v1.schema.json",
    "objc3c-package-channels-manifest-v1": ROOT
    / "schemas"
    / "objc3c-package-channels-manifest-v1.schema.json",
    "objc3c-package-manifest-v1": ROOT / "schemas" / "objc3c-package-manifest-v1.schema.json",
    "objc3c-package-lock-v1": ROOT / "schemas" / "objc3c-package-lock-v1.schema.json",
    "objc3c-package-signing-trust-v1": ROOT
    / "schemas"
    / "objc3c-package-signing-trust-v1.schema.json",
    "objc3c-package-offline-mirror-index-v1": ROOT
    / "schemas"
    / "objc3c-package-offline-mirror-index-v1.schema.json",
    "objc3c-package-hosted-registry-index-v1": ROOT
    / "schemas"
    / "objc3c-package-hosted-registry-index-v1.schema.json",
    "objc3c-package-hosted-registry-service-v1": ROOT
    / "schemas"
    / "objc3c-package-hosted-registry-service-v1.schema.json",
    "objc3c-package-network-resolution-v1": ROOT
    / "schemas"
    / "objc3c-package-network-resolution-v1.schema.json",
    "objc3c-package-release-channel-publication-v1": ROOT
    / "schemas"
    / "objc3c-package-release-channel-publication-v1.schema.json",
    "objc3c-package-local-registry-index-v1": ROOT
    / "schemas"
    / "objc3c-package-local-registry-index-v1.schema.json",
    "objc3c-package-install-receipt-v1": ROOT
    / "schemas"
    / "objc3c-package-install-receipt-v1.schema.json",
    "objc3c-sanitizer-runtime-library-manifest-v1": ROOT
    / "schemas"
    / "objc3c-sanitizer-runtime-library-manifest-v1.schema.json",
    "objc3c-package-install-distribution-receipt-v1": ROOT
    / "schemas"
    / "objc3c-package-install-distribution-receipt-v1.schema.json",
    "objc3c-package-install-distribution-operation-receipt-v1": ROOT
    / "schemas"
    / "objc3c-package-install-distribution-operation-receipt-v1.schema.json",
    "objc3c-package-operation-receipt-v1": ROOT
    / "schemas"
    / "objc3c-package-operation-receipt-v1.schema.json",
    "objc3c-platform-support-matrix-v1": ROOT
    / "schemas"
    / "objc3c-platform-support-matrix-v1.schema.json",
    "objc3c-platform-toolchain-support-evidence-v1": ROOT
    / "schemas"
    / "objc3c-platform-toolchain-support-evidence-v1.schema.json",
    "objc3c-platform-expansion-claim-contract-v1": ROOT
    / "schemas"
    / "objc3c-platform-expansion-claim-contract-v1.schema.json",
    "objc3c-platform-support-source-truth-v1": ROOT
    / "schemas"
    / "objc3c-platform-support-source-truth-v1.schema.json",
    "objc3c-compiler-throughput-summary-v1": ROOT
    / "schemas"
    / "objc3c-compiler-throughput-summary-v1.schema.json",
    "objc3c-developer-tooling-editor-surface-v1": ROOT
    / "schemas"
    / "objc3c-developer-tooling-editor-surface-v1.schema.json",
    "objc3c-developer-tooling-schema-surface-summary-v1": ROOT
    / "schemas"
    / "objc3c-developer-tooling-schema-surface-summary-v1.schema.json",
    "objc3c-runtime-debug-trace-v1": ROOT
    / "schemas"
    / "objc3c-runtime-debug-trace-v1.schema.json",
    "objc3c-advanced-runtime-executable-contract-v1": ROOT
    / "schemas"
    / "objc3c-advanced-runtime-executable-contract-v1.schema.json",
    "objc3c-standalone-textual-interface-payload-v1": ROOT
    / "schemas"
    / "objc3c-standalone-textual-interface-payload-v1.schema.json",
    "objc3c-typed-throws-effect-contract-v1": ROOT
    / "schemas"
    / "objc3c-typed-throws-effect-contract-v1.schema.json",
    "objc3c-foundations-umbrella-source-truth-v1": ROOT
    / "schemas"
    / "objc3c-foundations-umbrella-source-truth-v1.schema.json",
    "objc3c-performance-telemetry-v1": ROOT
    / "schemas"
    / "objc3c-performance-telemetry-v1.schema.json",
    "objc3c-optimization-runtime-debug-safety-v1": ROOT
    / "schemas"
    / "objc3c-optimization-runtime-debug-safety-v1.schema.json",
    "objc3c-performance-dashboard-summary-v1": ROOT
    / "schemas"
    / "objc3c-performance-dashboard-summary-v1.schema.json",
    "objc3c-performance-public-report-v1": ROOT
    / "schemas"
    / "objc3c-performance-public-report-v1.schema.json",
    "objc3c-public-command-contract-v1": ROOT / "schemas" / "objc3c-public-command-contract-v1.schema.json",
    "objc3c-public-conformance-scorecard-v1": ROOT
    / "schemas"
    / "objc3c-public-conformance-scorecard-v1.schema.json",
    "objc3c-public-conformance-suite-v1": ROOT
    / "schemas"
    / "objc3c-public-conformance-suite-v1.schema.json",
    "objc3c-public-conformance-summary-v1": ROOT
    / "schemas"
    / "objc3c-public-conformance-summary-v1.schema.json",
    "objc3c-release-attestation-v1": ROOT / "schemas" / "objc3c-release-attestation-v1.schema.json",
    "objc3c-release-manifest-v1": ROOT / "schemas" / "objc3c-release-manifest-v1.schema.json",
    "objc3c-release-channel-operations-v1": ROOT
    / "schemas"
    / "objc3c-release-channel-operations-v1.schema.json",
    "objc3c-update-manifest-v1": ROOT / "schemas" / "objc3c-update-manifest-v1.schema.json",
    "objc3c-release-sbom-v1": ROOT / "schemas" / "objc3c-release-sbom-v1.schema.json",
    "objc3c-runtime-performance-telemetry-v1": ROOT
    / "schemas"
    / "objc3c-runtime-performance-telemetry-v1.schema.json",
    "objc3c-security-advisory-index-v1": ROOT
    / "schemas"
    / "objc3c-security-advisory-index-v1.schema.json",
    "objc3c-security-posture-v1": ROOT / "schemas" / "objc3c-security-posture-v1.schema.json",
    "objc3c-distribution-credibility-dashboard-v1": ROOT
    / "schemas"
    / "objc3c-distribution-credibility-dashboard-v1.schema.json",
    "objc3c-distribution-trust-report-v1": ROOT
    / "schemas"
    / "objc3c-distribution-trust-report-v1.schema.json",
    "objc3c-full-envelope-dashboard-summary-v1": ROOT
    / "schemas"
    / "objc3c-full-envelope-dashboard-summary-v1.schema.json",
    "objc3c-validation-acceptance-artifact-index-v1": ROOT
    / "schemas"
    / "objc3c-validation-acceptance-artifact-index-v1.schema.json",
    "objc3c-capability-matrix-v1": ROOT
    / "schemas"
    / "objc3c-capability-matrix-v1.schema.json",
    "objc3c-capability-evidence-map-v1": ROOT
    / "schemas"
    / "objc3c-capability-evidence-map-v1.schema.json",
    "objc3c-umbrella-readiness-v1": ROOT
    / "schemas"
    / "objc3c-umbrella-readiness-v1.schema.json",
}


def schema_path(schema_id: str) -> Path:
    try:
        return SCHEMA_PATHS[schema_id]
    except KeyError as exc:
        raise KeyError(f"unknown schema id: {schema_id}") from exc


def schema_ids() -> tuple[str, ...]:
    return tuple(sorted(SCHEMA_PATHS))


def load_schema(schema_id: str) -> dict[str, Any]:
    return load_json_object(schema_path(schema_id))


def validate_registered_schema(payload: Any, schema_id: str, *, label: str | None = None) -> None:
    validate_json_schema(payload, load_schema(schema_id), label=label or schema_id)


def load_json_with_registered_schema(path: Path | str, schema_id: str) -> Any:
    payload = load_json_any(path)
    validate_registered_schema(payload, schema_id, label=str(path))
    return payload


def schema_registry_summary() -> dict[str, str]:
    return {
        schema_id: path.relative_to(ROOT).as_posix()
        for schema_id, path in sorted(SCHEMA_PATHS.items())
    }
