#!/usr/bin/env python3
"""Fail-closed checker for M263-E002 runnable multi-image bootstrap matrix closeout."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-e002-runnable-multi-image-bootstrap-matrix-closeout-v1"
CONTRACT_ID = "objc3c-runtime-runnable-bootstrap-matrix-closeout/m263-e002-v1"
CLOSEOUT_MODEL = "e001-gate-plus-published-bootstrap-matrix-runbook"
RUNBOOK_RUN_ID = "m263_e002_bootstrap_matrix_closeout"
EXPECTED_NATIVE_EXE = "artifacts/bin/objc3c-native.exe"
EXPECTED_RUNTIME_LIBRARY = "artifacts/lib/objc3_runtime.lib"
EXPECTED_MATRIX_SCRIPT = "scripts/check_objc3c_bootstrap_matrix.ps1"
EXPECTED_RUNBOOK = "docs/runbooks/m263_bootstrap_matrix_operator_runbook.md"
EXPECTED_SUMMARY = "tmp/reports/m263/M263-E002/bootstrap_matrix_closeout_summary.json"
EXPECTED_MATRIX_SUMMARY = f"tmp/artifacts/objc3c-native/bootstrap-matrix/{RUNBOOK_RUN_ID}/summary.json"
EXPECTED_LLC_PATH = Path(r"C:\Program Files\LLVM\bin\llc.exe")
E001_CONTRACT_ID = "objc3c-runtime-bootstrap-completion-gate/m263-e001-v1"
C003_CONTRACT_ID = "objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1"
D003_CONTRACT_ID = "objc3c-runtime-live-restart-hardening/m263-d003-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync_e002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync_packet.md"
DEFAULT_RUNBOOK_DOC = ROOT / "docs" / "runbooks" / "m263_bootstrap_matrix_operator_runbook.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_MATRIX_SCRIPT = ROOT / "scripts" / "check_objc3c_bootstrap_matrix.ps1"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m263_e002_lane_e_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_E001_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-E001" / "bootstrap_completion_conformance_gate_summary.json"
DEFAULT_C003_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-C003" / "archive_static_link_bootstrap_replay_corpus_summary.json"
DEFAULT_D003_SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-D003" / "live_restart_hardening_summary.json"
DEFAULT_SUMMARY_OUT = ROOT / "tmp" / "reports" / "m263" / "M263-E002" / "bootstrap_matrix_closeout_summary.json"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M263-E002-DOC-EXP-01", "# M263 Runnable Multi-Image Bootstrap Matrix Cross-Lane Integration Sync Expectations (E002)"),
    SnippetCheck("M263-E002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-E002-DOC-EXP-03", "docs/runbooks/m263_bootstrap_matrix_operator_runbook.md"),
    SnippetCheck("M263-E002-DOC-EXP-04", "scripts/check_objc3c_bootstrap_matrix.ps1"),
    SnippetCheck("M263-E002-DOC-EXP-05", EXPECTED_SUMMARY),
)
PACKET_SNIPPETS = (
    SnippetCheck("M263-E002-DOC-PKT-01", "# M263-E002 Runnable Multi-Image Bootstrap Matrix Cross-Lane Integration Sync Packet"),
    SnippetCheck("M263-E002-DOC-PKT-02", "Packet: `M263-E002`"),
    SnippetCheck("M263-E002-DOC-PKT-03", "- `M263-E001`"),
    SnippetCheck("M263-E002-DOC-PKT-04", "- `M263-C003`"),
    SnippetCheck("M263-E002-DOC-PKT-05", "- `M263-D003`"),
    SnippetCheck("M263-E002-DOC-PKT-06", "`check:objc3c:m263-e002-lane-e-readiness`"),
)
RUNBOOK_SNIPPETS = (
    SnippetCheck("M263-E002-RUNBOOK-01", "# M263 Bootstrap Matrix Operator Runbook"),
    SnippetCheck("M263-E002-RUNBOOK-02", "npm run check:objc3c:m263-e001-lane-e-readiness"),
    SnippetCheck("M263-E002-RUNBOOK-03", "$env:OBJC3C_BOOTSTRAP_MATRIX_RUN_ID='m263_e002_bootstrap_matrix_closeout'"),
    SnippetCheck("M263-E002-RUNBOOK-04", r"$env:OBJC3C_BOOTSTRAP_MATRIX_LLC_PATH='C:\Program Files\LLVM\bin\llc.exe'"),
    SnippetCheck("M263-E002-RUNBOOK-05", r"& 'C:\Program Files\PowerShell\7\pwsh.exe' -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_bootstrap_matrix.ps1"),
    SnippetCheck("M263-E002-RUNBOOK-06", EXPECTED_MATRIX_SUMMARY),
    SnippetCheck("M263-E002-RUNBOOK-07", "npm run check:objc3c:m263-e002-lane-e-readiness"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M263-E002-NDOC-01", "## Runnable multi-image bootstrap matrix (M263-E002)"),
    SnippetCheck("M263-E002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-E002-NDOC-03", f"`{CLOSEOUT_MODEL}`"),
    SnippetCheck("M263-E002-NDOC-04", EXPECTED_RUNBOOK),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M263-E002-SPC-01", "## M263 runnable multi-image bootstrap matrix closeout (E002)"),
    SnippetCheck("M263-E002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-E002-SPC-03", f"`{CLOSEOUT_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M263-E002-META-01", "## M263 runnable bootstrap matrix metadata anchors (E002)"),
    SnippetCheck("M263-E002-META-02", "`module.runtime-registration-descriptor.json`"),
    SnippetCheck("M263-E002-META-03", EXPECTED_MATRIX_SUMMARY),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M263-E002-RTDOC-01", "`M263-E002` closes the bootstrap-hardening milestone with one published operator runbook"),
    SnippetCheck("M263-E002-RTDOC-02", "`docs/runbooks/m263_bootstrap_matrix_operator_runbook.md`"),
    SnippetCheck("M263-E002-RTDOC-03", "`scripts/check_objc3c_bootstrap_matrix.ps1`"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M263-E002-DRV-01", "M263-E002 bootstrap-matrix closeout anchor"),
    SnippetCheck("M263-E002-DRV-02", "published operator runbook and matrix proof"),
)
MANIFEST_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M263-E002-MAN-01", "M263-E002 bootstrap-matrix closeout anchor"),
    SnippetCheck("M263-E002-MAN-02", "published bootstrap matrix consumes the same canonical artifact pair"),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M263-E002-FAPI-01", "M263-E002 bootstrap-matrix closeout anchor"),
    SnippetCheck("M263-E002-FAPI-02", "published bootstrap matrix stays identical across CLI and frontend C API entry points"),
)
MATRIX_SCRIPT_SNIPPETS = (
    SnippetCheck("M263-E002-SCRIPT-01", "m263_e002_bootstrap_matrix_closeout"),
    SnippetCheck("M263-E002-SCRIPT-02", "archive-backed-merged-retained"),
    SnippetCheck("M263-E002-SCRIPT-03", "tmp/artifacts/objc3c-native/bootstrap-matrix"),
)
READINESS_RUNNER_SNIPPETS = (
    SnippetCheck("M263-E002-RUN-01", "check:objc3c:m263-e001-lane-e-readiness"),
    SnippetCheck("M263-E002-RUN-02", "check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py"),
    SnippetCheck("M263-E002-RUN-03", "test_check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M263-E002-PKG-01", '"check:objc3c:m263-e002-runnable-multi-image-bootstrap-matrix": "python scripts/check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py"'),
    SnippetCheck("M263-E002-PKG-02", '"test:tooling:m263-e002-runnable-multi-image-bootstrap-matrix": "python -m pytest tests/tooling/test_check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py -q"'),
    SnippetCheck("M263-E002-PKG-03", '"check:objc3c:m263-e002-lane-e-readiness": "python scripts/run_m263_e002_lane_e_readiness.py"'),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--runbook-doc", type=Path, default=DEFAULT_RUNBOOK_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--manifest-artifacts-cpp", type=Path, default=DEFAULT_MANIFEST_ARTIFACTS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--matrix-script", type=Path, default=DEFAULT_MATRIX_SCRIPT)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--e001-summary", type=Path, default=DEFAULT_E001_SUMMARY)
    parser.add_argument("--c003-summary", type=Path, default=DEFAULT_C003_SUMMARY)
    parser.add_argument("--d003-summary", type=Path, default=DEFAULT_D003_SUMMARY)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--runbook-run-id", default=RUNBOOK_RUN_ID)
    parser.add_argument("--llc-path", type=Path)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


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


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M263-E002-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def resolve_pwsh() -> str | None:
    return shutil.which("pwsh") or shutil.which("pwsh.exe") or shutil.which("powershell") or shutil.which("powershell.exe")


def resolve_llc_path(explicit: Path | None) -> Path | None:
    if explicit is not None:
        return explicit.resolve()
    if EXPECTED_LLC_PATH.exists():
        return EXPECTED_LLC_PATH.resolve()
    located = shutil.which("llc") or shutil.which("llc.exe")
    return Path(located).resolve() if located else None


def run_command(command: Sequence[str], env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=str(ROOT), capture_output=True, text=True, encoding="utf-8", errors="replace", env=env, check=False)


def validate_e001(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E002-E001-01", "E001 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M263-E002-E001-02", "E001 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M263-E002-E001-03", "E001 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == E001_CONTRACT_ID, artifact, "M263-E002-E001-04", "E001 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("next_closeout_issue") == "M263-E002", artifact, "M263-E002-E001-05", "E001 must hand off to M263-E002", failures)

    upstream = payload.get("upstream_evidence", {}) if isinstance(payload.get("upstream_evidence"), dict) else {}
    c003 = upstream.get("M263-C003", {}) if isinstance(upstream.get("M263-C003"), dict) else {}
    d003 = upstream.get("M263-D003", {}) if isinstance(upstream.get("M263-D003"), dict) else {}

    checks_total += 1
    checks_passed += require(c003.get("ok") is True, artifact, "M263-E002-E001-06", "E001 must retain passing C003 evidence", failures)
    checks_total += 1
    checks_passed += require(c003.get("merged_startup_registered_image_count") == 2, artifact, "M263-E002-E001-07", "E001 must retain two-image merged startup evidence", failures)
    checks_total += 1
    checks_passed += require(d003.get("ok") is True, artifact, "M263-E002-E001-08", "E001 must retain passing D003 evidence", failures)
    checks_total += 1
    checks_passed += require(d003.get("default_second_restart_replay_generation") == 2, artifact, "M263-E002-E001-09", "E001 must retain second restart replay generation 2", failures)

    return checks_total, checks_passed, {
        "ok": True,
        "contract_id": payload.get("contract_id"),
        "next_closeout_issue": payload.get("next_closeout_issue"),
        "merged_startup_registered_image_count": c003.get("merged_startup_registered_image_count"),
        "default_second_restart_replay_generation": d003.get("default_second_restart_replay_generation"),
    }


def validate_c003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    dynamic = payload.get("dynamic_summary", {}) if isinstance(payload.get("dynamic_summary"), dict) else {}
    plain = dynamic.get("plain_probe", {}) if isinstance(dynamic.get("plain_probe"), dict) else {}
    single = dynamic.get("single_probe", {}) if isinstance(dynamic.get("single_probe"), dict) else {}
    merged = dynamic.get("merged_probe", {}) if isinstance(dynamic.get("merged_probe"), dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E002-C003-01", "C003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M263-E002-C003-02", "C003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_executed") is True, artifact, "M263-E002-C003-03", "C003 dynamic probes must execute", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == C003_CONTRACT_ID, artifact, "M263-E002-C003-04", "C003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("object_format") == "coff", artifact, "M263-E002-C003-05", "C003 object format drifted", failures)
    checks_total += 1
    checks_passed += require(plain.get("startup_registered_image_count") == 0, artifact, "M263-E002-C003-06", "C003 plain archive startup count drifted", failures)
    checks_total += 1
    checks_passed += require(single.get("startup_registered_image_count") == 1 and single.get("post_replay_registered_image_count") == 1, artifact, "M263-E002-C003-07", "C003 single retained replay evidence drifted", failures)
    checks_total += 1
    checks_passed += require(merged.get("startup_registered_image_count") == 2 and merged.get("post_replay_registered_image_count") == 2, artifact, "M263-E002-C003-08", "C003 merged retained replay evidence drifted", failures)
    checks_total += 1
    checks_passed += require(merged.get("post_replay_next_expected_registration_order_ordinal") == 3, artifact, "M263-E002-C003-09", "C003 merged replay ordinal drifted", failures)

    return checks_total, checks_passed, {
        "ok": True,
        "contract_id": payload.get("contract_id"),
        "plain_startup_registered_image_count": plain.get("startup_registered_image_count"),
        "single_startup_registered_image_count": single.get("startup_registered_image_count"),
        "merged_startup_registered_image_count": merged.get("startup_registered_image_count"),
    }


def validate_d003(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    cases = payload.get("dynamic_cases", []) if isinstance(payload.get("dynamic_cases"), list) else []
    case_map = {str(case.get("case_id")): case for case in cases if isinstance(case, dict)}
    default = case_map.get("default", {}).get("probe_payload", {}) if isinstance(case_map.get("default"), dict) else {}
    explicit = case_map.get("explicit", {}).get("probe_payload", {}) if isinstance(case_map.get("explicit"), dict) else {}

    checks_total += 1
    checks_passed += require(payload.get("ok") is True, artifact, "M263-E002-D003-01", "D003 summary must report ok=true", failures)
    checks_total += 1
    checks_passed += require(payload.get("checks_passed") == payload.get("checks_total"), artifact, "M263-E002-D003-02", "D003 summary must report full check coverage", failures)
    checks_total += 1
    checks_passed += require(payload.get("contract_id") == D003_CONTRACT_ID, artifact, "M263-E002-D003-03", "D003 contract id drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("dynamic_probes_skipped") is False and payload.get("dynamic_case_count") == 2, artifact, "M263-E002-D003-04", "D003 dynamic coverage drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("startup_registered_image_count") == 1 and explicit.get("startup_registered_image_count") == 1, artifact, "M263-E002-D003-05", "D003 startup counts drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("post_reset_registered_image_count") == 0 and explicit.get("post_reset_registered_image_count") == 0, artifact, "M263-E002-D003-06", "D003 reset counts drifted", failures)
    checks_total += 1
    checks_passed += require(default.get("second_restart_replay_generation") == 2 and explicit.get("second_restart_replay_generation") == 2, artifact, "M263-E002-D003-07", "D003 replay generation drifted", failures)

    return checks_total, checks_passed, {
        "ok": True,
        "contract_id": payload.get("contract_id"),
        "default_startup_registered_image_count": default.get("startup_registered_image_count"),
        "explicit_startup_registered_image_count": explicit.get("startup_registered_image_count"),
        "default_second_restart_replay_generation": default.get("second_restart_replay_generation"),
    }


def validate_matrix_summary(path: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    payload = load_json(path)
    artifact = display_path(path)
    checks_total = 0
    checks_passed = 0
    cases = payload.get("cases", []) if isinstance(payload.get("cases"), list) else []
    case_map = {str(case.get("case_id")): case for case in cases if isinstance(case, dict)}

    checks_total += 1
    checks_passed += require(payload.get("mode") == "m263-bootstrap-matrix-v1", artifact, "M263-E002-MAT-01", "matrix mode drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("status") == "PASS", artifact, "M263-E002-MAT-02", "matrix summary must report PASS", failures)
    checks_total += 1
    checks_passed += require(payload.get("native_exe") == EXPECTED_NATIVE_EXE, artifact, "M263-E002-MAT-03", "matrix native exe drifted", failures)
    checks_total += 1
    checks_passed += require(payload.get("runtime_library") == EXPECTED_RUNTIME_LIBRARY, artifact, "M263-E002-MAT-04", "matrix runtime library drifted", failures)
    checks_total += 1
    checks_passed += require(len(cases) == 5, artifact, "M263-E002-MAT-05", "matrix must publish exactly five cases", failures)

    expected_ids = {
        "single-image-default",
        "single-image-explicit",
        "archive-backed-plain",
        "archive-backed-single-retained",
        "archive-backed-merged-retained",
    }
    checks_total += 1
    checks_passed += require(set(case_map) == expected_ids, artifact, "M263-E002-MAT-06", "matrix case ids drifted", failures)

    checks_total += 1
    checks_passed += require(case_map.get("single-image-default", {}).get("startup_registered_image_count") == 1, artifact, "M263-E002-MAT-07", "single-image-default startup count drifted", failures)
    checks_total += 1
    checks_passed += require(case_map.get("single-image-explicit", {}).get("startup_registered_image_count") == 1, artifact, "M263-E002-MAT-08", "single-image-explicit startup count drifted", failures)
    checks_total += 1
    checks_passed += require(case_map.get("archive-backed-plain", {}).get("startup_registered_image_count") == 0, artifact, "M263-E002-MAT-09", "archive-backed-plain startup count drifted", failures)
    checks_total += 1
    checks_passed += require(case_map.get("archive-backed-single-retained", {}).get("startup_registered_image_count") == 1, artifact, "M263-E002-MAT-10", "archive-backed-single-retained startup count drifted", failures)
    checks_total += 1
    checks_passed += require(case_map.get("archive-backed-merged-retained", {}).get("startup_registered_image_count") == 2, artifact, "M263-E002-MAT-11", "archive-backed-merged-retained startup count drifted", failures)
    checks_total += 1
    checks_passed += require(case_map.get("archive-backed-merged-retained", {}).get("post_replay_next_expected_registration_order_ordinal") == 3, artifact, "M263-E002-MAT-12", "archive-backed-merged-retained ordinal drifted", failures)
    checks_total += 1
    checks_passed += require(all(case.get("status") == "PASS" for case in case_map.values()), artifact, "M263-E002-MAT-13", "every matrix case must report PASS", failures)

    llc_source = str(payload.get("llc_source", ""))
    checks_total += 1
    checks_passed += require(llc_source.lower().endswith("llc.exe"), artifact, "M263-E002-MAT-14", "matrix llc source must resolve to llc.exe", failures)

    return checks_total, checks_passed, {
        "status": payload.get("status"),
        "summary": display_path(path),
        "case_count": len(cases),
        "llc_source": payload.get("llc_source"),
    }


def run_matrix_probe(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    artifact = display_path(args.matrix_script)
    pwsh = resolve_pwsh()
    llc_path = resolve_llc_path(args.llc_path)

    checks_total += 1
    checks_passed += require(pwsh is not None, artifact, "M263-E002-DYN-01", "PowerShell executable is unavailable", failures)
    checks_total += 1
    checks_passed += require(llc_path is not None and llc_path.exists(), artifact, "M263-E002-DYN-02", "llc executable is unavailable", failures)
    if pwsh is None or llc_path is None or not llc_path.exists():
        return checks_total, checks_passed, {"skipped": True}

    env = os.environ.copy()
    env["OBJC3C_BOOTSTRAP_MATRIX_RUN_ID"] = args.runbook_run_id
    env["OBJC3C_BOOTSTRAP_MATRIX_LLC_PATH"] = str(llc_path)

    completed = run_command(
        [pwsh, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(args.matrix_script), "-RunId", args.runbook_run_id, "-LlcPath", str(llc_path)],
        env=env,
    )
    checks_total += 1
    checks_passed += require(completed.returncode == 0, artifact, "M263-E002-DYN-03", f"bootstrap matrix command failed with exit code {completed.returncode}: {completed.stderr.strip() or completed.stdout.strip()}", failures)

    matrix_summary_path = ROOT / "tmp" / "artifacts" / "objc3c-native" / "bootstrap-matrix" / args.runbook_run_id / "summary.json"
    checks_total += 1
    checks_passed += require(matrix_summary_path.exists(), artifact, "M263-E002-DYN-04", "bootstrap matrix summary is missing", failures)
    if not matrix_summary_path.exists():
        return checks_total, checks_passed, {"stdout": completed.stdout, "stderr": completed.stderr}

    extra_total, extra_passed, distilled = validate_matrix_summary(matrix_summary_path, failures)
    checks_total += extra_total
    checks_passed += extra_passed
    distilled.update({"stdout": completed.stdout, "stderr": completed.stderr})
    return checks_total, checks_passed, distilled


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_targets = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.runbook_doc, RUNBOOK_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.manifest_artifacts_cpp, MANIFEST_ARTIFACTS_SNIPPETS),
        (args.frontend_anchor_cpp, FRONTEND_ANCHOR_SNIPPETS),
        (args.matrix_script, MATRIX_SCRIPT_SNIPPETS),
        (args.readiness_runner, READINESS_RUNNER_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in static_targets:
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    extra_total, extra_passed, e001_distilled = validate_e001(args.e001_summary, failures)
    checks_total += extra_total
    checks_passed += extra_passed

    extra_total, extra_passed, c003_distilled = validate_c003(args.c003_summary, failures)
    checks_total += extra_total
    checks_passed += extra_passed

    extra_total, extra_passed, d003_distilled = validate_d003(args.d003_summary, failures)
    checks_total += extra_total
    checks_passed += extra_passed

    dynamic_probes_executed = not args.skip_dynamic_probes
    matrix_probe: dict[str, Any] = {"skipped": args.skip_dynamic_probes}
    if not args.skip_dynamic_probes:
        extra_total, extra_passed, matrix_probe = run_matrix_probe(args, failures)
        checks_total += extra_total
        checks_passed += extra_passed

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "closeout_model": CLOSEOUT_MODEL,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "runbook": EXPECTED_RUNBOOK,
        "upstream_evidence": {
            "M263-E001": e001_distilled,
            "M263-C003": c003_distilled,
            "M263-D003": d003_distilled,
        },
        "matrix_probe": matrix_probe,
        "failures": [finding.__dict__ for finding in failures],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[FAIL] {finding.artifact} :: {finding.check_id} :: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
