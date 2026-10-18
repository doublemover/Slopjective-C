"""Resolve one sealed native toolchain for an immutable native validation batch.

Only the coordinator calls these functions, before and after its batch. Workers
use the recorded absolute paths and child environment without rehashing tools.
"""

from __future__ import annotations

import hashlib
import json
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any

from scripts.objc3c_toolchain.core import ToolchainError, verify_development
from scripts.objc3c_toolchain.facade import facade_identity

SCHEMA = "objc3c.native-toolchain-snapshot.v1"
ENVIRONMENT_KEYS = (
    "PATH", "INCLUDE", "LIB", "LIBPATH", "VCToolsInstallDir", "VCToolsVersion",
    "WindowsSdkDir", "WindowsSDKVersion", "UniversalCRTSdkDir", "UCRTVersion",
    "VCINSTALLDIR", "VSINSTALLDIR", "VSCMD_ARG_TGT_ARCH", "VSCMD_ARG_HOST_ARCH",
    "SystemRoot", "SystemDrive", "TEMP", "TMP",
)


class NativeToolchainError(RuntimeError):
    """A missing, unsealed, unsupported, or changed native toolchain input."""


def _canonical(value: Any) -> bytes:
    return json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=True).encode("utf-8")


def _snapshot_digest(snapshot: dict[str, Any]) -> str:
    return hashlib.sha256(_canonical({key: value for key, value in snapshot.items()
                                     if key != "build_identity"})).hexdigest()


def _host_platform() -> str:
    if sys.platform == "linux" and platform.machine().lower() in {"amd64", "x86_64"}:
        return "linux-x86_64"
    if os.name != "nt" or platform.machine().lower() not in {"amd64", "x86_64"}:
        raise NativeToolchainError("native validation currently admits Windows x86-64 only")
    return "windows-x86_64"


def _required_file(path: Path, label: str) -> Path:
    path = path.resolve()
    if not path.is_file():
        raise NativeToolchainError(f"missing {label}: {path}")
    return path


def _run(command: list[str], *, environment: dict[str, str] | None = None) -> str:
    try:
        options = {"env": environment, "timeout": 30} if environment is not None else {}
        result = subprocess.run(command, check=False, capture_output=True, text=True,
                                creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0), **options)
    except (OSError, subprocess.TimeoutExpired) as exc:
        raise NativeToolchainError(f"cannot run {command[0]}: {exc}") from exc
    if result.returncode:
        raise NativeToolchainError(f"native toolchain admission failed ({result.returncode}): "
                                   f"{result.stderr.strip() or result.stdout.strip()}")
    return result.stdout.strip()


def _capture_environment(profile: dict[str, Any]) -> dict[str, str]:
    """Enter DevShell in a private process when the parent has no admitted shell."""
    current = {key: os.environ[key] for key in ENVIRONMENT_KEYS if key in os.environ}
    if (current.get("VCToolsVersion", "").rstrip("\\/") == profile["v143_toolset_version"] and
            current.get("WindowsSDKVersion", "").rstrip("\\/") == profile["windows_sdk_version"] and
            all(current.get(key) for key in ("VCToolsInstallDir", "WindowsSdkDir", "INCLUDE", "LIB"))):
        return current
    programs = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
    vswhere = _required_file(programs / "Microsoft Visual Studio/Installer/vswhere.exe", "VS2022 discovery tool")
    installation = _run([str(vswhere), "-latest", "-version", "[17.0,18.0)", "-products", "*",
                         "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"])
    if not installation:
        raise NativeToolchainError("the admitted Visual Studio 2022 v143 installation is unavailable")
    module = _required_file(Path(installation) / "Common7/Tools/Microsoft.VisualStudio.DevShell.dll", "VS2022 DevShell module")
    powershell = shutil.which("pwsh") or str(Path(os.environ.get("SystemRoot", r"C:\Windows")) /
                                               "System32/WindowsPowerShell/v1.0/powershell.exe")
    keys = ",".join("'" + key + "'" for key in ENVIRONMENT_KEYS)
    quoted_module = str(module).replace("'", "''")
    quoted_installation = installation.replace("'", "''")
    arguments = ("-arch=x64 -host_arch=x64 -winsdk=" + profile["windows_sdk_version"] +
                 " -vcvars_ver=" + profile["v143_toolset_version"]).replace("'", "''")
    script = ("$ErrorActionPreference='Stop'; Import-Module '" + quoted_module +
              "'; Enter-VsDevShell -VsInstallPath '" + quoted_installation +
              "' -SkipAutomaticLocation -DevCmdArguments '" + arguments + "' | Out-Null; "
              "$selected=@{}; foreach($key in @(" + keys + ")) { "
              "$value=[Environment]::GetEnvironmentVariable($key); "
              "if($null -ne $value) {$selected[$key]=$value} }; "
              "Write-Output ('OBJC3_ENV_JSON:' + ($selected | ConvertTo-Json -Compress))")
    output = _run([powershell, "-NoLogo", "-NoProfile", "-NonInteractive", "-Command", script])
    for line in reversed(output.splitlines()):
        if line.startswith("OBJC3_ENV_JSON:"):
            captured = json.loads(line.removeprefix("OBJC3_ENV_JSON:"))
            if isinstance(captured, dict) and all(isinstance(value, str) for value in captured.values()):
                return captured
    raise NativeToolchainError("private DevShell did not return an admitted child environment")


def _environment_directories(environment: dict[str, str], key: str) -> list[str]:
    directories = sorted({str(Path(item).resolve()) for item in environment.get(key, "").split(";") if item})
    if not directories or any(not Path(item).is_dir() for item in directories):
        raise NativeToolchainError(f"admitted {key} must select existing native input directories")
    return directories


def _inventory(root: Path, selection: str) -> list[dict[str, str]]:
    if not root.is_dir():
        raise NativeToolchainError(f"missing native input directory: {root}")
    entries = []
    for path in root.rglob("*"):
        if not path.is_file() or ".git" in path.relative_to(root).parts:
            continue
        if selection == "headers" and path.suffix.lower() not in {".h", ".hpp", ".inc"}:
            continue
        if selection == "libraries" and path.suffix.lower() != ".lib":
            continue
        entries.append({"relative_path": path.relative_to(root).as_posix(), "path": str(path.resolve())})
    return sorted(entries, key=lambda entry: entry["relative_path"])


class _Inputs:
    def __init__(self) -> None:
        self.files: dict[str, str] = {}
        self.trees: dict[tuple[str, str], dict[str, Any]] = {}
        self.cache: dict[tuple[Any, ...], tuple[str, dict[str, bool]]] = {}

    def add_file(self, path: Path, label: str, markers: tuple[str, ...] = ()) -> str:
        path = _required_file(path, label)
        metadata = path.stat()
        key = (metadata.st_dev, metadata.st_ino, metadata.st_size, metadata.st_mtime_ns,
               str(path) if not metadata.st_ino else "")
        cached = self.cache.get(key)
        if cached is None or any(marker not in cached[1] for marker in markers):
            digest = hashlib.sha256()
            marker_bytes = {marker: marker.encode("utf-8") for marker in markers}
            found = {marker: False for marker in markers}
            overlap = max((len(marker) for marker in marker_bytes.values()), default=1) - 1
            tail = b""
            try:
                with path.open("rb") as stream:
                    for block in iter(lambda: stream.read(1024 * 1024), b""):
                        digest.update(block)
                        scanned = tail + block
                        for marker, spelling in marker_bytes.items():
                            found[marker] |= spelling in scanned
                        tail = scanned[-overlap:] if overlap else b""
            except OSError as exc:
                raise NativeToolchainError(f"cannot read native input {path}: {exc}") from exc
            cached = (digest.hexdigest(), found)
            self.cache[key] = cached
        for marker in markers:
            if not cached[1][marker]:
                raise NativeToolchainError(f"stale or unsealed {label}: {path}; missing sealed marker {marker}")
        self.files[str(path)] = cached[0]
        return cached[0]

    def add_tree(self, root: Path, selection: str = "all") -> dict[str, Any]:
        root = root.resolve()
        key = (str(root), selection)
        if key not in self.trees:
            entries = _inventory(root, selection)
            if not entries:
                raise NativeToolchainError(f"native input directory has no selected {selection}: {root}")
            for entry in entries:
                self.add_file(Path(entry["path"]), "native tree input")
            self.trees[key] = {"path": str(root), "selection": selection, "files": entries}
        return self.trees[key]


def _resource_digest(tree: dict[str, Any], files: dict[str, str]) -> str:
    # Match Objc3ClangFoundation.cmake's existing compiled resource identity.
    lines = (f"{len(entry['relative_path'].encode('utf-8'))}:{entry['relative_path']}"
             f"{files[entry['path']]}\n" for entry in tree["files"])
    return hashlib.sha256("".join(lines).encode("utf-8")).hexdigest()


def _verify_seal(repo: Path) -> dict[str, Any]:
    try:
        return verify_development(repo)
    except (ToolchainError, OSError) as exc:
        raise NativeToolchainError(f"development compiler source seal rejected: {exc}") from exc


def _tool_version(path: Path, label: str, upstream: str, marker: str = "", *,
                  environment: dict[str, str] | None = None) -> str:
    _required_file(path, label)
    version = (_run([str(path), "--version"], environment=environment)
               if environment is not None else _run([str(path), "--version"]))
    # LLVM composes the prefix and digest from separate string literals; the
    # observable version marker need not occur contiguously inside the binary.
    if upstream not in version or (marker and marker not in version):
        raise NativeToolchainError(f"stale or unsealed {label}: {path}; actual version: {version.splitlines()[0] if version else '<empty>'}")
    return version


def resolve_toolchain(repo: Path, *, require_editor: bool = False) -> dict[str, Any]:
    """Hash the actual admitted inputs once and return a JSON-serializable snapshot."""
    repo = repo.resolve()
    selected_platform = _host_platform()
    if selected_platform == "linux-x86_64":
        from .identity_linux import resolve_linux_toolchain
        return resolve_linux_toolchain(repo, require_editor=require_editor)
    seal = _verify_seal(repo)
    manifest_path = repo / "toolchains/llvm/manifest.json"
    profile_path = repo / "scripts/objc3c_toolchain_runtime/runtime-profile.json"
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        profile = json.loads(profile_path.read_text(encoding="utf-8"))["profiles"][selected_platform]
    except (OSError, ValueError, KeyError) as exc:
        raise NativeToolchainError(f"cannot read admitted native toolchain configuration: {exc}") from exc
    if profile["target_triple"] != "x86_64-pc-windows-msvc":
        raise NativeToolchainError("runtime profile does not select the admitted Windows x86-64 target")
    environment = _capture_environment(profile)
    if (environment.get("VCToolsVersion", "").rstrip("\\/") != profile["v143_toolset_version"] or
            environment.get("WindowsSDKVersion", "").rstrip("\\/") != profile["windows_sdk_version"]):
        raise NativeToolchainError("captured DevShell differs from the admitted v143/Windows SDK versions")
    include_dirs = _environment_directories(environment, "INCLUDE")
    lib_dirs = _environment_directories(environment, "LIB")
    inputs = _Inputs()
    for path in (manifest_path, profile_path, repo / manifest["patch_series"],
                 repo / "out/toolchains/objc3-development/src/.objc3c-prepared-source.json"):
        inputs.add_file(path, "compiler source seal input")
    for patch in seal["patches"]:
        inputs.add_file((repo / manifest["patch_series"]).parent / patch["file"], "locked LLVM patch")
    sdk = repo / "out/toolchains/objc3-development/build"
    inputs.add_file(sdk / "tools/clang/include/clang/Basic/ObjC3Foundation.h", "compiled SDK identity header",
                    (seal["patch_digest"], seal["upstream_commit"]))
    resource_dir = sdk / "lib/clang/23"
    _required_file(resource_dir / "include/stddef.h", "matching Clang resource headers")
    resources = inputs.add_tree(resource_dir)
    resource_sha256 = _resource_digest(resources, inputs.files)
    marker = "ObjC3-patches=" + seal["patch_digest"]
    tools: dict[str, str] = {}
    versions: dict[str, str] = {}
    for name, filename in {"clang": "clang.exe", "clangxx": "clang++.exe", "clang_cl": "clang-cl.exe"}.items():
        path = (sdk / "bin" / filename).resolve()
        versions[name] = _tool_version(path, f"{name} executable", seal["upstream_commit"], marker)
        inputs.add_file(path, f"{name} executable")
        tools[name] = str(path)
    linker = (sdk / "bin/lld-link.exe").resolve()
    versions["linker"] = _tool_version(linker, "LLD linker", seal["upstream_commit"])
    inputs.add_file(linker, "sealed LLD linker")
    tools["linker"] = str(linker)
    facade = (repo / "out/toolchains/objc3-facade/build/src/clang_foundation/objc3c-clang.exe").resolve()
    inputs.add_file(facade, "native facade executable", (marker, seal["upstream_commit"],
                    inputs.files[tools["clang"]], resource_sha256, facade_identity(repo)["sha256"]))
    tools["facade"] = str(facade)
    editors = {}
    for name, filename in {"clangd": "clangd.exe", "clang_format": "clang-format.exe"}.items():
        path = (sdk / "bin" / filename).resolve()
        try:
            version = _tool_version(path, f"editor {name}", seal["upstream_commit"], marker)
            inputs.add_file(path, f"sealed editor {name}")
        except NativeToolchainError as exc:
            editors[name] = {"status": "unavailable", "reason": str(exc)}
            if require_editor:
                raise NativeToolchainError(f"required editor tool rejected: {exc}") from exc
        else:
            tools[name] = str(path)
            versions[name] = version
            editors[name] = {"status": "sealed"}
    runtime_source = (repo / "out/toolchains/libobjc2-v2.3/src").resolve()
    robin_source = (repo / "out/toolchains/robin-map-v1.4.0/src").resolve()
    runtime_stage = (repo / "out/toolchains/libobjc2-v2.3/build/stage").resolve()
    for source, expected in ((runtime_source, profile["runtime_commit"]),
                             (robin_source, profile["robin_map_commit"])):
        if _run(["git", "-C", str(source), "rev-parse", "HEAD"]) != expected:
            raise NativeToolchainError(f"native runtime source differs from its admitted commit: {source}")
    _required_file(runtime_source / "objc/objc.h", "libobjc2 runtime source headers")
    _required_file(runtime_stage / "objc/objc-config.h", "libobjc2 generated runtime headers")
    inputs.add_tree(runtime_source, "headers")
    inputs.add_tree(runtime_stage / "objc")
    runtime_dll = runtime_stage / "objc.dll"
    runtime_import = runtime_stage / "objc.lib"
    inputs.add_file(runtime_dll, "native libobjc2 DLL")
    inputs.add_file(runtime_import, "native libobjc2 import library")
    for directory in include_dirs:
        inputs.add_tree(Path(directory))
    for directory in lib_dirs:
        inputs.add_tree(Path(directory), "libraries")
    crt_dlls = {}
    system = Path(environment.get("SystemRoot", r"C:\Windows")) / "System32"
    search = [sdk / "bin", system, *[Path(item) for item in environment.get("PATH", "").split(";") if item]]
    for filename in ("ucrtbase.dll", "vcruntime140.dll", "vcruntime140_1.dll", "msvcp140.dll"):
        actual = next((directory / filename for directory in search if (directory / filename).is_file()), None)
        if actual is None:
            raise NativeToolchainError(f"selected Windows CRT runtime DLL is missing: {filename}")
        actual = actual.resolve()
        inputs.add_file(actual, "selected Windows CRT DLL")
        crt_dlls[filename] = str(actual)
    snapshot = {
        "schema": SCHEMA, "schema_version": 1, "repo": str(repo), "platform": selected_platform,
        "target_triple": profile["target_triple"], "sdk_build_id": seal["build_id"],
        "patch_digest": seal["patch_digest"], "source_seal": seal, "tools": tools, "tool_versions": versions,
        "resource_dir": str(resource_dir.resolve()), "resource_sha256": resource_sha256,
        "editor_tools": editors, "environment": environment,
        "platform_inputs": {"include_dirs": include_dirs, "lib_dirs": lib_dirs},
        "runtime": {"source": str(runtime_source), "include_dirs": [str(runtime_source), str(runtime_stage / "objc")],
                    "lib_dir": str(runtime_stage), "bin_dir": str(runtime_stage), "dll": str(runtime_dll),
                    "import_library": str(runtime_import), "crt_dlls": crt_dlls,
                    "source_commit": profile["runtime_commit"], "robin_map_source": str(robin_source),
                    "robin_map_commit": profile["robin_map_commit"]},
        "input_files": [{"path": path, "sha256": digest} for path, digest in sorted(inputs.files.items())],
        "input_trees": [tree for _, tree in sorted(inputs.trees.items())],
    }
    snapshot["build_identity"] = _snapshot_digest(snapshot)
    return snapshot


def verify_toolchain_snapshot(snapshot: dict) -> None:
    """Rehash once after the batch; reject file, membership, seal, or metadata drift."""
    if (snapshot.get("schema") != SCHEMA or snapshot.get("schema_version") != 1 or
            snapshot.get("build_identity") != _snapshot_digest(snapshot)):
        raise NativeToolchainError("native toolchain snapshot metadata or build identity changed")
    inputs = _Inputs()
    for tree in snapshot["input_trees"]:
        if _inventory(Path(tree["path"]), tree["selection"]) != tree["files"]:
            raise NativeToolchainError(f"native input tree membership changed during validation: {tree['path']}")
    for entry in snapshot["input_files"]:
        if inputs.add_file(Path(entry["path"]), "snapshotted native input") != entry["sha256"]:
            raise NativeToolchainError(f"native toolchain input changed during validation: {entry['path']}")
    if _verify_seal(Path(snapshot["repo"])) != snapshot["source_seal"]:
        raise NativeToolchainError("development source seal changed during native validation")
    for path_key, commit_key in (("source", "source_commit"), ("robin_map_source", "robin_map_commit")):
        if _run(["git", "-C", snapshot["runtime"][path_key], "rev-parse", "HEAD"]) != snapshot["runtime"][commit_key]:
            raise NativeToolchainError(f"native runtime source revision changed during validation: {snapshot['runtime'][path_key]}")
