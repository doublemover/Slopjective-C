#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate_rows.h"

void WriteFrontendCApiRunnerCOwnershipAggregateLifetimeRows(
    objc3::io::json::JsonObjectWriter &object,
    const FrontendCApiRunnerCOwnershipView &ownership) {
  object.StringField("result_handle_owner", ownership.result_handle_owner);
  object.StringField("result_release_function",
                     ownership.result_release_function);
  object.StringField("result_release_timing", ownership.result_release_timing);
  object.StringField("publication_snapshot_owner",
                     ownership.publication_snapshot_owner);
  object.StringField("context_lifetime", ownership.context_lifetime);
  object.StringField("compile_options_lifetime",
                     ownership.compile_options_lifetime);
}
