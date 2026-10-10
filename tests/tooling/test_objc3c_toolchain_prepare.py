"""Focused preparation tests use only tiny local synthetic Git repositories."""

from __future__ import annotations

import hashlib
import json
import os
import subprocess
from pathlib import Path

import pytest

from scripts.objc3c_toolchain import core
from scripts.objc3c_toolchain.core import ToolchainError, prepare


def git(*args: str, cwd: Path | None = None) -> str:
    result = subprocess.run(["git", *args], cwd=cwd, text=True, capture_output=True, check=True)
    return result.stdout.strip()


def make_repo(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> tuple[Path, Path, Path]:
    repo = tmp_path / "repo"
    repo.mkdir()
    base = repo / "toolchains" / "llvm"
    patches = base / "patches"
    patches.mkdir(parents=True)
    cache = repo / "out" / "toolchains" / "upstream-cache.git"
    cache.parent.mkdir(parents=True)
    git("init", "--bare", str(cache))

    upstream = tmp_path / "upstream"
    upstream.mkdir()
    git("init", str(upstream))
    git("config", "user.email", "test@example.invalid", cwd=upstream)
    git("config", "user.name", "Synthetic Test", cwd=upstream)
    (upstream / "source.txt").write_text("base\n", encoding="utf-8")
    git("add", "source.txt", cwd=upstream)
    git("commit", "-m", "synthetic base", cwd=upstream)
    commit = git("rev-parse", "HEAD", cwd=upstream)
    git("tag", "-a", "synthetic-v1", "-m", "synthetic tag", cwd=upstream)
    tag_object = git("rev-parse", "synthetic-v1", cwd=upstream)
    git("push", str(cache), "HEAD:refs/heads/main", cwd=upstream)
    git("push", str(cache), "refs/tags/synthetic-v1", cwd=upstream)
    monkeypatch.setattr(core, "PINNED_UPSTREAM", {"tag": "synthetic-v1", "tag_object": tag_object, "commit": commit})

    patch = patches / "0001-change.patch"
    patch.write_text(
        "diff --git a/source.txt b/source.txt\n"
        "--- a/source.txt\n+++ b/source.txt\n@@ -1 +1 @@\n-base\n+patched\n"
        "diff --git a/clang/lib/Parse/new.txt b/clang/lib/Parse/new.txt\n"
        "new file mode 100644\n--- /dev/null\n+++ b/clang/lib/Parse/new.txt\n"
        "@@ -0,0 +1 @@\n+new parser source\n",
        encoding="utf-8",
    )
    patch_hash = hashlib.sha256(patch.read_bytes()).hexdigest()
    (patches / "series").write_text("0001-change.patch\n", encoding="utf-8")
    manifest = {
        "schema_version": 1,
        "upstream": {"url": str(cache), **core.PINNED_UPSTREAM},
        "patch_series": "toolchains/llvm/patches/series",
        "patches": [{"file": "0001-change.patch", "sha256": patch_hash}],
        "pristine_baseline_build_id": "synthetic-pristine",
        "build": {
            "generator": "Ninja",
            "configuration": "Release",
            "windows_sdk_version": "synthetic-sdk",
            "windows_crt": "MD",
            "projects": ["clang", "lld", "clang-tools-extra"],
            "targets": ["X86"],
            "build_targets": ["clang", "FileCheck", "count", "not", "clangTooling", "clangIndex"],
            "build_target_groups": [
                {"name": "compiler", "targets": ["clang", "FileCheck", "count", "not"]},
                {"name": "facade", "targets": ["clangTooling", "clangIndex"]},
            ],
            "distribution_components": ["clang", "lld", "clang-libraries", "llvm-libraries"],
            "test_suite": "clang/test/ObjC3/Parser",
            "test_binaries": ["tools/clang/unittests/AST/ObjC3TypeContractsTests.exe"],
            "include_tests": True,
            "compile_jobs": 8,
            "link_jobs": 1,
        },
    }
    manifest_path = base / "manifest.json"
    manifest_path.write_text(json.dumps(manifest), encoding="utf-8")
    return repo, manifest_path, patch


def test_prepare_is_repeatable_and_verifiable(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, _ = make_repo(tmp_path, monkeypatch)
    result = prepare(repo, manifest_path=manifest)
    assert prepare(repo, manifest_path=manifest, verify_only=True) == result
    with pytest.raises(ToolchainError, match="already exists"):
        prepare(repo, manifest_path=manifest)


def test_fetch_checks_annotated_tag_and_commit(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, _ = make_repo(tmp_path, monkeypatch)
    result = prepare(repo, manifest_path=manifest, fetch=True)
    source = repo / "out" / "toolchains" / result["build_id"] / "src"
    assert git("rev-parse", "HEAD", cwd=source) == result["upstream_commit"]


def test_verify_rejects_dirty_prepared_source(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, _ = make_repo(tmp_path, monkeypatch)
    result = prepare(repo, manifest_path=manifest)
    source = repo / "out" / "toolchains" / result["build_id"] / "src"
    (source / "source.txt").write_text("modified\n", encoding="utf-8")
    with pytest.raises(ToolchainError, match="dirty or damaged"):
        prepare(repo, manifest_path=manifest, verify_only=True)


def test_verify_rejects_wrong_base_even_when_tree_digest_matches(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, _ = make_repo(tmp_path, monkeypatch)
    result = prepare(repo, manifest_path=manifest)
    source = repo / "out" / "toolchains" / result["build_id"] / "src"
    git("config", "user.email", "test@example.invalid", cwd=source)
    git("config", "user.name", "Synthetic Test", cwd=source)
    (source / "source.txt").write_text("different base\n", encoding="utf-8")
    git("add", "source.txt", cwd=source)
    git("commit", "-m", "wrong base", cwd=source)
    marker = source / ".objc3c-prepared-source.json"
    identity = json.loads(marker.read_text(encoding="utf-8"))
    identity["tree_sha256"] = core._tree_digest(source)
    marker.write_text(json.dumps(identity), encoding="utf-8")
    with pytest.raises(ToolchainError, match="wrong base"):
        prepare(repo, manifest_path=manifest, verify_only=True)


def test_prepare_rejects_partial_existing_tree_without_overwriting(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, _ = make_repo(tmp_path, monkeypatch)
    data = json.loads(manifest.read_text(encoding="utf-8"))
    expected = core._identity(data, [("0001-change.patch", manifest.parent / "patches" / "0001-change.patch", data["patches"][0]["sha256"])], False)
    source = repo / "out" / "toolchains" / expected["build_id"] / "src"
    source.mkdir(parents=True)
    (source / "important.txt").write_text("preserve", encoding="utf-8")
    with pytest.raises(ToolchainError, match="refusing to overwrite"):
        prepare(repo, manifest_path=manifest)
    assert (source / "important.txt").read_text(encoding="utf-8") == "preserve"


def test_patch_hash_mismatch_fails_before_materialization(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, patch = make_repo(tmp_path, monkeypatch)
    patch.write_text(patch.read_text(encoding="utf-8") + "changed\n", encoding="utf-8")
    with pytest.raises(ToolchainError, match="patch hash mismatch"):
        prepare(repo, manifest_path=manifest)


def make_development_source(repo: Path, manifest: Path, patch: Path) -> Path:
    data = json.loads(manifest.read_text(encoding="utf-8"))
    source = repo / "out" / "toolchains" / "objc3-development" / "src"
    source.parent.mkdir(parents=True)
    git("clone", str(repo / "out" / "toolchains" / "upstream-cache.git"), str(source))
    git("checkout", "--detach", data["upstream"]["commit"], cwd=source)
    git("apply", str(patch), cwd=source)
    return source


def test_development_seal_compares_tracked_and_new_untracked_files(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, patch = make_repo(tmp_path, monkeypatch)
    source = make_development_source(repo, manifest, patch)
    sealed = core.seal_development(repo, manifest_path=manifest, source_dir=source)
    assert (source / "clang" / "lib" / "Parse" / "new.txt").read_text(encoding="utf-8") == "new parser source\n"
    assert core.verify_development(repo, manifest_path=manifest, source_dir=source) == sealed


def test_development_verifier_rejects_extra_untracked_source_without_rewriting(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, patch = make_repo(tmp_path, monkeypatch)
    source = make_development_source(repo, manifest, patch)
    core.seal_development(repo, manifest_path=manifest, source_dir=source)
    (source / ".gitignore").write_text("ignored-*\n", encoding="utf-8")
    extra = source / "clang" / "lib" / "Parse" / "ignored-unlocked.cpp"
    extra.write_text("helper", encoding="utf-8")
    with pytest.raises(ToolchainError, match="differs from the locked patch series"):
        core.verify_development(repo, manifest_path=manifest, source_dir=source)
    assert extra.read_text(encoding="utf-8") == "helper"


def test_development_verifier_detects_same_size_timestamp_restored_edit(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    repo, manifest, patch = make_repo(tmp_path, monkeypatch)
    source = make_development_source(repo, manifest, patch)
    core.seal_development(repo, manifest_path=manifest, source_dir=source)
    tracked = source / "source.txt"
    before = tracked.stat()
    tracked.write_text("changed\n", encoding="utf-8")
    os.utime(tracked, ns=(before.st_atime_ns, before.st_mtime_ns))
    assert tracked.stat().st_size == before.st_size
    assert tracked.stat().st_mtime_ns == before.st_mtime_ns
    with pytest.raises(ToolchainError, match="differs from the locked patch series"):
        core.verify_development(repo, manifest_path=manifest, source_dir=source)
    assert tracked.read_text(encoding="utf-8") == "changed\n"


def test_build_plan_carries_pinned_patch_and_windows_sdk_identity(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    repo, manifest, patch = make_repo(tmp_path, monkeypatch)
    source = make_development_source(repo, manifest, patch)
    sealed = core.seal_development(repo, manifest_path=manifest, source_dir=source)
    monkeypatch.setattr(core, "_windows_build_environment", lambda _manifest: {
        "host_toolset": "synthetic-v143",
        "windows_sdk_version": "synthetic-sdk",
        "cl_path": "C:/msvc/cl.exe",
        "link_path": "C:/msvc/link.exe",
        "cl_sha256": "compiler-hash",
        "ninja_version": "synthetic-ninja",
        "cmake_version": "synthetic-cmake",
        "python_executable": "C:/Python313/python.exe",
        "python_version": "Python 3.13.synthetic",
        "python_sha256": "python-hash",
    })
    plan = core.build_plan(repo, phase="configure", manifest_path=manifest, source_dir=source)
    assert f"-DCLANG_OBJC3_PATCH_DIGEST={sealed['patch_digest']}" in plan["command"]
    assert "-DCMAKE_SYSTEM_VERSION=synthetic-sdk" in plan["command"]
    assert "-DPython3_EXECUTABLE=C:/Python313/python.exe" in plan["command"]
    assert "-DCMAKE_JOB_POOLS=objc3_link=1" in plan["command"]
    assert plan["command"][plan["command"].index("-S") + 1] == (source / "llvm").as_posix()
    assert all("\\" not in value for value in plan["command"] if value.startswith(("C:/", "-D")))
    assert plan["sdk_consumers"]["LLVM_DIR"]["sha256"] is None
    assert plan["host_environment"]["python_sha256"] == "python-hash"
    assert plan["include_roots"]["objc3_foundation_header"].endswith(
        "/build/tools/clang/include/clang/Basic/ObjC3Foundation.h"
    )
    assert plan["sdk_consumers"]["build_tree_Clang_DIR"]["sha256"] is None

    tests = core.build_plan(repo, phase="test", manifest_path=manifest, source_dir=source)
    assert tests["command"] == [
        "C:/Python313/python.exe",
        "-B",
        (repo / "out" / "toolchains" / "objc3-development" / "build" / "bin" / "llvm-lit.py").as_posix(),
        "-sv", "-j", "4",
        (source / "clang" / "test" / "ObjC3" / "Parser").as_posix(),
    ]
    assert tests["follow_up_commands"] == [[
        (repo / "out/toolchains/objc3-development/build/tools/clang/unittests/AST/ObjC3TypeContractsTests.exe").as_posix()
    ]]
    assert tests["command_environment"] == {"PYTHONDONTWRITEBYTECODE": "1"}

    staged = core.build_plan(repo, phase="stage", manifest_path=manifest, source_dir=source)
    assert "install-distribution" in staged["command"]
    build = core.build_plan(repo, phase="build", manifest_path=manifest, source_dir=source)
    assert "clang" in build["command"]
    assert "FileCheck" in build["command"]
    assert "clangTooling" not in build["command"]
    assert "clangTooling" in build["follow_up_commands"][0]
    assert "clangIndex" in build["follow_up_commands"][0]
    assert "lld-link" not in build["command"]
    assert "clang-libraries" not in build["command"]
    assert "llvm-libraries" not in build["command"]
    components = json.loads(manifest.read_text(encoding="utf-8"))["build"]["distribution_components"]
    assert "clang-libraries" in components
    assert "lld" in components
