#!/usr/bin/env python3
"""Build the live distribution-credibility dashboard from upstream release reports."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
POLICY_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility"
SOURCE_SURFACE_PATH = POLICY_ROOT / "source_surface.json"
TRUST_ARCHITECTURE_PATH = POLICY_ROOT / "trust_signal_architecture.json"
INSTALL_DOC_SURFACE_PATH = POLICY_ROOT / "install_release_doc_surface.json"
OPERATOR_POLICY_PATH = POLICY_ROOT / "operator_release_policy.json"
RELEASE_DRILL_POLICY_PATH = POLICY_ROOT / "release_drill_policy.json"
SCHEMA_SURFACE_PATH = POLICY_ROOT / "schema_surface.json"
ARTIFACT_SURFACE_PATH = POLICY_ROOT / "artifact_surface.json"
RELEASE_FOUNDATION_MANIFEST = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
PACKAGE_CHANNELS_E2E = ROOT / "tmp" / "reports" / "package-channels" / "end-to-end-summary.json"
RELEASE_OPERATIONS_PUBLICATION = ROOT / "tmp" / "reports" / "release-operations" / "publication-summary.json"
RELEASE_OPERATIONS_E2E = ROOT / "tmp" / "reports" / "release-operations" / "end-to-end-summary.json"
RELEASE_EVIDENCE_INDEX = ROOT / "tmp" / "reports" / "release_evidence" / "evidence-index.json"
RELEASE_EVIDENCE_CHECK = ROOT / "scripts" / "check_release_evidence.py"
OUTPUT_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "dashboard-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.dashboard.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_json(path: Path, *, kind: str) -> dict[str, Any]:
    if not path.is_file():
        raise RuntimeError(f"missing {kind}: {repo_rel(path)}")
    return load_json(path)


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(list(command), cwd=ROOT, text=True, capture_output=True, check=False)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def ensure_release_evidence_index() -> dict[str, Any]:
    if not RELEASE_EVIDENCE_INDEX.is_file():
        result = run_capture([sys.executable, str(RELEASE_EVIDENCE_CHECK)])
        if result.returncode != 0:
            raise RuntimeError("failed to generate release-evidence index")
    return require_json(RELEASE_EVIDENCE_INDEX, kind="release-evidence index")


def signal_status(passed: bool, *, warning: bool = False) -> str:
    if not passed:
        return "FAIL"
    if warning:
        return "WARN"
    return "PASS"


def main() -> int:
    try:
        source_surface = require_json(SOURCE_SURFACE_PATH, kind="source surface")
        trust_architecture = require_json(TRUST_ARCHITECTURE_PATH, kind="trust architecture")
        install_doc_surface = require_json(INSTALL_DOC_SURFACE_PATH, kind="install doc surface")
        operator_policy = require_json(OPERATOR_POLICY_PATH, kind="operator policy")
        release_drill_policy = require_json(RELEASE_DRILL_POLICY_PATH, kind="release drill policy")
        schema_surface = require_json(SCHEMA_SURFACE_PATH, kind="schema surface")
        artifact_surface = require_json(ARTIFACT_SURFACE_PATH, kind="artifact surface")
        release_manifest = require_json(RELEASE_FOUNDATION_MANIFEST, kind="release-foundation manifest")
        package_channels = require_json(PACKAGE_CHANNELS_E2E, kind="package-channels end-to-end summary")
        release_ops_publication = require_json(RELEASE_OPERATIONS_PUBLICATION, kind="release-operations publication summary")
        release_ops_e2e = require_json(RELEASE_OPERATIONS_E2E, kind="release-operations end-to-end summary")
        release_evidence = ensure_release_evidence_index()
    except RuntimeError as exc:
        print(f"objc3c-distribution-credibility-dashboard: FAIL\n- {exc}", file=sys.stderr)
        return 1

    failures: list[str] = []
    if release_manifest.get("contract_id") != "objc3c.release.foundation.manifest.v1":
        failures.append("release-foundation manifest contract drifted")
    if package_channels.get("status") != "PASS":
        failures.append("package-channels end-to-end summary did not pass")
    if release_ops_publication.get("status") != "PASS":
        failures.append("release-operations publication summary did not pass")
    if release_ops_e2e.get("status") != "PASS":
        failures.append("release-operations end-to-end summary did not pass")
    if release_evidence.get("schema_id") != "objc3-conformance-evidence-index/v1":
        failures.append("release-evidence index schema drifted")

    required_drill_steps = release_drill_policy.get("required_drill_steps", [])
    if not isinstance(required_drill_steps, list) or len(required_drill_steps) < 5:
        failures.append("release drill policy required_drill_steps drifted")
    operator_states = operator_policy.get("states", [])
    if operator_states != ["ready", "degraded", "blocked"]:
        failures.append("operator release states drifted")
    if source_surface.get("contract_id") != "objc3c.distribution.credibility.source.surface.v1":
        failures.append("source surface contract drifted")
    if trust_architecture.get("contract_id") != "objc3c.distribution.credibility.trust.signal.architecture.v1":
        failures.append("trust signal architecture contract drifted")
    if install_doc_surface.get("contract_id") != "objc3c.distribution.credibility.install.release.doc.surface.v1":
        failures.append("install release doc surface contract drifted")
    if schema_surface.get("contract_id") != "objc3c.distribution.credibility.schema.surface.v1":
        failures.append("schema surface contract drifted")
    if artifact_surface.get("contract_id") != "objc3c.distribution.credibility.artifact.surface.v1":
        failures.append("artifact surface contract drifted")

    release_id = str(release_manifest.get("primary_package_manifest_sha256", ""))
    release_version = str(release_manifest.get("release_version", "v0.11"))
    warning_count = int(release_ops_publication.get("warning_count", 0))

    trust_signals = [
        {
            "signal_id": "release-foundation-lineage",
            "status": signal_status(
                release_manifest.get("reproducibility_match") is True
                and isinstance(release_manifest.get("release_evidence_index_sha256"), str)
                and bool(release_manifest.get("release_evidence_index_sha256"))
            ),
            "source_path": repo_rel(RELEASE_FOUNDATION_MANIFEST),
            "detail": "release manifest retains reproducibility and evidence-index linkage",
        },
        {
            "signal_id": "package-channel-install-smoke",
            "status": signal_status(
                package_channels.get("status") == "PASS"
                and all(isinstance(package_channels.get(key), str) for key in ("portable_archive", "installer_archive", "offline_archive"))
            ),
            "source_path": repo_rel(PACKAGE_CHANNELS_E2E),
            "detail": "package channel install and rollback smoke stayed executable on the packaged release surface",
        },
        {
            "signal_id": "release-operations-metadata",
            "status": signal_status(
                release_ops_publication.get("status") == "PASS" and release_ops_e2e.get("status") == "PASS"
            ),
            "source_path": repo_rel(RELEASE_OPERATIONS_E2E),
            "detail": f"release operations publication remained coherent across {warning_count} expected compatibility warnings",
        },
        {
            "signal_id": "release-evidence-gate",
            "status": signal_status(release_evidence.get("schema_id") == "objc3-conformance-evidence-index/v1"),
            "source_path": repo_rel(RELEASE_EVIDENCE_INDEX),
            "detail": "release evidence index exists for the shipped conformance artifact set",
        },
    ]

    if any(signal["status"] == "FAIL" for signal in trust_signals) or failures:
        trust_state = "blocked"
    elif any(signal["status"] == "WARN" for signal in trust_signals):
        trust_state = "degraded"
    else:
        trust_state = "ready"

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "trust_state": trust_state,
        "release_id": release_id,
        "release_version": release_version,
        "warning_count": warning_count,
        "upstream_reports": {
            "release_manifest": repo_rel(RELEASE_FOUNDATION_MANIFEST),
            "package_channels_end_to_end": repo_rel(PACKAGE_CHANNELS_E2E),
            "release_operations_publication": repo_rel(RELEASE_OPERATIONS_PUBLICATION),
            "release_operations_end_to_end": repo_rel(RELEASE_OPERATIONS_E2E),
            "release_evidence_index": repo_rel(RELEASE_EVIDENCE_INDEX),
        },
        "trust_signals": trust_signals,
        "required_drill_steps": required_drill_steps,
        "install_docs": install_doc_surface.get("primary_docs", []),
        "release_docs": install_doc_surface.get("release_docs", []),
        "dashboard_schema": schema_surface.get("dashboard_schema"),
        "trust_report_schema": schema_surface.get("trust_report_schema"),
        "artifact_surface": artifact_surface,
        "failures": failures,
    }
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(OUTPUT_PATH)}")
    print("objc3c-distribution-credibility-dashboard: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
