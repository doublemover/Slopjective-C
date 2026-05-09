"""Package install receipt contracts."""

from __future__ import annotations

from .action_catalog_package_public_workflows import PackageSchemaContract

PACKAGE_INSTALL_RECEIPT_SCHEMA = PackageSchemaContract(
    contract_key="install_receipt",
    schema_path="schemas/objc3c-package-install-receipt-v1.schema.json",
    schema_id="https://objc3c.dev/schemas/objc3c-package-install-receipt-v1.schema.json",
    document_contract_id="objc3c.packaging.channels.install-receipt.v1",
)

PACKAGE_INSTALL_RECEIPT_PATH = "objc3c-install-receipt.json"
PACKAGE_INSTALL_RECEIPT_PACKAGE_BRIDGE = "objc3c"
PACKAGE_INSTALL_RECEIPT_BOOTSTRAP_ENTRYPOINT = "Bootstrap-objc3cEnvironment.ps1"


__all__ = [
    "PACKAGE_INSTALL_RECEIPT_BOOTSTRAP_ENTRYPOINT",
    "PACKAGE_INSTALL_RECEIPT_PACKAGE_BRIDGE",
    "PACKAGE_INSTALL_RECEIPT_PATH",
    "PACKAGE_INSTALL_RECEIPT_SCHEMA",
]
