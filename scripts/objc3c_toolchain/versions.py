"""Check development tool pins against their consumers and optional live tools."""
from __future__ import annotations

import json
import importlib.metadata
import re
import shutil
import subprocess
import sys
import tomllib
from pathlib import Path
from typing import Any


def check_host_tools(repo: Path, *, runtime: bool = False) -> dict[str, Any]:
    repo = repo.resolve()
    manifest = json.loads((repo / "toolchains/llvm/manifest.json").read_text())
    tools = manifest["host_tools"]
    errors = []
    for name, filename in (("python", ".python-version"), ("node", ".node-version")):
        if (repo / filename).read_text().strip() != tools[name]:
            errors.append(f"{filename} differs from host_tools.{name}")
    package = json.loads((repo / "package.json").read_text())
    if package.get("packageManager") != "npm@" + tools["npm"]:
        errors.append("package.json packageManager differs from host_tools.npm")
    for name in ("node", "npm"):
        if package.get("engines", {}).get(name) != tools[name]:
            errors.append(f"package.json engines.{name} differs from host_tools.{name}")
    project = tomllib.loads((repo / "pyproject.toml").read_text())
    for dependency in project.get("dependency-groups", {}).get("dev", []):
        if not isinstance(dependency, str) or "==" not in dependency:
            errors.append("Python development dependencies must have exact version pins")
            continue
        name, version = dependency.split("==", 1)
        if name in tools and version != tools[name]:
            errors.append(f"pyproject.toml {name} differs from host_tools.{name}")
    llvm_version = manifest["upstream"]["tag"].removeprefix("llvmorg-")
    readme = (repo / "README.md").read_text()
    for label, expected in (("Python", tools["python"]), ("Node.js", tools["node"]),
                            ("npm", tools["npm"]), ("PowerShell", tools["powershell"]),
                            ("LLVM", llvm_version)):
        for value in re.findall(re.escape(label) + r" ([0-9]+\.[0-9]+\.[0-9]+)", readme):
            if value != expected:
                errors.append(f"README.md {label}={value}; expected {expected}")
    llvm_consumers = {
        "native/objc3c/cmake/Objc3ClangFoundation.cmake": [rf"find_package\(LLVM ([0-9.]+) EXACT"],
        "native/objc3c/cmake/Objc3LinuxRuntime.cmake": [rf'CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "([0-9.]+)"'],
        "native/objc3c/src/driver/objc3_cli_environment.cpp": [r'version = "([0-9.]+)"'],
        "scripts/install_objc3c_ci_llvm_linux.sh": [r'OBJC3C_CI_LLVM_VERSION:-([0-9.]+)'],
        "scripts/install_objc3c_ci_llvm_darwin.sh": [r'OBJC3C_CI_LLVM_VERSION:-([0-9.]+)'],
        "scripts/install_objc3c_ci_llvm.ps1": [r'OBJC3C_CI_LLVM_VERSION.*else \{ "([0-9.]+)"'],
        "scripts/objc3c_tooling/llvm_discovery.py": [r'(?m)^DEFAULT_LLVM_VERSION = "([0-9.]+)"'],
    }
    for filename, patterns in llvm_consumers.items():
        path = repo / filename
        if not path.exists():
            continue
        contents = path.read_text()
        for pattern in patterns:
            matches = re.findall(pattern, contents)
            if not matches:
                errors.append(f"{filename} has no checked LLVM version reference")
            for value in matches:
                if value != llvm_version:
                    errors.append(f"{filename} LLVM={value}; expected {llvm_version}")
    workflow_count = 0
    for path in sorted((repo / ".github/workflows").glob("*.yml")):
        contents = path.read_text()
        for name, field in (("node", "node-version"), ("python", "python-version")):
            for value in re.findall(rf"^\s*{field}:\s*['\"]?([^'\"\s]+)", contents, re.M):
                if value != tools[name]:
                    errors.append(f"{path.name} {field}={value}; expected {tools[name]}")
        for value in re.findall(r'(?m)^\s*OBJC3C_CI_LLVM_VERSION:\s*["\']?([0-9.]+)', contents):
            if value != llvm_version:
                errors.append(f"{path.name} LLVM={value}; expected {llvm_version}")
        for value in re.findall(r"npm install --global npm@([^\s]+)", contents):
            if value != tools["npm"]:
                errors.append(f"{path.name} npm={value}; expected {tools['npm']}")
        for action, revision in re.findall(r"uses: (actions/[A-Za-z0-9_-]+)@([^\s#]+)", contents):
            pin = manifest.get("ci_actions", {}).get(action)
            if not pin or revision != pin["commit"]:
                errors.append(f"{path.name} {action} does not use its locked stable commit")
        workflow_count += 1
    observed = {}
    if runtime:
        for name in ("pytest", "jsonschema"):
            try:
                observed[name] = importlib.metadata.version(name)
            except importlib.metadata.PackageNotFoundError:
                observed[name] = None
            if observed[name] != tools[name]:
                errors.append(f"{name}: expected {tools[name]}, observed {observed[name]}")
        commands = {
            "python": [sys.executable, "--version"], "node": ["node", "--version"],
            "npm": ["npm", "--version"], "cmake": ["cmake", "--version"],
            "ninja": ["ninja", "--version"], "ccache": ["ccache", "--version"],
            "uv": ["uv", "--version"], "pip": [sys.executable, "-m", "pip", "--version"],
            "powershell": ["pwsh", "-NoLogo", "-NoProfile", "-NonInteractive", "-Command",
                           "$PSVersionTable.PSVersion.ToString()"],
        }
        for name, command in commands.items():
            if not Path(command[0]).is_absolute():
                executable = shutil.which(command[0])
                if not executable:
                    errors.append(f"missing development tool: {name}")
                    continue
                command = [executable, *command[1:]]
            result = subprocess.run(command, text=True, capture_output=True, timeout=30)
            text = result.stdout.strip().splitlines()
            version = re.search(r"(?<![\d.])\d+\.\d+(?:\.\d+)?\b", text[0] if text else "")
            observed[name] = version.group() if version else None
            if result.returncode or observed[name] != tools[name]:
                errors.append(f"{name}: expected {tools[name]}, observed {observed[name]}")
    if errors:
        raise ValueError("development toolchain drift: " + "; ".join(errors))
    return {"status": "PASS", "authority": "toolchains/llvm/manifest.json#host_tools",
            "expected": tools, "observed": observed, "workflows_checked": workflow_count}
