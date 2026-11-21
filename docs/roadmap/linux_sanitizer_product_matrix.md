# Parallel sanitizer product qualification

The native Linux campaign distributes its thirteen sanitizer product owners
across eight jobs, using one exact SDK archive and one exact instrumented compiler
producer. Runtime schedule qualification remains an independent consumer.

| Partition | Product owners                               |
| --------- | -------------------------------------------- |
| 0         | Structured concurrency                       |
| 1         | Native async                                 |
| 2         | C++ task coroutines                          |
| 3         | C frame bridge and installed ownership       |
| 4         | Feature witnesses at O0, ASan/UBSan and TSan |
| 5         | Feature witnesses at O2, ASan/UBSan and TSan |
| 6         | Factory witnesses at O0, ASan/UBSan and TSan |
| 7         | Factory witnesses at O2, ASan/UBSan and TSan |

Each job builds only its required runtime profiles. Only the two factory
partitions build the instrumented frontend facade. Independent owners retain
bounded concurrency and preserve every failure. The complete local campaign
entry point remains available for an explicit all-owner run.

The core terminal job requires all eight distinct partition receipts, the exact
revision/plan, the same sealed SDK identity, matching source inventories and
passing owner results. It rehashes the fourteen campaign implementation inputs
against the checked-out source. Missing, duplicated, overlapping, substituted or
changed evidence fails admission. The resulting canonical campaign still
contains all thirteen product owners, fourteen product-evidence receipts and all
six sanitizer/optimization runtime configurations.

The terminal merge preserves owner receipts and exact retained replay bytes,
consolidating partition stores into one content-addressed campaign store. It
checks byte identity on collisions and validates replay bindings after relocation.
Repeated bindings are verified once per owner/record identity. The existing
3 GiB replay allowance remains a campaign-wide bound. Temporary merge trees are
removed before publishing canonical core evidence.

138 tooling checks pass, including all partition selections, profile minimization,
source-checkout ownership, byte replay, corruption/substitution rejection,
workflow enrollment and integration with the existing core aggregate. Actual
parallel wall-time measurements and full new-source SDK acceptance remain CI
work; no speedup or SDK promotion is claimed from these unit controls.
