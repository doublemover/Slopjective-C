# Actual sanitizer product evidence

Issue [#8282](https://github.com/doublemover/Slopjective-C/issues/8282) requires
both detector controls and evidence from the actual project processes. The shared
product owner now records exact compiler/link/execution argv, working directory,
effective environment, exit/signal, duration and complete diagnostic hash.
Compiler version probes and pre-execution loader graphs bind the selected route.

Fixture/header inputs, real compiler dependency closures, generated sources,
interfaces, objects, PCH and client executables are retained in content-addressed
Zstandard blobs. Each blob carries compressed and uncompressed hashes and sizes.
Retention has per-file and per-owner byte limits. Inputs and retained evidence
are checked again before promotion. Wrong-runtime substitution at the same path,
removed sanitizer flags, wrong resources, unexpected loader routes and changed
logs/artifacts have negative controls. Failures remain in the parent receipt.

The seven migrated product runners cover authored features, source-free
factories, native async, task coroutines, structured runtime, the C frame bridge,
and installed ownership projections. Multi-TU commands compile separate objects
so each source has a real dependency record. Pure C bridge clients retain their
libobjc-free contract. Installed ownership instruments provider/re-export/client
objects and uses the SDK runtime as shipped; it does not claim that runtime is
instrumented. Other runtime profiles separately prove all seventeen service TUs
and the selected GNUstep dependency.

The campaign builds six runtime profiles once, before parallel readers start,
then runs thirteen owner batches in at most four workers. Individual receipts
list only profiles actually used. The default owns fresh output and performs
installed-tool/fixture preflight before building runtimes. Independent owner
failures are retained while other selected owners complete. No SDK archive is
built by this campaign.

```sh
python3 -B -m scripts.objc3c_toolchain.sanitizer_products \
  --sdk /path/to/sealed-prefix --compiler-root /path/to/matched/compiler \
  --facade /path/to/matched/objc3c --resource /path/to/sanitizer/resources \
  --output out/sanitizer-products --jobs 4

python3 -B -m scripts.objc3c_toolchain.product_evidence \
  --receipt /path/to/product-evidence/qualification.json \
  --command CASE-execute --output /path/to/fresh/replay
```

Replay restores retained inputs into a fresh directory and executes the exact
saved client with its immutable compiler/resource/runtime route. Compilation
commands and exact source/artifact bytes are retained separately; this command
does not claim to recompile a complete PCH/interface pipeline automatically.

The initial structured/native-async/coroutine checkpoint passed. A missing
development-prefix inspector stopped its C bridge owner before execution and is
repaired by full preflight. The final campaign uses the compiler with the
dependent-diagnostic storage fix. Its complete current-input matrices must pass
before the original product-evidence requirement closes.
