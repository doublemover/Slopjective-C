# Native build and installation costs

The operational owner extends #8291 using work already selected by native CI.
It starts no build, installer or archive qualification. The foundation builder
records its initial directory/cache state, source preparation, source/config/tool
identity, host limits, actual phase commands, durations and log digests. Empty
build directories are labeled cold; exact-input incremental state requires the
passing prior source and configure-plan identity. Changed inputs and unqualified
existing trees remain separate. Shared compiler/filesystem caches are not cleared
or described as cold.

The installer records archive admission, extraction, seal verification/publication,
activation and cleanup separately, with archive bytes/digest, SDK identity, actual
decoder identity, Python/host context and whether the destination already existed.
Existing prefixes still decode and verify the archive; reuse is not zero work.
Postcommit cleanup warnings retain successful activation and are preserved with
the observation. No new fallible filesystem work follows the activation commit.

`semantic_build_costs` accepts only passing complete receipts, verifies the
foundation's raw command logs, rejects duplicate/reformatted copies and groups
observations only when source/configuration/host/instrumentation identities and
initial states match. It reports per-phase distributions and actual observation
counts. A single observation is not a calibrated baseline, and timing noise does
not become an invented product allowance.

```sh
python3 -B -m scripts.objc3c_toolchain.semantic_build_costs \
  --foundation out/toolchains/objc3-development/build-linux-x86_64/foundation-build-report.json \
  --installation /path/to/native-evidence/package/release.json \
  --output out/semantic-build-costs
```

The native owner automatically writes `operational-costs.json` from the existing
foundation and selected package receipts. Its terminal gate verifies the artifact
hash and recomputes the complete snapshot after relocation. Missing measurements,
changed logs, false cold/reuse labels, unrelated archive identities, invalid
intervals or a fabricated performance-pass decision fail. The cost artifact joins
the existing 256 MiB evidence budget and introduces no new cadence or matrix.

Real native controls execute a small CMake/CTest/install project through cold,
two unchanged and changed-source builds. Real small sealed archives exercise
first installation, existing-prefix readmission, relocation and a bounded
verifier-delay control with unchanged SDK bytes. Those controls validate the
measurement/evidence mechanisms, not LLVM build speed or a replacement SDK.
Actual matching LLVM/SDK runner observations and reviewed product goals are still
required before any performance gate can be promoted. The current decision stays
`advisory-inconclusive-uncalibrated`.
