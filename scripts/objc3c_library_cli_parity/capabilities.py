from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

LLVM_CAPABILITY_MODE = "objc3c-llvm-capabilities-v2"


@dataclass(frozen=True)
class LLVMCapabilitySummary:
    summary_path: str
    clang_path: str
    clang_found: bool
    llc_path: str
    llc_found: bool
    llc_supports_filetype_obj: bool
    parity_ready: bool
    blockers: tuple[str, ...]


def read_capability_summary(path: Path) -> LLVMCapabilitySummary:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"llvm capabilities summary missing: {display_path(path)}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"llvm capabilities summary parse error at {display_path(path)}: {exc}"
        ) from exc

    if not isinstance(payload, dict):
        raise ValueError(
            f"llvm capabilities summary must be a JSON object: {display_path(path)}"
        )

    mode = payload.get("mode")
    if mode != LLVM_CAPABILITY_MODE:
        raise ValueError(
            "llvm capabilities summary mode mismatch: "
            f"expected {LLVM_CAPABILITY_MODE!r}, observed {mode!r}"
        )

    def require_object(container: dict[str, Any], key: str) -> dict[str, Any]:
        value = container.get(key)
        if not isinstance(value, dict):
            raise ValueError(
                f"llvm capabilities summary field '{key}' must be an object"
            )
        return value

    def require_bool(container: dict[str, Any], key: str) -> bool:
        value = container.get(key)
        if not isinstance(value, bool):
            raise ValueError(
                f"llvm capabilities summary field '{key}' must be boolean"
            )
        return value

    def require_str(container: dict[str, Any], key: str) -> str:
        value = container.get(key)
        if not isinstance(value, str) or not value.strip():
            raise ValueError(
                f"llvm capabilities summary field '{key}' must be a non-empty string"
            )
        return value

    clang = require_object(payload, "clang")
    llc = require_object(payload, "llc")
    llc_features = require_object(payload, "llc_features")
    sema_type_system_parity = require_object(payload, "sema_type_system_parity")
    blockers_raw = sema_type_system_parity.get("blockers")
    if not isinstance(blockers_raw, list) or not all(
        isinstance(item, str) for item in blockers_raw
    ):
        raise ValueError(
            "llvm capabilities summary field 'sema_type_system_parity.blockers' "
            "must be a list of strings"
        )

    return LLVMCapabilitySummary(
        summary_path=display_path(path),
        clang_path=require_str(clang, "path"),
        clang_found=require_bool(clang, "found"),
        llc_path=require_str(llc, "path"),
        llc_found=require_bool(llc, "found"),
        llc_supports_filetype_obj=require_bool(llc_features, "supports_filetype_obj"),
        parity_ready=require_bool(sema_type_system_parity, "parity_ready"),
        blockers=tuple(blockers_raw),
    )





