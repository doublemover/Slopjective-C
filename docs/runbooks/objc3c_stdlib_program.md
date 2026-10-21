# Native standard-library development

The current compiler and installed SDK own the public source and package entrypoints. Build native headers/modules, run the installed mixed-language standard-library gate, and refresh the SDK acceptance receipt after packaging. Removed `module`/`fn` sources and synthetic integer helpers are not current inputs.

See the [standard-library guide](../../stdlib/README.md) for imports, ownership, linking and the executable integration gate, and the [Linux roadmap](../roadmap/linux_forward_development.md) for the complete implementation sequence.
