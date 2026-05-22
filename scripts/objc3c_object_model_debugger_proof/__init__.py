"""Object-model debugger proof validation surface."""

from .model import (
    CONTRACT_ID,
    DEFAULT_CONTRACT_PATH,
    VALIDATION_CONTRACT_ID,
    Diagnostic,
    ValidationResult,
    validate_contract_path,
)

__all__ = [
    "CONTRACT_ID",
    "DEFAULT_CONTRACT_PATH",
    "VALIDATION_CONTRACT_ID",
    "Diagnostic",
    "ValidationResult",
    "validate_contract_path",
]
