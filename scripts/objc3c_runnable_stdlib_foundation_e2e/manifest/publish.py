from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import normalize_rel_path, repo_rel

from ..assertions import expect


def collect_packaged_publish_inputs(
    *,
    package_root: Path,
    publish_inputs: object,
) -> list[str]:
    expect(
        isinstance(publish_inputs, list) and publish_inputs,
        "package manifest missing stdlib program publish inputs",
    )
    assert isinstance(publish_inputs, list)
    packaged_publish_inputs: list[str] = []
    for raw_path in publish_inputs:
        expect(
            isinstance(raw_path, str) and raw_path,
            "package manifest published malformed stdlib program input",
        )
        packaged_path = package_root / normalize_rel_path(raw_path)
        expect(
            packaged_path.is_file(),
            f"packaged runnable toolchain missing stdlib program publish input {packaged_path}",
        )
        packaged_publish_inputs.append(repo_rel(packaged_path))
    return packaged_publish_inputs
