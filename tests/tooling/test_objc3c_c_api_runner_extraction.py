from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RUNNER_CPP = ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_c_api_runner_uses_c_shim_compile_path() -> None:
    source = _read(RUNNER_CPP)

    assert '#include "libobjc3c_frontend/c_api.h"' in source
    assert "objc3c_frontend_c_context_create()" in source
    assert "objc3c_frontend_c_compile_file(context, &compile_options, &result)" in source
    assert "objc3c_frontend_c_copy_last_error(context, nullptr, 0)" in source
    assert "objc3c_frontend_c_context_destroy(context);" in source


def test_c_api_runner_reports_summary_and_cli_contract() -> None:
    source = _read(RUNNER_CPP)

    assert '\\"mode\\": \\"objc3c-frontend-c-api-runner-v1\\"' in source
    assert 'fs::path("tmp") / "artifacts" / "compilation" / "objc3c-native"' in source
    assert "wrote summary: " in source
    assert "--llc <path>" in source
    assert "--objc3-ir-object-backend <clang|llvm-direct>" in source
    assert "--objc3-compat-mode <canonical|legacy>" in source
    assert "--objc3-migration-assist" in source
    assert "--objc3-max-message-args" in source
    assert "--objc3-runtime-dispatch-symbol" in source
    assert "invalid --objc3-compat-mode (expected canonical|legacy): " in source
    assert '\\"compatibility_mode\\": \\"' in source
    assert '\\"migration_assist\\": ' in source
    assert "compile_options.llc_path =" in source
    assert "compile_options.compatibility_mode = options.compatibility_mode;" in source
    assert "compile_options.migration_assist = options.migration_assist ? 1u : 0u;" in source
    assert "compile_options.ir_object_backend = options.ir_object_backend;" in source
    assert "ExitCodeFromStatus" in source
