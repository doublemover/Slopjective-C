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

typedef int (*method_implementation)(const void *, const struct selector_record *);

struct method_record {
  method_implementation implementation;
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

struct instance_record {
  const struct class_record *isa;
  struct instance_record *next;
};

_Static_assert(offsetof(struct class_record, methods) == 56, "arm64 class ABI");
_Static_assert(offsetof(struct method_list, methods) == 24, "arm64 method list ABI");
_Static_assert(sizeof(struct method_record) == 24, "arm64 method ABI");
_Static_assert(offsetof(struct instance_record, isa) == 0, "arm64 object header ABI");

static pthread_mutex_t registry_lock = PTHREAD_MUTEX_INITIALIZER;
static const struct class_record **registry;
static size_t class_count;
static struct instance_record *instances;

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

static void check_method_list(const struct method_list *list) {
  if (!list)
    return;
  if (list->next || list->count <= 0 || list->stride != sizeof(struct method_record))
    registration_error("invalid method table");
  for (int i = 0; i < list->count; ++i) {
    const struct method_record *method = &list->methods[i];
    if (!method->implementation || !method->selector || !method->selector->name ||
        !method->selector->type || strcmp(method->selector->type, "i16@0:8") != 0)
      registration_error("invalid no-argument int method");
  }
}

static void check_class(const struct class_record *cls) {
  if (!cls || !cls->name || !*cls->name || !cls->isa || cls->superclass ||
      cls->flags != 0 || cls->instance_size != 0 || cls->ivars || cls->version != 0 ||
      cls->isa->flags != 1 || cls->isa->superclass || cls->isa->instance_size != 0 ||
      cls->isa->ivars || cls->isa->version != 0)
    registration_error("invalid stateless root class metadata");
  check_method_list(cls->methods);
  check_method_list(cls->isa->methods);
}

static int class_is_registered(const void *cls) {
  for (size_t i = 0; i < class_count; ++i)
    if (registry[i] == cls)
      return 1;
  return 0;
}

static struct instance_record **find_instance(const void *receiver) {
  struct instance_record **link = &instances;
  while (*link && *link != receiver)
    link = &(*link)->next;
  return link;
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

void *objc3_darwin_allocate_v1(const struct class_record *cls) {
  if (!cls)
    return NULL;
  pthread_mutex_lock(&registry_lock);
  if (!class_is_registered(cls))
    registration_error("allocation requires a registered root class");
  struct instance_record *instance = malloc(sizeof(*instance));
  if (instance) {
    *instance = (struct instance_record){cls, instances};
    instances = instance;
  }
  pthread_mutex_unlock(&registry_lock);
  return instance;
}

void objc3_darwin_deallocate_v1(void *receiver) {
  if (!receiver)
    return;
  pthread_mutex_lock(&registry_lock);
  struct instance_record **link = find_instance(receiver);
  struct instance_record *instance = *link;
  if (!instance)
    registration_error("deallocation requires a live instance");
  *link = instance->next;
  pthread_mutex_unlock(&registry_lock);
  free(instance);
}

static int nil_method(const void *receiver, const struct selector_record *sel) {
  (void)receiver;
  (void)sel;
  return 0;
}

method_implementation objc3_darwin_lookup_v1(const void *receiver,
                                             const struct selector_record *selector) {
  if (!receiver)
    return nil_method;
  if (!selector || !selector->name || !selector->type)
    registration_error("invalid selector");

  pthread_mutex_lock(&registry_lock);
  const struct class_record *dispatch_class;
  if (class_is_registered(receiver)) {
    const struct class_record *cls = receiver;
    dispatch_class = cls->isa;
  } else {
    const struct instance_record *instance = *find_instance(receiver);
    if (!instance)
      registration_error("receiver has no registered class or live instance");
    dispatch_class = instance->isa;
  }
  pthread_mutex_unlock(&registry_lock);

  const struct method_list *list = dispatch_class->methods;
  for (int i = 0; list && i < list->count; ++i) {
    const struct method_record *method = &list->methods[i];
    if (strcmp(method->selector->name, selector->name) == 0 &&
        strcmp(method->selector->type, selector->type) == 0)
      return method->implementation;
  }
  registration_error("method selector is missing");
}
