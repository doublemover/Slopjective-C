#!/usr/bin/env python3
"""Publish the M264 release/runtime claim matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
JSON_OUT = ROOT / "tmp" / "reports" / "release_claims" / "M264-E002" / "release_runtime_claim_matrix.json"
MD_OUT = ROOT / "tmp" / "reports" / "release_claims" / "M264-E002" / "release_runtime_claim_matrix.md"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
METADATA_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "runtime_metadata_source_records_class_protocol_property_ivar.objc3"
A002_SUMMARY = ROOT / "tmp" / "reports" / "release_claims" / "M264-A002" / "frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json"
B003_SUMMARY = ROOT / "tmp" / "reports" / "release_claims" / "M264-B003" / "canonical_interface_and_feature_macro_truthfulness_summary.json"
C002_SUMMARY = ROOT / "tmp" / "reports" / "release_claims" / "M264-C002" / "machine_readable_runtime_capability_reporting_summary.json"
D002_SUMMARY = ROOT / "tmp" / "reports" / "release_claims" / "M264-D002" / "cli_and_toolchain_conformance_claim_operations_summary.json"
E001_SUMMARY = ROOT / "tmp" / "reports" / "release_claims" / "M264-E001" / "versioning_and_conformance_truth_gate_summary.json"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def run(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
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


def summary_status(payload: dict[str, Any]) -> bool:
    if payload.get("ok") is True:
        return True
    return (
        isinstance(payload.get("checks_total"), int)
        and isinstance(payload.get("checks_passed"), int)
        and payload.get("checks_total") == payload.get("checks_passed")
        and not payload.get("failures")
    )


def ensure_success(result: subprocess.CompletedProcess[str], context: str) -> None:
    if result.returncode == 0:
        return
    detail = (result.stderr or result.stdout).strip()
    raise SystemExit(f"{context} failed ({result.returncode}): {detail}")


def first_line(text: str) -> str:
    stripped = text.strip()
    if not stripped:
        return ""
    return stripped.splitlines()[0]


def publish_matrix(json_out: Path, md_out: Path) -> None:
    json_out.parent.mkdir(parents=True, exist_ok=True)
    md_out.parent.mkdir(parents=True, exist_ok=True)

    ensure_summary = ROOT / "tmp" / "reports" / "release_claims" / "M264-E002" / "ensure_build_summary.json"
    build = run([
        "python",
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "release_claims-e002",
        "--summary-out",
        str(ensure_summary),
    ])
    ensure_success(build, "ensure_objc3c_native_build")

    dependency_cases = {
        "M264-A002": {"summary_path": A002_SUMMARY, "payload": load_json(A002_SUMMARY)},
        "M264-B003": {"summary_path": B003_SUMMARY, "payload": load_json(B003_SUMMARY)},
        "M264-C002": {"summary_path": C002_SUMMARY, "payload": load_json(C002_SUMMARY)},
        "M264-D002": {"summary_path": D002_SUMMARY, "payload": load_json(D002_SUMMARY)},
        "M264-E001": {"summary_path": E001_SUMMARY, "payload": load_json(E001_SUMMARY)},
    }
    for name, case in dependency_cases.items():
        if not summary_status(case["payload"]):
            raise SystemExit(f"{name} summary is not green: {display_path(case['summary_path'])}")

    native_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "release_claims" / "e002" / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    native = run([
        str(NATIVE_EXE),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(native_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "json",
    ])
    ensure_success(native, "native CLI conformance emit")

    native_report_path = native_dir / "module.objc3-conformance-report.json"
    native_publication_path = native_dir / "module.objc3-conformance-publication.json"
    if not native_report_path.exists() or not native_publication_path.exists():
        raise SystemExit("native CLI did not publish the expected conformance report/publication sidecars")

    validate_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "release_claims" / "e002" / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate = run([
        str(NATIVE_EXE),
        "--validate-objc3-conformance",
        str(native_report_path),
        "--out-dir",
        str(validate_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance-format",
        "json",
    ])
    ensure_success(validate, "native CLI conformance validate")
    validation_path = validate_dir / "module.objc3-conformance-validation.json"
    if not validation_path.exists():
        raise SystemExit("native CLI validation artifact missing")

    runner_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "release_claims" / "e002" / "frontend-runner"
    runner_dir.mkdir(parents=True, exist_ok=True)
    runner = run([
        str(RUNNER_EXE),
        str(METADATA_FIXTURE),
        "--out-dir",
        str(runner_dir),
        "--emit-prefix",
        "module",
        "--no-emit-ir",
        "--no-emit-object",
    ])
    ensure_success(runner, "frontend C API publication")
    runner_report_path = runner_dir / "module.objc3-conformance-report.json"
    runner_publication_path = runner_dir / "module.objc3-conformance-publication.json"
    if not runner_report_path.exists() or not runner_publication_path.exists():
        raise SystemExit("frontend runner did not publish the expected conformance sidecars")

    strict_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "release_claims" / "e002" / "strict-reject"
    strict_dir.mkdir(parents=True, exist_ok=True)
    strict_reject = run([
        str(NATIVE_EXE),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(strict_dir),
        "--emit-prefix",
        "module",
        "--objc3-conformance-profile",
        "strict",
    ])

    yaml_dir = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "release_claims" / "e002" / "yaml-reject"
    yaml_dir.mkdir(parents=True, exist_ok=True)
    yaml_reject = run([
        str(NATIVE_EXE),
        str(HELLO_FIXTURE),
        "--out-dir",
        str(yaml_dir),
        "--emit-prefix",
        "module",
        "--emit-objc3-conformance",
        "--emit-objc3-conformance-format",
        "yaml",
    ])

    native_report = load_json(native_report_path)
    native_publication = load_json(native_publication_path)
    validation_payload = load_json(validation_path)
    runner_report = load_json(runner_report_path)
    runner_publication = load_json(runner_publication_path)

    matrix = {
        "contract_id": "objc3c-release-runtime-claim-matrix/release_claims-e002-v1",
        "schema_id": "objc3c-release-runtime-claim-matrix-v1",
        "publication_model": "derived-from-a002-b003-c002-d002-e001-and-live-native-frontend-probes",
        "profiles": [
            {"id": "core", "claim_status": "claimed", "selection_status": "supported", "runtime_status": "runnable"},
            {"id": "strict", "claim_status": "not-claimed", "selection_status": "fail-closed", "runtime_status": "unsupported"},
            {"id": "strict-concurrency", "claim_status": "not-claimed", "selection_status": "fail-closed", "runtime_status": "unsupported"},
            {"id": "strict-system", "claim_status": "not-claimed", "selection_status": "fail-closed", "runtime_status": "unsupported"},
        ],
        "compatibility_modes": [
            {"id": "canonical", "status": "supported"},
            {"id": "legacy", "status": "supported"},
        ],
        "migration_assist": {"status": "supported"},
        "macro_claim_surface": {"status": "suppressed"},
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
                "report_path": display_path(native_report_path),
                "publication_path": display_path(native_publication_path),
                "selected_profile": native_report.get("selected_profile"),
                "runtime_capability_profile": native_report.get("runtime_capability_report", {}).get("selected_profile"),
                "publication_surface_kind": native_publication.get("publication_surface_kind"),
            },
            "native_validation": {
                "validation_path": display_path(validation_path),
                "selected_profile": validation_payload.get("selected_profile"),
                "format": validation_payload.get("format"),
                "publication_surface_kind": validation_payload.get("publication_surface_kind"),
            },
            "frontend_c_api": {
                "report_path": display_path(runner_report_path),
                "publication_path": display_path(runner_publication_path),
                "selected_profile": runner_report.get("selected_profile"),
                "publication_surface_kind": runner_publication.get("publication_surface_kind"),
            },
            "strict_profile_reject": {
                "returncode": strict_reject.returncode,
                "diagnostic": first_line(strict_reject.stderr or strict_reject.stdout),
            },
            "yaml_emit_reject": {
                "returncode": yaml_reject.returncode,
                "diagnostic": first_line(yaml_reject.stderr or yaml_reject.stdout),
            },
        },
        "next_issue": "M265-A001",
        "ready": True,
    }
    json_out.write_text(json.dumps(matrix, indent=2) + "\n", encoding="utf-8")

    md = f"""# M264 Release/Runtime Claim Matrix

| Surface | Current state |
| --- | --- |
| Claimed profile | `core` |
| Compatibility modes | `canonical`, `legacy` |
| Migration assist | supported |
| Strict / strict-concurrency / strict-system | fail-closed and not claimed |
| Feature-macro publication | suppressed |
| Emit/validate format | `json` only |
| Native CLI sidecars | report + publication + validation |
| Frontend C API sidecars | report + publication |
| Optional features | `throws`, `async-await`, `actors`, `blocks`, and `arc` remain not claimed |
| Next issue | `M265-A001` |

## Live proofs

- Native CLI report: `{display_path(native_report_path)}`
- Native CLI publication: `{display_path(native_publication_path)}`
- Native CLI validation: `{display_path(validation_path)}`
- Frontend C API report: `{display_path(runner_report_path)}`
- Frontend C API publication: `{display_path(runner_publication_path)}`
- Strict profile reject rc: `{strict_reject.returncode}`
- YAML emit reject rc: `{yaml_reject.returncode}`
"""
    md_out.write_text(md, encoding="utf-8")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json-out", type=Path, default=JSON_OUT)
    parser.add_argument("--md-out", type=Path, default=MD_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    publish_matrix(args.json_out, args.md_out)
    print(f"[ok] wrote {display_path(args.json_out)}")
    print(f"[ok] wrote {display_path(args.md_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(__import__("sys").argv[1:]))
