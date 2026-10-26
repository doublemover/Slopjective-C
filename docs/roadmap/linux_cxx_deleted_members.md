# Deleted C++ aggregate member identity

The concurrency 2.8.1 source correction makes deleted ordinary methods on public
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

The accepted [2.8.0 SDK](../support/linux_cxx_class_lifecycle_acceptance.json)
retains its full foundation, installed and fresh-distribution evidence. The
corrected SDK is being rebuilt. LLVM, GNU runtime sources, runtime services and
public ABI revisions are unchanged. All installed gates will run against the
new SDK; fresh Debian/Ubuntu checks select the affected class/package boundary,
including installation, relocation, ordinary-user consumers and debugger audits.
The broader fresh feature matrix retains its explicit 2.8.0 archive scope.
