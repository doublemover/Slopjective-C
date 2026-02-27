# Appendix F - Formal Grammar and Precedence for ObjC 3.0 Additions {#f}

This appendix integrates the grammar additions defined across [Part 2](#part-2), [Part 3](#part-3), [Part 5](#part-5), [Part 6](#part-6), [Part 7](#part-7), and [Part 8](#part-8) into a single EBNF reference.

Unlisted productions are inherited from baseline C/Objective-C grammar as defined in [Part 0](#part-0).

## F.1 Scope and Normative Status {#f-1}

This appendix is normative for:

- parse shape for ObjC 3.0 syntax additions,
- operator precedence/associativity for the integrated expression grammar,
- syntactic disambiguation rules where ObjC 3.0 additions overlap existing C/Objective-C forms.

Static and dynamic semantics remain defined in the feature parts (especially [Part 3](#part-3), [Part 6](#part-6), [Part 7](#part-7), and [Part 12](#part-12)).

## F.2 Notation and Lexical Conventions {#f-2}

- EBNF operators: `[ ... ]` optional, `{ ... }` repetition (zero or more), `|` alternative.
- Quoted terminals use exact token spelling.
- Nonterminals ending in `-base` are inherited unchanged from the baseline grammar.

Tokenization conventions used by this appendix:

- `?.` and `??` are distinct punctuators.
- `?.` takes precedence over `?` followed by `.` when contiguous.
- `??` takes precedence over two consecutive `?` tokens when contiguous.
- `try?` and `try!` are `try` forms in prefix-expression context.
- `PropagationFollowSet = { ')', ']', '}', ',', ';' }`.
- Whitespace breaks multi-character punctuators; for example, `a ? .b` is not `a?.b`.

```ebnf
reserved-keyword-objc3 =
      "defer" | "guard" | "match" | "case" | "let" | "var"
    | "throws" | "throw" | "try" | "do" | "catch"
    | "async" | "await" | "actor" ;

contextual-keyword-objc3 =
      "borrowed" | "move" | "weak" | "unowned" | "strong"
    | "is" | "where" ;

propagation-follow-token = ")" | "]" | "}" | "," | ";" ;
```

## F.3 Integrated EBNF Grammar (ObjC 3.0 Additions) {#f-3}

### F.3.1 Names, Declarations, and Type-Surface Additions {#f-3-1}

```ebnf
module-qualified-name = "@" module-path "." identifier ;
module-path = identifier { "." identifier } ;

optional-type-suffix = "?" | "!" ;

generic-parameter-clause = "<" generic-parameter { "," generic-parameter } ">" ;
generic-parameter = [ variance ] identifier [ generic-constraints ] ;
variance = "__covariant" | "__contravariant" ;
generic-constraints = ":" type-constraint { "," type-constraint } ;

function-effect-specifier =
      "async"
    | "throws"
    | "async" "throws"
    | "throws" "async" ;

throws-specifier = "throws" ;

actor-class-declaration =
    "actor" "class" identifier [ ":" objc-superclass-name ] objc-interface-body ;

keypath-literal = "@keypath" "(" keypath-root "," keypath-components ")" ;
keypath-root = type-name | "self" ;
keypath-components = identifier { "." identifier } ;

resource-local-annotation =
      "@cleanup" "(" identifier ")"
    | "@resource" "(" identifier "," "invalid" ":" constant-expression ")" ;

borrowed-type-qualifier = "borrowed" ;

objc3-attribute-specifier = "__attribute__" "(" "(" objc3-attribute ")" ")" ;
objc3-attribute =
      executor-affinity-attribute
    | task-recognition-attribute
    | resource-attribute
    | cleanup-attribute
    | returns-borrowed-attribute ;

executor-affinity-attribute =
    "objc_executor" "(" executor-affinity-argument ")" ;
executor-affinity-argument =
      "main"
    | "global"
    | "named" "(" string-literal ")" ;

task-recognition-attribute =
      "objc_task_spawn"
    | "objc_task_detached"
    | "objc_task_group" ;

resource-attribute =
    "objc_resource" "(" "close" "=" identifier "," "invalid" "=" constant-expression ")" ;
cleanup-attribute = "cleanup" "(" identifier ")" ;
returns-borrowed-attribute =
    "objc_returns_borrowed" "(" "owner_index" "=" integer-constant ")" ;
```

### F.3.2 Statement and Pattern Additions {#f-3-2}

```ebnf
let-binding-list = ( "let" | "var" ) let-binding { "," let-binding } ;
let-binding = identifier "=" expression ;

if-let-statement =
    "if" let-binding-list compound-statement [ "else" compound-statement ] ;

guard-let-statement =
    "guard" let-binding-list "else" compound-statement ;

guard-condition = expression | let-binding ;
guard-condition-list = guard-condition { "," guard-condition } ;

guard-statement =
    "guard" guard-condition-list "else" compound-statement ;

defer-statement = "defer" compound-statement ;

match-statement =
    "match" "(" expression ")" "{" match-case { match-case } [ match-default ] "}" ;
match-case = "case" pattern ":" compound-statement ;
match-case-guarded-reserved = "case" pattern "where" expression ":" compound-statement ;
(* reserved for future guarded patterns; rejected in ObjC 3.0 v1 *)
match-default = "default" ":" compound-statement ;

match-expression-reserved =
    "match" "(" expression ")" "{" match-expression-case { match-expression-case } [ match-expression-default ] "}" ;
match-expression-case = "case" pattern "=>" expression ";" ;
match-expression-default = "default" "=>" expression ";" ;
(* reserved for ObjC >= 3.1; ObjC 3.0 v1 parsers shall reject with a targeted diagnostic *)

pattern =
      "_"
    | literal
    | binding-pattern
    | result-pattern
    | type-test-pattern ;

binding-pattern = ( "let" | "var" ) identifier ;
result-pattern =
      "." "Ok" "(" binding-pattern ")"
    | "." "Err" "(" binding-pattern ")" ;

type-test-pattern =
      "is" type-name
    | "is" type-name ( "let" | "var" ) identifier ;
(* enabled only when __OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 1 *)

throw-statement = "throw" expression ";" ;

do-statement = "do" compound-statement catch-clause { catch-clause } ;
catch-clause = "catch" [ catch-pattern ] compound-statement ;
catch-pattern = "(" type-name [ identifier ] ")" ;

with-lifetime-statement = "withLifetime" "(" expression ")" statement ;
keep-alive-statement = "keepAlive" "(" expression ")" ";" ;
```

### F.3.3 Expression Additions (Integrated) {#f-3-3}

```ebnf
primary-expression =
      primary-expression-base
    | module-qualified-name
    | keypath-literal
    | message-expression
    | block-literal ;

block-literal =
    "^" [ capture-list ] [ block-parameter-clause ] compound-statement ;

capture-list = "[" capture-item { "," capture-item } "]" ;
capture-item = [ capture-modifier ] identifier ;
capture-modifier = "strong" | "weak" | "unowned" | "move" ;

message-expression =
    "[" receiver-expression [ optional-send-indicator ] message-selector "]" ;
optional-send-indicator = "?" ;

postfix-expression = primary-expression { postfix-operator } ;
postfix-operator =
      "[" expression "]"
    | "(" [ argument-expression-list ] ")"
    | "." identifier
    | "->" identifier
    | "?." identifier
    | "++"
    | "--" ;

postfix-propagation-expression =
    postfix-expression [ postfix-propagation-operator ] ;

postfix-propagation-operator = "?" ;
(* valid only when the next token is in PropagationFollowSet *)

await-expression = "await" cast-expression ;
try-expression =
      "try" cast-expression
    | "try?" cast-expression
    | "try!" cast-expression ;

unary-expression =
      postfix-propagation-expression
    | await-expression
    | try-expression
    | unary-operator cast-expression
    | "sizeof" unary-expression
    | "sizeof" "(" type-name ")" ;

cast-expression =
      "(" type-name ")" cast-expression
    | unary-expression ;

logical-or-expression = logical-or-expression-base ;

nil-coalescing-expression =
      logical-or-expression
    | logical-or-expression "??" nil-coalescing-expression ;

conditional-expression =
      nil-coalescing-expression
    | nil-coalescing-expression "?" expression ":" conditional-expression ;

assignment-expression =
      conditional-expression
    | unary-expression assignment-operator assignment-expression ;

expression = assignment-expression { "," assignment-expression } ;
```

### F.3.4 Canonical Attribute Integration Notes {#f-3-4}

The productions above include canonical header spellings from [ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b), while preserving ergonomic sugar (`@cleanup`, `@resource`) used in [Part 8](#part-8).

Attribute placement constraints (declaration vs type-position legality) are inherited from baseline Objective-C attribute grammar and from part-specific rules.

## F.4 Operator Precedence and Associativity {#f-4}

Precedence is listed from highest (binds tightest) to lowest.

| Level | Operators / forms | Associativity | Notes |
| --- | --- | --- | --- |
| 1 | Postfix selectors and calls: `x.y`, `x->y`, `x?.y`, `x(...)`, `x[...]`, `x++`, `x--` | Left-to-right | Includes ordinary member access `.`, pointer member access `->`, and optional member access `?.`. |
| 2 | Postfix propagation: `x?` | Postfix (single suffix) | Allowed only when next token is in `PropagationFollowSet`; see [F.5](#f-5). |
| 3 | Prefix and cast forms: `(T)x`, `await x`, `try x`, `try? x`, `try! x`, unary ops | Right-to-left | `await`/`try` forms compose with casts and unary operators. |
| 4 | Multiplicative: `*`, `/`, `%` | Left-to-right | Baseline C tier. |
| 5 | Additive: `+`, `-` | Left-to-right | Baseline C tier. |
| 6 | Shift: `<<`, `>>` | Left-to-right | Baseline C tier. |
| 7 | Relational: `<`, `<=`, `>`, `>=` | Left-to-right | Baseline C tier. |
| 8 | Equality: `==`, `!=` | Left-to-right | Baseline C tier. |
| 9 | Bitwise AND: `&` | Left-to-right | Baseline C tier. |
| 10 | Bitwise XOR: `^` | Left-to-right | Baseline C tier. |
| 11 | Bitwise OR: `\|` | Left-to-right | Baseline C tier. |
| 12 | Logical AND: `&&` | Left-to-right | Baseline C tier. |
| 13 | Logical OR: `\|\|` | Left-to-right | Baseline C tier. |
| 14 | Nil-coalescing: `??` | Right-to-left | As defined in [Part 3](#part-3). |
| 15 | Conditional: `?:` | Right-to-left | Ternary conditional. |
| 16 | Assignment: `=`, `+=`, `-=`, ... | Right-to-left | Baseline C tier. |
| 17 | Comma: `,` | Left-to-right | Baseline C tier. |

### F.4.1 Required Binding Examples {#f-4-1}

| Source form | Required grouping | Rationale |
| --- | --- | --- |
| `a?.b.c` | `(a?.b).c` | `?.` is a postfix selector at the same precedence tier as `.`. |
| `a->b?.c` | `(a->b)?.c` | `->` and `?.` are both postfix selectors with left-to-right grouping. |
| `a?.b ? c : d` | `(a?.b) ? c : d` | Postfix selectors bind tighter than ternary `?:`. |
| `a ?? b ? c : d` | `(a ?? b) ? c : d` | `??` binds tighter than `?:`. |
| `a ? b ?? c : d` | `a ? (b ?? c) : d` | `??` in ternary arms still binds tighter than `?:`. |
| `(T)a?.b` | `(T)(a?.b)` | Cast consumes a `cast-expression`; postfix selectors are inside that operand. |
| `((T)a)?` | `((T)a)?` | Propagation on a cast result requires parenthesized expression result. |
| `(T)a?` | `(T)a ? ... : ...` | Disambiguates as ternary introducer, not propagation; see [F.5.2](#f-5-2). |

## F.5 Disambiguation Rules (Normative) {#f-5}

### F.5.1 Tokenization priority {#f-5-1}

A conforming lexer shall use maximal munch for overlapping punctuators.

- `?.` is tokenized as one punctuator when contiguous.
- `??` is tokenized as one punctuator when contiguous.
- `a ? .b` tokenizes as `?` then `.`, not as `?.`.
- `a ? ? b` tokenizes as two `?` tokens, not as `??`.
- In prefix-expression context, `try?` and `try!` are parsed as `try` forms, not as `try` followed by ternary or logical-not parsing.

### F.5.2 Cast vs parenthesized expression {#f-5-2}

Cast disambiguation follows baseline C/Objective-C rules:

- If `(` ... `)` forms a valid `type-name` in the current scope, parse as cast.
- Otherwise parse as parenthesized expression.

Because postfix propagation applies to a completed `postfix-expression` node, not through an enclosing cast production, propagation on a cast result shall parenthesize the cast as an expression first:

- valid: `((T)x)?`
- not propagation: `(T)x?` (parsed as ternary `?` start, then diagnosed if incomplete)

### F.5.3 Postfix propagation `?` vs ternary `?:` {#f-5-3}

When `?` follows a `postfix-expression`, parser behavior is:

1. If lookahead token after `?` is in `PropagationFollowSet = { ')', ']', '}', ',', ';' }`, parse postfix propagation.
2. Otherwise, parse `?` as ternary conditional introducer.

Consequences:

- `foo()?;` is propagation.
- `foo()? + 1` is not propagation and shall be diagnosed with the follow-token rule.
- `a ? b : c` remains ternary.

### F.5.4 Interaction with `??` {#f-5-4}

Because `??` is a distinct punctuator, `a??b` is parsed as `a ?? b`, not as `a? ?b`.

If parsing later fails in a way consistent with likely ternary intent (for example `a??b:c`), the implementation shall provide a targeted diagnostic that explains `??` tokenization and offer fix-it `a ? b : c`.

### F.5.5 `await` and `try` composition {#f-5-5}

Parsers shall accept both orders in prefix chains:

- `try await f()`
- `await try f()`

Canonical order is `try await ...` per [Part 7](#part-7). Implementations should warn on non-canonical order and offer a mechanical reorder fix-it when safe.

### F.5.6 Optional send indicator vs conditional expressions {#f-5-6}

Inside message expressions, `[` receiver `?` selector `]` uses `?` as `optional-send-indicator`.

If a conditional expression is intended for the receiver, it shall be parenthesized, for example:

- `[(cond ? a : b) sel]`

### F.5.7 Required diagnostics and fix-its for ambiguous forms {#f-5-7}

For the ambiguous/token-overlap forms below, diagnostics and fix-its are required:

| Pattern | Required diagnostic | Required fix-it |
| --- | --- | --- |
| `expr? tok` where `tok` is not in `PropagationFollowSet` | Explain that postfix propagation `?` is disallowed by follow-token restriction and that parse falls back to ternary interpretation. | Parenthesize propagation subexpression when that rewrite is mechanical (for example `foo()? + 1` -> `(foo()?) + 1`). |
| `(T)x?` with missing `:`/third operand | Explain cast-vs-propagation disambiguation and that `?` starts ternary in this form. | `((T)x)?` when propagation intent is inferred from local context. |
| `a??b:c` | Explain maximal-munch tokenization to `a ?? b : c`. | `a ? b : c`. |
| `await try f()` | Explain non-canonical effect ordering, while parse remains valid. | Reorder to `try await f()` when no comments/macros would be reordered unsafely. |
| `[cond ? a : b ? sel]` | Explain optional-send/conditional-receiver ambiguity and required receiver parenthesization. | `[(cond ? a : b) ? sel]`. |
| `match (...) { case pat => expr; ... }` in ObjC 3.0 v1 mode | Explain that `=>` arms are reserved for future `match`-expression syntax and are not enabled in v1. | Rewrite to statement form `case pat: { ... }` or rewrite as `if`/`switch`. |
| `x = match (...) { case pat: { ... } ... };` in ObjC 3.0 v1 mode | Explain that `match` is statement-only in v1 and cannot appear in expression position. | Hoist into a statement `match` that assigns to a temporary, or rewrite as `if`/`switch`. |
| `case is Type ...` when `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0` | Explain that type-test patterns are optional and disabled in the current mode. | Guard with `#if __OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__` or rewrite to `default` + explicit `if`/cast chain. |
| `case pattern where condition: ...` in ObjC 3.0 v1 mode | Explain that guarded patterns are deferred and this grammar slot is reserved for a future revision. | Drop the guard and perform the condition inside the case body, or rewrite as nested `if`. |

### F.5.8 `match` Statement vs Reserved Expression Form {#f-5-8}

In ObjC 3.0 v1:

- `match-statement` is accepted with `case ... : compound-statement`,
- `match-expression-reserved` is never accepted (even if parsed as a candidate),
- encountering `=>` after `case pattern` inside `match` shall produce a targeted “reserved for future expression form” diagnostic.

Parsers shall not silently reinterpret `case ... => ...` as statement syntax.

### F.5.9 Type-Test Pattern Feature Gate {#f-5-9}

The token `is` is a contextual keyword in pattern position only.

- When `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 1`, `type-test-pattern` is enabled.
- When `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0`, `case is ...` shall be rejected with a targeted diagnostic.
- Outside pattern positions, `is` remains available as an identifier for source-compatibility.

If an imported metadata payload declares required capability `objc3.pattern.type_test.v1` and the importer does not support that capability, import shall fail as a hard error per [D.2.3](#d-2-3).

### F.5.10 Guarded Pattern Reservation (`where`) {#f-5-10}

In ObjC 3.0 v1, guarded patterns are not part of accepted `match` syntax.

- `match-case` remains `case pattern : compound-statement`.
- `match-case-guarded-reserved` is a reserved future slot and shall be rejected in v1.
- `where` is contextual: it is only treated specially after a complete `case pattern` candidate.

Parsers shall emit a targeted deferred-feature diagnostic for `case pattern where condition:`.

## F.6 Parser Conformance Test Matrix (Ambiguity and Edge Cases) {#f-6}

Each row is required for parser conformance. Diagnostics and fix-its are required when a transformation is mechanical, consistent with [Part 12](#part-12).

| ID | Snippet | Expected parse/result | Required diagnostic / fix-it |
| --- | --- | --- | --- |
| P-01 | `a?.b` | Parse as optional member access. | If `b` is scalar/struct return in v1: error (Part 3 restriction), suggest binding + ordinary access. |
| P-02 | `a?b:c` | Parse as ternary conditional. | No parse diagnostic. |
| P-03 | `a ?? b ?? c` | Parse as `a ?? (b ?? c)` (right-associative). | No parse diagnostic. |
| P-04 | `a ?? b ? c : d` | Parse as `(a ?? b) ? c : d`. | No parse diagnostic. |
| P-05 | `a ? b ?? c : d` | Parse as `a ? (b ?? c) : d`. | No parse diagnostic. |
| P-06 | `foo()?;` | Parse as postfix propagation. | If carrier/type rules fail: error per Part 6 with carrier-preserving explanation. |
| P-07 | `foo()? + 1;` | Not propagation (fails follow-token rule). | Error: postfix `?` follow-token restriction; fix-it: `(foo()?) + 1`. |
| P-08 | `((T)x)?;` | Parse as propagation of parenthesized cast result. | No parse diagnostic. |
| P-09 | `(T)x?;` | Parse `?` as ternary start, then incomplete conditional. | Targeted diagnostic for cast/propagation ambiguity; fix-it: `((T)x)?` when propagation intent is inferred. |
| P-10 | `try await f();` | Parse and type-check as canonical combined effects. | No ordering diagnostic. |
| P-11 | `await try f();` | Parse successfully. | Warning: non-canonical `await try` order; fix-it: `try await f();`. |
| P-12 | `try? await f();` | Parse as optionalizing `try` over awaited call. | No parse diagnostic; apply Part 6 semantics. |
| P-13 | `fThrows();` | Parse call expression. | Error: calling `throws` declaration without `try`; fix-it: insert `try`. |
| P-14 | `await fAsync();` in non-`async` function | Parse prefix expression. | Error: `await` outside async context; suggest adding `async` to enclosing declaration or restructuring call site. |
| P-15 | `opt?;` in non-optional-returning function | Parse propagation form. | Error: optional propagation requires optional-returning function; fix-it: `guard let` or explicit conditional return path. |
| P-16 | `opt?;` in `throws` or `Result` context expecting nil->error mapping | Parse propagation form. | Error: carrier-preserving rule forbids implicit nil->error conversion; fix-it: explicit `guard let ... else { throw ... }` or explicit adapter helper. |
| P-17 | `[obj? scalarValue]` where method returns scalar/struct | Parse optional send. | Error (v1 restriction); fix-it: bind/unwrap receiver and use ordinary send in proven-nonnull path. |
| P-18 | `obj?.count` where member type is scalar/struct | Parse optional member access. | Error (v1 restriction); fix-it: bind/unwrap then access `count`. |
| P-19 | `a??b:c` | Tokenize as `a ?? b : c`; parse fails at `:` in this context. | Diagnostic should explain `??` tokenization; fix-it for likely ternary intent: `a ? b : c`. |
| P-20 | `f(x?, y)` | Parse `x?` as propagation (`,` is follow token). | If carrier mismatch: error per Part 6; otherwise no parse diagnostic. |
| P-21 | `a?.b.c` | Parse as `(a?.b).c`. | No parse diagnostic. |
| P-22 | `a->b?.c` | Parse as `(a->b)?.c`. | If typing later fails for selected members, issue semantic diagnostic only. |
| P-23 | `a?.b?;` | Parse as propagation applied to `a?.b` (`;` is follow token). | If carrier mismatch: error per Part 6; otherwise no parse diagnostic. |
| P-24 | `a?.b? + c;` | Not propagation (fails follow-token rule at `+`); ternary path then fails if incomplete. | Error: follow-token restriction; fix-it: `(a?.b?) + c`. |
| P-25 | `(T)a?.b` | Parse as cast over optional member expression: `(T)(a?.b)`. | No parse diagnostic. |
| P-26 | `a?.b ?? c ? d : e` | Parse as `((a?.b) ?? c) ? d : e`. | No parse diagnostic. |
| P-27 | `try! await f() ?? g` | Parse as `(try! (await f())) ?? g`. | No parse diagnostic. |
| P-28 | `a ? .b : c` | Tokenization is `?` then `.` (not `?.`); parse fails near `.`. | Diagnostic: optional-member punctuator requires contiguous `?.`; fix-it: `a?.b` when context supports optional member access intent. |
| P-29 | `a ? ? b : c` | Tokenization is two `?` tokens (not `??`); parse fails. | Diagnostic: `??` must be contiguous; fix-it: remove intervening whitespace. |
| P-30 | `[cond ? a : b ? sel]` | Parse as optional send with conditional receiver candidate; receiver parenthesization required by [F.5.6](#f-5-6). | Fix-it: `[(cond ? a : b) ? sel]`. |
| P-31 | `match (r) { case .Ok(let v): { use(v); } default: { useDefault(); } }` | Parse as `match-statement` (statement position). | No parse diagnostic. |
| P-32 | `x = match (r) { case .Ok(let v): { use(v); } default: { useDefault(); } };` | Reject in ObjC 3.0 v1: `match` in expression position. | Error: `match` is statement-only in v1; suggest statement rewrite with temporary assignment. |
| P-33 | `x = match (r) { case .Ok(let v) => v; default => 0; };` | Recognize reserved expression-form candidate and reject in ObjC 3.0 v1. | Error: `=>` arm syntax reserved for future `match` expression form; suggest statement-form rewrite. |
| P-34 | `match (obj) { case is MyType let t: { use(t); } default: { fallback(); } }` with `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 1` | Parse with `type-test-pattern` + binding. | No parse diagnostic. |
| P-35 | `match (obj) { case is MyType let t: { use(t); } default: { fallback(); } }` with `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0` | Reject in parser/semantic gate check. | Error: type-test patterns disabled; suggest feature guard or explicit `if`/cast chain. |
| P-36 | `int is = 1;` | Parse `is` as ordinary identifier outside pattern position. | No parse diagnostic. |
| P-37 | `match (x) { case let v where v > 0: { use(v); } default: { fallback(); } }` | Reject in ObjC 3.0 v1 (reserved guarded pattern). | Error: guarded patterns are deferred in v1; suggest moving condition into case body. |
| P-38 | `match (x) { case .Ok(let v) where v > 0: { use(v); } default: { fallback(); } }` | Reject in ObjC 3.0 v1 (reserved guarded pattern), including after composite pattern. | Error: reserved `where` guard slot not enabled in v1; suggest nested `if` rewrite. |
| P-39 | `match (x) { case let where: { use(where); } default: { fallback(); } }` | Parse as binding pattern where identifier is `where`. | No parse diagnostic (contextual keyword does not reserve identifier slot). |

### F.6.1 Minimum diagnostic quality for matrix failures {#f-6-1}

For rows requiring rejection, a conforming implementation shall:

- identify the `?`/operator token that triggered ambiguity,
- state which rule failed (follow-token, carrier-preserving, tokenization, async context, optional-chain restriction, v1 `match` form restriction, feature-gate restriction, or guarded-pattern reservation),
- provide at least one actionable fix-it when the rewrite is mechanical.
