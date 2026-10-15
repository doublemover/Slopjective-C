"""Native package node contracts; subprocesses are mocked in this module."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

import pytest

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "scripts"))
from objc3c_native_validation import package_nodes as package  # noqa: E402


def _setup(tmp_path: Path) -> tuple[Path, dict]:
    repo = tmp_path / "checkout"
    fixture_root = repo / package.FIXTURES
    fixture_root.mkdir(parents=True)
    for name in ("provider.m", "consumer-eager.m", "consumer-source.m"):
        shutil.copy2(REPO / package.FIXTURES / name, fixture_root / name)
    sdk = tmp_path / "sealed-sdk"
    sdk.mkdir()
    tools = {}
    for role in ("facade", "clang", "clangxx", "clang_cl", "linker"):
        path = sdk / ("lld-link.exe" if role == "linker" else role + ".exe")
        path.write_bytes(("fake-" + role).encode())
        tools[role] = str(path)
    resource = sdk / "resource"
    resource.mkdir()
    runtime = sdk / "runtime"
    runtime.mkdir()
    (runtime / "objc.lib").write_bytes(b"fake native import library")
    (runtime / "objc.dll").write_bytes(b"fake native runtime")
    return repo, {"sdk_build_id": "sealed-sdk-test", "build_identity": "a" * 64,
                  "patch_digest": "b" * 64, "target_triple": "x86_64-pc-windows-msvc",
                  "resource_dir": str(resource), "tools": tools,
                  "runtime": {"import_library": str(runtime / "objc.lib"),
                              "dll": str(runtime / "objc.dll"), "bin_dir": str(runtime)},
                  "environment": {"PACKAGE_TEST_SNAPSHOT": "captured"}}


def _identity(toolchain: dict) -> dict:
    return {"canonical_name": package.MODULE, "package_identity": package.PACKAGE,
            "semantic_fingerprint": "c" * 64, "target_triple": toolchain["target_triple"],
            "runtime_abi": "objc-runtime=gnustep-2.0;manual-reference-counting",
            "extension_abi": "objc3-extension-abi-2:" + toolchain["patch_digest"],
            "language_revision": "objc3-enabled", "language_profile": "compatibility"}


def _record(toolchain: dict, identity: dict) -> dict:
    return {"schema": "objc3c.dependency-record.v2", "visible_modules": [package.MODULE],
            "modules": [{"name": identity["canonical_name"], "fingerprint": identity["semantic_fingerprint"],
                         "package": identity["package_identity"], "target": identity["target_triple"],
                         "runtime": identity["runtime_abi"], "extension_abi": identity["extension_abi"],
                         "language_revision": identity["language_revision"], "language_profile": identity["language_profile"]}],
            "toolchain": {"driver_path": toolchain["tools"]["clang"],
                          "resource_directory": toolchain["resource_dir"], "patch_digest": toolchain["patch_digest"]}}


class FakeNativeCommands:
    def __init__(self, repo: Path, toolchain: dict, *, fail_phase: str = "", bad_driver: bool = False,
                 emitted_target: str | None = None):
        self.repo = repo
        self.toolchain = toolchain
        self.commands: list[dict] = []
        self.fail_phase = fail_phase
        self.bad_driver = bad_driver
        self.emitted_target = emitted_target

    def __call__(self, command, **kwargs):
        phase = "consumer-run"
        if command[0] == self.toolchain["tools"]["facade"]:
            phase = "provider-compile" if "--emit-interface" in command else "consumer-compile"
        elif command[0] == self.toolchain["tools"]["clangxx"]:
            phase = "consumer-link"
        self.commands.append({"phase": phase, "argv": command, **kwargs})
        if phase == self.fail_phase:
            return subprocess.CompletedProcess(command, 7, stdout="reached failing phase\n", stderr="native failure\n")
        identity = _identity(self.toolchain)
        if self.emitted_target is not None:
            identity["target_triple"] = self.emitted_target
        if phase == "provider-compile":
            object_path = Path(command[command.index("--emit-object") + 1])
            object_path.write_bytes(b"mock native provider object")
            interface = Path(command[command.index("--emit-interface") + 1])
            interface.write_text("typedef int Answer;\nAnswer answer(void);\n", encoding="utf-8")
            sidecar = {"schema_id": "objc3c.semantic-interface.v1", "schema_major": 1,
                       "module": identity, "source_binding_sha256": hashlib.sha256(interface.read_bytes()).hexdigest(),
                       "declarations": [{"qualified_name": package.MODULE + "::Answer", "kind": "typedef"},
                                        {"qualified_name": package.MODULE + "::answer", "kind": "function"}]}
            Path(str(interface) + ".json").write_text(json.dumps(sidecar), encoding="utf-8")
        elif phase == "consumer-compile":
            scratch = Path(kwargs["cwd"])
            assert not scratch.is_relative_to(self.repo)
            assert sorted(path.name for path in scratch.iterdir()) == sorted([*package.PACKAGE_FILENAMES.values(), "consumer.m"])
            assert not (scratch / "provider.m").exists()
            assert not Path(command[1]).is_relative_to(self.repo)
            assert (scratch / "consumer.m").read_text().find("return 42;") == -1
            Path(command[command.index("--emit-object") + 1]).write_bytes(b"mock native consumer object")
            record = _record(self.toolchain, identity)
            if self.bad_driver:
                record["toolchain"]["driver_path"] = str(scratch / "unsealed-clang.exe")
            Path(command[command.index("--emit-dependency-record") + 1]).write_text(json.dumps(record), encoding="utf-8")
        elif phase == "consumer-link":
            assert str(self.toolchain["runtime"]["import_library"]) in command
            assert "-fuse-ld=lld" in command
            assert command[command.index("-B") + 1] == str(Path(self.toolchain["tools"]["linker"]).parent)
            assert not any(token.startswith("--ld-path") for token in command)
            Path(command[command.index("-o") + 1]).write_bytes(b"mock native consumer executable")
        else:
            assert kwargs["env"]["PATH"].split(os.pathsep)[0] == self.toolchain["runtime"]["bin_dir"]
        assert kwargs["env"]["PACKAGE_TEST_SNAPSHOT"] == "captured"
        return subprocess.CompletedProcess(command, 0, stdout="native phase completed\n", stderr="")


def _produce(repo: Path, toolchain: dict, output: Path) -> dict:
    return package.run_package_node(repo, {"node_id": package.PROVIDER_NODE, "operation": "package-provider"}, output, toolchain, {})


def _dependencies(provider: dict) -> dict:
    return {package.PROVIDER_NODE: {"data": provider["data"], "artifacts": {
        role: {"path": str(path), "sha256": hashlib.sha256(path.read_bytes()).hexdigest()}
        for role, path in provider["artifacts"].items()}}}


def test_provider_emits_an_explicit_three_file_package(tmp_path, monkeypatch):
    repo, toolchain = _setup(tmp_path)
    commands = FakeNativeCommands(repo, toolchain)
    monkeypatch.setattr(package.subprocess, "run", commands)
    result = _produce(repo, toolchain, tmp_path / "provider-output")
    manifest = result["data"]["package_manifest"]
    assert manifest["contract_id"] == package.PACKAGE_CONTRACT
    assert {row["role"] for row in manifest["files"]} == set(package.PACKAGE_FILENAMES)
    assert manifest["provider_source_in_package"] is False
    assert result["data"]["module_identity"] == _identity(toolchain)
    assert result["data"]["runtime_count"] == 0
    assert result["data"]["commands"][0]["exit_code"] == 0
    assert "--objc3" in commands.commands[0]["argv"]
    for row in manifest["files"]:
        assert row["sha256"] == hashlib.sha256(result["artifacts"][row["role"]].read_bytes()).hexdigest()


@pytest.mark.parametrize("mode", ["eager", "source-directed"])
def test_installed_consumer_compiles_links_and_runs_outside_checkout(tmp_path, monkeypatch, mode):
    repo, toolchain = _setup(tmp_path)
    commands = FakeNativeCommands(repo, toolchain)
    monkeypatch.setattr(package.subprocess, "run", commands)
    provider = _produce(repo, toolchain, tmp_path / "provider-output")
    before = dict(os.environ)
    result = package.run_package_node(repo, {"node_id": "native-package:" + mode,
        "operation": "package-consumer", "mode": mode}, tmp_path / mode, toolchain, _dependencies(provider))
    assert os.environ == before
    assert result["data"]["status"] == "PASS" and result["data"]["runtime_count"] == 1
    assert result["data"]["provider_source_absent"] and result["data"]["scratch_outside_checkout"]
    assert result["data"]["scratch_removed"] and not Path(result["data"]["scratch_directory"]).exists()
    assert result["data"]["actual_exit_code"] == 0
    assert all(path.is_file() for path in result["artifacts"].values())
    assert all(path.is_relative_to(tmp_path / mode) for path in result["artifacts"].values())
    compile_command = next(row["argv"] for row in commands.commands if row["phase"] == "consumer-compile")
    link_command = next(row["argv"] for row in commands.commands if row["phase"] == "consumer-link")
    assert "-fuse-ld=lld" in link_command
    assert link_command[link_command.index("-B") + 1] == str(Path(toolchain["tools"]["linker"]).parent)
    assert not any(token.startswith("--ld-path") for token in link_command)
    assert ("--source-imports" in compile_command) == (mode == "source-directed")
    lock = compile_command.index("--import-module")
    assert compile_command[lock + 1:lock + 4] == [package.MODULE, "c" * 64, package.PACKAGE]
    source = result["artifacts"]["consumer_source"].read_text()
    if mode == "source-directed":
        assert source.count("@fixture.NativePackage.Answer") == 2
        assert "@fixture.NativePackage.answer()" in source
    else:
        assert "@import" not in source


@pytest.mark.parametrize("mutation", ["object", "interface", "sidecar", "sdk", "identity"])
def test_consumer_rejects_changed_or_wrong_identity_packages_before_compile(tmp_path, monkeypatch, mutation):
    repo, toolchain = _setup(tmp_path)
    commands = FakeNativeCommands(repo, toolchain)
    monkeypatch.setattr(package.subprocess, "run", commands)
    provider = _produce(repo, toolchain, tmp_path / "provider-output")
    if mutation in package.PACKAGE_FILENAMES:
        provider["artifacts"][mutation].write_bytes(b"tampered artifact")
    else:
        manifest_path = provider["artifacts"]["package_manifest"]
        manifest = json.loads(manifest_path.read_text())
        if mutation == "sdk":
            manifest["sdk_build_id"] = "another-sdk"
        else:
            manifest["module_identity"]["package_identity"] = "another-package"
        manifest_path.write_text(json.dumps(manifest))
    with pytest.raises(package.PackageNodeError):
        package.run_package_node(repo, {"operation": "package-consumer", "mode": "eager"},
            tmp_path / "rejected", toolchain, _dependencies(provider))
    assert len(commands.commands) == 1
    evidence = json.loads((tmp_path / "rejected/package-node-evidence.json").read_text())
    assert evidence["status"] == "FAIL" and evidence["runtime_count"] == 0


@pytest.mark.parametrize("phase", ["consumer-compile", "consumer-link", "consumer-run"])
def test_failures_preserve_evidence_and_remove_only_allocated_scratch(tmp_path, monkeypatch, phase):
    repo, toolchain = _setup(tmp_path)
    commands = FakeNativeCommands(repo, toolchain, fail_phase=phase)
    monkeypatch.setattr(package.subprocess, "run", commands)
    provider = _produce(repo, toolchain, tmp_path / "provider-output")
    marker = tmp_path / "must-survive.txt"
    marker.write_text("unrelated evidence")
    with pytest.raises(package.PackageNodeError) as failure:
        package.run_package_node(repo, {"operation": "package-consumer", "mode": "eager"},
            tmp_path / "failed", toolchain, _dependencies(provider))
    data = failure.value.data
    assert data["scratch_removed"] and not Path(data["scratch_directory"]).exists()
    assert data["status"] == "FAIL" and data["commands"][-1]["exit_code"] == 7
    assert failure.value.artifacts["consumer_source"].is_file()
    assert failure.value.artifacts["installed_object"].is_file()
    assert (tmp_path / "failed" / f"{phase}.stderr.log").read_text() == "native failure\n"
    assert marker.read_text() == "unrelated evidence"
    if phase == "consumer-run":
        assert data["actual_exit_code"] == 7


def test_dependency_record_must_use_the_sealed_driver(tmp_path, monkeypatch):
    repo, toolchain = _setup(tmp_path)
    commands = FakeNativeCommands(repo, toolchain, bad_driver=True)
    monkeypatch.setattr(package.subprocess, "run", commands)
    provider = _produce(repo, toolchain, tmp_path / "provider-output")
    with pytest.raises(package.PackageNodeError, match="another compiler driver"):
        package.run_package_node(repo, {"operation": "package-consumer", "mode": "source-directed"},
            tmp_path / "rejected", toolchain, _dependencies(provider))
    assert [row["phase"] for row in commands.commands] == ["provider-compile", "consumer-compile"]


def test_consumer_rejects_checkout_local_system_temp(tmp_path, monkeypatch):
    repo, toolchain = _setup(tmp_path)
    commands = FakeNativeCommands(repo, toolchain)
    monkeypatch.setattr(package.subprocess, "run", commands)
    provider = _produce(repo, toolchain, tmp_path / "provider-output")
    monkeypatch.setattr(package.tempfile, "gettempdir", lambda: str(repo / "tmp"))
    with pytest.raises(package.PackageNodeError, match="inside the checkout"):
        package.run_package_node(repo, {"operation": "package-consumer", "mode": "eager"},
            tmp_path / "rejected", toolchain, _dependencies(provider))


def test_existing_node_evidence_is_never_overwritten(tmp_path, monkeypatch):
    repo, toolchain = _setup(tmp_path)
    monkeypatch.setattr(package.subprocess, "run", FakeNativeCommands(repo, toolchain))
    result = _produce(repo, toolchain, tmp_path / "provider-output")
    evidence = result["artifacts"]["evidence"].read_bytes()
    with pytest.raises(ValueError, match="already contains evidence"):
        _produce(repo, toolchain, tmp_path / "provider-output")
    assert result["artifacts"]["evidence"].read_bytes() == evidence


@pytest.mark.parametrize("mode", ["eager", "source-directed"])
def test_versioned_msvc_target_is_admitted_and_retained_in_module_lock(tmp_path, monkeypatch, mode):
    repo, toolchain = _setup(tmp_path)
    target = "x86_64-pc-windows-msvc19.44.35228"
    monkeypatch.setattr(package.subprocess, "run", FakeNativeCommands(repo, toolchain, emitted_target=target))
    provider = _produce(repo, toolchain, tmp_path / "provider-output")
    assert provider["data"]["module_identity"]["target_triple"] == target
    assert provider["data"]["package_manifest"]["module_identity"]["target_triple"] == target
    consumer = package.run_package_node(repo, {"operation": "package-consumer", "mode": mode},
        tmp_path / "consumer-output", toolchain, _dependencies(provider))
    assert consumer["data"]["module_identity"]["target_triple"] == target
    assert consumer["data"]["dependency_record"]["modules"][0]["target"] == target


@pytest.mark.parametrize("target", [
    "i686-pc-windows-msvc19.44.35228",
    "aarch64-pc-windows-msvc19.44.35228",
    "x86_64-unknown-windows-msvc19.44.35228",
    "x86_64-pc-linux-gnu",
    "x86_64-pc-windows-gnu",
    "x86_64-pc-windows-msvc19.44.35228-extra",
    "x86_64-pc-windows-msvc19..44",
    "x86_64-pc-windows-msvc19.44.35228.1.2",
])
def test_target_admission_rejects_other_architecture_os_abi_and_nonversion_suffixes(target):
    assert not package._target_compatible("x86_64-pc-windows-msvc", target)


def test_explicit_msvc_version_requires_its_exact_target():
    assert package._target_compatible("x86_64-pc-windows-msvc19.44.35228", "x86_64-pc-windows-msvc19.44.35228")
    assert not package._target_compatible("x86_64-pc-windows-msvc19.44.35228", "x86_64-pc-windows-msvc19.43.1")
