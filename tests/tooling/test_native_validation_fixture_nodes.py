from __future__ import annotations

from datetime import datetime, timedelta, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_native_validation import fixture_nodes as fixtures


def _hash(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


class FixtureRun:
    """Write real fixture evidence without loading VS or compiling native code."""

    def __init__(self, tmp_path: Path, suite: str = "expression", driver: str = "clang"):
        self.repo = tmp_path / "repo"
        self.repo.mkdir()
        self.suite, self.driver = suite, driver
        self.node = {"operation": "fixture", "suite": suite, "driver": driver}
        self.output = tmp_path / "node-output"
        sdk = self.repo / "out/toolchains/objc3-development/build/bin"
        self.toolchain = {"sdk_build_id": "sdk-test", "tools": {
            "clang": str(sdk / "clang.exe"), "clangxx": str(sdk / "clang++.exe"),
            "clang_cl": str(sdk / "clang-cl.exe"), "facade": str(self.repo / "facade.exe"),
            "linker": str(sdk / "lld-link.exe")}, "resource_dir": str(sdk.parent / "lib/clang/22"),
            "environment": {"PATH": str(sdk), "INCLUDE": str(self.repo / "admitted-include"),
                            "LIB": str(self.repo / "admitted-lib"),
                            "VSINSTALLDIR": str(self.repo / "discovered-vs"),
                            "VCToolsInstallDir": str(self.repo / "discovered-vs/VC/Tools"),
                            "VCToolsVersion": "14.44.35207", "WindowsSDKVersion": "10.0.26100.0",
                            "WindowsSdkDir": str(self.repo / "admitted-sdk")}}
        runner_name, folder = fixtures._RUNNERS[suite]
        self.runner = self.repo / "scripts/objc3c_toolchain_runtime" / runner_name
        self.runner.parent.mkdir(parents=True)
        self.runner.write_text("# fake tracked runner\n", encoding="utf-8")
        self.base = self.repo / "out/toolchains/objc3-development" / folder / "sdk-test" / driver
        self.round = 0
        self.mutate_report = lambda report: None
        self.mutate_files = lambda report: None
        self.exit_code = 0
        self.calls: list[tuple] = []
        self.environments: list[dict] = []
        self.report_path: Path | None = None

    def _file(self, path: Path, text: str) -> Path:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text + f"\nround={self.round}\n", encoding="utf-8")
        return path

    def write_report(self) -> dict:
        modern = self.suite in {"match", "transfer"}
        run_id = f"{self.round:032x}"
        self.report_path = self.base / run_id / "results.json" if modern else self.base / "results.json"
        inputs = self.repo / "generated-inputs"
        rows = []
        for case in fixtures._CASES[self.suite]:
            for level in (0, 2):
                name = f"{case}-O{level}"
                source = self._file(inputs / (case + ".mm"), "source " + case)
                exe = self._file(self.report_path.parent / (name + ".exe"), "binary " + name)
                obj = self._file(self.report_path.parent / (name + ".obj"), "object " + name)
                row = {"name": name, "source": str(source), "source_sha256": _hash(source),
                       "exit_code": 0, "executable_sha256": _hash(exe)}
                if self.suite == "abi":
                    companion = self._file(inputs / (case + "-client.c"), "companion " + case)
                    header = self._file(inputs / ("carrier-native-abi.h" if case == "extension-provider-consumer"
                                                else "carrier-native-abi-c-mirror.h"), "header " + case)
                    row.update(sources=[{"file": str(source), "sha256": _hash(source)},
                                        {"file": str(companion), "sha256": _hash(companion)}],
                               header_sha256=_hash(header), executable=str(exe),
                               objects=[{"file": str(obj), "sha256": _hash(obj)}],
                               commands=[{"argv": ["clang", "-c", str(source), "-o", str(obj)], "exit_code": 0}])
                    row.pop("source")
                    row.pop("source_sha256")
                elif modern:
                    row.update(status="passed", compile_exit_code=0, link_exit_code=0,
                               compile_command=["clang", "-c", str(source), "-o", str(obj)],
                               runtime_command=[str(exe)], object_sha256=_hash(obj))
                    if case != "carrier-transfer-local-runtime":
                        generated = self._file(self.repo / "sealed-inputs" / source.name, "source " + case)
                        row.update(generated_source_required=True, generated_source=str(generated),
                                   generated_source_sha256=_hash(generated))
                elif self.suite == "arc-lifetime":
                    support = self._file(inputs / "carrier-arc-local-runtime-support.m", "support")
                    support_obj = self._file(self.report_path.parent / (name + "-support.obj"), "support object " + name)
                    row.update(status="passed", phase="execute", object_sha256=_hash(obj),
                               command=["clang", "-c", str(source), "-o", str(obj)],
                               support_source_sha256=_hash(support), support_object_sha256=_hash(support_obj),
                               support_command=["clang", "-c", str(support), "-o", str(support_obj)])
                elif self.driver == "clang-cl":
                    row["command"] = ["clang-cl", str(source), "/Fo" + str(obj), "/Fe" + str(exe)]
                else:
                    row["command"] = ["clang", str(source), "-o", str(exe)]
                if self.suite == "typed-errors" and "-trap-O" in name:
                    row.update(exit_code=-1073741795, expected_exit_code=-1073741795)
                rows.append(row)
        stamp = datetime.now(timezone.utc).isoformat()
        report = {"status": "passed", "build_identity": "sdk-test", "client_driver": self.driver,
                  "started_utc": stamp, "finished_utc": stamp, "fixtures": rows}
        if modern:
            report.update(suite=self.suite, run_id=run_id)
        self.mutate_report(report)
        self.mutate_files(report)
        self.report_path.parent.mkdir(parents=True, exist_ok=True)
        # PowerShell reports can include a UTF-8 BOM.
        self.report_path.write_text(json.dumps(report), encoding="utf-8-sig")
        return report

    def launch(self, command, repo, stdout, stderr, timeout, environment):
        self.calls.append((command, repo, stdout, stderr, timeout))
        self.environments.append(environment)
        self.round += 1
        self.write_report()
        stdout.write_text(f"Results: {self.report_path}\n" if self.suite in {"match", "transfer"} else
                          "CARRIER PASS\n", encoding="utf-8")
        stderr.write_text("", encoding="utf-8")
        return self.exit_code

    def run(self, monkeypatch):
        monkeypatch.setattr(fixtures, "_run_hidden", self.launch)
        return fixtures.run_fixture_node(self.repo, self.node, self.output, self.toolchain)


@pytest.mark.parametrize("suite,driver", [
    (suite, driver) for suite in fixtures._RUNNERS for driver in
    (("clang", "clang-cl", "objc3c-clang") if suite in {"match", "transfer"} else ("clang", "clang-cl"))
])
def test_complete_runner_schemas_retain_independent_artifacts(tmp_path, monkeypatch, suite, driver):
    run = FixtureRun(tmp_path, suite, driver)
    result = run.run(monkeypatch)
    assert result["data"]["count"] == 2 * len(fixtures._CASES[suite])
    assert result["data"]["runtime_count"] == result["data"]["count"]
    assert result["data"]["build_identity"] == "sdk-test"
    assert run.calls[0][-1] == 300
    assert all(isinstance(path, Path) and path.is_file() and path.is_relative_to(run.output)
               for path in result["artifacts"].values())
    assert result["artifacts"]["report"].read_bytes() == run.report_path.read_bytes()
    wrapper = result["artifacts"]["launch_script"].read_text(encoding="utf-8")
    assert "-NonInteractive" in result["data"]["command"]
    assert "'VCToolsVersion' = '14.44.35207'" in wrapper
    assert "'WindowsSDKVersion' = '10.0.26100.0'" in wrapper
    assert "discovered-vs" in wrapper and "Enter-VsDevShell" not in wrapper
    assert run.environments[0] == run.toolchain["environment"]
    assert "-NativeDriver" in wrapper if driver == "objc3c-clang" else "-NativeDriver" not in wrapper
    assert not any(path.suffix == ".exe" and "build/bin" in str(path) for path in result["artifacts"].values())
    json.dumps(result["data"])


def test_fixed_output_rerun_cannot_change_prior_cached_products(tmp_path, monkeypatch):
    run = FixtureRun(tmp_path, "cpp-lifetime", "clang-cl")
    first = run.run(monkeypatch)
    frozen = {role: path.read_bytes() for role, path in first["artifacts"].items()}
    second = run.run(monkeypatch)
    assert first["data"]["attempt_id"] != second["data"]["attempt_id"]
    assert all(path.read_bytes() == frozen[role] for role, path in first["artifacts"].items())
    assert first["artifacts"]["report"].read_bytes() != second["artifacts"]["report"].read_bytes()


@pytest.mark.parametrize("exit_code", [0, 1, -1073741819])
@pytest.mark.parametrize("trap_case", [
    "typed-error-dynamic-nil-trap",
    "typed-error-block-dynamic-nil-trap",
    "typed-error-mrc-block-force-error-trap",
    "typed-error-member-dynamic-nil-trap",
    "typed-error-mrc-member-force-error-trap",
    "typed-error-message-dynamic-nil-trap",
    "typed-error-mrc-message-force-error-trap",
])
def test_typed_error_trap_rejects_success_and_other_failure_causes(
    tmp_path, monkeypatch, exit_code, trap_case,
):
    run = FixtureRun(tmp_path, "typed-errors")

    def wrong_trap(report):
        trap = next(row for row in report["fixtures"] if row["name"].startswith(trap_case + "-O"))
        trap["exit_code"] = exit_code

    run.mutate_report = wrong_trap
    with pytest.raises(ValueError, match="did not exit successfully"):
        run.run(monkeypatch)


def test_typed_error_trap_cannot_redefine_its_expected_exit(tmp_path, monkeypatch):
    run = FixtureRun(tmp_path, "typed-errors")

    def forged_trap(report):
        trap = next(row for row in report["fixtures"] if "-trap-O" in row["name"])
        trap.update(exit_code=0, expected_exit_code=0)

    run.mutate_report = forged_trap
    with pytest.raises(ValueError, match="invalid expected exit"):
        run.run(monkeypatch)


@pytest.mark.parametrize("suite", ["expression", "match"])
@pytest.mark.parametrize("exit_code", [0, 9])
def test_prior_passed_report_never_satisfies_failed_or_noop_invocation(tmp_path, monkeypatch, suite, exit_code):
    run = FixtureRun(tmp_path, suite)
    run.round = 1
    run.write_report()

    def noop(command, repo, stdout, stderr, timeout, environment):
        stdout.write_text(f"Results: {run.report_path}\n" if suite == "match" else "", encoding="utf-8")
        stderr.write_text("failed before report creation", encoding="utf-8")
        return exit_code

    monkeypatch.setattr(fixtures, "_run_hidden", noop)
    with pytest.raises((RuntimeError, ValueError), match="failed|stale"):
        fixtures.run_fixture_node(run.repo, run.node, run.output, run.toolchain)
    assert not list(run.output.glob("*/fixture-snapshot.json"))


@pytest.mark.parametrize("mutation,message", [
    (lambda report: report.update(status="failed"), "did not pass"),
    (lambda report: report.update(build_identity="another-sdk"), "identity"),
    (lambda report: report.update(client_driver="clang-cl"), "driver"),
    (lambda report: report["fixtures"][0].update(exit_code=7), "exit successfully"),
    (lambda report: report["fixtures"][0].update(exit_code=False), "exit successfully"),
    (lambda report: report["fixtures"].pop(), "O0/O2"),
    (lambda report: report["fixtures"].append(report["fixtures"][0]), "duplicates"),
    (lambda report: report.update(started_utc=(datetime.now(timezone.utc) - timedelta(days=1)).isoformat()), "timestamps"),
    (lambda report: report["fixtures"][0].update(source_sha256=None), "invalid source SHA256"),
])
def test_passed_process_requires_complete_fresh_runtime_report(tmp_path, monkeypatch, mutation, message):
    run = FixtureRun(tmp_path)
    run.mutate_report = mutation
    with pytest.raises(ValueError, match=message):
        run.run(monkeypatch)


@pytest.mark.parametrize("role", ["source", "executable"])
def test_real_products_must_match_reported_hashes(tmp_path, monkeypatch, role):
    run = FixtureRun(tmp_path)

    def tamper(report):
        row = report["fixtures"][0]
        path = Path(row["source"]) if role == "source" else run.report_path.parent / (row["name"] + ".exe")
        path.write_bytes(b"changed after the runner recorded its identity")

    run.mutate_files = tamper
    with pytest.raises(ValueError, match=f"{role} differs"):
        run.run(monkeypatch)


def test_modern_report_path_and_run_identity_are_bound_to_invocation(tmp_path, monkeypatch):
    run = FixtureRun(tmp_path, "match")
    run.mutate_report = lambda report: report.update(run_id="f" * 32)
    with pytest.raises(ValueError, match="run identity"):
        run.run(monkeypatch)
    outside = tmp_path / "elsewhere/results.json"
    with pytest.raises(ValueError, match="outside"):
        fixtures._report_path(run.base, True, f"Results: {outside}\n")


def test_explicit_node_timeout_is_forwarded_without_environment_mutation(tmp_path, monkeypatch):
    run = FixtureRun(tmp_path)
    run.node["timeout_seconds"] = 27
    before = dict(os.environ)
    run.run(monkeypatch)
    assert run.calls[0][-1] == 27
    assert dict(os.environ) == before


@pytest.mark.parametrize("node", [
    {"operation": "fixture", "suite": "expression", "driver": "objc3c-clang"},
    {"operation": "fixture", "suite": "unknown", "driver": "clang"},
    {"operation": "build", "suite": "match", "driver": "clang"},
    {"operation": "fixture", "suite": "match", "driver": "clang", "timeout_seconds": 0},
    {"operation": "fixture", "suite": "match", "driver": "clang", "timeout_seconds": float("nan")},
])
def test_unadmitted_nodes_never_launch(tmp_path, monkeypatch, node):
    run = FixtureRun(tmp_path)
    run.node = node
    with pytest.raises(ValueError):
        run.run(monkeypatch)
    assert not run.calls


def test_hidden_child_uses_its_own_environment_and_closes_stdin(tmp_path, monkeypatch):
    captured = {}

    class Child:
        def wait(self, timeout):
            captured["timeout"] = timeout
            return 0

    def launch(command, **kwargs):
        captured.update(kwargs)
        return Child()

    monkeypatch.setattr(fixtures, "_WINDOWS", True)
    monkeypatch.setattr(fixtures.subprocess, "Popen", launch)
    monkeypatch.setenv("VCTOOLSVERSION", "ambient-old-version")
    before = dict(os.environ)
    admitted = {"PATH": "snapshot-path", "VCToolsVersion": "admitted-version"}
    assert fixtures._run_hidden(["pwsh", "-NonInteractive"], tmp_path,
                                tmp_path / "stdout", tmp_path / "stderr", 13, admitted) == 0
    assert captured["stdin"] == subprocess.DEVNULL
    assert captured["env"] is not os.environ
    assert captured["env"]["PYTHONDONTWRITEBYTECODE"] == "1"
    assert all(captured["env"][key] == value for key, value in admitted.items())
    assert sum(key.casefold() == "vctoolsversion" for key in captured["env"]) == 1
    assert captured["creationflags"] & 0x08000000
    assert captured["timeout"] == 13
    assert dict(os.environ) == before


def test_timeout_terminates_only_owned_child_tree(tmp_path, monkeypatch):
    calls = []

    class Child:
        pid = 1234

        def wait(self, timeout):
            if timeout == 3:
                raise subprocess.TimeoutExpired("pwsh", timeout)
            return 1

        def poll(self):
            return 1

    monkeypatch.setattr(fixtures, "_WINDOWS", True)
    monkeypatch.setattr(fixtures.subprocess, "Popen", lambda *args, **kwargs: Child())
    monkeypatch.setattr(fixtures.subprocess, "run", lambda command, **kwargs: calls.append(command))
    with pytest.raises(RuntimeError, match="timed out after 3"):
        fixtures._run_hidden(["pwsh"], tmp_path, tmp_path / "out", tmp_path / "err", 3, {})
    assert calls == [["taskkill.exe", "/PID", "1234", "/T", "/F"]]


def test_powershell_paths_are_literal_and_native_driver_is_explicit(tmp_path):
    run = FixtureRun(tmp_path, "match", "objc3c-clang")
    wrapper = fixtures._wrapper(run.repo / "a'b$(value)", run.runner, run.driver, run.toolchain)
    assert "a''b$(value)'" in wrapper
    assert "-NativeDriver '" + run.toolchain["tools"]["facade"] + "'" in wrapper


def test_missing_snapshot_environment_is_rejected_before_launch(tmp_path, monkeypatch):
    run = FixtureRun(tmp_path)
    run.toolchain.pop("environment")
    with pytest.raises(ValueError, match="captured toolchain environment"):
        run.run(monkeypatch)
    assert not run.calls


@pytest.mark.skipif(not fixtures._WINDOWS or not fixtures._POWERSHELL.is_file(),
                    reason="The PowerShell parser is available on the admitted Windows host")
def test_generated_wrapper_is_valid_powershell(tmp_path):
    run = FixtureRun(tmp_path, "match", "objc3c-clang")
    script = tmp_path / "wrapper.ps1"
    script.write_text(fixtures._wrapper(run.repo, run.runner, run.driver, run.toolchain), encoding="utf-8")
    parser = ("$tokens=$null; $errors=$null; "
              "[void][System.Management.Automation.Language.Parser]::ParseFile(" +
              fixtures._literal(script) + ",[ref]$tokens,[ref]$errors); "
              "if($errors.Count){$errors | Out-String | Write-Error; exit 1}")
    result = subprocess.run([str(fixtures._POWERSHELL), "-NoLogo", "-NoProfile", "-NonInteractive",
                             "-Command", parser], capture_output=True, text=True, timeout=30,
                            creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
    assert result.returncode == 0, result.stderr


@pytest.mark.skipif(not fixtures._WINDOWS or not fixtures._POWERSHELL.is_file(),
                    reason="The PowerShell child runs on the admitted Windows host")
def test_wrapper_restores_snapshot_after_powershell_startup_path_change(tmp_path):
    run = FixtureRun(tmp_path)
    observed = tmp_path / "child-environment.json"
    # This runner records the environment it actually receives. It does not
    # load VS, compile a fixture, or mutate the test's parent environment.
    run.runner.write_text("\n".join((
        "param([string]$ClangBin, [string]$ClientDriver)",
        "$observed = @{}",
        "foreach ($key in @('PATH', 'INCLUDE', 'LIB', 'VCToolsVersion', 'WindowsSDKVersion')) {",
        "  $observed[$key] = [Environment]::GetEnvironmentVariable($key)",
        "}",
        "$observed | ConvertTo-Json | Set-Content -Encoding utf8 -LiteralPath " + fixtures._literal(observed),
        "$global:LASTEXITCODE = 0",
    )), encoding="utf-8")
    script = tmp_path / "wrapper.ps1"
    script.write_text(fixtures._wrapper(run.repo, run.runner, run.driver, run.toolchain), encoding="utf-8")
    startup = ("[Environment]::SetEnvironmentVariable('PATH', " +
               fixtures._literal("powershell-startup-prefix;" + run.toolchain["environment"]["PATH"]) +
               ", [EnvironmentVariableTarget]::Process); & " + fixtures._literal(script))
    environment = os.environ.copy()
    before = dict(os.environ)
    result = subprocess.run([str(fixtures._POWERSHELL), "-NoLogo", "-NoProfile", "-NonInteractive",
                             "-Command", startup], capture_output=True, text=True, timeout=30,
                            env=environment, creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
    assert result.returncode == 0, result.stderr
    actual = json.loads(observed.read_text(encoding="utf-8-sig"))
    assert actual == {key: run.toolchain["environment"][key] for key in actual}
    assert set(actual) == {"PATH", "INCLUDE", "LIB", "VCToolsVersion", "WindowsSDKVersion"}
    assert dict(os.environ) == before
