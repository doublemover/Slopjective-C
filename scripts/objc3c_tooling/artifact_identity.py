"""Platform-aware native artifact identity helpers."""

from __future__ import annotations

from dataclasses import dataclass
import platform


@dataclass(frozen=True)
class NativeArtifactIdentity:
    platform_id: str
    host_promotion_state: str
    target_triple: str
    native_executable_relative_path: str
    frontend_runner_relative_path: str
    runtime_library_relative_path: str
    runtime_library_name: str

    @property
    def tampered_runtime_library_relative_path(self) -> str:
        return f"artifacts/lib/tampered_{self.runtime_library_name}"


def host_architecture(machine: str | None = None) -> str:
    normalized = (machine if machine is not None else platform.machine()).lower()
    if normalized in {"amd64", "x86_64", "x64"}:
        return "x64"
    if normalized in {"arm64", "aarch64"}:
        return "arm64"
    return normalized or "unknown"


def current_host_platform_id() -> str:
    system = platform.system().lower()
    arch = host_architecture()
    if system in {"windows", "linux", "darwin"}:
        return f"{system}-{arch}"
    return f"unsupported-{arch}"


def artifact_identity_for_platform(platform_id: str) -> NativeArtifactIdentity:
    if platform_id.startswith("windows-"):
        arch = platform_id.removeprefix("windows-")
        target_triple = {
            "x64": "x86_64-pc-windows-msvc",
            "arm64": "aarch64-pc-windows-msvc",
        }.get(arch, f"{arch}-pc-windows-msvc")
        return NativeArtifactIdentity(
            platform_id=platform_id,
            host_promotion_state=(
                "supported-boundary"
                if platform_id == "windows-x64"
                else "unsupported-host"
            ),
            target_triple=target_triple,
            native_executable_relative_path="artifacts/bin/objc3c-native.exe",
            frontend_runner_relative_path=(
                "artifacts/bin/objc3c-frontend-c-api-runner.exe"
            ),
            runtime_library_relative_path="artifacts/lib/objc3_runtime.lib",
            runtime_library_name="objc3_runtime.lib",
        )

    if platform_id.startswith("linux-"):
        arch = platform_id.removeprefix("linux-")
        target_triple = {
            "x64": "x86_64-unknown-linux-gnu",
            "arm64": "aarch64-unknown-linux-gnu",
        }.get(arch, f"{arch}-unknown-linux-gnu")
        return NativeArtifactIdentity(
            platform_id=platform_id,
            host_promotion_state=(
                "fail-closed-until-native-host-evidence"
                if platform_id == "linux-x64"
                else "unsupported-host"
            ),
            target_triple=target_triple,
            native_executable_relative_path="artifacts/bin/objc3c-native",
            frontend_runner_relative_path="artifacts/bin/objc3c-frontend-c-api-runner",
            runtime_library_relative_path="artifacts/lib/libobjc3-runtime.so",
            runtime_library_name="libobjc3-runtime.so",
        )

    if platform_id.startswith("darwin-"):
        arch = platform_id.removeprefix("darwin-")
        target_triple = {
            "arm64": "aarch64-apple-darwin",
            "x64": "x86_64-apple-darwin",
        }.get(arch, f"{arch}-apple-darwin")
        return NativeArtifactIdentity(
            platform_id=platform_id,
            host_promotion_state=(
                "fail-closed-until-native-host-evidence"
                if platform_id == "darwin-arm64"
                else "unsupported-host"
            ),
            target_triple=target_triple,
            native_executable_relative_path="artifacts/bin/objc3c-native",
            frontend_runner_relative_path="artifacts/bin/objc3c-frontend-c-api-runner",
            runtime_library_relative_path="artifacts/lib/libobjc3-runtime.dylib",
            runtime_library_name="libobjc3-runtime.dylib",
        )

    return NativeArtifactIdentity(
        platform_id=platform_id,
        host_promotion_state="unsupported-host",
        target_triple=platform_id,
        native_executable_relative_path="artifacts/bin/objc3c-native",
        frontend_runner_relative_path="artifacts/bin/objc3c-frontend-c-api-runner",
        runtime_library_relative_path="artifacts/lib/libobjc3-runtime.so",
        runtime_library_name="libobjc3-runtime.so",
    )


def current_host_artifact_identity() -> NativeArtifactIdentity:
    return artifact_identity_for_platform(current_host_platform_id())


__all__ = [
    "NativeArtifactIdentity",
    "artifact_identity_for_platform",
    "current_host_artifact_identity",
    "current_host_platform_id",
    "host_architecture",
]
