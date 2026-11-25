# Objective-C 3.0 Linux SDK preview

This preview ships the native Clang-based compiler and GNUstep runtime for
Linux x86-64 and AArch64. The attached `linux-release.json` identifies the
qualified source revision, archives, SDK seals, Linux ABI minimums, and hosted
qualification runs. The distribution matrix covers Debian 13 and Ubuntu 26.04.

Download the archive for your architecture, the two installer files, and
`SHA256SUMS`. Follow the [Linux install guide](https://github.com/doublemover/Slopjective-C/blob/main/docs/releases/linux.md#install-and-run).

The attached source archives contain Slopjective-C, including the build
scripts and patches, and the pinned revisions of LLVM, libobjc2, and robin-map.
Use `linux-release.json` to identify the revisions and `SHA256SUMS` to verify
the downloads. Report suspected vulnerabilities through
[GitHub private vulnerability reporting](https://github.com/doublemover/Slopjective-C/security/advisories/new).

Public features include `defer`, `Optional`, `Result`, and typed `throws` in
the modes recorded in the
[language capability catalog](https://github.com/doublemover/Slopjective-C/blob/main/schemas/objc3-language-capabilities.json).
Public async/await and actors remain reserved. Preview qualification covers the
distributed products and the tested features and does not assert whole-profile
conformance.

## Preview 2 changes

- Automatic ARC `Optional` and `Result` locals can initialize their active
  payload by assignment on each control-flow path. The compiler diagnoses
  reads that may precede initialization during syntax checking.
- The SDK includes the matching `llvm-symbolizer` on both Linux architectures
  and checks source-line resolution after installation and relocation.
- A single compiler invocation can combine Objective-C 3, C, and C++ source
  files. Each input receives the language options appropriate to that file.
- Linux libobjc2 publishes association storage only after initializing its
  locks and synchronizes publication of class dispatch tables.
- Swift calling-convention functions can use erased indirect-result buffers
  under UBSan. LLVM treats the ABI marker as an unknown buffer size and still
  checks the bounds of typed results.

The candidate extension ABI is 24. Rebuild Objective-C 3 objects, PCH files,
semantic interfaces, and source packages produced by earlier SDKs, including
the ABI 21 checkpoint. Recompile all dependencies against the selected SDK
before linking or importing them.
