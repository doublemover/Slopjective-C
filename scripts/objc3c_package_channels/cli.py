"""CLI orchestration for package channel publication."""

from __future__ import annotations

from .commands import build_release_foundation_artifacts, build_runnable_package, build_support_matrix
from .loading import load_package_channel_inputs, load_package_channel_surface_inputs
from .model import MANIFEST_RELATIVE_PATH, package_channel_paths, package_channels_manifest_payload
from .paths import package_channel_run_id
from .publication import (
    print_package_channel_result,
    publish_installer_archive,
    publish_offline_bundle,
    publish_portable_archive,
    write_package_channel_artifacts,
)
from .validation import validate_manifest_required_fields


def main() -> int:
    surface_inputs = load_package_channel_surface_inputs()
    build_support_matrix()
    inputs = load_package_channel_inputs(surface_inputs)
    build_release_foundation_artifacts()

    paths = package_channel_paths(package_channel_run_id())
    build_runnable_package(paths.package_root, MANIFEST_RELATIVE_PATH)

    publish_portable_archive(paths)
    publish_installer_archive(paths)
    publish_offline_bundle(paths)

    manifest_payload = package_channels_manifest_payload(inputs=inputs, paths=paths)
    validate_manifest_required_fields(
        manifest_payload=manifest_payload,
        metadata_surface=inputs.metadata_surface,
    )
    write_package_channel_artifacts(
        inputs=inputs,
        paths=paths,
        manifest_payload=manifest_payload,
    )

    print_package_channel_result(paths)
    return 0
