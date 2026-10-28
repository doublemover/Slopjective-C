# Compiler build-stamp isolation

The public `ObjC3Foundation.h` retains stable upstream, language and extension-ABI
contracts and declares `clang::getObjC3PatchDigest()`. The sealed digest lives in
the separate generated `ObjC3BuildIdentity.h`, included only by `Version.cpp`.
Serialization and frontend interface/import identities read the compiled getter.
Changing the stamp therefore rebuilds its implementation without recompiling the
large serialization and interface-emitter translation units.

Compiler version output, PCH/PCM identity and semantic interface extension keys
still bind the complete sealed patch digest. Frontend CMake admission checks the
stable foundation header and separate stamp, plus the actual driver marker.
Linux SDK admission requires and hashes both headers. A missing or stale stamp
fails even when a former combined header contains the expected digest; no
compatibility fallback substitutes for the new contract.

The measured incremental `clang` target rebuild on this four-core instance
compiled only `Version.cpp` and linked in 3.343 seconds after marking the
unchanged generated stamp dirty. Serialization object hashes and modification
times stayed unchanged. Ninja dependency checks also confirm that the frontend
emitter uses the stable API without a stamp-header dependency. This measurement
covers the incremental compiler target; source validation and installed SDK
delivery retain their own records.
