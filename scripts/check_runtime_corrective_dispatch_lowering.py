#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/dispatch_lowering_implementation_contract.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/dispatch-lowering-proof"
SUMMARY_PATH = OUT_DIR / "dispatch_lowering_implementation_summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_compile(compile_script: Path, fixture: Path, out_dir: Path, emit_prefix: str, backend: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [
            "pwsh",
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            emit_prefix,
            "--objc3-ir-object-backend",
            backend,
        ],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    compile_script = ROOT / contract["compile_script"]
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    case_summaries: list[dict[str, Any]] = []
    ok = True
    for fixture_path in contract["authoritative_fixture_paths"]:
        fixture = ROOT / fixture_path
        fixture_stem = fixture.stem
        case_out_dir = OUT_DIR / fixture_stem / "compile"
        case_out_dir.mkdir(parents=True, exist_ok=True)
        result = run_compile(
            compile_script,
            fixture,
            case_out_dir,
            contract["emit_prefix"],
            contract["object_backend"],
        )

        manifest_path = case_out_dir / f"{contract['emit_prefix']}.manifest.json"
        registration_manifest_path = case_out_dir / f"{contract['emit_prefix']}.runtime-registration-manifest.json"
        provenance_path = case_out_dir / f"{contract['emit_prefix']}.compile-provenance.json"
        ll_path = case_out_dir / f"{contract['emit_prefix']}.ll"

        case_ok = result.returncode == 0
        case_payload: dict[str, Any] = {
            "fixture": fixture_path,
            "compile_exit_code": result.returncode,
            "report_dir": str(case_out_dir.relative_to(ROOT)).replace("\\", "/"),
            "checks": {},
        }
        if case_ok:
            manifest = read_json(manifest_path)
            registration_manifest = read_json(registration_manifest_path)
            provenance = read_json(provenance_path)
            ll_text = ll_path.read_text(encoding="utf-8")
            lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
            registration_dispatch_surface = registration_manifest.get("dispatch_accessor_runtime_abi_surface", {})
            live_dispatch_sites = int(lowering_surface.get("live_runtime_dispatch_sites", 0))
            case_checks = {
                "manifest_exists": manifest_path.is_file(),
                "registration_manifest_exists": registration_manifest_path.is_file(),
                "compile_provenance_exists": provenance_path.is_file(),
                "ll_exists": ll_path.is_file(),
                "required_manifest_fields_present": all(field in manifest for field in contract["required_manifest_fields"]),
                "required_lowering_surface_fields_present": all(field in lowering_surface for field in contract["required_surface_fields"]),
                "runtime_dispatch_symbol_is_live_entrypoint": lowering_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
                "required_ll_substrings_present": all(token in ll_text for token in contract["required_ll_substrings"]),
                "live_dispatch_calls_present_when_required": (
                    ("call i32 @objc3_runtime_dispatch_i32(" in ll_text) if live_dispatch_sites > 0 else True
                ),
                "forbidden_ll_substrings_absent": all(token not in ll_text for token in contract["forbidden_ll_substrings"]),
                "compile_truthfulness_contract_bound": provenance.get("compile_output_truthfulness", {}).get("contract_id") == "objc3c.native.compile.output.truthfulness.v1",
                "registration_manifest_truthfulness_bound": registration_manifest.get("compile_output_truthfulness_contract_id") == "objc3c.native.compile.output.truthfulness.v1",
                "registration_manifest_provenance_bound": registration_manifest.get("compile_output_provenance_artifact") == "module.compile-provenance.json",
                "registration_manifest_dispatch_contract_bound": (
                    registration_dispatch_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1"
                    and registration_dispatch_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32"
                    and registration_manifest.get("bootstrap_runtime_api_dispatch_entrypoint_symbol") == "objc3_runtime_dispatch_i32"
                ),
            }
            case_ok = all(case_checks.values())
            case_payload["checks"] = case_checks
        else:
            case_payload["stdout"] = result.stdout
            case_payload["stderr"] = result.stderr

        case_payload["ok"] = case_ok
        case_summaries.append(case_payload)
        ok = ok and case_ok

    summary = {
        "issue": "runtime-corrective-dispatch-lowering-proof",
        "contract_id": contract["contract_id"],
        "runbook_mentions_contract": "dispatch_lowering_implementation_contract.json" in runbook_text,
        "case_count": len(case_summaries),
        "cases": case_summaries,
        "ok": ok and ("dispatch_lowering_implementation_contract.json" in runbook_text),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
