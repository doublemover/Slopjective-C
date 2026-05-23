"""Release/runtime claim matrix model construction."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import display_path

from .load import summary_status
from .probes import MatrixProbeArtifacts


def first_line(text: str) -> str:
    stripped = text.strip()
    if not stripped:
        return ""
    return stripped.splitlines()[0]


def build_matrix(
    *,
    dependency_cases: dict[str, dict[str, Any]],
    probes: MatrixProbeArtifacts,
    native_report: dict[str, Any],
    native_publication: dict[str, Any],
    validation_payload: dict[str, Any],
    runner_report: dict[str, Any],
    runner_publication: dict[str, Any],
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c-release-runtime-claim-matrix/release_claims-published-matrix-v1",
        "schema_id": "objc3c-release-runtime-claim-matrix-v1",
        "publication_model": "derived-from-source-sema-lowering-runtime-and-integrated-native-frontend-probes",
        "profiles": [
            {"id": "core", "claim_status": "claimed", "selection_status": "supported", "runtime_status": "runnable"},
            {"id": "strict", "claim_status": "claimed", "selection_status": "supported", "runtime_status": "runnable"},
            {"id": "strict-concurrency", "claim_status": "claimed", "selection_status": "supported", "runtime_status": "runnable"},
            {"id": "strict-system", "claim_status": "not-claimed", "selection_status": "fail-closed", "runtime_status": "unsupported"},
        ],
        "language_profiles": [
            {"id": "canonical", "status": "supported"},
            {"id": "strict", "status": "supported"},
            {"id": "strict-concurrency", "status": "supported"},
        ],
        "canonical_literal_rejection_diagnostics": {"status": "supported"},
        "macro_claim_surface": {"status": "truthful-fail-closed"},
        "operator_formats": [
            {"format": "json", "emit_status": "supported", "validate_status": "supported"},
            {"format": "yaml", "emit_status": "fail-closed", "validate_status": "fail-closed"},
        ],
        "surfaces": [
            {"surface": "native-cli", "report": True, "publication": True, "validation": True},
            {"surface": "frontend-c-api", "report": True, "publication": True, "validation": False},
        ],
        "optional_features": [
            {"id": "throws", "status": "not-claimed"},
            {"id": "async-await", "status": "not-claimed"},
            {"id": "actors", "status": "not-claimed"},
            {"id": "blocks", "status": "not-claimed"},
            {"id": "arc", "status": "not-claimed"},
        ],
        "dependency_status": {
            name: {
                "summary_path": display_path(case["summary_path"]),
                "contract_id": case["payload"].get("contract_id"),
                "mode": case["payload"].get("mode"),
                "ok": summary_status(case["payload"]),
            }
            for name, case in dependency_cases.items()
        },
        "live_probes": {
            "native_cli": {
                "report_path": display_path(probes.native_report_path),
                "publication_path": display_path(probes.native_publication_path),
                "selected_profile": native_report.get("selected_profile"),
                "runtime_capability_profile": native_report.get("runtime_capability_report", {}).get("selected_profile"),
                "publication_surface_kind": native_publication.get("publication_surface_kind"),
            },
            "native_validation": {
                "validation_path": display_path(probes.validation_path),
                "selected_profile": validation_payload.get("selected_profile"),
                "format": validation_payload.get("format"),
                "publication_surface_kind": validation_payload.get("publication_surface_kind"),
            },
            "frontend_c_api": {
                "report_path": display_path(probes.runner_report_path),
                "publication_path": display_path(probes.runner_publication_path),
                "selected_profile": runner_report.get("selected_profile"),
                "publication_surface_kind": runner_publication.get("publication_surface_kind"),
            },
            "strict_system_reject": {
                "returncode": probes.strict_reject.returncode,
                "diagnostic": first_line(probes.strict_reject.stderr or probes.strict_reject.stdout),
            },
            "yaml_emit_reject": {
                "returncode": probes.yaml_reject.returncode,
                "diagnostic": first_line(probes.yaml_reject.stderr or probes.yaml_reject.stdout),
            },
        },
        "follow_on_surface": "objc3c.releaseclaims.compatibilityupgrade.boundary.v1",
        "ready": True,
    }
