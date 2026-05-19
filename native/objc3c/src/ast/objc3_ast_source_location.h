#pragma once

struct Objc3SourceLocation {
  unsigned line = 1;
  unsigned column = 1;
};

inline constexpr Objc3SourceLocation Objc3DefaultSourceLocation() {
  return Objc3SourceLocation{};
}
