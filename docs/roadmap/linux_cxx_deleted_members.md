# Deleted C++ aggregate member identity

The concurrency 2.8.1 delivery makes deleted ordinary methods on public
aggregate records participate in `objc3.cxx-class-lifecycles.v1`. Their canonical
member identity now includes deletion state even when the record has no user
constructor, destructor, private field or conversion function.

The 2.8.0 metadata audit reproduced an identical semantic fingerprint for an
ordinary method and its deleted form. Source binding alone cannot protect this
boundary: semantic fingerprints intentionally exclude source formatting. The
classifier now recognizes any explicit deleted method. A maintained regression
requires different fingerprints and checks the required capability and deleted
member identity. Source-free C++ package consumers use a dependent requires
expression to prove that the imported method is unavailable.

The preceding [2.8.0 SDK](../support/linux_cxx_class_lifecycle_acceptance.json)
retains its full foundation, installed and fresh-distribution evidence. The
[corrected SDK acceptance](../support/linux_cxx_deleted_member_acceptance.json)
records the active archive and source checkpoint. LLVM, GNU runtime sources,
runtime services and
public ABI revisions are unchanged. All 33 installed gates pass against the
new SDK; fresh Debian/Ubuntu checks pass the affected class/package boundary,
including installation, relocation, ordinary-user consumers and debugger audits.
The broader fresh feature matrix retains its explicit 2.8.0 archive scope.

All eight C++20/23 ARC/MRC O0/O2 package preflights pass in each normal,
ASan/UBSan and TSan mode (24 cases / 96 commands). Four interface suites and
29 tooling checks pass. Installed qualification passes 32 facade suites, nine
GNUstep clients, 37 consumer checks and 13 integration nodes (76 runtime checks).
The five-family installed matrix passes 168 configurations / 966 commands.
Fresh class/package qualification passes 12 async package modes / 36 executions
and 12 C++ interface executions / 98 commands / 18 rejections per distribution,
with both owned containers removed. The LLVM foundation retains the accepted
370 compiler regressions and 166 native contracts; its bytes are reused.

After the checker-only correction, 3,893 compiler/runtime/header files were
verified byte-for-byte identical. The 32 facade and nine GNUstep client results
retain that build evidence. Installed consumer checks, native integration and
all 33 installed gates ran again against the corrected prefix. Direct AST-import
probes also preserve deletion state and layout in C++20 and C++23 after provider
removal.
