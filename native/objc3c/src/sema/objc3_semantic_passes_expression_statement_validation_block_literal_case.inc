    case Expr::Kind::BlockLiteral: {
      // block-runtime-semantic-rules freeze anchor: the current
      // block-literal semantic rule boundary only proves deterministic
      // parameter/capture metadata on the source surface and classifies the
      // value as function-shaped; runnable invocation and runtime object
      // semantics remain outside this boundary.
      // capture-legality, escape-classification, and invocation
      // typing anchor: lane-B now turns the parser-owned capture/source
      // annotations into live sema checks and a callable block signature for
      // source-only invocation typing while native block execution still fails
      // closed in later gates.
      const bool parameter_count_match = expr->block_parameter_names_lexicographic.size() == expr->block_parameter_count;
      const bool capture_count_match = expr->block_capture_names_lexicographic.size() == expr->block_capture_count;
      const bool parameters_deterministic =
          parameter_count_match && IsSortedUniqueStrings(expr->block_parameter_names_lexicographic);
      const bool captures_deterministic =
          capture_count_match && IsSortedUniqueStrings(expr->block_capture_names_lexicographic);

      if (!parameters_deterministic || !captures_deterministic) {
        diagnostics.push_back(MakeDiag(expr->line,
                                       expr->column,
                                       "O3S206",
                                       "type mismatch: block literal capture metadata must be deterministic"));
      }
      if (!expr->block_capture_set_deterministic) {
        diagnostics.push_back(MakeDiag(expr->line,
                                       expr->column,
                                       "O3S206",
                                       "type mismatch: block literal capture-set normalization failed"));
      }
      if (!expr->block_literal_is_normalized) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S206", "type mismatch: block literal semantic surface is not normalized"));
      }
      if (expr->block_capture_count > 0u && expr->block_capture_profile.empty()) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S206", "type mismatch: block literal capture profile is missing"));
      }
      ValidateBlockLiteralCaptureLegality(expr, scopes, globals, diagnostics);
      return MakeCallableSemanticTypeFromBlockLiteral(*expr);
    }
