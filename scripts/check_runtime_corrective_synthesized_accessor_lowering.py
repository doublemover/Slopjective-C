#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/synthesized_accessor_lowering_implementation_contract.json"
OUT_DIR = ROOT / "tmp/reports/m316/M316-C003"
SUMMARY_PATH = OUT_DIR / "synthesized_accessor_lowering_implementation_summary.json"
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

    cases: list[dict[str, Any]] = []
    ok = True
    for case_contract in contract["case_definitions"]:
        fixture = ROOT / case_contract["fixture_path"]
        case_out_dir = OUT_DIR / case_contract["case_id"] / "compile"
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
            "case_id": case_contract["case_id"],
            "fixture": case_contract["fixture_path"],
            "compile_exit_code": result.returncode,
            "report_dir": str(case_out_dir.relative_to(ROOT)).replace("\\", "/"),
            "checks": {},
        }

        if case_ok:
            manifest = read_json(manifest_path)
            registration_manifest = read_json(registration_manifest_path)
            provenance = read_json(provenance_path)
            ll_text = ll_path.read_text(encoding="utf-8")

            dispatch_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
            executable_surface = manifest.get("executable_synthesized_accessor_property_lowering_surface", {})

            case_checks: dict[str, bool] = {
                "manifest_exists": manifest_path.is_file(),
                "registration_manifest_exists": registration_manifest_path.is_file(),
                "compile_provenance_exists": provenance_path.is_file(),
                "ll_exists": ll_path.is_file(),
                "dispatch_surface_present": isinstance(dispatch_surface, dict),
                "executable_surface_present": isinstance(executable_surface, dict),
                "dispatch_surface_contract_bound": dispatch_surface.get("contract_id") == "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
                "executable_surface_contract_bound": executable_surface.get("contract_id") == "objc3c.executable.synthesized.accessor.property.lowering.v1",
                "dispatch_surface_symbol_bound": dispatch_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
                "dispatch_surface_symbol_matches_lowering": dispatch_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
                "executable_surface_dispatch_contract_bound": executable_surface.get("dispatch_and_synthesized_accessor_lowering_surface_contract_id") == "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
                "compile_truthfulness_contract_bound": provenance.get("compile_output_truthfulness", {}).get("contract_id") == "objc3c.native.compile.output.truthfulness.v1",
                "registration_manifest_truthfulness_bound": registration_manifest.get("compile_output_truthfulness_contract_id") == "objc3c.native.compile.output.truthfulness.v1",
                "registration_manifest_provenance_bound": registration_manifest.get("compile_output_provenance_artifact") == "module.compile-provenance.json",
                "required_ll_snippets_present": all(snippet in ll_text for snippet in case_contract["required_ll_snippets"]),
            }

            for field, expected_value in case_contract["expected_dispatch_lowering_counts"].items():
                case_checks[f"dispatch_surface_{field}_matches"] = dispatch_surface.get(field) == expected_value

            for field, expected_value in case_contract["expected_executable_surface_counts"].items():
                case_checks[f"executable_surface_{field}_matches"] = executable_surface.get(field) == expected_value

            for field, expected_value in case_contract["expected_registration_manifest_counts"].items():
                case_checks[f"registration_manifest_{field}_matches"] = registration_manifest.get(field) == expected_value

            case_ok = all(case_checks.values())
            case_payload["checks"] = case_checks
        else:
            case_payload["stdout"] = result.stdout
            case_payload["stderr"] = result.stderr

        case_payload["ok"] = case_ok
        cases.append(case_payload)
        ok = ok and case_ok

    summary = {
        "issue": "M316-C003",
        "contract_id": contract["contract_id"],
        "runbook_mentions_contract": "synthesized_accessor_lowering_implementation_contract.json" in runbook_text,
        "case_count": len(cases),
        "cases": cases,
        "ok": ok and ("synthesized_accessor_lowering_implementation_contract.json" in runbook_text),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
