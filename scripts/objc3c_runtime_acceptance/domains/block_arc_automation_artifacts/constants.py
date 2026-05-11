"""Block/ARC storage automation artifact paths and manifest keys."""

from __future__ import annotations


ARC_ARG = "-fobjc-arc"
MANIFEST_FILE_NAME = "module.manifest.json"
LLVM_IR_FILE_NAME = "module.ll"

SEMANTIC_SURFACE_PATH = ("frontend", "pipeline", "semantic_surface")
SEMA_PASS_MANAGER_PATH = ("frontend", "pipeline", "sema_pass_manager")

COPY_DISPOSE_SURFACE_KEY = "objc_block_copy_dispose_lowering_surface"
ESCAPE_SURFACE_KEY = "objc_block_storage_escape_lowering_surface"
ARC_DIAGNOSTICS_FIXIT_SURFACE_KEY = "objc_arc_diagnostics_fixit_lowering_surface"


__all__ = [
    "ARC_ARG",
    "ARC_DIAGNOSTICS_FIXIT_SURFACE_KEY",
    "COPY_DISPOSE_SURFACE_KEY",
    "ESCAPE_SURFACE_KEY",
    "LLVM_IR_FILE_NAME",
    "MANIFEST_FILE_NAME",
    "SEMA_PASS_MANAGER_PATH",
    "SEMANTIC_SURFACE_PATH",
]
