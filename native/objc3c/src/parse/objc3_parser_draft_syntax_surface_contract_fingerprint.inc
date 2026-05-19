#include "parse/objc3_parser_draft_syntax_surface_contract_fingerprint_summary_counts.inc"
#include "parse/objc3_parser_draft_syntax_surface_contract_fingerprint_callable_readiness.inc"
#include "parse/objc3_parser_draft_syntax_surface_contract_fingerprint_macro_property_diagnostics.inc"
#include "parse/objc3_parser_draft_syntax_surface_contract_fingerprint_interop_readiness.inc"
#include "parse/objc3_parser_draft_syntax_surface_contract_fingerprint_final_handoff.inc"

inline std::uint64_t BuildObjc3DraftSyntaxSurfaceFingerprint(
    const Objc3DraftSyntaxSurfaceSummary &summary) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint =
      MixObjc3DraftSyntaxSurfaceFingerprintSummaryCounts(fingerprint, summary);
  fingerprint = MixObjc3DraftSyntaxSurfaceFingerprintCallableReadiness(
      fingerprint,
      summary);
  fingerprint = MixObjc3DraftSyntaxSurfaceFingerprintMacroPropertyDiagnostics(
      fingerprint,
      summary);
  fingerprint =
      MixObjc3DraftSyntaxSurfaceFingerprintInteropReadiness(fingerprint,
                                                            summary);
  fingerprint =
      MixObjc3DraftSyntaxSurfaceFingerprintFinalHandoff(fingerprint, summary);
  return fingerprint;
}
