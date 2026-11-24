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

struct weak_slot {
  void **address;
  struct weak_slot *next;
};

struct instance_record {
  struct instance_record *next;
  struct weak_slot *weak;
  size_t references;
  const struct class_record *isa;
};

struct autorelease_entry {
  struct autorelease_entry *next;
  void *object;
};

struct autorelease_pool {
  struct autorelease_pool *parent;
  struct autorelease_entry *entries;
};

struct super_record {
  const void *receiver;
  const struct class_record *start;
};

_Static_assert(offsetof(struct class_record, methods) == 56, "arm64 class ABI");
_Static_assert(offsetof(struct method_list, methods) == 24, "arm64 method list ABI");
_Static_assert(sizeof(struct method_record) == 24, "arm64 method ABI");
_Static_assert(sizeof(struct instance_record) ==
               offsetof(struct instance_record, isa) + sizeof(void *),
               "the object starts at isa, after all private runtime state");
_Static_assert(sizeof(struct super_record) == 16, "arm64 super dispatch ABI");

static pthread_mutex_t registry_lock = PTHREAD_MUTEX_INITIALIZER;
static const struct class_record **registry;
static size_t class_count;
static struct instance_record *instances;
static _Thread_local struct autorelease_pool *current_pool;

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
  if (!cls || !cls->name || !*cls->name || !cls->isa ||
      cls->flags != 0 || cls->instance_size != 0 || cls->ivars || cls->version != 0 ||
      cls->isa->flags != 1 || cls->isa->superclass || cls->isa->instance_size != 0 ||
      cls->isa->ivars || cls->isa->version != 0)
    registration_error("invalid stateless class metadata");
  check_method_list(cls->methods);
  check_method_list(cls->isa->methods);
}

static int class_is_registered(const void *cls) {
  for (size_t i = 0; i < class_count; ++i)
    if (registry[i] == cls)
      return 1;
  return 0;
}

/* Image constructors can run in either order. Resolve the complete chain
 * before an allocation or send and before exposing the record by name. */
static void check_class_lineage(const struct class_record *cls) {
  size_t remaining = class_count;
  for (; cls; cls = cls->superclass) {
    if (!class_is_registered(cls))
      registration_error("class has an unregistered superclass");
    if (!remaining)
      registration_error("class hierarchy has a cycle");
    --remaining;
  }
}

static struct instance_record **find_instance(const void *receiver) {
  struct instance_record **link = &instances;
  while (*link && &(*link)->isa != receiver)
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
  check_class_lineage(cls);
  pthread_mutex_unlock(&registry_lock);
  return cls;
}

void *objc3_darwin_allocate_v1(const struct class_record *cls) {
  if (!cls)
    return NULL;
  pthread_mutex_lock(&registry_lock);
  if (!class_is_registered(cls))
    registration_error("allocation requires a registered class");
  check_class_lineage(cls);
  struct instance_record *instance = malloc(sizeof(*instance));
  if (instance) {
    *instance = (struct instance_record){.next = instances, .references = 1,
                                         .isa = cls};
    instances = instance;
  }
  pthread_mutex_unlock(&registry_lock);
  return instance ? &instance->isa : NULL;
}

static void destroy_instance(struct instance_record **link) {
  struct instance_record *instance = *link;
  *link = instance->next;
  while (instance->weak) {
    struct weak_slot *slot = instance->weak;
    *slot->address = NULL;
    instance->weak = slot->next;
    free(slot);
  }
  free(instance);
}

void objc3_darwin_deallocate_v1(void *receiver) {
  if (!receiver)
    return;
  pthread_mutex_lock(&registry_lock);
  struct instance_record **link = find_instance(receiver);
  if (!*link)
    registration_error("deallocation requires a live instance");
  if ((*link)->references != 1)
    registration_error("deallocation requires an exclusively owned instance");
  destroy_instance(link);
  pthread_mutex_unlock(&registry_lock);
}

static void retain_locked(void *object) {
  if (!object || class_is_registered(object))
    return;
  struct instance_record *instance = *find_instance(object);
  if (!instance || instance->references == SIZE_MAX)
    registration_error("invalid ARC retain");
  ++instance->references;
}

void *objc3_darwin_arc_retain_v1(void *object) {
  pthread_mutex_lock(&registry_lock);
  retain_locked(object);
  pthread_mutex_unlock(&registry_lock);
  return object;
}

void objc3_darwin_arc_release_v1(void *object) {
  if (!object)
    return;
  pthread_mutex_lock(&registry_lock);
  if (!class_is_registered(object)) {
    struct instance_record **link = find_instance(object);
    if (!*link)
      registration_error("ARC release requires a live object");
    if (--(*link)->references == 0)
      destroy_instance(link);
  }
  pthread_mutex_unlock(&registry_lock);
}

void objc3_darwin_arc_store_strong_v1(void *address, void *value) {
  objc3_darwin_arc_retain_v1(value);
  void *old;
  // Strong source fields can be underaligned. The slot representation is
  // copied only after retaining the new owner and before releasing the old.
  memcpy(&old, address, sizeof(old));
  memcpy(address, &value, sizeof(value));
  objc3_darwin_arc_release_v1(old);
}

static void check_weak_slot(void **address) {
  if (!address || (uintptr_t)address % _Alignof(void *) != 0)
    registration_error("ARC weak storage requires native pointer alignment");
}

static void register_weak_locked(void **address, void *object) {
  if (object && !class_is_registered(object)) {
    struct instance_record *instance = *find_instance(object);
    if (!instance)
      registration_error("ARC weak initialization requires a live object");
    struct weak_slot *slot = malloc(sizeof(*slot));
    if (!slot)
      registration_error("ARC weak registration allocation failed");
    *slot = (struct weak_slot){address, instance->weak};
    instance->weak = slot;
  }
  *address = object;
}

static void unregister_weak_locked(void **address) {
  void *object = *address;
  if (!object || class_is_registered(object))
    return;
  struct instance_record *instance = *find_instance(object);
  if (!instance)
    registration_error("ARC weak slot does not refer to a live object");
  struct weak_slot **link = &instance->weak;
  while (*link && (*link)->address != address)
    link = &(*link)->next;
  if (!*link)
    registration_error("ARC weak slot was not registered");
  struct weak_slot *slot = *link;
  *link = slot->next;
  free(slot);
}

void *objc3_darwin_arc_init_weak_v1(void **address, void *value) {
  check_weak_slot(address);
  pthread_mutex_lock(&registry_lock);
  register_weak_locked(address, value);
  pthread_mutex_unlock(&registry_lock);
  return value;
}

void *objc3_darwin_arc_store_weak_v1(void **address, void *value) {
  check_weak_slot(address);
  pthread_mutex_lock(&registry_lock);
  unregister_weak_locked(address);
  register_weak_locked(address, value);
  pthread_mutex_unlock(&registry_lock);
  return value;
}

void *objc3_darwin_arc_load_weak_retained_v1(void **address) {
  check_weak_slot(address);
  pthread_mutex_lock(&registry_lock);
  void *value = *address;
  retain_locked(value);
  pthread_mutex_unlock(&registry_lock);
  return value;
}

void objc3_darwin_arc_destroy_weak_v1(void **address) {
  check_weak_slot(address);
  pthread_mutex_lock(&registry_lock);
  unregister_weak_locked(address);
  *address = NULL;
  pthread_mutex_unlock(&registry_lock);
}

static void transfer_weak(void **destination, void **source, int move) {
  check_weak_slot(destination);
  check_weak_slot(source);
  pthread_mutex_lock(&registry_lock);
  register_weak_locked(destination, *source);
  if (move) {
    unregister_weak_locked(source);
    *source = NULL;
  }
  pthread_mutex_unlock(&registry_lock);
}

void objc3_darwin_arc_copy_weak_v1(void **destination, void **source) {
  transfer_weak(destination, source, 0);
}

void objc3_darwin_arc_move_weak_v1(void **destination, void **source) {
  transfer_weak(destination, source, 1);
}

void *objc3_darwin_arc_pool_push_v1(void) {
  struct autorelease_pool *pool = malloc(sizeof(*pool));
  if (!pool)
    registration_error("ARC autorelease pool allocation failed");
  *pool = (struct autorelease_pool){.parent = current_pool};
  current_pool = pool;
  return pool;
}

void objc3_darwin_arc_pool_pop_v1(void *token) {
  struct autorelease_pool *pool = current_pool;
  if (!pool || pool != token)
    registration_error("ARC pool pop requires this thread's latest pool");
  current_pool = pool->parent;
  while (pool->entries) {
    struct autorelease_entry *entry = pool->entries;
    pool->entries = entry->next;
    objc3_darwin_arc_release_v1(entry->object);
    free(entry);
  }
  free(pool);
}

void *objc3_darwin_arc_autorelease_v1(void *object) {
  if (!object)
    return NULL;
  pthread_mutex_lock(&registry_lock);
  int immortal = class_is_registered(object);
  if (!immortal && !*find_instance(object))
    registration_error("ARC autorelease requires a live object");
  pthread_mutex_unlock(&registry_lock);
  if (immortal)
    return object;
  if (!current_pool)
    registration_error("ARC autorelease requires an active pool");
  struct autorelease_entry *entry = malloc(sizeof(*entry));
  if (!entry)
    registration_error("ARC autorelease entry allocation failed");
  *entry = (struct autorelease_entry){current_pool->entries, object};
  current_pool->entries = entry;
  return object;
}

void *objc3_darwin_arc_retain_autorelease_v1(void *object) {
  return objc3_darwin_arc_autorelease_v1(objc3_darwin_arc_retain_v1(object));
}

void *objc3_darwin_arc_load_weak_v1(void **address) {
  return objc3_darwin_arc_autorelease_v1(
      objc3_darwin_arc_load_weak_retained_v1(address));
}

static int nil_method(const void *receiver, const struct selector_record *sel) {
  (void)receiver;
  (void)sel;
  return 0;
}

static method_implementation lookup(const void *receiver, const struct selector_record *selector,
                                    const struct super_record *super) {
  if (!receiver)
    return nil_method;
  if (!selector || !selector->name || !selector->type)
    registration_error("invalid selector");

  pthread_mutex_lock(&registry_lock);
  const struct class_record *dispatch_class;
  int class_send = class_is_registered(receiver);
  if (class_send) {
    dispatch_class = receiver;
  } else {
    const struct instance_record *instance = *find_instance(receiver);
    if (!instance)
      registration_error("receiver has no registered class or live instance");
    dispatch_class = instance->isa;
  }
  check_class_lineage(dispatch_class);
  if (super) {
    for (dispatch_class = dispatch_class->superclass; dispatch_class; dispatch_class = dispatch_class->superclass) {
      const struct class_record *candidate = class_send ? dispatch_class->isa : dispatch_class;
      if (candidate == super->start)
        break;
    }
    if (!dispatch_class)
      registration_error("super dispatch start is not an ancestor of the receiver");
  }

  for (; dispatch_class; dispatch_class = dispatch_class->superclass) {
    const struct method_list *list = class_send ? dispatch_class->isa->methods : dispatch_class->methods;
    for (int i = 0; list && i < list->count; ++i) {
      const struct method_record *method = &list->methods[i];
      if (strcmp(method->selector->name, selector->name) == 0 &&
          strcmp(method->selector->type, selector->type) == 0) {
        method_implementation implementation = method->implementation;
        pthread_mutex_unlock(&registry_lock);
        return implementation;
      }
    }
  }
  registration_error("method selector is missing");
}

method_implementation objc3_darwin_lookup_v1(const void *receiver,
                                             const struct selector_record *selector) {
  return lookup(receiver, selector, NULL);
}

method_implementation objc3_darwin_lookup_super_v1(const struct super_record *super,
                                                   const struct selector_record *selector) {
  if (!super)
    registration_error("super dispatch record is missing");
  return lookup(super->receiver, selector, super);
}
