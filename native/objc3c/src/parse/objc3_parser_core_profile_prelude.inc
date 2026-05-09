#include "parse/objc3_parser.h"

// source-closure anchors: unsupported 'try' expression,
// unsupported 'throw' statement, and unsupported 'do/catch' statement remain
// explicit fail-closed parser-owned diagnostics until the later runnable Part 6
// lowering/runtime tranches land.
#include "ast/objc3_ast_block_surface.h"
#include "ast/objc3_ast_decl_surface.h"
#include "parse/objc3_ast_builder.h"
#include "parse/objc3_message_send_profiles.h"
#include "parse/objc3_parse_support.h"
#include "parse/objc3_parser_actor_isolation_sendability_profiles.h"
#include "parse/objc3_parser_async_continuation_profiles.h"
#include "parse/objc3_parser_attributes.h"
#include "parse/objc3_parser_await_suspension_profiles.h"
#include "parse/objc3_parser_block_literal_source.h"
#include "parse/objc3_parser_concurrency_replay_race_guard_profiles.h"
#include "parse/objc3_parser_cstyle_type_classifier.h"
#include "parse/objc3_parser_cstyle_type_parser.h"
#include "parse/objc3_parser_cursor.h"
#include "parse/objc3_parser_core.h"
#include "parse/objc3_parser_declaration_surface.h"
#include "parse/objc3_parser_diagnostics.h"
#include "parse/objc3_parser_error_diagnostics_recovery_profiles.h"
#include "parse/objc3_parser_expression_nodes.h"
#include "parse/objc3_parser_expression_surface.h"
#include "parse/objc3_parser_inline_asm_intrinsic_profiles.h"
#include "parse/objc3_parser_interop_profiles.h"
#include "parse/objc3_parser_ns_error_bridging_profiles.h"
#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_recovery.h"
#include "parse/objc3_parser_result_like_profiles.h"
#include "parse/objc3_parser_statement_nodes.h"
#include "parse/objc3_parser_statement_profiles.h"
#include "parse/objc3_parser_statement_surface.h"
#include "parse/objc3_parser_task_runtime_cancellation_profiles.h"
#include "parse/objc3_parser_token_metadata.h"
#include "parse/objc3_parser_unsafe_pointer_profiles.h"
#include "parse/objc3_parser_unwind_cleanup_profiles.h"
#include "parse/objc3_type_feature_profiles.h"
#include "support/objc3_method_family.h"

#include <algorithm>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace objc3c::parse {
namespace {

// source note: Part 12 does not admit new syntax here. The
// diagnostics/fix-it inventory and the canonicalization completion packet
// aggregate existing Part 6 through Part 11 parser-owned surfaces and frontend
// summary packets.

using Token = Objc3LexToken;

using support::MakeDiag;
using support::ParseIntegerLiteralValue;

static Objc3SemaTokenMetadata MakeSemaTokenMetadata(Objc3SemaTokenKind kind, const Token &token) {
  return MakeObjc3ParserSemaTokenMetadata(kind, token);
}
