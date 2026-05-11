from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import display_path


def build_source_mode_artifacts(*, emit_prefix: str) -> list[str]:
    return [
        f"{emit_prefix}.diagnostics.json",
        f"{emit_prefix}.manifest.json",
        f"{emit_prefix}.ll",
        f"{emit_prefix}.obj",
    ]


def _expected_source_mode_outputs(*, emit_prefix: str) -> list[str]:
    expected_artifacts = list(build_source_mode_artifacts(emit_prefix=emit_prefix))
    expected_artifacts.extend(
        (
            f"{emit_prefix}.diagnostics.txt",
            f"{emit_prefix}.c_api_summary.json",
        )
    )
    return expected_artifacts


def assert_no_stale_source_mode_outputs(
    *,
    directory: Path,
    emit_prefix: str,
    label: str,
) -> None:
    stale_paths: list[str] = []
    for artifact_name in _expected_source_mode_outputs(emit_prefix=emit_prefix):
        artifact_path = directory / artifact_name
        proxy_path = directory / f"{artifact_name}.sha256"
        if artifact_path.exists():
            stale_paths.append(display_path(artifact_path))
        if proxy_path.exists():
            stale_paths.append(display_path(proxy_path))
    if stale_paths:
        stale_display = ", ".join(sorted(set(stale_paths)))
        raise ValueError(
            f"{label} contains stale generated artifacts; choose a unique --work-key "
            f"or clear outputs before replay: {stale_display}"
        )
