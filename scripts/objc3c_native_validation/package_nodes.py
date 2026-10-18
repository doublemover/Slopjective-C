"""Compile a native package and run consumers installed outside the checkout."""
from __future__ import annotations

import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import time
from typing import Any

MODULE = "fixture.NativePackage"
PACKAGE = "native-package-v1"
PROVIDER_NODE = "native-package:provider"
PACKAGE_CONTRACT = "objc3c.native_validation.compiled_package.v1"
EVIDENCE_CONTRACT = "objc3c.native_validation.package_node.v1"
FIXTURES = Path("tests/clang_foundation/native_package")
PACKAGE_FILENAMES = {"object": "provider.obj", "interface": "NativePackage.objc3i",
                     "sidecar": "NativePackage.objc3i.json"}
_ADMITTED_TARGET = "x86_64-pc-windows-msvc"
_MODULE_FIELDS = ("canonical_name", "semantic_fingerprint", "target_triple", "runtime_abi",
                  "extension_abi", "language_revision", "language_profile", "package_identity")


class PackageNodeError(RuntimeError):
    """The node failed; its command evidence remains in the requested output."""
    def __init__(self, message: str, *, data: dict[str, Any], artifacts: dict[str, Path]):
        super().__init__(message)
        self.data = data
        self.artifacts = artifacts


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def _json(path: Path) -> dict[str, Any]:
    value = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(value, dict):
        raise ValueError(f"expected a JSON object: {path}")
    return value


def _write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, name = tempfile.mkstemp(prefix="." + path.name + "-", suffix=".tmp", dir=path.parent)
    temporary = Path(name)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="") as stream:
            json.dump(value, stream, indent=2, sort_keys=True)
            stream.write("\n")
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def _file(value: Any, description: str) -> Path:
    if isinstance(value, dict):
        value = value.get("path")
    if not isinstance(value, (str, Path)) or not str(value):
        raise ValueError(f"{description} has no file path")
    path = Path(value)
    if path.is_symlink() or not path.is_file():
        raise ValueError(f"{description} is missing or is a symbolic link: {path}")
    return path.resolve()


def _tool(toolchain: dict[str, Any], name: str) -> Path:
    return _file(toolchain.get("tools", {}).get(name), f"sealed {name} tool")


def _environment(toolchain: dict[str, Any], *, executable: bool = False) -> dict[str, str]:
    environment = dict(os.environ)
    supplied = toolchain.get("environment", {})
    if not isinstance(supplied, dict) or any(not isinstance(k, str) or not isinstance(v, str)
                                             for k, v in supplied.items()):
        raise ValueError("toolchain environment must contain string keys and values")
    for key, value in supplied.items():
        if os.name == "nt":
            for existing in list(environment):
                if existing.upper() == key.upper():
                    del environment[existing]
        environment[key] = value
    if executable:
        directory = Path(toolchain["runtime"]["bin_dir"]).resolve()
        if not directory.is_dir():
            raise ValueError("native runtime binary directory is missing")
        # Windows environment keys are case insensitive. Avoid retaining a
        # second Path key that could replace the intended private child PATH.
        existing = next((key for key in environment if key.upper() == "PATH"), "PATH")
        prior = environment.pop(existing, "")
        environment["PATH"] = str(directory) + os.pathsep + prior
    return environment


def _execute(command: list[str], *, phase: str, cwd: Path, output: Path,
             environment: dict[str, str], timeout: float, data: dict[str, Any],
             artifacts: dict[str, Path]) -> None:
    stdout = output / f"{phase}.stdout.log"
    stderr = output / f"{phase}.stderr.log"
    record: dict[str, Any] = {"phase": phase, "argv": command, "cwd": str(cwd),
                              "timeout_seconds": timeout, "exit_code": None}
    data["commands"].append(record)
    artifacts[f"{phase}_stdout"] = stdout
    artifacts[f"{phase}_stderr"] = stderr
    _write_json(output / "package-node-evidence.json", data)
    started = time.monotonic()
    try:
        process = subprocess.run(command, cwd=cwd, env=environment, text=True,
                                 encoding="utf-8", errors="replace", capture_output=True,
                                 check=False, timeout=timeout)
        stdout.write_text(process.stdout or "", encoding="utf-8")
        stderr.write_text(process.stderr or "", encoding="utf-8")
        record["exit_code"] = process.returncode
        if process.returncode != 0:
            raise RuntimeError(f"{phase} exited {process.returncode}; see {stderr}")
    except subprocess.TimeoutExpired as error:
        record.update({"exit_code": 124, "timed_out": True})
        stdout.write_text(_text(error.stdout), encoding="utf-8")
        stderr.write_text(_text(error.stderr), encoding="utf-8")
        raise RuntimeError(f"{phase} exceeded its {timeout:g}s deadline") from error
    except OSError as error:
        record.update({"exit_code": 127, "spawn_error": str(error)})
        stdout.write_text("", encoding="utf-8")
        stderr.write_text(str(error) + "\n", encoding="utf-8")
        raise RuntimeError(f"could not start {phase}: {error}") from error
    finally:
        record["elapsed_seconds"] = round(time.monotonic() - started, 6)
        _write_json(output / "package-node-evidence.json", data)


def _text(value: str | bytes | None) -> str:
    return value.decode("utf-8", errors="replace") if isinstance(value, bytes) else (value or "")


def _fixture(repo: Path, name: str) -> Path:
    source = _file(repo / FIXTURES / name, "tracked package fixture")
    if not source.is_relative_to(repo):
        raise ValueError("package fixture escapes the checkout")
    return source


def _compile_arguments(toolchain: dict[str, Any], source: Path, object_path: Path | None) -> list[str]:
    target = toolchain.get("target_triple")
    if not isinstance(target, str) or not target:
        raise ValueError("sealed toolchain target_triple is required")
    command = [str(_tool(toolchain, "facade")), str(source), "--objc3", "--target", target,
               "--runtime", "gnustep-2.0"]
    return command + (["--emit-object", str(object_path)] if object_path is not None else [])


def _clang_arguments(toolchain: dict[str, Any]) -> list[str]:
    directory = Path(toolchain["resource_dir"]).resolve()
    if not directory.is_dir():
        raise ValueError("sealed Clang resource directory is missing")
    flags = ["--", "-std=c17", "-fblocks", "-O2", "-resource-dir", str(directory)]
    if toolchain.get("platform") != "linux-x86_64":
        flags.append("-fms-runtime-lib=dll")
    return flags


def _module_identity(interface: Path, sidecar: Path) -> dict[str, str]:
    payload = _json(sidecar)
    if payload.get("schema_id") != "objc3c.semantic-interface.v1" or payload.get("schema_major") != 1:
        raise ValueError("unsupported native semantic interface sidecar")
    identity = payload.get("module")
    if not isinstance(identity, dict) or any(not isinstance(identity.get(key), str) or not identity[key]
                                            for key in _MODULE_FIELDS):
        raise ValueError("native package module identity is incomplete")
    if identity["canonical_name"] != MODULE or identity["package_identity"] != PACKAGE:
        raise ValueError("native package module or package identity mismatch")
    if payload.get("source_binding_sha256") != _sha256(interface):
        raise ValueError("native package sidecar is not bound to its interface bytes")
    declarations = payload.get("declarations", [])
    exported_names = {row.get("qualified_name", "").rsplit("::", 1)[-1]
                      for row in declarations if isinstance(row, dict)}
    if not {"Answer", "answer"}.issubset(exported_names):
        raise ValueError("native package interface omits the exported typedef or function")
    return {key: identity[key] for key in _MODULE_FIELDS}


def _target_compatible(admitted: str, emitted: str) -> bool:
    """Allow LLVM's numeric MSVC environment version on the admitted profile.

    Architecture, vendor, OS, and environment spelling remain exact. The
    emitted versioned triple is preserved in the artifact and dependency lock.
    """
    if emitted == admitted:
        return True
    return admitted == _ADMITTED_TARGET and re.fullmatch(
        re.escape(_ADMITTED_TARGET) + r"[0-9]+(?:\.[0-9]+){0,3}", emitted
    ) is not None


def _provider(repo: Path, output: Path, toolchain: dict[str, Any], timeout: float,
              data: dict[str, Any], artifacts: dict[str, Path]) -> None:
    source = _fixture(repo, "provider.m")
    bundle = output / "bundle"
    bundle.mkdir(exist_ok=False)
    files = {role: bundle / name for role, name in PACKAGE_FILENAMES.items()}
    command = _compile_arguments(toolchain, source, files["object"])
    command += ["--emit-interface", str(files["interface"]), "--module", MODULE, "--package", PACKAGE]
    command += _clang_arguments(toolchain)
    _execute(command, phase="provider-compile", cwd=output, output=output,
             environment=_environment(toolchain), timeout=timeout, data=data, artifacts=artifacts)
    for role, path in files.items():
        artifacts[role] = _file(path, f"emitted provider {role}")
    identity = _module_identity(files["interface"], files["sidecar"])
    if not _target_compatible(toolchain["target_triple"], identity["target_triple"]):
        raise ValueError("provider module target differs from the sealed toolchain")
    manifest = {"contract_id": PACKAGE_CONTRACT, "sdk_build_id": toolchain["sdk_build_id"],
                "build_identity": toolchain.get("build_identity"), "module_identity": identity,
                "provider_fixture": {"path": (FIXTURES / source.name).as_posix(), "sha256": _sha256(source)},
                "provider_source_in_package": False,
                "files": [{"role": role, "filename": path.name, "sha256": _sha256(path)}
                          for role, path in files.items()]}
    manifest_path = output / "package-manifest.json"
    _write_json(manifest_path, manifest)
    artifacts["package_manifest"] = manifest_path
    data.update({"module_identity": identity, "package_manifest": manifest,
                 "provider_source_in_package": False, "fixture_sha256": _sha256(source)})
    # Reparse the same declarations under ARC and bind a distinct interface
    # identity to the same native provider object. Runtime ownership modes
    # remain exact; consumers never silently rewrite the provider's ABI lock.
    arc_bundle = output / "bundle-arc"
    arc_bundle.mkdir()
    arc_files = {role: arc_bundle / name for role, name in PACKAGE_FILENAMES.items()}
    shutil.copy2(files["object"], arc_files["object"])
    arc_source = output / "provider-arc-interface.m"
    shutil.copy2(files["interface"], arc_source)
    command = _compile_arguments(toolchain, arc_source, None)
    command += ["--arc", "--emit-interface", str(arc_files["interface"]), "--module", MODULE, "--package", PACKAGE]
    command += _clang_arguments(toolchain)
    _execute(command, phase="provider-arc-interface", cwd=output, output=output,
             environment=_environment(toolchain), timeout=timeout, data=data, artifacts=artifacts)
    arc_identity = _module_identity(arc_files["interface"], arc_files["sidecar"])
    if not arc_identity["runtime_abi"].endswith(";arc"):
        raise ValueError("ARC interface view does not identify ARC ownership")
    arc_manifest = {**manifest, "module_identity": arc_identity,
        "files": [{"role": role, "filename": path.name, "sha256": _sha256(path)}
                  for role, path in arc_files.items()]}
    arc_manifest_path = output / "package-manifest-arc.json"
    _write_json(arc_manifest_path, arc_manifest)
    artifacts.update({"arc_" + role: path for role, path in arc_files.items()})
    artifacts["arc_package_manifest"] = arc_manifest_path
    data["arc_package_manifest"] = arc_manifest



def _package_dependency(dependencies: dict[str, dict[str, Any]], toolchain: dict[str, Any],
                        *, arc: bool = False) -> tuple[dict[str, Path], dict[str, Any]]:
    dependency = dependencies.get(PROVIDER_NODE)
    if not isinstance(dependency, dict) or not isinstance(dependency.get("artifacts"), dict):
        raise ValueError("native package consumer requires native-package:provider artifacts")
    prefix = "arc_" if arc else ""
    files = {role: _file(dependency["artifacts"].get(prefix + role), f"provider dependency {role}")
             for role in (*PACKAGE_FILENAMES, "package_manifest")}
    manifest = _json(files["package_manifest"])
    if manifest.get("contract_id") != PACKAGE_CONTRACT or manifest.get("sdk_build_id") != toolchain["sdk_build_id"]:
        raise ValueError("native package manifest contract or SDK identity mismatch")
    if manifest.get("build_identity") != toolchain.get("build_identity"):
        raise ValueError("native package manifest build identity mismatch")
    rows = manifest.get("files")
    if (not isinstance(rows, list) or len(rows) != 3 or any(not isinstance(row, dict) for row in rows)
            or {row.get("role") for row in rows} != set(PACKAGE_FILENAMES)):
        raise ValueError("native package manifest must declare exactly object, interface and sidecar")
    for row in rows:
        role = row["role"]
        if row.get("filename") != PACKAGE_FILENAMES[role] or row.get("sha256") != _sha256(files[role]):
            raise ValueError(f"native package {role} differs from the locked manifest")
    identity = _module_identity(files["interface"], files["sidecar"])
    if (manifest.get("module_identity") != identity
            or not _target_compatible(toolchain["target_triple"], identity["target_triple"])):
        raise ValueError("native package locked module identity mismatch")
    if manifest.get("provider_source_in_package") is not False:
        raise ValueError("native package must not contain provider source")
    return files, manifest


def _dependency_record(path: Path, identity: dict[str, str], toolchain: dict[str, Any]) -> dict[str, Any]:
    record = _json(_file(path, "consumer dependency record"))
    if record.get("schema") != "objc3c.dependency-record.v2":
        raise ValueError("consumer did not emit a native v2 dependency record")
    expected = {"name": identity["canonical_name"], "fingerprint": identity["semantic_fingerprint"],
                "target": identity["target_triple"], "runtime": identity["runtime_abi"],
                "extension_abi": identity["extension_abi"], "language_revision": identity["language_revision"],
                "language_profile": identity["language_profile"], "package": identity["package_identity"]}
    if record.get("modules") != [expected] or record.get("visible_modules") != [MODULE]:
        raise ValueError("consumer dependency record does not attest the locked installed module")
    compiler = record.get("toolchain", {})
    if not isinstance(compiler, dict) or Path(str(compiler.get("driver_path", ""))).resolve() != _tool(toolchain, "clang"):
        raise ValueError("consumer dependency record uses another compiler driver")
    if Path(str(compiler.get("resource_directory", ""))).resolve() != Path(toolchain["resource_dir"]).resolve():
        raise ValueError("consumer dependency record uses another resource directory")
    if toolchain.get("patch_digest") and compiler.get("patch_digest") != toolchain["patch_digest"]:
        raise ValueError("consumer dependency record uses another native patch seal")
    return record


def _consumer(repo: Path, node: dict[str, Any], output: Path, toolchain: dict[str, Any],
              dependencies: dict[str, dict[str, Any]], timeout: float,
              data: dict[str, Any], artifacts: dict[str, Path]) -> None:
    mode = node.get("mode")
    if mode not in ("eager", "source-directed"):
        raise ValueError("native package consumer mode must be eager or source-directed")
    source = _fixture(repo, "consumer-eager.m" if mode == "eager" else "consumer-source.m")
    files, manifest = _package_dependency(dependencies, toolchain, arc=mode == "source-directed")
    identity = manifest["module_identity"]
    runtime_library = _file(toolchain.get("runtime", {}).get("import_library"), "native runtime import library")
    _file(toolchain.get("runtime", {}).get("dll"), "native runtime DLL")
    scratch_parent = Path(tempfile.gettempdir()).resolve()
    if scratch_parent.is_relative_to(repo):
        raise ValueError("system temporary directory is inside the checkout; cannot isolate package consumer")
    captured = output / "installed-consumer"
    captured.mkdir(exist_ok=False)
    scratch = Path(tempfile.mkdtemp(prefix="objc3-native-package-", dir=scratch_parent)).resolve()
    data.update({"mode": mode, "module_identity": identity, "package_manifest": manifest,
                 "scratch_directory": str(scratch), "scratch_outside_checkout": True,
                 "provider_source_absent": False, "scratch_removed": False,
                 "runtime_import_library": str(runtime_library),
                 "fixture_sha256": _sha256(source)})
    scratch_outputs: dict[str, Path] = {}
    try:
        installed = {role: scratch / name for role, name in PACKAGE_FILENAMES.items()}
        expected_hashes = {row["role"]: row["sha256"] for row in manifest["files"]}
        for role, target in installed.items():
            shutil.copy2(files[role], target)
            if _sha256(target) != expected_hashes[role]:
                raise ValueError(f"installed native package {role} changed during copying")
            scratch_outputs[f"installed_{role}"] = target
        consumer_source = scratch / "consumer.m"
        shutil.copy2(source, consumer_source)
        scratch_outputs["consumer_source"] = consumer_source
        initial_names = sorted(path.name for path in scratch.iterdir())
        if initial_names != sorted([*PACKAGE_FILENAMES.values(), "consumer.m"]):
            raise ValueError("outside-checkout install contains undeclared package files")
        data.update({"provider_source_absent": True, "initial_scratch_files": initial_names,
                     "installed_package_files": list(PACKAGE_FILENAMES.values())})
        object_path = scratch / "consumer.obj"
        executable = scratch / "consumer.exe"
        dependency_path = scratch / "consumer-dependencies.json"
        scratch_outputs.update({"consumer_object": object_path, "executable": executable,
                                "dependency_record": dependency_path})
        compile_command = _compile_arguments(toolchain, consumer_source, object_path)
        compile_command += ["--module-search-path", str(scratch), "--import-module", MODULE,
                            identity["semantic_fingerprint"], PACKAGE,
                            "--emit-dependency-record", str(dependency_path)]
        if mode == "source-directed":
            compile_command += ["--source-imports", "--arc"]
        compile_command += _clang_arguments(toolchain)
        _execute(compile_command, phase="consumer-compile", cwd=scratch, output=output,
                 environment=_environment(toolchain), timeout=timeout, data=data, artifacts=artifacts)
        _file(object_path, "emitted consumer object")
        data["dependency_record"] = _dependency_record(dependency_path, identity, toolchain)
        linker = _tool(toolchain, "linker")
        linux = toolchain.get("platform") == "linux-x86_64"
        if not linux and linker.name.lower() != "lld-link.exe":
            raise ValueError("the native MSVC package profile requires the sealed lld-link.exe")
        link_command = [str(_tool(toolchain, "clangxx")), "--target=" + toolchain["target_triple"],
                        "-fms-runtime-lib=dll", "-B", str(linker.parent), "-fuse-ld=lld", str(installed["object"]),
                        str(object_path), str(runtime_library), "-o", str(executable)]
        if linux:
            link_command.remove("-fms-runtime-lib=dll")
            library_dir = scratch / "lib"
            library_dir.mkdir()
            installed_runtime = library_dir / runtime_library.name
            shutil.copy2(runtime_library, installed_runtime)
            link_command[link_command.index(str(runtime_library))] = str(installed_runtime)
            link_command += ["--driver-mode=g++", "-Wl,-rpath,$ORIGIN/lib"]
            data.update(runtime_in_package=True, runtime_sha256=_sha256(installed_runtime))
        _execute(link_command, phase="consumer-link", cwd=scratch, output=output,
                 environment=_environment(toolchain), timeout=timeout, data=data, artifacts=artifacts)
        _file(executable, "linked native package consumer")
        data["expected_exit_code"] = 0
        try:
            run_environment = _environment(toolchain, executable=True)
            if linux:
                run_environment.pop("LD_LIBRARY_PATH", None)
                run_environment.pop("LD_PRELOAD", None)
            _execute([str(executable)], phase="consumer-run", cwd=scratch, output=output,
                     environment=run_environment, timeout=timeout, data=data, artifacts=artifacts)
        finally:
            if data["commands"][-1]["phase"] == "consumer-run":
                data["actual_exit_code"] = data["commands"][-1]["exit_code"]
        if linux:
            _execute([str(_tool(toolchain, "readobj")), "--file-headers", "--needed-libs", str(executable)],
                     phase="consumer-elf", cwd=scratch, output=output,
                     environment=run_environment, timeout=timeout, data=data, artifacts=artifacts)
            elf = (output / "consumer-elf.stdout.log").read_text()
            if "elf64-x86-64" not in elf or installed_runtime.name not in elf:
                raise ValueError("installed Linux consumer has the wrong ELF ABI or runtime dependency")
            _execute([str(_tool(toolchain, "debugger")), "--batch", "-o", "settings set target.disable-aslr false",
                      "-o", "breakpoint set --name main", "-o", "run", "-o", "thread backtrace", "-o", "continue",
                      str(executable)], phase="consumer-debug", cwd=scratch, output=output,
                     environment=run_environment, timeout=timeout, data=data, artifacts=artifacts)
            debug = (output / "consumer-debug.stdout.log").read_text()
            if "stop reason = breakpoint" not in debug or "exited with status = 0" not in debug:
                raise ValueError("installed Linux consumer did not stop at main and exit under LLDB")
            data.update(elf_verified=True, debugger_verified=True, checkout_runtime_path_required=False)
        for role, path in installed.items():
            if _sha256(path) != expected_hashes[role]:
                raise ValueError(f"installed native package {role} changed during consumer execution")
        if sorted(path.name for path in scratch.rglob("*.m")) != ["consumer.m"]:
            data["provider_source_absent"] = False
            raise ValueError("consumer scratch acquired an undeclared source file")
        data.update({"expected_exit_code": 0, "actual_exit_code": 0, "runtime_count": 1})
    finally:
        # Preserve all available evidence, including partial object/dependency
        # outputs, before deleting precisely the directory allocated above.
        for role, path in scratch_outputs.items():
            if path.is_file() and not path.is_symlink():
                destination = captured / path.name
                shutil.copy2(path, destination)
                artifacts[role] = destination
        installed_manifest = output / "installed-package-manifest.json"
        _write_json(installed_manifest, manifest)
        artifacts["package_manifest"] = installed_manifest
        if (scratch.parent != scratch_parent or not scratch.name.startswith("objc3-native-package-")
                or scratch.is_relative_to(repo) or scratch.is_symlink() or scratch.resolve() != scratch):
            raise RuntimeError("refusing to clean an unverified native package scratch directory")
        shutil.rmtree(scratch)
        data["scratch_removed"] = True


def run_package_node(repo: Path, node: dict[str, Any], output_dir: Path,
                     toolchain: dict[str, Any], dependencies: dict[str, dict[str, Any]]) -> dict[str, Any]:
    """Execute a provider or one installed consumer; return durable artifacts."""
    repo = repo.resolve()
    output = output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    evidence = output / "package-node-evidence.json"
    if evidence.exists():
        raise ValueError("native package node output already contains evidence; choose a fresh output directory")
    artifacts: dict[str, Path] = {"evidence": evidence}
    data: dict[str, Any] = {"contract_id": EVIDENCE_CONTRACT,
                            "node_id": node.get("node_id", node.get("id")),
                            "operation": node.get("operation"), "commands": [], "runtime_count": 0,
                            "sdk_build_id": toolchain.get("sdk_build_id"), "build_identity": toolchain.get("build_identity"),
                            "status": "RUNNING"}
    try:
        if not isinstance(toolchain.get("sdk_build_id"), str) or not toolchain["sdk_build_id"]:
            raise ValueError("sealed sdk_build_id is required")
        if not isinstance(toolchain.get("build_identity"), str) or not toolchain["build_identity"]:
            raise ValueError("sealed build_identity is required")
        timeout = float(node.get("timeout_seconds", 120))
        if not math.isfinite(timeout) or timeout <= 0 or timeout > 86400:
            raise ValueError("package command timeout must be finite, positive and at most 24 hours")
        if node.get("operation") == "package-provider":
            _provider(repo, output, toolchain, timeout, data, artifacts)
        elif node.get("operation") == "package-consumer":
            _consumer(repo, node, output, toolchain, dependencies, timeout, data, artifacts)
        else:
            raise ValueError("unsupported native package node operation")
        data["status"] = "PASS"
    except Exception as error:
        data.update({"status": "FAIL", "error": str(error)})
        _write_json(evidence, data)
        raise PackageNodeError(str(error), data=data, artifacts=artifacts) from error
    _write_json(evidence, data)
    return {"artifacts": artifacts, "data": data}


__all__ = ["run_package_node", "PackageNodeError"]
