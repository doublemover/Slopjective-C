/* The experimental Apple Silicon class registration ABI. */
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct class_record;

struct selector_record {
  const char *name;
  const char *type;
};

typedef int (*class_method)(const struct class_record *,
                            const struct selector_record *);

struct method_record {
  class_method implementation;
  const struct selector_record *selector;
  const char *extended_type;
};

struct method_list {
  const struct method_list *next;
  int count;
  size_t stride;
  struct method_record methods[];
};

/* The GNUstep-v2 metadata prefix emitted by the pinned compiler. */
struct class_record {
  const struct class_record *isa;
  const struct class_record *superclass;
  const char *name;
  long version;
  unsigned long flags;
  long instance_size;
  const void *ivars;
  const struct method_list *methods;
};

_Static_assert(offsetof(struct class_record, methods) == 56, "arm64 class ABI");
_Static_assert(offsetof(struct method_list, methods) == 24, "arm64 method list ABI");
_Static_assert(sizeof(struct method_record) == 24, "arm64 method ABI");

static pthread_mutex_t registry_lock = PTHREAD_MUTEX_INITIALIZER;
static const struct class_record **registry;
static size_t class_count;

static _Noreturn void registration_error(const char *message) {
  fprintf(stderr, "objc3 Darwin registration: %s\n", message);
  abort();
}

static const struct class_record *find_class(const char *name) {
  for (size_t i = 0; i < class_count; ++i)
    if (strcmp(registry[i]->name, name) == 0)
      return registry[i];
  return NULL;
}

static void check_class(const struct class_record *cls) {
  if (!cls || !cls->name || !*cls->name || !cls->isa || cls->superclass ||
      cls->flags != 0 || cls->ivars || cls->methods || cls->isa->flags != 1 ||
      cls->isa->superclass || cls->isa->ivars || cls->isa->version != 0 ||
      cls->version != 0)
    registration_error("invalid root class metadata");
  const struct method_list *list = cls->isa->methods;
  if (!list)
    return;
  if (list->next || list->count <= 0 || list->stride != sizeof(struct method_record))
    registration_error("invalid class method table");
  for (int i = 0; i < list->count; ++i) {
    const struct method_record *method = &list->methods[i];
    if (!method->implementation || !method->selector || !method->selector->name ||
        !method->selector->type || method->selector->type[0] != 'i')
      registration_error("invalid int class method");
  }
}

/* A Mach-O image must remain loaded while its registered records are in use. */
void objc3_darwin_register_v1(const struct class_record *const *classes, size_t count) {
  pthread_mutex_lock(&registry_lock);
  if (!classes || !count || count > (SIZE_MAX / sizeof(*registry)) - class_count)
    registration_error("invalid class registration batch");
  const struct class_record **updated = realloc(registry, (class_count + count) * sizeof(*registry));
  if (!updated)
    registration_error("class registry allocation failed");
  registry = updated;
  for (size_t i = 0; i < count; ++i) {
    check_class(classes[i]);
    if (find_class(classes[i]->name))
      registration_error("duplicate class name");
    registry[class_count++] = classes[i];
  }
  pthread_mutex_unlock(&registry_lock);
}

const struct class_record *objc3_darwin_class_v1(const char *name) {
  pthread_mutex_lock(&registry_lock);
  const struct class_record *cls = name ? find_class(name) : NULL;
  pthread_mutex_unlock(&registry_lock);
  return cls;
}

static int nil_class_method(const struct class_record *cls, const struct selector_record *sel) {
  (void)cls;
  (void)sel;
  return 0;
}

class_method objc3_darwin_lookup_v1(const struct class_record *receiver,
                                   const struct selector_record *selector) {
  if (!receiver)
    return nil_class_method;
  if (!selector || !selector->name || !selector->type)
    registration_error("invalid class selector");

  pthread_mutex_lock(&registry_lock);
  size_t position = 0;
  while (position < class_count && registry[position] != receiver)
    ++position;
  int registered = position != class_count;
  pthread_mutex_unlock(&registry_lock);
  if (!registered)
    registration_error("receiver is not a registered class");

  const struct method_list *list = receiver->isa->methods;
  for (int i = 0; list && i < list->count; ++i) {
    const struct method_record *method = &list->methods[i];
    if (strcmp(method->selector->name, selector->name) == 0 &&
        strcmp(method->selector->type, selector->type) == 0)
      return method->implementation;
  }
  registration_error("class method selector is missing");
}
