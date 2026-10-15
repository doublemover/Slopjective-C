"""Run the admitted Windows fixture scripts and retain independent evidence."""

from __future__ import annotations

from datetime import datetime, timezone
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import uuid


_WINDOWS = os.name == "nt"
_POWERSHELL = Path(r"C:\Program Files\PowerShell\7\pwsh.exe")
_RUNNERS = {
    "expression": ("run_carrier_expression_fixtures_windows.ps1", "carrier-expression-fixtures"),
    "abi": ("run_carrier_abi_fixtures_windows.ps1", "carrier-abi-fixtures"),
    "cpp-lifetime": ("run_carrier_cpp_lifetime_fixtures_windows.ps1", "carrier-cpp-lifetime-fixtures"),
    "arc-lifetime": ("run_carrier_arc_lifetime_fixtures_windows.ps1", "carrier-arc-lifetime-fixtures"),
    "match": ("run_native_match_fixtures_windows.ps1", "native-match-fixtures"),
    "transfer": ("run_carrier_transfer_fixtures_windows.ps1", "native-transfer-fixtures"),
    "typed-errors": ("run_native_typed_error_fixtures_windows.ps1", "native-typed-error-fixtures"),
}
_CASES = {
    "expression": (
        "optional-expression-runtime", "optional-copy-runtime", "result-expression-runtime",
        "carrier-copy-adversarial-runtime", "carrier-expression-adversarial-runtime",
    ),
    "abi": ("extension-provider-consumer", "ordinary-c-active-payload"),
    "cpp-lifetime": ("carrier-cxx-local-lifetime-runtime",),
    "arc-lifetime": ("carrier-arc-local-lifetime",),
    "match": ("match-native-runtime", "match-guard-cleanup-runtime"),
    "transfer": (
        "carrier-transfer-runtime", "carrier-transfer-runtime-v2", "carrier-transfer-local-runtime",
    ),
    "typed-errors": (
        "typed-error-out-runtime", "typed-error-dynamic-nil-trap", "typed-error-force-error-trap",
    ),
}
_SHA256 = re.compile(r"[0-9a-fA-F]{64}\Z")
_BUILD_ID = re.compile(r"[A-Za-z0-9_.-]+\Z")
_UNREPORTED = object()


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def _literal(value: str | Path) -> str:
    return "'" + str(value).replace("'", "''") + "'"


def _tool_path(toolchain: dict, key: str) -> Path:
    value = toolchain.get("tools", {}).get(key)
    if not isinstance(value, (str, Path)) or not str(value):
        raise ValueError(f"Fixture node requires toolchain.tools.{key}")
    path = Path(value)
    if not path.is_absolute():
        raise ValueError(f"Fixture tool path must be absolute: {key}")
    return path


def _fixture_environment(toolchain: dict) -> dict[str, str]:
    environment = toolchain.get("environment")
    if (not isinstance(environment, dict) or
            any(not isinstance(key, str) or not isinstance(value, str)
                for key, value in environment.items())):
        raise ValueError("Fixture node requires the captured toolchain environment")
    required = ("PATH", "INCLUDE", "LIB", "VCToolsInstallDir", "VCToolsVersion",
                "WindowsSdkDir", "WindowsSDKVersion")
    if any(not environment.get(key) for key in required):
        raise ValueError("Fixture node captured developer environment is incomplete")
    return dict(environment)


def _wrapper(repo: Path, runner: Path, driver: str, toolchain: dict) -> str:
    clang_bin = _tool_path(toolchain, "clang").parent
    client_key = "clang_cl" if driver == "clang-cl" else "clangxx"
    if driver != "objc3c-clang" and _tool_path(toolchain, client_key).parent != clang_bin:
        raise ValueError("Fixture compiler drivers must belong to the same SDK bin directory")
    arguments = ["-ClangBin", _literal(clang_bin),
                 "-ClientDriver", _literal(driver)]
    if driver == "objc3c-clang":
        arguments.extend(("-NativeDriver", _literal(_tool_path(toolchain, "facade"))))
    environment = _fixture_environment(toolchain)
    entries = [f"    {_literal(key)} = {_literal(value)}" for key, value in sorted(environment.items())]
    return "\n".join((
        "$ErrorActionPreference = 'Stop'",
        "$PSNativeCommandUseErrorActionPreference = $false",
        "[Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)",
        "try {",
        "  $objc3SnapshotEnvironment = @{",
        *entries,
        "  }",
        "  # PowerShell startup may change PATH. Restore the admitted values in",
        "  # this private process before invoking the runner or its native tools.",
        "  foreach ($objc3EnvironmentKey in $objc3SnapshotEnvironment.Keys) {",
        "    [Environment]::SetEnvironmentVariable($objc3EnvironmentKey,",
        "      $objc3SnapshotEnvironment[$objc3EnvironmentKey], [EnvironmentVariableTarget]::Process)",
        "    if ([Environment]::GetEnvironmentVariable($objc3EnvironmentKey) -cne",
        "        $objc3SnapshotEnvironment[$objc3EnvironmentKey]) {",
        "      throw \"Fixture node environment differs from the admitted snapshot: $objc3EnvironmentKey\"",
        "    }",
        "  }",
        f"  Set-Location -LiteralPath {_literal(repo)}",
        "  $env:PYTHONDONTWRITEBYTECODE = '1'",
        f"  & {_literal(runner)} {' '.join(arguments)}",
        "  if ($LASTEXITCODE -ne 0) { throw \"Fixture runner exited with $LASTEXITCODE\" }",
        "  exit 0",
        "} catch {",
        "  Write-Error $_ -ErrorAction Continue",
        "  exit 1",
        "}",
        "",
    ))


def _run_hidden(command: list[str], repo: Path, stdout: Path, stderr: Path,
                timeout_seconds: float, snapshot_environment: dict[str, str]) -> int:
    if not _WINDOWS:
        raise RuntimeError("Native runtime fixture nodes require the admitted Windows host")
    # Windows environment names are case insensitive, whereas a plain Python
    # dict is not. Remove ambient spellings before applying captured values.
    captured_keys = {key.casefold() for key in snapshot_environment}
    environment = {key: value for key, value in os.environ.items()
                   if key.casefold() not in captured_keys}
    environment.update(snapshot_environment)
    environment["PYTHONDONTWRITEBYTECODE"] = "1"
    flags = getattr(subprocess, "CREATE_NO_WINDOW", 0x08000000) | getattr(
        subprocess, "CREATE_NEW_PROCESS_GROUP", 0x00000200)
    with stdout.open("xb") as out, stderr.open("xb") as err:
        process = subprocess.Popen(
            command, cwd=repo, stdin=subprocess.DEVNULL, stdout=out, stderr=err,
            env=environment, close_fds=True, creationflags=flags,
        )
        try:
            return process.wait(timeout=timeout_seconds)
        except subprocess.TimeoutExpired as exc:
            # Kill only this owned process tree; otherwise its compiler children
            # could continue overwriting shared fixture outputs after node failure.
            try:
                subprocess.run(
                    ["taskkill.exe", "/PID", str(process.pid), "/T", "/F"],
                    stdin=subprocess.DEVNULL, stdout=out, stderr=err,
                    env=environment, creationflags=flags, timeout=10, check=False,
                )
            except (OSError, subprocess.TimeoutExpired):
                pass
            if process.poll() is None:
                process.kill()
            process.wait(timeout=10)
            raise RuntimeError(f"Fixture runner timed out after {timeout_seconds:g} seconds") from exc


def _timestamp(report: dict, field: str) -> datetime:
    value = report.get(field)
    if not isinstance(value, str):
        raise ValueError(f"Fixture report is missing {field}")
    try:
        stamp = datetime.fromisoformat(value.replace("Z", "+00:00"))
    except ValueError as exc:
        raise ValueError(f"Fixture report has invalid {field}") from exc
    if stamp.tzinfo is None:
        raise ValueError(f"Fixture report {field} must include a timezone")
    return stamp.astimezone(timezone.utc)


def _report_path(base: Path, modern: bool, stdout: str) -> Path:
    printed = re.findall(r"^Results:\s*(.+?)\s*$", stdout, flags=re.MULTILINE)
    if modern:
        if len(printed) != 1:
            raise ValueError("Fixture runner did not identify exactly one results report")
        path = Path(printed[0]).resolve()
        if path.name != "results.json" or path.parent.parent != base.resolve():
            raise ValueError("Fixture runner report path is outside its suite/build/driver output")
        return path
    expected = (base / "results.json").resolve()
    if printed and (len(printed) != 1 or Path(printed[0]).resolve() != expected):
        raise ValueError("Fixture runner printed an unexpected results report path")
    return expected


def _validate_report(report: dict, suite: str, driver: str, build_id: str,
                     path: Path, previous: dict[Path, str], raw_digest: str,
                     started: datetime, finished: datetime) -> list[dict]:
    if report.get("status") != "passed":
        raise ValueError("Fixture runner report did not pass")
    if report.get("build_identity") != build_id or report.get("client_driver") != driver:
        raise ValueError("Fixture report build identity or client driver differs from the node")
    if path in previous and previous[path] == raw_digest:
        raise ValueError("Fixture runner left a stale results report unchanged")
    report_started = _timestamp(report, "started_utc")
    report_finished = _timestamp(report, "finished_utc")
    if not started <= report_started <= report_finished <= finished:
        raise ValueError("Fixture results report timestamps are outside this invocation")
    if suite in {"match", "transfer"}:
        run_id = report.get("run_id")
        if (report.get("suite") != suite or not isinstance(run_id, str) or
                not re.fullmatch(r"[0-9a-f]{32}", run_id) or path.parent.name != run_id or
                path in previous):
            raise ValueError("Fixture report does not have a fresh matching run identity")
    fixtures = report.get("fixtures")
    expected = {f"{case}-O{level}" for case in _CASES[suite] for level in (0, 2)}
    if not isinstance(fixtures, list) or any(not isinstance(row, dict) for row in fixtures):
        raise ValueError("Fixture report must contain runtime fixture records")
    names = [row.get("name") for row in fixtures]
    if any(not isinstance(name, str) for name in names) or set(names) != expected or len(names) != len(expected):
        raise ValueError("Fixture report is missing expected O0/O2 cases or contains duplicates")
    for row in fixtures:
        expected_exit = 0
        if suite == "typed-errors" and "-trap-O" in row["name"]:
            expected_exit = -1073741795  # Windows illegal-instruction trap.
            if type(row.get("expected_exit_code")) is not int or row["expected_exit_code"] != expected_exit:
                raise ValueError(f"Runtime trap fixture has an invalid expected exit: {row['name']}")
        if type(row.get("exit_code")) is not int or row["exit_code"] != expected_exit:
            raise ValueError(f"Runtime fixture did not exit successfully: {row['name']}")
        if "status" in row and row["status"] != "passed":
            raise ValueError(f"Runtime fixture did not pass: {row['name']}")
        for field in ("compile_exit_code", "link_exit_code"):
            if field in row and (type(row[field]) is not int or row[field] != 0):
                raise ValueError(f"Runtime fixture has failed {field}: {row['name']}")
        for command in row.get("commands", []):
            if (not isinstance(command, dict) or type(command.get("exit_code")) is not int or
                    command["exit_code"] != 0):
                raise ValueError(f"Runtime fixture has a failed compile/link command: {row['name']}")
    return fixtures


def _command_output(command: object, extension: str) -> str | None:
    if not isinstance(command, list) or not all(isinstance(arg, str) for arg in command):
        return None
    for index, argument in enumerate(command):
        if argument in {"-o", "--emit-object"} and index + 1 < len(command):
            if Path(command[index + 1]).suffix.lower() == extension:
                return command[index + 1]
        if argument.lower().startswith(("/fo", "/fe")) and len(argument) > 3:
            if Path(argument[3:]).suffix.lower() == extension:
                return argument[3:]
    return None


def _snapshot_products(repo: Path, report_path: Path, fixtures: list[dict], suite: str,
                       output: Path) -> tuple[dict[str, Path], list[dict]]:
    artifacts: dict[str, Path] = {}
    records: list[dict] = []

    def retain(name: str, role: str, original: object, expected: object = _UNREPORTED) -> None:
        if not isinstance(original, (str, Path)) or not str(original):
            raise ValueError(f"Fixture report is missing {role} path: {name}")
        path = Path(original).resolve()
        if not path.is_relative_to(repo) or not path.is_file():
            raise ValueError(f"Fixture {role} is unavailable or outside the repository: {path}")
        if expected is not _UNREPORTED and (not isinstance(expected, str) or not _SHA256.fullmatch(expected)):
            raise ValueError(f"Fixture report has invalid {role} SHA256: {name}")
        digest = _sha256(path)
        if expected is not _UNREPORTED and digest != expected.lower():
            raise ValueError(f"Fixture {role} differs from its reported SHA256: {name}")
        snapshot = output / "products" / name / role / path.name
        snapshot.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(path, snapshot)
        if _sha256(snapshot) != digest:
            raise ValueError(f"Fixture {role} changed while being retained: {name}")
        key = f"{name}.{role}"
        artifacts[key] = snapshot
        records.append({"role": key, "original_path": str(path),
                        "snapshot_path": str(snapshot), "sha256": digest})

    for fixture in fixtures:
        name = fixture["name"]
        if "sources" in fixture:
            if not isinstance(fixture["sources"], list) or not fixture["sources"]:
                raise ValueError(f"Fixture report has no input sources: {name}")
            for index, source in enumerate(fixture["sources"]):
                if not isinstance(source, dict) or "sha256" not in source:
                    raise ValueError(f"Fixture source identity is missing: {name}")
                retain(name, f"source-{index}", source.get("file"), source["sha256"])
        else:
            if "source_sha256" not in fixture:
                raise ValueError(f"Fixture source identity is missing: {name}")
            retain(name, "source", fixture.get("source"), fixture["source_sha256"])
        if fixture.get("generated_source_required"):
            retain(name, "generated-source", fixture.get("generated_source"),
                   fixture.get("generated_source_sha256"))
        if "support_source_sha256" in fixture:
            retain(name, "support-source", Path(fixture["source"]).parent /
                   "carrier-arc-local-runtime-support.m", fixture["support_source_sha256"])
        if "header_sha256" in fixture:
            header = ("carrier-native-abi.h" if name.startswith("extension-provider-consumer-")
                      else "carrier-native-abi-c-mirror.h")
            retain(name, "header", Path(fixture["sources"][0]["file"]).parent / header,
                   fixture["header_sha256"])
        for index, obj in enumerate(fixture.get("objects", [])):
            if not isinstance(obj, dict) or "sha256" not in obj:
                raise ValueError(f"Fixture object identity is missing: {name}")
            retain(name, f"object-{index}", obj.get("file"), obj["sha256"])
        for role, field, command_field in (
                ("object", "object_sha256", "compile_command"),
                ("support-object", "support_object_sha256", "support_command")):
            if field in fixture:
                command = fixture.get(command_field, fixture.get("command"))
                retain(name, role, _command_output(command, ".obj"), fixture[field])
        # Older clang-cl runners record /Fo but omit the object digest. Preserve
        # that real compile product too, with its own snapshot identity.
        if "object_sha256" not in fixture and "objects" not in fixture:
            obj = _command_output(fixture.get("command"), ".obj")
            if obj is not None:
                retain(name, "object", obj)
        executable = fixture.get("executable")
        if executable is None and isinstance(fixture.get("runtime_command"), list):
            executable = fixture["runtime_command"][0] if fixture["runtime_command"] else None
        if executable is None:
            executable = report_path.parent / (name + ".exe")
        if "executable_sha256" not in fixture:
            raise ValueError(f"Fixture executable identity is missing: {name}")
        retain(name, "executable", executable, fixture["executable_sha256"])
    return artifacts, records


def run_fixture_node(repo: Path, node: dict, output_dir: Path, toolchain: dict) -> dict:
    """Run one complete suite/driver node; raise on failed or stale evidence.

    Reports and mutable fixture products are copied beneath a unique attempt
    directory, so unrelated executions of fixed-output runners cannot corrupt
    the artifacts returned to the DAG executor for later reuse.
    """
    suite, driver = node.get("suite"), node.get("driver")
    if node.get("operation") != "fixture" or suite not in _RUNNERS:
        raise ValueError("Invalid native fixture node operation or suite")
    admitted = {"clang", "clang-cl"}
    if suite in {"match", "transfer"}:
        admitted.add("objc3c-clang")
    if driver not in admitted:
        raise ValueError(f"Unsupported fixture client driver for {suite}: {driver}")
    build_id = toolchain.get("sdk_build_id")
    if not isinstance(build_id, str) or not _BUILD_ID.fullmatch(build_id):
        raise ValueError("Fixture node requires a safe sdk_build_id")
    timeout = node.get("timeout_seconds", 300)
    if isinstance(timeout, bool) or not isinstance(timeout, (int, float)) or not math.isfinite(timeout) or timeout <= 0:
        raise ValueError("Fixture node timeout_seconds must be positive and finite")
    environment = _fixture_environment(toolchain)
    repo = Path(repo).resolve(strict=True)
    runner_name, folder = _RUNNERS[suite]
    runner = repo / "scripts/objc3c_toolchain_runtime" / runner_name
    if not runner.is_file():
        raise ValueError(f"Tracked native fixture runner is unavailable: {runner}")
    modern = suite in {"match", "transfer"}
    base = repo / "out/toolchains/objc3-development" / folder / build_id / driver
    candidates = list(base.glob("*/results.json")) if modern else [base / "results.json"]
    previous = {path.resolve(): _sha256(path) for path in candidates if path.is_file()}
    attempt_id = uuid.uuid4().hex
    output = Path(output_dir).resolve() / ("fixture-" + attempt_id)
    output.mkdir(parents=True, exist_ok=False)
    wrapper = output / "run.ps1"
    wrapper.write_text(_wrapper(repo, runner, driver, toolchain), encoding="utf-8")
    stdout, stderr = output / "stdout.log", output / "stderr.log"
    command = [str(_POWERSHELL), "-NoLogo", "-NoProfile", "-NonInteractive", "-File", str(wrapper)]
    started = datetime.now(timezone.utc)
    exit_code = _run_hidden(command, repo, stdout, stderr, timeout, environment)
    finished = datetime.now(timezone.utc)
    if exit_code != 0:
        raise RuntimeError(f"Native fixture runner failed ({suite}/{driver}, exit {exit_code}); logs: {output}")
    path = _report_path(base, modern, stdout.read_text(encoding="utf-8-sig", errors="replace"))
    try:
        raw = path.read_bytes()
        report = json.loads(raw.decode("utf-8-sig"))
    except (OSError, UnicodeError, ValueError) as exc:
        raise ValueError(f"Native fixture results report is unavailable or invalid: {path}") from exc
    if not isinstance(report, dict):
        raise ValueError("Native fixture results report must be a JSON object")
    report_digest = hashlib.sha256(raw).hexdigest()
    fixtures = _validate_report(report, suite, driver, build_id, path, previous,
                                report_digest, started, finished)
    products, retained = _snapshot_products(repo, path, fixtures, suite, output)
    copied_report = output / "results.json"
    copied_report.write_bytes(raw)
    manifest = output / "fixture-snapshot.json"
    manifest.write_text(json.dumps({"attempt_id": attempt_id, "runner_report_path": str(path),
                                   "runner_report_sha256": report_digest, "products": retained},
                                  indent=2) + "\n", encoding="utf-8")
    return {
        "artifacts": {"report": copied_report, "stdout": stdout, "stderr": stderr,
                      "launch_script": wrapper, "fixture_snapshot": manifest, **products},
        "data": {"suite": suite, "driver": driver, "build_identity": build_id,
                 "count": len(fixtures), "runtime_count": len(fixtures),
                 "command": command, "exit_code": exit_code,
                 "report_path": str(copied_report), "runner_report_path": str(path),
                 "attempt_id": attempt_id, "run_id": report.get("run_id"),
                 "started_utc": started.isoformat(), "finished_utc": finished.isoformat()},
    }
