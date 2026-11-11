# Native finally control flow

System source 2.38 models Objective-C `@finally` as a registered action around
the entire protected try/catch. A matching catch runs before the finalizer.
Normal completion, returns, gotos, loop exits, unmatched native exceptions and
exceptions from a catch each execute the finalizer once. Catch bindings and
body defers clean up before finalization; enclosing owner claims remain active
until their own cleanup regions end.

A finalizer can replace a pending transfer with a return, throw or outward jump.
It therefore retains the surrounding handler and lexical break/continue targets,
rather than inheriting a defer destructor's second-exception termination rule.
Each emitted cleanup copy has its own local label mapping. Forward and backward
outward jumps keep function-level backpatching, including nested cleanup copies.
Existing bounded borrowed analysis checks the finalizer's owner mutations and
releases before a returned view or later consumer is admitted. Independently
stable finalizer returns can replace a previously pending borrowed return.

Native controls exposed a GNUstep Objective-C++ lowering defect: raw unwind
resumption did not mark an exception as rethrown before end-catch cleanup. That
could destroy a C++ payload before an outer handler read it. The runtime entry
now uses the C++ ABI's `__cxa_rethrow`, including bare Objective-C rethrows in
Objective-C++. This preserves exception ownership through the enclosing catch.
An IR regression and an instrumented mixed-language witness defend the fix.

Published finalizer and bare native rethrow bodies require `objc3.native-finally.v1` with matching
canonical identity. All codec, artifact, module and import readers enforce exact
agreement. Private CFG registration is rebuilt from the published body without
changing public AST layouts, physical extension ABI 24 or native service/frame
layouts. The `runtime-scopes` optional feature also installs the native finally
witness for C17/C++20/23, ARC/MRC, source/PCH and O0/O2.

Controls include owner expiry in and after finalizers, pinned snapshots, catch
binding lifetime, nested finalizers, local/outward labels, borrowed return cleanup,
exception replacement and mixed C++ propagation. Native checks assert exact
defer/handler/finalizer order, destruction counts, loop transfers, return and
exception suppression, replacement exception identity and live payload access.
Source-free controls cover replay, relocation, capability admission and expired
consumers across six language/ownership profiles. All 76 affected compiler cases,
six source-free profiles, four adjacent frontend groups, codec/isolation and all
nineteen AST controls pass. Twelve instrumented runtime-scope configurations /
42 commands and twelve existing native frame-exception configurations / 42
commands pass with ASan/UBSan, including the previously failing C++ payload read.

Partial-construction cleanup, open callable and global/interprocedural ownership
precision, complete profile conformance and the final matched SDK remain separate
roadmap obligations.
