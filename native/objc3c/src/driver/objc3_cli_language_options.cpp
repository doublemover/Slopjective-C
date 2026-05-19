#include "driver/objc3_cli_language_options.h"

#include "driver/objc3_cli_language_version.h"

bool TryApplyObjc3CliLanguageOption(const std::string &flag,
                                    int &index,
                                    int argc,
                                    char **argv,
                                    Objc3CliOptions &options,
                                    std::string &error,
                                    bool &matched) {
  (void)index;
  (void)argc;
  (void)argv;
  matched = true;
  if (flag.rfind("-fobjc-version=", 0) == 0) {
    const std::string version_value =
        flag.substr(std::string("-fobjc-version=").size());
    std::uint32_t parsed_version = 0;
    if (!ParseObjc3LanguageVersion(version_value, parsed_version)) {
      error =
          "invalid -fobjc-version (expected unsigned integer): " +
          version_value;
      return false;
    }
    options.language_version = parsed_version;
    return true;
  }
  if (flag == "-fobjc-arc") {
    options.arc_mode = Objc3ArcMode::kEnabled;
    return true;
  }
  if (flag == "-fno-objc-arc") {
    options.arc_mode = Objc3ArcMode::kDisabled;
    return true;
  }
  matched = false;
  return true;
}
