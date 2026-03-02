from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_d001_frontend_build_invocation_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_d001_frontend_build_invocation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m226_d001_frontend_build_invocation_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-d001-frontend-build-invocation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_when_build_script_drops_frontend_c_api_runner_source(tmp_path: Path) -> None:
    build_script_copy = tmp_path / "build_objc3c_native.ps1"
    build_script_copy.write_text(
        contract.DEFAULT_BUILD_SCRIPT.read_text(encoding="utf-8").replace(
            '"native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"',
            '"native/objc3c/src/tools/objc3c_frontend_c_api_runner_drift.cpp"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--build-script",
            str(build_script_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-BLD-03" for failure in payload["failures"])


def test_contract_fails_when_driver_main_drops_capability_routing_call(tmp_path: Path) -> None:
    driver_main_copy = tmp_path / "objc3_driver_main.cpp"
    driver_main_copy.write_text(
        contract.DEFAULT_DRIVER_MAIN_CPP.read_text(encoding="utf-8").replace(
            "ApplyObjc3LLVMCabilityRouting(cli_options, cli_error)",
            "ApplyObjc3LLVMCabilityRoutingDisabled(cli_options, cli_error)",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--driver-main-cpp",
            str(driver_main_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-INV-02" for failure in payload["failures"])


def test_contract_fails_when_capability_routing_invokes_hidden_compile_fallback(
    tmp_path: Path,
) -> None:
    routing_copy = tmp_path / "objc3_llvm_capability_routing.cpp"
    routing_text = contract.DEFAULT_CAPABILITY_ROUTING_CPP.read_text(encoding="utf-8")
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
    routing_copy.write_text(mutated, encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--capability-routing-cpp",
            str(routing_copy),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-INV-16" for failure in payload["failures"])
