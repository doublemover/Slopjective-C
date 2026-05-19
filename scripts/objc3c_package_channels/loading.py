"""Checked-in input loading for package channel publication."""

from __future__ import annotations

from objc3c_tooling.json_io import load_json_object as load_json

from .model import PackageChannelInputs, PackageChannelSurfaceInputs
from .paths import (
    INSTALLER_POLICY,
    METADATA_SURFACE,
    PLATFORM_SUPPORT_MATRIX_ARTIFACT,
    SCHEMA_SURFACE,
    SOURCE_SURFACE,
    SUPPORTED_PLATFORMS,
)


def load_package_channel_surface_inputs() -> PackageChannelSurfaceInputs:
    load_json(SOURCE_SURFACE)
    supported_platforms = load_json(SUPPORTED_PLATFORMS)
    load_json(INSTALLER_POLICY)
    metadata_surface = load_json(METADATA_SURFACE)
    load_json(SCHEMA_SURFACE)
    return PackageChannelSurfaceInputs(
        supported_platforms=supported_platforms,
        metadata_surface=metadata_surface,
    )


def load_package_channel_inputs(surface_inputs: PackageChannelSurfaceInputs) -> PackageChannelInputs:
    platform_support_matrix = load_json(PLATFORM_SUPPORT_MATRIX_ARTIFACT)
    return PackageChannelInputs(
        supported_platforms=surface_inputs.supported_platforms,
        metadata_surface=surface_inputs.metadata_surface,
        platform_support_matrix=platform_support_matrix,
    )
