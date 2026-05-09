#include "parse/objc3_parser.h"

#include <utility>

#include "parse/objc3_parser_core.h"

Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens) {
  objc3c::parse::Objc3ParserCoreResult core =
      objc3c::parse::ParseObjc3ProgramCore(tokens);
  // source/frontend-truth anchor: the parser contract snapshot stays
  // the canonical declaration/grammar coverage record consumed by the emitted
  // runnable feature-claim inventory. Later lanes may refine claims, but they
  // must not invent a second frontend source-of-truth surface.
  // truth-surface wiring anchor: the parser does not admit hidden
  // strictness/concurrency claim syntax, so unsupported selection surfaces stay
  // explicit in the frontend truth packet rather than being inferred.
  return BuildObjc3ParseResult(
      std::move(core.program), std::move(core.diagnostics), tokens.size());
}
