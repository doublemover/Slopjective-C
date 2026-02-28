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
    assert payload["checks_total"] >= 56
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


def test_contract_fails_when_frontend_runtime_backend_dispatch_has_hidden_fallback(
    tmp_path: Path,
) -> None:
    frontend_anchor_copy = tmp_path / "frontend_anchor.cpp"
    frontend_anchor_text = contract.DEFAULT_FRONTEND_ANCHOR_CPP.read_text(encoding="utf-8")
    frontend_anchor_copy.write_text(
        frontend_anchor_text.replace(
            "compile_status = RunIRCompileLLVMDirect(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error);",
            "const int hidden_fallback = RunIRCompile(std::filesystem::path(options->clang_path), ir_out, object_out);\n"
            "        (void)hidden_fallback;\n"
            "        compile_status = RunIRCompileLLVMDirect(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error);",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-anchor-cpp",
            str(frontend_anchor_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["id"] == "frontend-m145-07" for failure in payload["failures"]
    )


def test_contract_fails_when_c_api_runner_drops_llvm_direct_backend_parser(
    tmp_path: Path,
) -> None:
    c_api_runner_copy = tmp_path / "objc3c_frontend_c_api_runner.cpp"
    c_api_runner_text = contract.DEFAULT_C_API_RUNNER_CPP.read_text(encoding="utf-8")
    c_api_runner_copy.write_text(
        c_api_runner_text.replace(
            'if (value == "llvm-direct") {',
            'if (value == "llvm-direct-disabled") {',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c-api-runner-cpp",
            str(c_api_runner_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["id"] == "runner-m145-02" for failure in payload["failures"]
    )


def test_contract_fails_when_lane_b_matrix_drops_forced_missing_llc_case(
    tmp_path: Path,
) -> None:
    sema_contract_copy = tmp_path / "check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1"
    sema_contract_text = contract.DEFAULT_SEMA_CONTRACT_PS1.read_text(encoding="utf-8")
    sema_contract_copy.write_text(
        sema_contract_text.replace(
            "runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes",
            "runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes_drifted",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--sema-contract-ps1",
            str(sema_contract_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["id"] == "laneb-m145-02" for failure in payload["failures"]
    )


def test_contract_fails_when_lane_d_fixture_drops_llvm_direct_marker(
    tmp_path: Path,
) -> None:
    fixture_copy = tmp_path / "M145-D001.json"
    fixture_text = contract.DEFAULT_M145_D001_FIXTURE.read_text(encoding="utf-8")
    fixture_copy.write_text(
        fixture_text.replace(
            "--objc3-ir-object-backend llvm-direct",
            "--objc3-ir-object-backend llvm-direct-disabled",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--m145-d001-fixture",
            str(fixture_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["id"] == "laned-m145-06" for failure in payload["failures"]
    )


def test_package_wires_m145_closeout_contract() -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]

    assert "test:objc3c:m145-direct-llvm-matrix" in scripts
    assert "python -m pytest" in scripts["test:objc3c:m145-direct-llvm-matrix"]
    assert "tests/tooling/test_check_m145_direct_llvm_matrix_contract.py" in scripts[
        "test:objc3c:m145-direct-llvm-matrix"
    ]
    assert "test:objc3c:m145-direct-llvm-matrix:lane-d" in scripts
    assert "scripts/check_conformance_suite.ps1" in scripts[
        "test:objc3c:m145-direct-llvm-matrix:lane-d"
    ]
    assert "npm run test:objc3c:perf-budget" in scripts[
        "test:objc3c:m145-direct-llvm-matrix:lane-d"
    ]
    assert "check:compiler-closeout:m145" in scripts
    assert "python scripts/check_m145_direct_llvm_matrix_contract.py" in scripts[
        "check:compiler-closeout:m145"
    ]
    assert "npm run test:objc3c:m145-direct-llvm-matrix" in scripts[
        "check:compiler-closeout:m145"
    ]
    assert "npm run test:objc3c:m145-direct-llvm-matrix:lane-d" in scripts[
        "check:compiler-closeout:m145"
    ]
    assert '--glob "docs/contracts/direct_llvm_emission_expectations.md"' in scripts[
        "check:compiler-closeout:m145"
    ]
