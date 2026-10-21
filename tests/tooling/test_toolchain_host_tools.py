from __future__ import annotations

import json
from pathlib import Path
import shutil
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import versions, host

ROOT = Path(__file__).resolve().parents[2]


@pytest.fixture
def repo(tmp_path: Path) -> Path:
    for name in ("toolchains/llvm/manifest.json", ".python-version", ".node-version",
                 "package.json", "pyproject.toml", "README.md", ".github/workflows/spec-quality.yml",
                 "native/objc3c/cmake/Objc3ClangFoundation.cmake",
                 "native/objc3c/cmake/Objc3LinuxRuntime.cmake",
                 "scripts/install_objc3c_ci_llvm.ps1", "scripts/install_objc3c_ci_llvm_linux.sh",
                 "scripts/install_objc3c_ci_llvm_darwin.sh", "scripts/objc3c_tooling/llvm_discovery.py"):
        target = tmp_path / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / name, target)
    return tmp_path


def test_checked_references_agree(repo: Path) -> None:
    assert versions.check_host_tools(repo)["status"] == "PASS"


@pytest.mark.parametrize("filename,old,new", [
    (".node-version", "26.12.0", "24.19.0"),
    (".python-version", "3.15.0", "3.13.3"),
    ("README.md", "Python 3.15.0", "Python 3.13.3"),
    ("package.json", "npm@12.2.1", "npm@11.20.0"),
    ("pyproject.toml", "pytest==9.1.1", "pytest==8.0.0"),
    ("native/objc3c/cmake/Objc3ClangFoundation.cmake", "LLVM 23.1.3 EXACT", "LLVM 22.1.6 EXACT"),
    (".github/workflows/spec-quality.yml", "3d3c42e5aac5ba805825da76410c181273ba90b1", "v4"),
    (".github/workflows/spec-quality.yml", 'python-version: "3.15.0"', 'python-version: "3.13.3"'),
])
def test_stale_consumer_is_rejected(repo: Path, filename: str, old: str, new: str) -> None:
    path = repo / filename
    original = path.read_text()
    assert old in original
    path.write_text(original.replace(old, new))
    with pytest.raises(ValueError, match="development toolchain drift"):
        versions.check_host_tools(repo)


def test_live_versions_accept_node_prefix_and_ninja_build_suffix(repo: Path, monkeypatch) -> None:
    tools = json.loads((repo / "toolchains/llvm/manifest.json").read_text())["host_tools"]
    def run(command, **kwargs):
        name = Path(command[0]).name
        key = {"pwsh": "powershell"}.get(name, name)
        if "-m" in command:
            key = "pip"
        elif name.startswith("python"):
            key = "python"
        value = tools[key]
        value = "v" + value if key == "node" else value
        value += ".git.kitware.jobserver-pipe-1" if key == "ninja" else ""
        return SimpleNamespace(returncode=0, stdout=value, stderr="")
    monkeypatch.setattr(versions.shutil, "which", lambda name: "/tools/" + name)
    monkeypatch.setattr(versions.importlib.metadata, "version", lambda name: tools[name])
    monkeypatch.setattr(versions.subprocess, "run", run)
    assert versions.check_host_tools(repo, runtime=True)["observed"]["node"] == tools["node"]


def test_cpu_and_memory_limits_bound_compilers_and_linkers(monkeypatch) -> None:
    monkeypatch.setattr(host.os, "process_cpu_count", lambda: 32, raising=False)
    monkeypatch.setattr(host.os, "sysconf", lambda key: 4096 if key == "SC_PAGE_SIZE" else 16 * 1024 ** 3 // 4096)
    original = Path.read_text
    def read(path, *args, **kwargs):
        if str(path) == "/sys/fs/cgroup/cpu.max":
            return "200000 100000"
        if str(path) == "/sys/fs/cgroup/memory.max":
            return str(8 * 1024 ** 3)
        return original(path, *args, **kwargs)
    monkeypatch.setattr(Path, "read_text", read)
    limits = host.resource_limits({"compile_jobs": 16, "link_jobs": 8})
    assert limits["compile_jobs"] == 2
    assert limits["link_jobs"] == 1
    assert limits["memory_bytes"] == 8 * 1024 ** 3
