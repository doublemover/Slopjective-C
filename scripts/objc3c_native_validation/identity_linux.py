"""Admit the sealed native Linux SDK and pinned Objective-C runtime."""
from __future__ import annotations

import json
import os
from pathlib import Path

from scripts.objc3c_toolchain.facade import facade_identity

from .identity import (
    SCHEMA, NativeToolchainError, _Inputs, _required_file, _resource_digest,
    _run, _snapshot_digest, _tool_version, _verify_seal,
)


def resolve_linux_toolchain(repo: Path, *, require_editor: bool = False) -> dict:
    seal = _verify_seal(repo)
    manifest_path = repo / "toolchains/llvm/manifest.json"
    profile_path = repo / "scripts/objc3c_toolchain_runtime/runtime-profile.json"
    manifest = json.loads(manifest_path.read_text())
    profile = json.loads(profile_path.read_text())["profiles"]["linux-x86_64"]
    if profile["target_triple"] != "x86_64-unknown-linux-gnu":
        raise NativeToolchainError("Linux runtime profile has an unsupported target")
    sdk = Path(os.environ.get("OBJC3C_CLANG_FOUNDATION_ROOT", str(repo / "out/toolchains/objc3-development/stage-linux-x86_64"))).resolve()
    runtime_source = repo / "out/toolchains/libobjc2-v2.3/src"
    robin_source = repo / f"out/toolchains/robin-map-{profile['robin_map_release']}/src"
    stage = repo / "out/toolchains/libobjc2-v2.3/build-linux-x86_64/stage"
    installed_runtime = (sdk / "lib/libobjc.so").is_file() and (sdk / "include/objc/objc-config.h").is_file()
    if not installed_runtime:
        raise NativeToolchainError('a complete installed runtime is required; run build-sdk')
    stage = sdk
    installed_profile = sdk / 'share/objc3c/runtime/runtime-profile.json'
    if json.loads(installed_profile.read_text())['profiles']['linux-x86_64'] != profile:
        raise NativeToolchainError('installed runtime profile differs from the current qualified pin')
    environment = {key: os.environ[key] for key in ("PATH", "LANG", "LC_ALL", "TMPDIR") if key in os.environ}
    environment["LD_LIBRARY_PATH"] = os.pathsep.join((str(sdk / "lib"), str(stage / "lib")))
    environment["PYTHONDONTWRITEBYTECODE"] = "1"
    admission_environment = {**os.environ, **environment}
    inputs = _Inputs()
    inputs.add_file(installed_profile, "installed runtime profile")
    for path in (manifest_path, profile_path, repo / manifest["patch_series"],
                 repo / "out/toolchains/objc3-development/src/.objc3c-prepared-source.json"):
        inputs.add_file(path, "compiler source seal input")
    for patch in seal["patches"]:
        inputs.add_file((repo / manifest["patch_series"]).parent / patch["file"], "locked LLVM patch")
    inputs.add_file(sdk / "include/clang/Basic/ObjC3Foundation.h", "compiled SDK identity header",
                    (seal["patch_digest"], seal["upstream_commit"]))
    major = manifest["upstream"]["tag"].removeprefix("llvmorg-").split(".")[0]
    resource_dir = sdk / "lib/clang" / major
    _required_file(resource_dir / "include/stddef.h", "matching Clang resource headers")
    resource_sha = _resource_digest(inputs.add_tree(resource_dir), inputs.files)
    marker = "ObjC3-patches=" + seal["patch_digest"]
    tools, versions, editors = {}, {}, {}
    names = {"clang": "clang", "clangxx": "clang++", "linker": "ld.lld",
             "clangd": "clangd", "clang_format": "clang-format"}
    for name, filename in names.items():
        path = (sdk / "bin" / filename).resolve()
        editor = name in {"clangd", "clang_format"}
        try:
            version = _tool_version(sdk / "bin" / filename, name, seal["upstream_commit"],
                                    marker if name != "linker" else "", environment=admission_environment)
            inputs.add_file(path, name)
        except NativeToolchainError as error:
            if not editor or require_editor:
                raise
            editors[name] = {"status": "unavailable", "reason": str(error)}
        else:
            tools[name] = str(path)
            versions[name] = version
            if editor:
                editors[name] = {"status": "sealed"}
    expected_version = manifest["upstream"]["tag"].removeprefix("llvmorg-")
    for name, filename in (("readobj", "llvm-readobj"), ("dwarfdump", "llvm-dwarfdump"),
                           ("debugger", "lldb"), ("scan_deps", "clang-scan-deps")):
        path = (sdk / "bin" / filename).resolve()
        version = _run([str(path), "--version"], environment=admission_environment)
        if expected_version not in version:
            raise NativeToolchainError(f"{filename} differs from the pinned LLVM version")
        inputs.add_file(path, name)
        tools[name], versions[name] = str(path), version
    facade = (repo / "out/toolchains/objc3-facade/build-linux-x86_64/src/clang_foundation/objc3c").resolve()
    if (sdk / "bin/objc3c").is_file():
        facade = (sdk / "bin/objc3c").resolve()
    inputs.add_file(facade, "native facade executable", (marker, seal["upstream_commit"],
                    inputs.files[tools["clang"]], resource_sha, facade_identity(repo)["sha256"]))
    tools["facade"] = str(facade)
    for source, expected in ((runtime_source, profile["runtime_commit"]),
                             (robin_source, profile["robin_map_commit"])):
        if _run(["git", "-C", str(source), "rev-parse", "HEAD"]) != expected:
            raise NativeToolchainError(f"native runtime dependency differs from its pinned revision: {source}")
    _required_file(stage / "include/objc/objc-config.h", "generated native runtime headers")
    if not installed_runtime:
        inputs.add_tree(runtime_source, "headers")
    inputs.add_tree(stage / "include/objc")
    library = (stage / "lib/libobjc.so").resolve()
    inputs.add_file(library, "native libobjc2 shared library")
    # Include the actual system headers and loader dependencies in the batch
    # identity. Compiler binaries alone do not identify a Linux development ABI.
    include_dirs = [str(Path("/usr/include").resolve())]
    for directory in include_dirs:
        inputs.add_tree(Path(directory))
    inputs.add_file(Path("/etc/os-release"), "Linux distribution ABI")
    for binary in {*(Path(path) for path in tools.values()), facade, library}:
        for word in _run(["ldd", str(binary)], environment=admission_environment).split():
            candidate = Path(word)
            if candidate.is_absolute() and candidate.is_file():
                inputs.add_file(candidate.resolve(), "Linux loader dependency")
    snapshot = {"schema": SCHEMA, "schema_version": 1, "repo": str(repo),
                "platform": "linux-x86_64", "target_triple": profile["target_triple"],
                "sdk_build_id": seal["build_id"], "patch_digest": seal["patch_digest"],
                "source_seal": seal, "tools": tools, "tool_versions": versions,
                "resource_dir": str(resource_dir.resolve()), "resource_sha256": resource_sha,
                "editor_tools": editors, "environment": environment,
                "platform_inputs": {"include_dirs": include_dirs, "lib_dirs": []},
                "runtime": {"source": str(runtime_source),
                    "include_dirs": ([str(stage / "include"), str(stage / "include/objc")]
                                     if installed_runtime else [str(runtime_source), str(stage / "include/objc")]),
                    "lib_dir": str(stage / "lib"), "bin_dir": str(stage / "lib"),
                    "dll": str(library), "import_library": str(library), "crt_dlls": {},
                    "source_commit": profile["runtime_commit"], "robin_map_source": str(robin_source),
                    "robin_map_commit": profile["robin_map_commit"]},
                "input_files": [{"path": path, "sha256": digest} for path, digest in sorted(inputs.files.items())],
                "input_trees": [tree for _, tree in sorted(inputs.trees.items())]}
    snapshot["build_identity"] = _snapshot_digest(snapshot)
    return snapshot
