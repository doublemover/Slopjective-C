from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from c_api_smoke_support import (
    C_API_HELPER_CONTRACT,
    C_API_SOURCES,
    FRONTEND_COMPILE_CONTRACT_CPP,
    FRONTEND_OPTION_BORROWING_CPP,
    OWNED_STRING_STORAGE_CPP,
    RESULT_PAYLOAD_CPP,
    RESULT_OWNERSHIP_CPP,
    c_api_source_text,
    c_api_surface_text,
    read_text,
    squash_ws,
)


def c_api_source_paths() -> list[Path]:
    return C_API_SOURCES


def c_api_source_and_squashed_text() -> tuple[str, str]:
    source = c_api_source_text()
    return source, squash_ws(source)


def c_api_helper_contract_payload() -> dict[str, Any]:
    return json.loads(read_text(C_API_HELPER_CONTRACT))


def c_api_surface_and_implementation_text() -> tuple[str, str, str]:
    c_api_surface = c_api_surface_text()
    source = c_api_source_text()
    implementation = "\n".join(
        [
            source,
            read_text(RESULT_OWNERSHIP_CPP),
            read_text(OWNED_STRING_STORAGE_CPP),
            read_text(RESULT_PAYLOAD_CPP),
        ]
    )
    return c_api_surface, source, implementation


def frontend_result_ownership_texts() -> tuple[str, str]:
    ownership = "\n".join(
        [
            read_text(RESULT_OWNERSHIP_CPP),
            read_text(OWNED_STRING_STORAGE_CPP),
            read_text(RESULT_PAYLOAD_CPP),
        ]
    )
    compile_contract = "\n".join(
        [
            read_text(FRONTEND_COMPILE_CONTRACT_CPP),
            read_text(FRONTEND_OPTION_BORROWING_CPP),
        ]
    )
    return ownership, compile_contract
