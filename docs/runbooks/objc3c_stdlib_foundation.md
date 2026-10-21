# Native library foundation

The authoritative module inventory is `stdlib/module_inventory.json`. Public headers and the Clang module map live in `stdlib/include/objc3`; runtime declarations live in `native/objc3c/src/runtime/public`. The retired frontend workspace/package contracts have been removed.

See the [standard-library guide](../../stdlib/README.md) for imports, ownership, linking and the executable integration gate, and the [Linux roadmap](../roadmap/linux_forward_development.md) for the complete implementation sequence.
