static void DiagnoseUnsupportedFunctionFeatureClaims(
    const FunctionDecl &fn,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  // ARC mode-handling core implementation anchor: executable
  // function ownership qualifiers become runnable only under explicit
  // -fobjc-arc mode while the non-ARC path remains fail-closed.
  // runtime-backed-object-ownership freeze anchor: the current
  // runnable ownership slice is still limited to object/property/accessor
  // ownership profiles plus the legacy ownership summary lanes below. ARC
  // ownership qualifiers on executable functions/methods remain fail-closed
  // until the later runtime-backed ownership implementation work lands.
  // retainable-object semantic-rule freeze anchor: runtime-backed
  // property/member ownership metadata is truthful, but executable
  // retain/release legality, `@autoreleasepool`, and destruction ordering
  // remain fail-closed until the later storage legality/runtime work.
  // ownership-lowering baseline freeze anchor: lane-C preserves the
  // legacy ownership lowering summaries those later runtime issues must
  // replace, but no executable retain/release/autorelease/weak runtime hooks
  // are emitted out of function-level ownership qualifiers yet.
  // ARC cleanup/weak/lifetime lowering anchor: sema still only
  // publishes the canonical ownership and block-interaction packets; scope
  // cleanup scheduling, weak current-property helper calls, and block-capture
  // lifetime cleanup remain the responsibility of lane-C lowering.
  // ARC/block autorelease-return lowering anchor: sema still only
  // publishes return-autorelease and block-escape intent; terminal branch
  // cleanup ordering and escaping-block/autorelease-return composition remain
  // the responsibility of lane-C lowering.
  // runnable-arc-runtime gate anchor: lane-E freezes the supported
  // ARC slice against parser-only or metadata-only claims; sema remains the
  // truthful source of the A/B-lane legality and ownership packets it consumes.
  // runnable-arc-closeout anchor: lane-E closes the current ARC
  // tranche by consuming this preserved semantic state through integrated smoke
  // and runbook proof rather than widening semantic acceptance here.
  if (fn.throws_declared && ShouldRejectThrowsFeatureClaim(context)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.throws_source_rejection_site_count,
        fn.line,
        fn.column,
        "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  const bool allow_source_only_block_owned_bindings =
      context.allow_source_only_block_literals &&
      FunctionBodyContainsBlockLiteral(fn);
  const bool allow_runnable_block_owned_bindings =
      FunctionBodyContainsRunnableBlockLiteral(fn);
  for (const auto &param : fn.params) {
    if (!param.has_ownership_qualifier) {
      continue;
    }
    if (!ShouldRejectOwnershipQualifierFeatureClaim(
            context,
            allow_source_only_block_owned_bindings,
            allow_runnable_block_owned_bindings)) {
      // byref/copy-dispose/object-ownership anchor: source-only block
      // semantic modeling may admit ownership-qualified executable bindings so
      // lane-B can classify owned vs weak/unowned captures without widening the
      // native runnable ARC surface.
      continue;
    }
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(param.ownership_qualifier_tokens, param.line),
        OwnershipQualifierColumn(param.ownership_qualifier_tokens, param.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  if (fn.has_return_ownership_qualifier &&
      ShouldRejectOwnershipQualifierFeatureClaim(
          context,
          allow_source_only_block_owned_bindings,
          allow_runnable_block_owned_bindings)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(
            fn.return_ownership_qualifier_tokens, fn.line),
        OwnershipQualifierColumn(
            fn.return_ownership_qualifier_tokens, fn.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  for (const auto &stmt : fn.body) {
    DiagnoseUnsupportedFeatureClaimsInStmt(
        stmt.get(), diagnostics, stats, context);
  }
}

static void DiagnoseUnsupportedMethodFeatureClaims(
    const Objc3MethodDecl &method,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  // ARC mode-handling core implementation anchor: executable method
  // ownership qualifiers become runnable only under explicit -fobjc-arc mode
  // while the non-ARC path remains fail-closed.
  // runtime-backed-object-ownership freeze anchor: method-level ARC
  // ownership qualifiers still sit outside the truthful runnable object-model
  // claim even though property/member ownership profiles are already preserved
  // through the object runtime metadata path.
  // retainable-object semantic-rule freeze anchor: method-level
  // retain/release legality and destruction-order behavior remain deferred even
  // though runtime-backed property/member ownership metadata is already live.
  // ownership-lowering baseline freeze anchor: method-level
  // ownership qualifiers still terminate in semantic unsupported-feature
  // diagnostics while the preserved lowering summaries remain the only
  // ownership hook surface available to IR closeout.
  if (method.throws_declared && ShouldRejectThrowsFeatureClaim(context)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.throws_source_rejection_site_count,
        method.line,
        method.column,
        "unsupported feature claim: 'throws' is not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  const bool allow_source_only_block_owned_bindings =
      context.allow_source_only_block_literals &&
      MethodBodyContainsBlockLiteral(method);
  const bool allow_runnable_block_owned_bindings =
      MethodBodyContainsRunnableBlockLiteral(method);
  for (const auto &param : method.params) {
    if (!param.has_ownership_qualifier) {
      continue;
    }
    if (!ShouldRejectOwnershipQualifierFeatureClaim(
            context,
            allow_source_only_block_owned_bindings,
            allow_runnable_block_owned_bindings)) {
      // byref/copy-dispose/object-ownership anchor: source-only block
      // semantic modeling may admit ownership-qualified executable bindings so
      // lane-B can classify owned vs weak/unowned captures without widening the
      // native runnable ARC surface.
      continue;
    }
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(param.ownership_qualifier_tokens, param.line),
        OwnershipQualifierColumn(param.ownership_qualifier_tokens, param.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  if (method.has_return_ownership_qualifier &&
      ShouldRejectOwnershipQualifierFeatureClaim(
          context,
          allow_source_only_block_owned_bindings,
          allow_runnable_block_owned_bindings)) {
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.arc_source_rejection_site_count,
        OwnershipQualifierLine(
            method.return_ownership_qualifier_tokens, method.line),
        OwnershipQualifierColumn(
            method.return_ownership_qualifier_tokens, method.column),
        "unsupported feature claim: ARC ownership qualifiers are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
  }
  for (const auto &stmt : method.body) {
    DiagnoseUnsupportedFeatureClaimsInStmt(
        stmt.get(), diagnostics, stats, context);
  }
}
