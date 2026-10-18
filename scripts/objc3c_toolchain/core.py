"""Safe preparation and verification for the repository-owned LLVM patch stack."""

from __future__ import annotations

import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any


class ToolchainError(RuntimeError):
    """An invalid manifest, source tree, or failed preparation operation."""


PINNED_UPSTREAM = {
    "tag": "llvmorg-23.1.3",
    "tag_object": "856f0005e57023e63664c560a829c620360ea1f1",
    "commit": "0d261d1ca552c95a8f007e061c787ac7132fbcbc",
}


def _run(args: list[str], *, cwd: Path | None = None) -> str:
    result = subprocess.run(args, cwd=cwd, text=True, capture_output=True, check=False)
    if result.returncode:
        detail = result.stderr.strip() or result.stdout.strip()
        raise ToolchainError(f"command failed ({result.returncode}): {' '.join(args)}\n{detail}")
    return result.stdout.strip()


def _git(args: list[str], *, cwd: Path, index_path: Path) -> str:
    env = os.environ.copy()
    env["GIT_INDEX_FILE"] = str(index_path)
    result = subprocess.run(["git", *args], cwd=cwd, env=env, text=True,
                            capture_output=True, check=False)
    if result.returncode:
        detail = result.stderr.strip() or result.stdout.strip()
        raise ToolchainError(f"git command failed ({result.returncode}): {' '.join(args)}\n{detail}")
    return result.stdout.strip()


def _git_apply_index_patch(patch: Path, *, cwd: Path, index_path: Path) -> None:
    env = os.environ.copy()
    env["GIT_INDEX_FILE"] = str(index_path)
    patch_bytes = patch.read_bytes().replace(b"\r\n", b"\n")
    result = subprocess.run(["git", "apply", "--cached", "--whitespace=nowarn"],
                            cwd=cwd, env=env, input=patch_bytes, capture_output=True, check=False)
    if result.returncode:
        detail = (result.stderr or result.stdout).decode(errors="replace").strip()
        raise ToolchainError(f"cannot apply locked patch {patch.name} to temporary index: {detail}")


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def _tree_digest(root: Path) -> str:
    digest = hashlib.sha256()
    for path in sorted(p for p in root.rglob("*") if p.is_file() and ".git" not in p.parts
                        and p.name != ".objc3c-prepared-source.json"):
        relative = path.relative_to(root).as_posix().encode("utf-8")
        digest.update(len(relative).to_bytes(8, "big"))
        digest.update(relative)
        digest.update(bytes.fromhex(_sha256(path)))
    return digest.hexdigest()


def _load_manifest(path: Path) -> tuple[dict[str, Any], Path]:
    path = path.resolve()
    try:
        manifest = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ToolchainError(f"cannot read manifest {path}: {exc}") from exc
    base = path.parents[2] if path.parent.name == "llvm" and path.parent.parent.name == "toolchains" else path.parent
    upstream = manifest.get("upstream", {})
    for key, value in PINNED_UPSTREAM.items():
        if upstream.get(key) != value:
            raise ToolchainError(f"manifest upstream.{key} must be pinned to {value}")
    return manifest, base


def _series(manifest: dict[str, Any], repo: Path, base: Path, *, pristine: bool) -> list[tuple[str, Path, str]]:
    series_path = (base / manifest.get("patch_series", "")).resolve()
    if not series_path.is_relative_to(base.resolve()):
        raise ToolchainError("patch_series must remain inside the repository")
    try:
        entries = [line.strip() for line in series_path.read_text(encoding="utf-8").splitlines()
                   if line.strip() and not line.lstrip().startswith("#")]
    except OSError as exc:
        raise ToolchainError(f"cannot read patch series {series_path}: {exc}") from exc
    locked = manifest.get("patches")
    if not isinstance(locked, list):
        raise ToolchainError("manifest patches must be an ordered array")
    if entries != [entry.get("file") for entry in locked]:
        raise ToolchainError("patch series order does not match the locked manifest patch list")
    if not entries and not pristine:
        raise ToolchainError("patch series is empty; pass --pristine-baseline for the upstream-only source")
    patches: list[tuple[str, Path, str]] = []
    seen: set[str] = set()
    for entry in locked:
        name = entry["file"]
        if Path(name).name != name or name in seen:
            raise ToolchainError(f"invalid or duplicate patch filename: {name}")
        seen.add(name)
        patch_path = (series_path.parent / name).resolve()
        if not patch_path.is_relative_to(series_path.parent.resolve()) or not patch_path.is_file():
            raise ToolchainError(f"patch file is missing or escapes patch directory: {name}")
        actual = _sha256(patch_path)
        if entry.get("sha256") != actual:
            raise ToolchainError(f"patch hash mismatch for {name}: expected {entry.get('sha256')}, got {actual}")
        patches.append((name, patch_path, actual))
    return patches


def _identity(manifest: dict[str, Any], patches: list[tuple[str, Path, str]], pristine: bool) -> dict[str, Any]:
    upstream = manifest["upstream"]
    patch_rows = [{"file": name, "sha256": digest} for name, _, digest in patches]
    patch_digest = hashlib.sha256(json.dumps(patch_rows, sort_keys=True, separators=(",", ":")).encode()).hexdigest()
    build_digest = hashlib.sha256(json.dumps(manifest["build"], sort_keys=True, separators=(",", ":")).encode()).hexdigest()
    if pristine:
        build_id = f"{manifest['pristine_baseline_build_id']}-{build_digest[:8]}"
    else:
        build_id = f"{upstream['tag']}-{upstream['commit'][:12]}-{patch_digest[:12]}-{build_digest[:8]}"
    return {
        "schema_version": 1,
        "build_id": build_id,
        "upstream_tag": upstream["tag"],
        "upstream_tag_object": upstream["tag_object"],
        "upstream_commit": upstream["commit"],
        "patch_digest": patch_digest,
        "build_config_digest": build_digest,
        "patches": patch_rows,
        "pristine_baseline": pristine,
        "build": manifest["build"],
    }


def _identity_path(source: Path) -> Path:
    return source / ".objc3c-prepared-source.json"


def _development_tree(source: Path, commit: str,
                      patches: list[tuple[str, Path, str]]) -> tuple[str, str]:
    """Compare actual working files with the locked patches using temporary indexes only."""
    actual_fd, actual_name = tempfile.mkstemp(prefix="objc3c-actual-index-")
    expected_fd, expected_name = tempfile.mkstemp(prefix="objc3c-expected-index-")
    os.close(actual_fd)
    os.close(expected_fd)
    actual_index = Path(actual_name)
    expected_index = Path(expected_name)
    try:
        _git(["read-tree", commit], cwd=source, index_path=expected_index)
        for _, patch, _ in patches:
            _git_apply_index_patch(patch, cwd=source, index_path=expected_index)
        expected_tree = _git(["write-tree"], cwd=source, index_path=expected_index)

        checkout_index = Path(_run(["git", "rev-parse", "--git-path", "index"], cwd=source))
        if not checkout_index.is_absolute():
            checkout_index = source / checkout_index
        if checkout_index.is_file():
            shutil.copyfile(checkout_index, actual_index)
        else:
            _git(["read-tree", commit], cwd=source, index_path=actual_index)
        _git(["add", "--force", "-A", "--", ".", ":(exclude).objc3c-prepared-source.json"],
             cwd=source, index_path=actual_index)
        actual_tree = _git(["write-tree"], cwd=source, index_path=actual_index)
        return expected_tree, actual_tree
    finally:
        actual_index.unlink(missing_ok=True)
        expected_index.unlink(missing_ok=True)


def _development_identity(source: Path, repo: Path, manifest_path: Path) -> tuple[dict[str, Any], dict[str, Any], list[tuple[str, Path, str]], str]:
    manifest, base = _load_manifest(manifest_path)
    patches = _series(manifest, repo, base, pristine=False)
    expected = _identity(manifest, patches, False)
    if not source.is_relative_to((repo / "out" / "toolchains").resolve()):
        raise ToolchainError("development source must remain under repo/out/toolchains")
    head = _run(["git", "rev-parse", "HEAD"], cwd=source)
    if head != expected["upstream_commit"]:
        raise ToolchainError(f"development source has wrong base {head}; expected {expected['upstream_commit']}")
    expected_tree, actual_tree = _development_tree(source, expected["upstream_commit"], patches)
    if actual_tree != expected_tree:
        index_fd, index_name = tempfile.mkstemp(prefix="objc3c-diff-index-")
        os.close(index_fd)
        try:
            paths = _git(["diff", "--name-status", expected_tree, actual_tree], cwd=source,
                         index_path=Path(index_name))
        finally:
            Path(index_name).unlink(missing_ok=True)
        raise ToolchainError(
            "development source differs from the locked patch series "
            f"(expected tree {expected_tree}, actual tree {actual_tree}); "
            f"mismatched paths: {paths or '<tree metadata>'}; preserving source"
        )
    return manifest, expected, patches, expected_tree


def seal_development(repo: Path, *, manifest_path: Path | None = None,
                     source_dir: Path | None = None) -> dict[str, Any]:
    repo = repo.resolve()
    manifest_path = manifest_path or repo / "toolchains" / "llvm" / "manifest.json"
    source = (source_dir or repo / "out" / "toolchains" / "objc3-development" / "src").resolve()
    _, expected, _, tree = _development_identity(source, repo, manifest_path)
    sealed = {**expected, "source_kind": "development", "tree_sha256": tree}
    _identity_path(source).write_text(json.dumps(sealed, indent=2) + "\n", encoding="utf-8")
    return sealed


def verify_development(repo: Path, *, manifest_path: Path | None = None,
                       source_dir: Path | None = None) -> dict[str, Any]:
    repo = repo.resolve()
    manifest_path = manifest_path or repo / "toolchains" / "llvm" / "manifest.json"
    source = (source_dir or repo / "out" / "toolchains" / "objc3-development" / "src").resolve()
    _, expected, _, tree = _development_identity(source, repo, manifest_path)
    try:
        recorded = json.loads(_identity_path(source).read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ToolchainError(f"development source has not been sealed: {exc}") from exc
    if recorded.get("source_kind") != "development" or recorded.get("tree_sha256") != tree:
        raise ToolchainError("development seal is missing or does not match the current source tree")
    if any(recorded.get(key) != value for key, value in expected.items()):
        raise ToolchainError("development seal does not match current upstream, patches, or build configuration")
    return recorded


def _windows_build_environment(manifest: dict[str, Any]) -> dict[str, Any]:
    if os.name != "nt":
        raise ToolchainError("the pinned foundation preset currently targets Windows x86-64 only")
    program_files_x86 = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
    vswhere = program_files_x86 / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    if not vswhere.is_file():
        raise ToolchainError(f"Visual Studio discovery tool is missing: {vswhere}")
    # A newer Visual Studio installation must not silently replace the
    # manifest's VS 2022/v143 toolchain or invalidate an existing Ninja graph.
    installation = _run([str(vswhere), "-latest", "-version", "[17.0,18.0)",
                            "-products", "*", "-requires",
                            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"])
    if not installation:
        raise ToolchainError("Visual Studio 2022 v143 with the x64 C++ toolset is not installed")
    vs_install = Path(installation)
    if not vs_install.is_dir():
        raise ToolchainError("Visual Studio 2022 v143 with the x64 C++ toolset is not installed")
    version_file = vs_install / "VC" / "Auxiliary" / "Build" / "Microsoft.VCToolsVersion.default.txt"
    try:
        toolset_version = version_file.read_text(encoding="utf-8").strip()
    except OSError as exc:
        raise ToolchainError(f"cannot read MSVC toolset version: {exc}") from exc
    toolset = vs_install / "VC" / "Tools" / "MSVC" / toolset_version
    cl = toolset / "bin" / "Hostx64" / "x64" / "cl.exe"
    link = toolset / "bin" / "Hostx64" / "x64" / "link.exe"
    if not cl.is_file() or not link.is_file():
        raise ToolchainError(f"MSVC v143 x64 compiler/linker are missing under {toolset}")
    sdk = manifest["build"]["windows_sdk_version"]
    kits = program_files_x86 / "Windows Kits" / "10"
    sdk_include = kits / "Include" / sdk
    sdk_lib = kits / "Lib" / sdk
    if not sdk_include.is_dir() or not sdk_lib.is_dir():
        raise ToolchainError(f"pinned Windows SDK {sdk} is not installed under {kits}")
    cmake = shutil.which("cmake")
    ninja = shutil.which("ninja")
    if not cmake or not ninja:
        raise ToolchainError("CMake and Ninja must be available on PATH")
    cmake_path = Path(cmake).resolve()
    ninja_path = Path(ninja).resolve()
    python_executable = Path(os.environ.get("OBJC3C_TOOLCHAIN_PYTHON", sys.executable)).resolve()
    python_version = _run([str(python_executable), "--version"])
    required_python = manifest.get("host_tools", {}).get("python", "3.15.0")
    if python_version != f"Python {required_python}":
        raise ToolchainError(f"Python {required_python} is required for the LLVM build, found {python_version}")
    sdk_inputs = {}
    for name, path in {
        "windows_header": sdk_include / "um" / "Windows.h",
        "ucrt_header": sdk_include / "ucrt" / "stdio.h",
        "kernel32_import_library": sdk_lib / "um" / "x64" / "kernel32.lib",
        "ucrt_import_library": sdk_lib / "ucrt" / "x64" / "ucrt.lib",
    }.items():
        if not path.is_file():
            raise ToolchainError(f"required Windows SDK file is missing: {path}")
        sdk_inputs[name] = {"path": str(path.resolve()), "sha256": _sha256(path)}
    return {
        "host_toolset": manifest["build"]["host_toolset"],
        "vs_installation": str(vs_install.resolve()),
        "msvc_toolset_version": toolset_version,
        "cl_path": str(cl.resolve()),
        "cl_sha256": _sha256(cl),
        "link_path": str(link.resolve()),
        "windows_sdk_version": sdk,
        "windows_sdk_root": str(kits.resolve()),
        "windows_sdk_input_hashes": sdk_inputs,
        "crt": manifest["build"]["windows_crt"],
        "cmake_path": str(cmake_path),
        "cmake_version": _run([str(cmake_path), "--version"]).splitlines()[0],
        "ninja_path": str(ninja_path),
        "ninja_version": _run([str(ninja_path), "--version"]),
        "python_executable": str(python_executable),
        "python_version": python_version,
        "python_sha256": _sha256(python_executable),
    }


def build_plans(repo: Path, *, manifest_path: Path | None = None,
                source_dir: Path | None = None, host: str | None = None) -> dict[str, dict[str, Any]]:
    """Identify one sealed build session and all its commands; never execute them."""
    repo = repo.resolve()
    manifest_path = manifest_path or repo / "toolchains" / "llvm" / "manifest.json"
    source = (source_dir or repo / "out" / "toolchains" / "objc3-development" / "src").resolve()
    sealed = verify_development(repo, manifest_path=manifest_path, source_dir=source)
    manifest, _ = _load_manifest(manifest_path)
    host = host or ("windows-x86_64" if os.name == "nt" else "linux-x86_64")
    if host == "windows-x86_64":
        environment = _windows_build_environment(manifest)
    elif host == "linux-x86_64":
        from .host import linux_environment
        try:
            environment = linux_environment(manifest)
        except (ValueError, OSError, subprocess.SubprocessError) as exc:
            raise ToolchainError(str(exc)) from exc
    else:
        raise ToolchainError(f"unsupported foundation host: {host}")
    build = manifest["build"]
    output_suffix = "" if host == "windows-x86_64" else "-" + host
    build_dir = repo / "out" / "toolchains" / "objc3-development" / ("build" + output_suffix)
    stage_dir = repo / "out" / "toolchains" / "objc3-development" / ("stage" + output_suffix)
    source_id = sealed["build_id"]
    llvm_source = (source / "llvm").as_posix()
    build_path = build_dir.as_posix()
    stage_path = stage_dir.as_posix()
    cl_path = Path(environment.get("cl_path", environment.get("clang_path", ""))).as_posix()
    link_path = Path(environment["link_path"]).as_posix()
    cmake_cache = (repo / "toolchains" / "llvm" / "cmake" / "ObjC3LLVMCache.cmake").as_posix()
    configure = [
        "cmake", "-S", llvm_source, "-B", build_path, "-G", "Ninja",
        "-C", cmake_cache,
        f"-DCMAKE_C_COMPILER={cl_path}",
        f"-DCMAKE_CXX_COMPILER={Path(environment.get('clangxx_path', cl_path)).as_posix()}",
        f"-DCMAKE_LINKER={link_path}",
        f"-DCMAKE_SYSTEM_VERSION={build['windows_sdk_version']}",
        f"-DPython3_EXECUTABLE={Path(environment['python_executable']).as_posix()}",
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL",
        f"-DLLVM_USE_CRT_RELEASE={build['windows_crt']}",
        f"-DLLVM_ENABLE_PROJECTS={';'.join(build['projects'])}",
        f"-DLLVM_TARGETS_TO_BUILD={';'.join(build['targets'])}",
        f"-DLLVM_DISTRIBUTION_COMPONENTS={';'.join(build['distribution_components'])}",
        f"-DCMAKE_INSTALL_PREFIX={stage_path}",
        f"-DLLVM_INCLUDE_TESTS={'ON' if build['include_tests'] else 'OFF'}",
        "-DCLANG_INCLUDE_TESTS=ON",
        "-DLLVM_INSTALL_UTILS=OFF",
        f"-DCLANG_OBJC3_PATCH_DIGEST={sealed['patch_digest']}",
        f"-DCMAKE_JOB_POOLS=objc3_link={build['link_jobs']}",
        "-DCMAKE_JOB_POOL_LINK=objc3_link",
    ]
    jobs = environment.get("resource_limits", build)
    if host == "linux-x86_64":
        configure = [argument for argument in configure if not argument.startswith((
            "-DCMAKE_SYSTEM_VERSION=", "-DCMAKE_MSVC_RUNTIME_LIBRARY=", "-DLLVM_USE_CRT_RELEASE="))]
        configure += ["-DLLVM_USE_LINKER=lld", "-DLLVM_ENABLE_LLD=OFF",
                      f"-DLLVM_DEFAULT_TARGET_TRIPLE={environment['target_triple']}",
                      f"-DLLVM_PARALLEL_COMPILE_JOBS={jobs['compile_jobs']}",
                      f"-DLLVM_PARALLEL_LINK_JOBS={jobs['link_jobs']}"]
        if environment.get("ccache_path"):
            configure += [f"-DCMAKE_C_COMPILER_LAUNCHER={environment['ccache_path']}",
                          f"-DCMAKE_CXX_COMPILER_LAUNCHER={environment['ccache_path']}"]
    target_groups = build.get("build_target_groups")
    if not isinstance(target_groups, list) or not target_groups:
        raise ToolchainError("build.build_target_groups must define ordered nonempty target groups")
    grouped_targets = [target for group in target_groups for target in group.get("targets", [])]
    if grouped_targets != build.get("build_targets"):
        raise ToolchainError("build target groups must exactly match build_targets in order")
    group_commands = [
        ["cmake", "--build", build_path, "--config", "Release", "--target",
         *group["targets"], "--parallel", str(jobs["compile_jobs"])]
        for group in target_groups
    ]
    build_cmd = group_commands[0]
    test_runner = (build_dir / "bin" / ("llvm-lit.py" if host == "windows-x86_64" else "llvm-lit")).as_posix()
    test_path = (source / build["test_suite"]).as_posix()
    test_cmd = [environment["python_executable"], "-B", test_runner, "-sv", "--show-unsupported", "-j",
                str(min(jobs["compile_jobs"], 4)), test_path]
    test_follow_ups = []
    test_binaries = build.get("test_binaries", [])
    if not isinstance(test_binaries, list):
        raise ToolchainError("test_binaries must be a list of relative paths")
    for relative in test_binaries:
        if host == "linux-x86_64" and relative.endswith(".exe"):
            relative = relative[:-4]
        if not isinstance(relative, str) or not relative:
            raise ToolchainError("test binary paths must be nonempty strings")
        executable = (build_dir / relative).resolve()
        if not executable.is_relative_to(build_dir.resolve()):
            raise ToolchainError("test binary paths must remain within the selected build")
        test_follow_ups.append([executable.as_posix()])
    stage_cmd = ["cmake", "--build", build_path, "--config", "Release",
                 "--target", "install-distribution", "--parallel", str(jobs["compile_jobs"])]
    commands = {"configure": configure, "build": build_cmd, "test": test_cmd, "stage": stage_cmd}
    common = {
        "schema_version": 1,
        "source_build_id": source_id,
        "source_tree": sealed["tree_sha256"],
        "build_config_digest": sealed["build_config_digest"],
        "host_environment": environment,
        "paths": {"source": str(source), "llvm_source": llvm_source, "build": build_path, "stage": stage_path},
        "include_roots": {
            "source_clang": (source / "clang" / "include").as_posix(),
            "source_llvm": (source / "llvm" / "include").as_posix(),
            "build_generated": (build_dir / "include").as_posix(),
            "build_clang_generated": (build_dir / "tools" / "clang" / "include").as_posix(),
            "objc3_foundation_header": (build_dir / "tools" / "clang" / "include" / "clang" / "Basic" / "ObjC3Foundation.h").as_posix(),
        },
        "sdk_consumers": {
            "build_tree_LLVM_DIR": _artifact_identity(build_dir / "lib" / "cmake" / "llvm"),
            "build_tree_Clang_DIR": _artifact_identity(build_dir / "lib" / "cmake" / "clang"),
            "build_tree_resource_dir": _artifact_identity(build_dir / "lib" / "clang" / "23"),
            "LLVM_DIR": _artifact_identity(stage_dir / "lib" / "cmake" / "llvm"),
            "Clang_DIR": _artifact_identity(stage_dir / "lib" / "cmake" / "clang"),
            "resource_dir": _artifact_identity(stage_dir / "lib" / "clang" / "23"),
        },
    }
    plans = {}
    for phase, command in commands.items():
        identity_payload = {**common, "phase": phase, "command": command}
        if phase == "build":
            identity_payload["follow_up_commands"] = group_commands[1:]
        elif phase == "test":
            identity_payload["follow_up_commands"] = test_follow_ups
            # lit workers launch new Python processes, which do not inherit -B.
            # Record the environment required to keep the sealed source unchanged.
            identity_payload["command_environment"] = {"PYTHONDONTWRITEBYTECODE": "1"}
        identity_payload["identity"] = hashlib.sha256(
            json.dumps(identity_payload, sort_keys=True, separators=(",", ":")).encode()
        ).hexdigest()
        plans[phase] = identity_payload
    return plans

def _artifact_identity(path: Path) -> dict[str, Any]:
    resolved = path.resolve()
    digest = _tree_digest(resolved) if resolved.is_dir() else _sha256(resolved) if resolved.is_file() else None
    return {"path": str(resolved), "sha256": digest}


def _validate_existing(source: Path, expected: dict[str, Any]) -> dict[str, Any]:
    marker = _identity_path(source)
    try:
        recorded = json.loads(marker.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ToolchainError(f"refusing to overwrite unverified/partial source tree at {source}: {exc}") from exc
    if any(recorded.get(key) != value for key, value in expected.items()):
        raise ToolchainError(f"prepared source identity differs from current manifest at {source}")
    if recorded.get("tree_sha256") != _tree_digest(source):
        raise ToolchainError(f"prepared source tree is dirty or damaged: {source}")
    head = _run(["git", "rev-parse", "HEAD"], cwd=source)
    if head != expected["upstream_commit"]:
        raise ToolchainError(f"prepared source has wrong base {head}; expected {expected['upstream_commit']}")
    return recorded


def prepare(repo: Path, *, manifest_path: Path | None = None, fetch: bool = False,
            pristine: bool = False, source_dir: Path | None = None, verify_only: bool = False) -> dict[str, Any]:
    repo = repo.resolve()
    manifest_path = manifest_path or repo / "toolchains" / "llvm" / "manifest.json"
    manifest, base = _load_manifest(manifest_path)
    patches = _series(manifest, repo, base, pristine=pristine)
    expected = _identity(manifest, patches, pristine)
    source = (source_dir or repo / "out" / "toolchains" / expected["build_id"] / "src").resolve()
    output_root = (repo / "out" / "toolchains").resolve()
    if not source.is_relative_to(output_root):
        raise ToolchainError(f"source directory must be under generated output root {output_root}")
    if source.exists():
        result = _validate_existing(source, expected)
        if verify_only:
            return result
        raise ToolchainError(f"verified source already exists at {source}; use --verify or choose a new identity")
    if verify_only:
        raise ToolchainError(f"prepared source directory does not exist: {source}")
    source.parent.mkdir(parents=True, exist_ok=True)
    temp_root = Path(tempfile.mkdtemp(prefix=".llvm-prepare-", dir=source.parent))
    checkout = temp_root / "src"
    try:
        remote = manifest["upstream"]["url"]
        commit = expected["upstream_commit"]
        if fetch:
            _run(["git", "init", str(checkout)])
            _run(["git", "-C", str(checkout), "remote", "add", "origin", remote])
            tag = expected["upstream_tag"]
            _run(["git", "-C", str(checkout), "fetch", "--depth", "1", "origin", f"refs/tags/{tag}:refs/tags/{tag}"])
            tag_object = _run(["git", "-C", str(checkout), "rev-parse", tag])
            if tag_object != expected["upstream_tag_object"]:
                raise ToolchainError(f"fetched tag object {tag_object}; expected {expected['upstream_tag_object']}")
            peeled = _run(["git", "-C", str(checkout), "rev-parse", f"{tag}^{{commit}}"])
            if peeled != commit:
                raise ToolchainError(f"pinned tag resolves to {peeled}; expected {commit}")
            _run(["git", "-C", str(checkout), "checkout", "--detach", commit])
        else:
            cache = (repo / "out" / "toolchains" / "upstream-cache.git").resolve()
            if not cache.exists():
                raise ToolchainError(f"missing local upstream cache {cache}; run prepare --fetch to acquire pinned source")
            _run(["git", "clone", "--no-checkout", str(cache), str(checkout)])
            _run(["git", "checkout", "--detach", commit], cwd=checkout)
        head = _run(["git", "rev-parse", "HEAD"], cwd=checkout)
        if head != commit:
            raise ToolchainError(f"fetched wrong upstream base {head}; expected {commit}")
        if patches:
            for _, patch_path, _ in patches:
                _run(["git", "apply", "--check", str(patch_path)], cwd=checkout)
                _run(["git", "apply", str(patch_path)], cwd=checkout)
        recorded = {**expected, "tree_sha256": _tree_digest(checkout)}
        _identity_path(checkout).write_text(json.dumps(recorded, indent=2) + "\n", encoding="utf-8")
        os.replace(checkout, source)
        return recorded
    finally:
        if temp_root.exists():
                shutil.rmtree(temp_root, ignore_errors=True)


def build_plan(repo: Path, *, phase: str, manifest_path: Path | None = None,
               source_dir: Path | None = None, host: str | None = None) -> dict[str, Any]:
    """Return one identified command plan; standalone calls reverify all inputs."""
    if phase not in {"configure", "build", "test", "stage"}:
        raise ToolchainError(f"unsupported build phase: {phase}")
    return build_plans(repo, manifest_path=manifest_path, source_dir=source_dir, host=host)[phase]
