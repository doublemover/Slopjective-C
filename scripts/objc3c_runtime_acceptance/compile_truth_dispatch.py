"""Runtime dispatch truth checks for native compile outputs."""

from __future__ import annotations

from dataclasses import dataclass
import re
from typing import Any


@dataclass(frozen=True)
class RuntimeDispatchTruth:
    symbol: str
    declaration_count: int
    call_count: int

    @property
    def declaration_present(self) -> bool:
        return self.declaration_count >= 1

    def failures(self) -> list[str]:
        if self.declaration_present:
            return []
        return [f"missing LLVM declaration for runtime dispatch symbol '{self.symbol}'"]

    def payload_fields(self) -> dict[str, object]:
        return {
            "runtime_dispatch_symbol": self.symbol,
            "runtime_dispatch_declaration_count": self.declaration_count,
            "runtime_dispatch_call_count": self.call_count,
        }


def runtime_dispatch_symbol_from_manifest(manifest: dict[str, Any]) -> str:
    lowering = manifest.get("lowering", {})
    runtime_dispatch_symbol = ""
    if isinstance(lowering, dict):
        runtime_dispatch_symbol = str(lowering.get("runtime_dispatch_symbol", ""))
    if runtime_dispatch_symbol == "":
        runtime_dispatch_symbol = str(
            manifest.get(
                "runtime_support_library_link_wiring_runtime_dispatch_symbol",
                "",
            )
        )
    if runtime_dispatch_symbol == "":
        runtime_dispatch_symbol = str(
            manifest.get("runtime_link_host_link_runtime_dispatch_symbol", "")
        )
    if runtime_dispatch_symbol == "":
        raise RuntimeError(
            "compile output truthfulness check could not resolve the runtime dispatch symbol"
        )
    return runtime_dispatch_symbol


def runtime_dispatch_truth_from_outputs(
    *,
    manifest: dict[str, Any],
    ll_text: str,
) -> RuntimeDispatchTruth:
    runtime_dispatch_symbol = runtime_dispatch_symbol_from_manifest(manifest)
    escaped_dispatch_symbol = re.escape(runtime_dispatch_symbol)
    return RuntimeDispatchTruth(
        symbol=runtime_dispatch_symbol,
        declaration_count=len(
            re.findall(r"(?m)declare i32 @" + escaped_dispatch_symbol + r"\(", ll_text)
        ),
        call_count=len(
            re.findall(r"(?m)call i32 @" + escaped_dispatch_symbol + r"\(", ll_text)
        ),
    )


__all__ = [
    "RuntimeDispatchTruth",
    "runtime_dispatch_symbol_from_manifest",
    "runtime_dispatch_truth_from_outputs",
]
