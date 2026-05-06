# Compiler Architecture

The native compiler is split into explicit lexer, parser, semantic, lowering,
IR, artifact, driver, and runtime targets in `native/objc3c/CMakeLists.txt`.
Current capability truth lives in `docs/support/capability_matrix.md`.
