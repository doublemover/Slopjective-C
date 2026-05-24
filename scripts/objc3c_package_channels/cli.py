"""CLI orchestration for package channel publication."""

from __future__ import annotations

import argparse
from collections.abc import Iterator, Sequence
from contextlib import contextmanager
import os

from .commands import build_release_foundation_artifacts, build_runnable_package, build_support_matrix
from .loading import load_package_channel_inputs, load_package_channel_surface_inputs
from .model import (
    MANIFEST_RELATIVE_PATH,
    RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES,
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
from .sanitizer_contracts import SANITIZER_VARIANTS
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
        choices=SANITIZER_VARIANTS,
        default="release",
        help="Build package-channel artifacts for the selected runtime sanitizer variant.",
    )
    parser.add_argument(
        "--target-platform-id",
        choices=RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES,
        default=None,
        help="Build release package-channel artifacts for the selected target platform.",
    )
    args = parser.parse_args(argv)
    if args.sanitizer_variant != "release" and args.target_platform_id is not None:
        parser.error(
            "--target-platform-id is only supported for release package channels; "
            "sanitizer package channels are windows-x64 only."
        )
    return args


@contextmanager
def selected_target_platform_environment(
    target_platform_id: str | None,
) -> Iterator[None]:
    if target_platform_id is None:
        yield
        return

    previous_target_platform_id = os.environ.get("OBJC3C_TARGET_PLATFORM_ID")
    os.environ["OBJC3C_TARGET_PLATFORM_ID"] = target_platform_id
    try:
        yield
    finally:
        if previous_target_platform_id is None:
            os.environ.pop("OBJC3C_TARGET_PLATFORM_ID", None)
        else:
            os.environ["OBJC3C_TARGET_PLATFORM_ID"] = previous_target_platform_id


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    target_platform_id = args.target_platform_id
    surface_inputs = load_package_channel_surface_inputs()
    build_support_matrix()
    inputs = load_package_channel_inputs(surface_inputs)

    paths = package_channel_paths(
        package_channel_run_id(),
        sanitizer_variant=str(args.sanitizer_variant),
        target_platform_id=target_platform_id,
    )
    with selected_target_platform_environment(target_platform_id):
        build_release_foundation_artifacts(
            reuse_existing=bool(args.reuse_release_foundation_artifacts)
        )
    prepare_package_channel_workspace(paths)
    with selected_target_platform_environment(target_platform_id):
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
