# Checked C++ results with implicit destructors

Source 2.142 / System 2.68 prepares implicit nontrivial C++ result destructors
before checked callable ABI admission. Sema declares the actual destructor and
resolves its exception specification for complete result records; dependent and
incomplete types retain their deferred requirements. This covers ordinary
functions, function pointers, Blocks and Objective-C methods. Throwing or deleted
destructors keep the existing admission boundary.

Native source records with ordinary C++ resources or ARC strong/weak/Block fields
now have the same success-only result ownership as records with explicit
destructor declarations. Controls cover direct/indirect/Block/message calls,
optionalization, discarded results, typed/native failures before and after
construction, and return-cleanup failures under ARC/MRC and C++-only exceptions.
No failure path fabricates or destroys an unconstructed result.

The hard cutover `objc3.checked-cxx-record-results.v2` also validates the marker in
ownership identities, so a declaration-only interface requires the reader
capability even without a published call body. Missing/invented capabilities,
older readers, relocation, templates, source-free replay and native AST import
have controls. The [verification receipt](../support/linux_implicit_checked_record_results_progress.json)
records completed checks: all 49 dedicated compiler commands, 59 AST controls,
two source-free profiles, 540 decoder replays and four native ASan/UBSan
configurations / fourteen commands pass. All five adjacent frontend groups also pass. Public ABI 24/frame 1/services 4/schema 2.2 retain their
layouts; final analysis and one matched SDK follow the remaining feature batch.
