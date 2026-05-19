"""Storage/reflection owner contract model."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from .storage_reflection_owner_contract_inventory import (
    STORAGE_REFLECTION_OWNER_CONTRACT_ID,
)


@dataclass(frozen=True)
class StorageReflectionOwnerContract:
    owner_id: str
    owner_surface: str
    case_ids: tuple[str, ...]
    source_modules: tuple[str, ...]
    owned_decisions: tuple[str, ...]

    def payload(self) -> dict[str, Any]:
        return {
            "contract_id": STORAGE_REFLECTION_OWNER_CONTRACT_ID,
            "owner_id": self.owner_id,
            "owner_surface": self.owner_surface,
            "case_ids": list(self.case_ids),
            "source_modules": list(self.source_modules),
            "owned_decisions": list(self.owned_decisions),
        }


__all__ = ["StorageReflectionOwnerContract"]
