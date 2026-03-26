from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
WRAPPER = ROOT / "scripts" / "objc3c_native_compile.ps1"
FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "m257_synthesized_accessor_property_lowering_positive.objc3"
)


def _find_pwsh() -> str | None:
    for candidate in ("pwsh", "powershell"):
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def test_compile_wrapper_emits_truthful_compile_output_envelope(tmp_path: Path) -> None:
    shell = _find_pwsh()
    if shell is None:
        pytest.skip("PowerShell is required for objc3c native compile wrapper integration")

    out_dir = tmp_path / "compile"
    command = [
        shell,
        "-NoProfile",
        "-File",
        str(WRAPPER),
        str(FIXTURE),
        "--out-dir",
        str(out_dir),
    ]
    result = subprocess.run(command, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        diagnostics = (result.stderr + "\n" + result.stdout).strip()
        pytest.fail(diagnostics)

    provenance_path = out_dir / "module.compile-provenance.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    assert provenance_path.is_file()
    assert registration_manifest_path.is_file()

    provenance = json.loads(provenance_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )

    truthfulness = provenance.get("compile_output_truthfulness")
    assert isinstance(truthfulness, dict)
    assert truthfulness["contract_id"] == "objc3c.native.compile.output.truthfulness.v1"
    assert truthfulness["truthful"] is True
    assert truthfulness["runtime_dispatch_symbol"] == "objc3_runtime_dispatch_i32"
    assert truthfulness["property_descriptor_count_expected"] == 6
    assert truthfulness["property_descriptor_definition_count"] == 6
    assert truthfulness["ivar_descriptor_count_expected"] == 3
    assert truthfulness["ivar_descriptor_definition_count"] == 3
    assert truthfulness["property_descriptor_section_present"] is True
    assert truthfulness["ivar_descriptor_section_present"] is True
    assert truthfulness["property_synthesis_sites_expected"] == 3
    assert truthfulness["synthesized_property_surface_matches"] is True
    assert truthfulness["current_property_helper_call_count"] >= 3
    assert truthfulness["failures"] == []

    assert (
        registration_manifest["compile_output_truthfulness_contract_id"]
        == "objc3c.native.compile.output.truthfulness.v1"
    )
    assert registration_manifest["compile_output_truthful"] is True
    assert (
        registration_manifest["compile_output_truthfulness_runtime_dispatch_symbol"]
        == "objc3_runtime_dispatch_i32"
    )
    assert registration_manifest["compile_output_truthfulness_property_descriptor_count"] == 6
    assert registration_manifest["compile_output_truthfulness_ivar_descriptor_count"] == 3
