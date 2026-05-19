from __future__ import annotations

import argparse
from typing import Any

from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.paths import display_path

from objc3c_library_cli_parity.artifacts import sha256_text


def default_source_mode_work_key(args: argparse.Namespace) -> str:
    fingerprint: dict[str, Any] = {
        "source": display_path(args.source),
        "emit_prefix": args.emit_prefix,
        "cli_bin": display_path(args.cli_bin),
        "c_api_bin": display_path(args.c_api_bin),
        "cli_ir_object_backend": args.cli_ir_object_backend,
        "clang_path": (
            display_path(args.clang_path) if args.clang_path is not None else None
        ),
        "llc_path": display_path(args.llc_path) if args.llc_path is not None else None,
        "llvm_capabilities_summary": (
            display_path(args.llvm_capabilities_summary)
            if args.llvm_capabilities_summary is not None
            else None
        ),
        "route_cli_backend_from_capabilities": args.route_cli_backend_from_capabilities,
        "objc3_max_message_args": args.objc3_max_message_args,
        "objc3_runtime_dispatch_symbol": args.objc3_runtime_dispatch_symbol,
    }
    return sha256_text(canonical_json(fingerprint))[:16]
