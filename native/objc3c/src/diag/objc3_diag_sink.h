#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "diag/objc3_diag_record.h"

class Objc3DiagnosticSink {
 public:
  explicit Objc3DiagnosticSink(std::vector<std::string> &diagnostics);

  void Emit(const Objc3DiagnosticPayload &payload);
  void EmitError(unsigned line,
                 unsigned column,
                 const std::string &code,
                 const std::string &message);

  [[nodiscard]] std::size_t size() const;
  [[nodiscard]] bool empty() const;

 private:
  std::vector<std::string> *diagnostics_;
};

void EmitDiagnostic(std::vector<std::string> &diagnostics,
                    const Objc3DiagnosticPayload &payload);
void EmitErrorDiagnostic(std::vector<std::string> &diagnostics,
                         unsigned line,
                         unsigned column,
                         const std::string &code,
                         const std::string &message);
