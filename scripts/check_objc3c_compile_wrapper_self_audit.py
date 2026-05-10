#!/usr/bin/env python3
from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any
from objc3c_tooling.json_io import require_json_object as load_json


ROOT = Path(__file__).resolve().parents[1]
WRAPPER = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "synthesized_accessor_property_lowering_positive.objc3"
)
REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-compile-wrapper-self-audit"
RUN_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-compile-wrapper-self-audit"
TRUTHFULNESS_CONTRACT_ID = "objc3c.native.compile.output.truthfulness.v1"
PROVENANCE_CONTRACT_ID = "objc3c.native.compile.output.provenance.v1"
SELF_AUDIT_CONTRACT_ID = "objc3c.native.compile.wrapper.self_audit.v1"
WRAPPER_TRUTH_OWNER = "objc3c-native-compile-wrapper-truth"
WRAPPER_RESULT_OWNER = "objc3c-native-compile-wrapper-result"
WRAPPER_ARTIFACT_OWNER = "objc3c-native-compile-wrapper-artifact"
WRAPPER_STATUS_OWNER = "objc3c-native-compile-wrapper-status"


def wrapper_truth_owner_contract() -> dict[str, Any]:
    return {
        "wrapper_truth_owner": WRAPPER_TRUTH_OWNER,
        "result_owner": WRAPPER_RESULT_OWNER,
        "artifact_owner": WRAPPER_ARTIFACT_OWNER,
        "status_owner": WRAPPER_STATUS_OWNER,
        "truthfulness_contract_id": TRUTHFULNESS_CONTRACT_ID,
        "provenance_contract_id": PROVENANCE_CONTRACT_ID,
        "no_fallback_or_evidence_log_claims": True,
    }


def repo_display_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def find_pwsh() -> str:
    for candidate in ("pwsh", "powershell"):
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    raise RuntimeError("PowerShell is required for objc3c compile-wrapper self-audit")


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def validate_compile_output(out_dir: Path) -> dict[str, Any]:
    required_artifacts = [
        "module.obj",
        "module.ll",
        "module.manifest.json",
        "module.runtime-registration-manifest.json",
        "module.runtime-registration-descriptor.json",
        "module.compile-provenance.json",
    ]
    missing = [name for name in required_artifacts if not (out_dir / name).is_file()]
    expect(not missing, "wrapper compile missed required artifacts: " + ", ".join(missing))

    provenance = load_json(out_dir / "module.compile-provenance.json")
    registration_manifest = load_json(out_dir / "module.runtime-registration-manifest.json")
    truthfulness = provenance.get("compile_output_truthfulness")
    expect(isinstance(truthfulness, dict), "provenance missing compile_output_truthfulness")
    expect(
        provenance.get("contract_id") == PROVENANCE_CONTRACT_ID,
        "compile provenance contract drifted",
    )
    expect(
        truthfulness.get("contract_id") == TRUTHFULNESS_CONTRACT_ID,
        "compile truthfulness contract drifted",
    )
    expect(truthfulness.get("truthful") is True, "compile truthfulness is not true")
    expect(
        truthfulness.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
        "runtime dispatch symbol drifted",
    )
    expect(
        truthfulness.get("property_descriptor_count_expected") == 6
        and truthfulness.get("property_descriptor_definition_count") == 6,
        "property descriptor truthfulness counts drifted",
    )
    expect(
        truthfulness.get("ivar_descriptor_count_expected") == 3
        and truthfulness.get("ivar_descriptor_definition_count") == 3,
        "ivar descriptor truthfulness counts drifted",
    )
    expect(
        truthfulness.get("property_descriptor_section_present") is True
        and truthfulness.get("ivar_descriptor_section_present") is True,
        "descriptor sections were not found in emitted IR",
    )
    expect(
        truthfulness.get("synthesized_property_surface_matches") is True,
        "synthesized property truthfulness surface drifted",
    )
    expect(truthfulness.get("failures") == [], "truthfulness failures are not empty")
    expect(
        registration_manifest.get("compile_output_provenance_contract_id")
        == PROVENANCE_CONTRACT_ID,
        "registration manifest provenance contract drifted",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_contract_id")
        == TRUTHFULNESS_CONTRACT_ID,
        "registration manifest truthfulness contract drifted",
    )
    expect(
        registration_manifest.get("compile_output_truthful") is True,
        "registration manifest did not certify truthful compile output",
    )
    expect(
        registration_manifest.get("compile_output_artifact_set_digest_sha256")
        == provenance.get("artifact_set_digest_sha256"),
        "registration manifest artifact digest drifted from provenance",
    )
    expect(
        provenance.get("artifact_count") == len(provenance.get("emitted_artifacts", [])),
        "provenance artifact count does not match emitted_artifacts",
    )
    return {
        "owner_contract": wrapper_truth_owner_contract(),
        "provenance_contract_id": provenance.get("contract_id"),
        "truthfulness_contract_id": truthfulness.get("contract_id"),
        "artifact_count": provenance.get("artifact_count"),
        "artifact_set_digest_sha256": provenance.get("artifact_set_digest_sha256"),
        "runtime_dispatch_symbol": truthfulness.get("runtime_dispatch_symbol"),
        "property_descriptor_count": truthfulness.get(
            "property_descriptor_definition_count"
        ),
        "ivar_descriptor_count": truthfulness.get("ivar_descriptor_definition_count"),
    }


def main() -> int:
    started_at = perf_counter()
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    out_dir = RUN_ROOT / run_id / "compile"
    report_path = REPORT_ROOT / "summary.json"
    out_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    status = "FAIL"
    error: str | None = None
    command: list[str] = []
    validation: dict[str, Any] = {}
    result: subprocess.CompletedProcess[str] | None = None
    try:
        shell = find_pwsh()
        command = [
            shell,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(WRAPPER),
            str(FIXTURE),
            "--out-dir",
            str(out_dir),
        ]
        print(
            "compile-wrapper-self-audit: START "
            f"fixture={repo_display_path(FIXTURE)} out_dir={repo_display_path(out_dir)}",
            flush=True,
        )
        result = subprocess.run(
            command,
            cwd=str(ROOT),
            capture_output=True,
            text=True,
            check=False,
        )
        expect(
            result.returncode == 0,
            "compile wrapper exited non-zero:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr,
        )
        validation = validate_compile_output(out_dir)
        status = "PASS"
    except Exception as exc:
        error = str(exc)

    elapsed = round(perf_counter() - started_at, 6)
    payload: dict[str, Any] = {
        "status": status,
        "contract_id": SELF_AUDIT_CONTRACT_ID,
        "owner_contract": wrapper_truth_owner_contract(),
        "run_id": run_id,
        "elapsed_seconds": elapsed,
        "wrapper": repo_display_path(WRAPPER),
        "fixture": repo_display_path(FIXTURE),
        "out_dir": repo_display_path(out_dir),
        "command": command,
        "exit_code": result.returncode if result is not None else None,
        "validation": validation,
        "audit_model": (
            "one wrapper compile validates invariant compile-output provenance, "
            "truthfulness, registration-manifest digest binding, and required "
            "artifact publication; downstream direct-native compiles can "
            "then avoid relaunching the PowerShell wrapper per fixture"
        ),
    }
    if error is not None:
        payload["error"] = error
    report_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_display_path(report_path)}")
    print(f"compile-wrapper-self-audit-report: {repo_display_path(report_path)}")
    if status == "PASS":
        print(f"compile-wrapper-self-audit: PASS elapsed={elapsed:.3f}s")
        return 0
    print(f"compile-wrapper-self-audit: FAIL elapsed={elapsed:.3f}s", file=sys.stderr)
    if error:
        print(error, file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
