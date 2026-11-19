# Objective-C 3.0 Linux SDK preview

This preview ships the native Clang-based compiler and GNUstep runtime for
Linux x86-64 and AArch64. The attached `linux-release.json` identifies the
qualified source revision, archives, SDK seals, Linux ABI minimums, and hosted
qualification runs. The distribution matrix covers Debian 13 and Ubuntu 26.04.

Download the archive for your architecture, the two installer files, and
`SHA256SUMS`. Follow the [Linux install guide](https://github.com/doublemover/Slopjective-C/blob/main/docs/releases/linux.md#install-and-run).

Public features include `defer`, `Optional`, `Result`, and typed `throws` in
the modes recorded in the
[language capability catalog](https://github.com/doublemover/Slopjective-C/blob/main/schemas/objc3-language-capabilities.json).
Public async/await and actors remain reserved. Preview qualification covers the
distributed products and the tested features and does not assert whole-profile
conformance.

The candidate extension ABI is 24. Rebuild Objective-C 3 objects, PCH files,
semantic interfaces, and source packages produced by earlier SDKs, including
the ABI 21 checkpoint. Recompile all dependencies against the selected SDK
before linking or importing them.
