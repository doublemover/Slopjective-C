#include "tools/objc3c_frontend_c_api_runner_object_inspection_command.h"

#include <cstddef>

#include "ast/objc3_ast.h"
#include "tools/objc3c_frontend_c_api_runner_read_command.h"
#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

std::string BuildFrontendCApiRunnerObjectInspectionCommand(
    const std::string &template_command,
    const std::string &object_path_text) {
  if (!FrontendCApiRunnerPathExists(object_path_text)) {
    return "";
  }
  const std::string placeholder =
      kObjc3RuntimeMetadataObjectInspectionObjectRelativePath;
  std::string command = template_command;
  const std::size_t placeholder_offset = command.find(placeholder);
  if (placeholder_offset != std::string::npos) {
    command.replace(
        placeholder_offset,
        placeholder.size(),
        QuoteFrontendCApiRunnerPowerShellArg(object_path_text));
    return command;
  }
  return command + " " +
         QuoteFrontendCApiRunnerPowerShellArg(object_path_text);
}
