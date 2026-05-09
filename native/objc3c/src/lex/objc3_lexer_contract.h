#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "config/objc3_language_profile.h"

enum class Objc3LexerLanguageProfile {
  kCanonical,
};

struct Objc3LexerOptions {
  std::uint8_t language_version = objc3c::config::kCanonicalLanguageVersion;
  Objc3LexerLanguageProfile language_profile = Objc3LexerLanguageProfile::kCanonical;
};

struct Objc3LexerMigrationHints {
  std::size_t legacy_yes_count = 0;
  std::size_t legacy_no_count = 0;
  std::size_t legacy_null_count = 0;

  std::size_t LegacyLiteralTotal() const { return legacy_yes_count + legacy_no_count + legacy_null_count; }
};

struct Objc3LexerLanguageVersionPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
};

struct Objc3LexerNamedIdentifierPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
  std::string identifier;
};

struct Objc3LexerBootstrapRegistrationSourceContract {
  Objc3LexerNamedIdentifierPragmaContract registration_descriptor;
  Objc3LexerNamedIdentifierPragmaContract image_root;
};
