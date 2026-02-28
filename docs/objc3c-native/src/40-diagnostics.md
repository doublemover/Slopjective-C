<!-- markdownlint-disable-file MD041 -->

## O3S201..O3S216 behavior (implemented now)

- `O3S201`:
  - Duplicate parameter names within a single function parameter list.
  - Duplicate local `let` declaration names in the same lexical scope.
- `O3S202`:
  - Identifier lookup failed in local scopes and globals.
- `O3S203`:
  - Call target name does not match a declared function.
- `O3S204`:
  - Function exists but call argument count does not match declared arity.
- `O3S205`:
  - Function body is not proven to always return.
  - A body is accepted when an explicit `return` is guaranteed by control flow (including `if/else` where both branches always return).
- `O3S206`:
  - Function symbol used as a value expression.
  - Arithmetic/relational operands with non-`i32` type.
  - Equality between incompatible types, except `bool` compared with literal `0` or `1`.
  - Logical operands that are neither `bool` nor `i32`.
  - Call argument type mismatch against declared parameter types.
  - Message argument type mismatch.
  - `i32` arguments are accepted for `bool` parameters at call sites.
- `O3S207`:
  - Message receiver is not `i32`-compatible.
- `O3S208`:
  - Message-send expressions exceed configured arg cap (default `4`, configurable via `--objc3-max-message-args`).
- `O3S209`:
  - Message argument expression is not `i32`-compatible.
- `O3S211`:
  - Return expression type does not match the declared function return type.
  - `bool` return annotations accept `0`/`1` literals as bool-compatible returns.
- `O3S212`:
  - `break` used outside an active loop context.
- `O3S213`:
  - `continue` used outside an active loop context.
- `O3S214`:
  - Assignment target must be an existing mutable local or global symbol.
- `O3S216`:
  - migration assist requires canonical literal replacement for legacy Objective-C aliases (`YES`, `NO`, `NULL`) in canonical compatibility mode.

Parser/lexer diagnostics currently emitted include:

- `O3L001` unexpected character
- `O3L002` unterminated block comment
- `O3L003` nested block comments are unsupported
- `O3L004` stray block comment terminator
- `O3P100` unsupported top-level statement
  - Also emitted for invalid declaration forms missing `fn` after `pure`/`extern`.
  - Also emitted for duplicate declaration qualifiers (`duplicate 'pure' qualifier in function declaration`, `duplicate 'extern' qualifier in function declaration`).
  - Also emitted for misplaced declaration qualifiers after `fn` (`unexpected qualifier 'pure' after 'fn'`, `unexpected qualifier 'extern' after 'fn'`).
  - Also emitted for misplaced declaration qualifiers after function identifiers (`unexpected qualifier 'pure' after function name`, `unexpected qualifier 'extern' after function name`).
  - Also emitted for misplaced declaration qualifiers after parameter lists (`unexpected qualifier 'pure' after parameter list`, `unexpected qualifier 'extern' after parameter list`).
  - Also emitted for misplaced declaration qualifiers after function return annotations (`unexpected qualifier 'pure' after function return annotation`, `unexpected qualifier 'extern' after function return annotation`).
  - Also emitted for misplaced qualifiers in parameter type annotations (`unexpected qualifier 'pure' in parameter type annotation`, `unexpected qualifier 'extern' in parameter type annotation`).
  - Also emitted for misplaced qualifiers in function return type annotations (`unexpected qualifier 'pure' in function return type annotation`, `unexpected qualifier 'extern' in function return type annotation`).
  - Also emitted for misplaced qualifiers after parameter type annotations (`unexpected qualifier 'pure' after parameter type annotation`, `unexpected qualifier 'extern' after parameter type annotation`).
  - Also emitted for misplaced qualifiers in parameter identifier positions (`unexpected qualifier 'pure' in parameter identifier position`, `unexpected qualifier 'extern' in parameter identifier position`).
  - Also emitted for misplaced qualifiers after parameter names (`unexpected qualifier 'pure' after parameter name`, `unexpected qualifier 'extern' after parameter name`).
  - Also emitted for misplaced qualifiers in statement positions (`unexpected qualifier 'pure' in statement position`, `unexpected qualifier 'extern' in statement position`).
  - Also emitted for misplaced qualifiers in expression positions (`unexpected qualifier 'pure' in expression position`, `unexpected qualifier 'extern' in expression position`).
  - Also emitted for misplaced qualifiers in message selector positions (`unexpected qualifier 'pure' in message selector position`, `unexpected qualifier 'extern' in message selector position`, `unexpected qualifier 'pure' in keyword selector segment position`, `unexpected qualifier 'extern' in keyword selector segment position`).
  - Also emitted for misplaced qualifiers in case-label expressions (`unexpected qualifier 'pure' in case label expression`, `unexpected qualifier 'extern' in case label expression`).
- `O3P101` invalid identifier in declaration positions
- `O3P102` missing `=`
- `O3P103` invalid expression
  - Also emitted for malformed switch case-label expressions, including signed-label forms missing numeric literals.
- `O3P104` missing `;`
  - Also emitted for `extern`-qualified declarations that do not terminate as prototypes.
- `O3P106` missing `(`
- `O3P107` missing `:`
- `O3P108` expected parameter type `i32`, `bool`, `BOOL`, `NSInteger`, `NSUInteger`, or `id`
  - Also accepts `Class`.
  - Also accepts `SEL`.
  - Also accepts `Protocol`.
  - Also accepts `instancetype`.
  - Also used for unsupported named parameter types and malformed generic suffix parse forms.
- `O3P109` missing `)`
- `O3P110` missing `{`
- `O3P111` missing `}`
- `O3P112` call target must be identifier
- `O3P113` invalid or unterminated message-send expression
  - Receiver fallback diagnostic is emitted only when receiver parsing fails without a prior primary receiver diagnostic.
- `O3P114` malformed or unsupported function return annotation type
  - Also accepts `Class`.
  - Also accepts `SEL`.
  - Also accepts `Protocol`.
  - Also accepts `instancetype`.
  - Also used for unterminated generic function return type suffix.

## Sema diagnostics bus contract (M139-E001)

- The frontend diagnostics bus is defined at:
  - `native/objc3c/src/parse/objc3_diagnostics_bus.h`
  - `Objc3FrontendDiagnosticsBus.{lexer,parser,semantic}` is the canonical deterministic diagnostics transport packet.
- Pipeline sema pass manager flow remains deterministic and fail-closed:
  - `RunObjc3SemaPassManager(...)` publishes each pass batch into `result.stage_diagnostics.semantic`.
  - `BuildIntegrationSurface`, `ValidateBodies`, and `ValidatePureContract` pass IDs run in deterministic order.
- Stage-local diagnostics are folded into final parsed-program diagnostics through:
  - `TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program)`
  - deterministic insert order: `lexer`, then `parser`, then `semantic`.

