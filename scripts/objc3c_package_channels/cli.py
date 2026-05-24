"""CLI orchestration for package channel publication."""

from __future__ import annotations

import argparse
from collections.abc import Sequence

from .commands import build_release_foundation_artifacts, build_runnable_package, build_support_matrix
from .loading import load_package_channel_inputs, load_package_channel_surface_inputs
from .model import (
    MANIFEST_RELATIVE_PATH,
    installer_signature_payload,
    package_channel_paths,
    package_channels_manifest_payload,
)
from .paths import package_channel_run_id
from .publication import (
    prepare_package_channel_workspace,
    print_package_channel_result,
    publish_installer_archive,
    publish_offline_bundle,
    publish_portable_archive,
    write_package_channel_artifacts,
)
from .validation import validate_manifest_required_fields


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--reuse-release-foundation-artifacts",
        action="store_true",
        help="Use the already validated release-foundation integration outputs instead of rebuilding them.",
    )
    parser.add_argument(
        "--sanitizer-variant",
        choices=("release", "address", "undefined"),
        default="release",
        help="Build package-channel artifacts for the selected runtime sanitizer variant.",
    )
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    surface_inputs = load_package_channel_surface_inputs()
    build_support_matrix()
    inputs = load_package_channel_inputs(surface_inputs)
    build_release_foundation_artifacts(
        reuse_existing=bool(args.reuse_release_foundation_artifacts)
    )

    paths = package_channel_paths(
        package_channel_run_id(),
        sanitizer_variant=str(args.sanitizer_variant),
    )
    prepare_package_channel_workspace(paths)
    build_runnable_package(
        paths.package_root,
        MANIFEST_RELATIVE_PATH,
        sanitizer_variant=paths.sanitizer_variant,
    )

    publish_portable_archive(paths)
    publish_installer_archive(paths)
    publish_offline_bundle(paths)

    manifest_payload = package_channels_manifest_payload(
        inputs=inputs,
        paths=paths,
        installer_signature=installer_signature_payload(paths.installer_archive),
    )
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
