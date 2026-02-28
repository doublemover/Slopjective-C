from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m145_direct_llvm_matrix_contract.py"
PACKAGE_JSON = ROOT / "package.json"
SPEC = importlib.util.spec_from_file_location("check_m145_direct_llvm_matrix_contract", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m145-direct-llvm-matrix-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 14
    assert payload["checks_passed"] == payload["checks_total"]


def test_contract_fails_when_llvm_direct_body_contains_clang_fallback(tmp_path: Path) -> None:
    process_copy = tmp_path / "objc3_process.cpp"
    process_text = contract.DEFAULT_PROCESS_CPP.read_text(encoding="utf-8")
    process_copy.write_text(
        process_text.replace(
            "const int llc_status =",
            "const int fallback_status = RunIRCompile(clang_path, ir_path, object_out);\n"
            "  (void)fallback_status;\n"
            "  const int llc_status =",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--process-cpp",
            str(process_copy),
            "--driver-cpp",
            str(contract.DEFAULT_DRIVER_CPP),
            "--cli-options-cpp",
            str(contract.DEFAULT_CLI_OPTIONS_CPP),
            "--routing-cpp",
            str(contract.DEFAULT_ROUTING_CPP),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["id"] == "process-m145-04" for failure in payload["failures"]
    )


def test_contract_fails_when_capability_routing_invokes_hidden_compile_fallback(
    tmp_path: Path,
) -> None:
    routing_copy = tmp_path / "objc3_llvm_capability_routing.cpp"
    routing_text = contract.DEFAULT_ROUTING_CPP.read_text(encoding="utf-8")
    marker = "  return true;"
    marker_index = routing_text.rfind(marker)
    assert marker_index != -1
    injection = (
        "  const int hidden_fallback = RunIRCompile(std::filesystem::path(\"clang\"), "
        "std::filesystem::path(\"in.ll\"), std::filesystem::path(\"out.obj\"));\n"
        "  (void)hidden_fallback;\n"
        "  return true;"
    )
    mutated = routing_text[:marker_index] + injection + routing_text[marker_index + len(marker) :]
    routing_copy.write_text(
        mutated,
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--process-cpp",
            str(contract.DEFAULT_PROCESS_CPP),
            "--driver-cpp",
            str(contract.DEFAULT_DRIVER_CPP),
            "--cli-options-cpp",
            str(contract.DEFAULT_CLI_OPTIONS_CPP),
            "--routing-cpp",
            str(routing_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["id"] == "routing-m145-04" for failure in payload["failures"]
    )


def test_package_wires_m145_closeout_contract() -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]

    assert "check:compiler-closeout:m145" in scripts
    assert scripts["check:compiler-closeout:m145"] == (
        "python scripts/check_m145_direct_llvm_matrix_contract.py"
    )
