#pragma once

#include <cstdint>
#include <string>
#include <string_view>

enum class Objc3DiagnosticSubsystem : std::uint8_t {
  kUnknown,
  kCanonicalConfig,
  kLanguageSurface,
  kParser,
  kSemantic,
  kRuntime,
  kFrontendApi,
  kArtifact,
  kTooling,
};

struct Objc3DiagnosticCode {
  std::string text;
  Objc3DiagnosticSubsystem subsystem = Objc3DiagnosticSubsystem::kUnknown;
  unsigned ordinal = 0;
  bool valid = false;
};

const char *DiagnosticSubsystemName(Objc3DiagnosticSubsystem subsystem);
Objc3DiagnosticSubsystem DiagnosticSubsystemForPrefix(char prefix);
bool TryParseNativeDiagCode(std::string_view candidate,
                            Objc3DiagnosticCode &code);
bool IsNativeDiagCode(std::string_view candidate);
