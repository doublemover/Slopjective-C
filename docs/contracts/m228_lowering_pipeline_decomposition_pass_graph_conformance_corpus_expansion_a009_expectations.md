# M228 Lowering Pipeline Decomposition and Pass-Graph Conformance Corpus Expansion Expectations (A010)

Contract ID: `objc3c-lowering-pipeline-pass-graph-conformance-corpus-expansion/m228-a010-v1`
Status: Accepted
Scope: pass-graph conformance-corpus synthesis, fail-closed artifact gate hardening, IR metadata conformance projection, and architecture/spec anchors.

## Objective

Expand A009 conformance-matrix closure with explicit conformance-corpus
consistency and readiness/key gates so pass-graph corpus drift fails closed
before IR emission.

## Required Invariants

1. Core-feature surface carries conformance-corpus fields:
   - `conformance_corpus_consistent`
   - `conformance_corpus_ready`
   - `conformance_corpus_key`
2. Conformance-corpus key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphConformanceCorpusKey(...)`
3. Frontend artifacts fail-close on lane-A conformance corpus before IR emission:
   - `IsObjc3LoweringPipelinePassGraphConformanceCorpusReady(...)`
   - deterministic diagnostic code `O3L313`.
4. IR frontend metadata carries conformance-corpus readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_conformance_corpus_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_conformance_corpus_key`
   - IR output includes `; lowering_pass_graph_conformance_corpus = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a010_lowering_pipeline_decomposition_pass_graph_conformance_corpus_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a010_lowering_pipeline_decomposition_pass_graph_conformance_corpus_contract.py -q`
- `npm run check:objc3c:m228-a010-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A010/lowering_pipeline_pass_graph_conformance_corpus_contract_summary.json`
