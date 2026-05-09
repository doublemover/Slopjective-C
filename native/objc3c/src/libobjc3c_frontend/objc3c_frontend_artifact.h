#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_ARTIFACT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_ARTIFACT_H_

/*
 * Artifact path selectors for objc3c_frontend_result_artifact_path().
 * Returned paths are result-owned strings borrowed by the caller until
 * objc3c_frontend_result_destroy(). Undefined enum values return NULL.
 *
 * This header owns only the public artifact kind identifiers. Publication,
 * file naming, and write behavior live behind the compile entrypoints.
 */
typedef enum objc3c_frontend_artifact_kind {
  OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS = 0,
  OBJC3C_FRONTEND_ARTIFACT_MANIFEST = 1,
  OBJC3C_FRONTEND_ARTIFACT_IR = 2,
  OBJC3C_FRONTEND_ARTIFACT_OBJECT = 3,
  OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA = 4
} objc3c_frontend_artifact_kind_t;

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_ARTIFACT_H_
