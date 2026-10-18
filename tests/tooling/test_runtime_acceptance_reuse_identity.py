from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any
from types import SimpleNamespace

from scripts.objc3c_runtime_acceptance.reuse_identity import (
    build_runtime_acceptance_reuse_identity,
    runtime_acceptance_options,
    validate_runtime_acceptance_reuse_identity,
)
from scripts.objc3c_workflow import composite_step_runtime_reuse as reuse


def _write(path: Path, text: str = "identity input\n") -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")
    return path


def _fixture_root(tmp_path: Path) -> tuple[Path, Path, Path, Path, Path, Path, Path, Path]:
    root = tmp_path / "repo"
    consumer = _write(root / "tests/tooling/fixtures/native/consumer.objc3", "module Consumer;\n@import Provider;\n")
    provider = _write(root / "tests/tooling/fixtures/native/provider.objc3", "module Provider;\n@import Transitive;\n")
    transitive = _write(root / "tests/native/runtime/transitive.objc3", "module Transitive;\n")
    exe = _write(root / "artifacts/bin/objc3c-native.exe", "compiler\n")
    runtime = _write(root / "artifacts/lib/objc3_runtime.lib", "runtime\n")
    clang = _write(root / "tools/clang++.exe", "linker\n")
    powershell = _write(root / "tools/pwsh.exe", "shell\n")
    for relative in (
        "scripts/check_objc3c_runtime_acceptance.py",
        "scripts/build_objc3c_native.ps1",
        "scripts/objc3c_native_compile.ps1",
        "scripts/objc3c_native_compile_command.psm1",
        "scripts/objc3c_native_compile_io.psm1",
        "scripts/objc3c_native_compile_wrapper.psm1",
        "scripts/objc3c_native_compile_toolchain.psm1",
        "scripts/objc3c_native_compile_frontend_guards.psm1",
        "scripts/objc3c_runtime_acceptance/stub.py",
        "scripts/objc3c_tooling/stub.py",
        "scripts/objc3c_workflow/command_powershell_policy.py",
        "scripts/objc3c_native_compile_io/cache_io.psm1",
        "scripts/objc3c_native_compile_wrapper/orchestration.psm1",
        "scripts/objc3c_native_compile_toolchain/availability.psm1",
        "scripts/objc3c_native_compile_frontend_guards/guards.psm1",
        "scripts/objc3c_native_compile_provenance/provenance.psm1",
        "native/objc3c/src/runtime/public/runtime.h",
        "tests/tooling/runtime/runtime_probe.cpp",
    ):
        _write(root / relative)
    return root, consumer, provider, transitive, exe, runtime, clang, powershell


def _identity(paths: tuple[Path, ...], *, fixture_text: str | None = None) -> dict[str, Any]:
    root, consumer, provider, transitive, exe, runtime, clang, powershell = paths
    if fixture_text is not None:
        consumer.write_text(fixture_text, encoding="utf-8")
    results = [
        SimpleNamespace(
            case_id="fixture-a",
            fixture=consumer.relative_to(root).as_posix(),
            summary={"provider_fixture": provider.relative_to(root).as_posix()},
        )
    ]
    return build_runtime_acceptance_reuse_identity(
        args=SimpleNamespace(suite="fast", cases=[]),
        run_dir=root / "tmp/artifacts/run-one",
        clangxx=str(clang),
        results=results,
        case_timings=[{"label": "label-a", "case_id": "fixture-a"}],
        root=root,
        native_executable=exe,
        runtime_library=runtime,
        powershell_executable=str(powershell),
    )


def test_runtime_reuse_identity_hashes_transitive_imports_and_rejects_changes(tmp_path: Path) -> None:
    paths = _fixture_root(tmp_path)
    root, consumer, provider, transitive, *_ = paths
    identity = _identity(paths)
    assert identity["complete"] is True, identity["failures"]
    assert {row["path"] for row in identity["source_closure"]} == {
        consumer.relative_to(root).as_posix(),
        provider.relative_to(root).as_posix(),
        transitive.relative_to(root).as_posix(),
    }
    assert validate_runtime_acceptance_reuse_identity(identity, root=root) == (True, "verified")

    provider.write_text("module Provider;\n// changed semantic provider\n@import Transitive;\n", encoding="utf-8")
    valid, reason = validate_runtime_acceptance_reuse_identity(identity, root=root)
    assert valid is False
    assert "source_closure" in reason


def test_runtime_reuse_identity_binds_emitted_import_interface_artifacts(tmp_path: Path) -> None:
    paths = _fixture_root(tmp_path)
    root = paths[0]
    interface = _write(
        root / "tmp/artifacts/run-one/consumer/module.runtime-import-surface.json",
        "{\"module_name\": \"Consumer\"}\n",
    )
    identity = _identity(paths)
    assert identity["complete"] is True, identity["failures"]
    assert identity["interface_artifacts"] == [{
        "path": interface.relative_to(root).as_posix(),
        "sha256": hashlib.sha256(interface.read_bytes()).hexdigest(),
    }]
    interface.write_text("{\"module_name\": \"Changed\"}\n", encoding="utf-8")
    valid, reason = validate_runtime_acceptance_reuse_identity(identity, root=root)
    assert valid is False
    assert reason == "interface_artifacts identity changed"


def test_runtime_reuse_identity_fails_closed_for_unresolved_import(tmp_path: Path) -> None:
    paths = _fixture_root(tmp_path)
    identity = _identity(paths, fixture_text="module Consumer;\n@import UnknownProvider;\n")
    assert identity["complete"] is False
    assert any("UnknownProvider" in failure for failure in identity["failures"])
    assert validate_runtime_acceptance_reuse_identity(identity, root=paths[0]) == (False, "reuse identity incomplete")


def test_produced_identity_is_accepted_only_for_matching_report_options(tmp_path: Path, monkeypatch) -> None:
    paths = _fixture_root(tmp_path)
    root = paths[0]
    identity = _identity(paths)
    assert identity["complete"] is True, identity["failures"]
    monkeypatch.setattr(reuse, "ROOT", root)
    options = runtime_acceptance_options(SimpleNamespace(suite="fast", cases=[]))
    options_digest = hashlib.sha256(
        json.dumps(options, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode("utf-8")
    ).hexdigest()
    report_path = root / "tmp/reports/runtime/acceptance/summary.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(
        json.dumps({
            "status": "PASS",
            "selected_suite": "fast",
            "selected_cases": [],
            "fixture_ids": ["fixture-a"],
            "case_labels": ["label-a"],
            "options_digest": options_digest,
            "runtime_reuse_identity": identity,
        }),
        encoding="utf-8",
    )
    assert reuse._valid_runtime_report(report_path, {"label-a"}, options_digest) == (True, "verified")
    assert reuse._valid_runtime_report(report_path, {"label-a"}, "wrong-options")[0] is False
    assert reuse._valid_runtime_report(report_path, {"fixture-a"}, options_digest)[0] is False


def test_reuse_identity_rejects_compiler_runtime_source_and_tool_mutations(tmp_path: Path) -> None:
    paths = _fixture_root(tmp_path)
    root, consumer, provider, _, exe, runtime, clang, powershell = paths
    identity = _identity(paths)
    assert identity["complete"] is True, identity["failures"]

    for path, changed in (
        (exe, "compiler changed\n"),
        (runtime, "runtime changed\n"),
        (provider, "module Provider;\n// imported source changed\n@import Transitive;\n"),
        (clang, "linker changed\n"),
        (powershell, "shell changed\n"),
        (consumer, "module Consumer;\n// fixture changed\n@import Provider;\n"),
        (root / "scripts/objc3c_native_compile_io/cache_io.psm1", "cache wrapper changed\n"),
        (root / "scripts/objc3c_tooling/stub.py", "toolchain identity changed\n"),
    ):
        original = path.read_text(encoding="utf-8")
        path.write_text(changed, encoding="utf-8")
        valid, _ = validate_runtime_acceptance_reuse_identity(identity, root=root)
        assert valid is False, path
        path.write_text(original, encoding="utf-8")


def test_current_fresh_runtime_acceptance_report_is_reusable_without_running_cases(monkeypatch) -> None:
    monkeypatch.setenv("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN", "1")
    report_path = reuse.ROOT / "tmp/reports/runtime/acceptance/summary.json"
    if not report_path.is_file():
        return
    report = json.loads(report_path.read_text(encoding="utf-8"))
    identity = report.get("runtime_reuse_identity", {})
    if identity.get("options", {}).get("python_version") != list(__import__("sys").version_info[:3]):
        return
    expected = set(map(str, report.get("case_labels", [])))
    options_digest = str(report.get("options_digest", ""))
    assert expected
    assert reuse._valid_runtime_report(report_path, expected, options_digest) == (True, "verified")
    step = reuse.runtime_acceptance_reuse_step(
        "test-runtime-acceptance-fast",
        ["python", "runner.py", "--suite", str(identity.get("options", {}).get("suite", "fast"))],
        0.0,
    )
    assert step is not None
    assert step["report_reused"] is True


def test_old_report_without_build_identity_is_rejected(tmp_path: Path, monkeypatch) -> None:
    monkeypatch.setattr(reuse, "ROOT", tmp_path)
    report_path = tmp_path / "tmp/reports/runtime/acceptance/summary.json"
    report_path.parent.mkdir(parents=True)
    report_path.write_text(
        json.dumps({"status": "PASS", "case_labels": ["label-a"], "fixture_ids": ["fixture-a"], "options_digest": "old"}),
        encoding="utf-8",
    )
    assert reuse._valid_runtime_report(report_path, {"label-a"}, "old")[0] is False
