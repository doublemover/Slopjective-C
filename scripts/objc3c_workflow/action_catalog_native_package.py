"""Native package/proof action specs."""

from __future__ import annotations

from .action_catalog_native_package_proof import NATIVE_PACKAGE_PROOF_ACTION_SPECS
from .action_catalog_native_package_toolchain import (
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS,
)
from .action_spec import ActionSpec

NATIVE_PACKAGE_ACTION_SPECS: dict[str, ActionSpec] = {
    "package-runnable-toolchain": NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS[
        "package-runnable-toolchain"
    ],
    "package-runnable-toolchain-asan": NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS[
        "package-runnable-toolchain-asan"
    ],
    "package-runnable-toolchain-ubsan": NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS[
        "package-runnable-toolchain-ubsan"
    ],
    "proof-objc3c": NATIVE_PACKAGE_PROOF_ACTION_SPECS["proof-objc3c"],
}

__all__ = ["NATIVE_PACKAGE_ACTION_SPECS"]
