#ifndef INCLUDE_UB_FLATMAP_H
#define INCLUDE_UB_FLATMAP_H

#ifndef UB_FLATMAP_DEFAULT_SIZE
#define UB_FLATMAP_DEFAULT_SIZE 16
#endif

#ifndef UB_FLATMAP_CAPACITY
#define UB_FLATMAP_CAPACITY(x) ((x) * 3 / 2)
#endif

#ifdef UB_FLATMAP_CXX_OVERLOADING
#define UB__DEFAULT_ARG(type, name, value) type name = value
#else
#define UB__DEFAULT_ARG(type, name, value) type name
#endif

#include <stdlib.h>
#include <string.h>

#if __cplusplus
extern "C" {
#endif

typedef int ub__kv_equal_func(const void*, const void*);

typedef struct ub_flatmap {
  size_t key_size;
  size_t value_size;

  size_t length;
  size_t capacity;

  void* key_pointer;
  void* value_pointer;

  ub__kv_equal_func* equality;
} ub_flatmap;

void ub_flatmap_init(
  ub_flatmap* self,
  ub__kv_equal_func* keys_equal,
  size_t key_size,
  size_t value_size
);

void* ub_flatmap_insert(
  ub_flatmap* self,
  void const* key,
  void const* value,
  UB__DEFAULT_ARG(void*, old_value, NULL)
);

void* ub_flatmap_get(
  ub_flatmap* self,
  void const* key
);

#ifdef UB_FLATMAP_IMPLEMENTATION

void ub_flatmap_init(
  ub_flatmap* self,
  ub__kv_equal_func* keys_equal,
  size_t key_size,
  size_t value_size
) {
  self->key_size = key_size;
  self->value_size = value_size;

  self->length = 0;
  self->capacity = 0;

  self->key_pointer = NULL;
  self->value_pointer = NULL;

  self->equality = keys_equal;
}

void* ub_flatmap_insert(
  ub_flatmap* self,
  void const* key,
  void const* value,
  void* old_value
) {
  /* first, check for the key already being in the store */
  {
    char* key_ptr = (char*)self->key_pointer;
    char const* key_ptr_end =
      (char*)self->key_pointer
      + self->length * self->key_size;

    char* value_ptr = (char*)self->value_pointer;

    while (key_ptr < key_ptr_end) {
      if (self->equality(key_ptr, key)) {
        if (old_value) {
          memcpy(old_value, value_ptr, self->value_size);
        }
        memcpy(value_ptr, value, self->value_size);
        return key_ptr;
      }
      key_ptr += self->key_size;
      value_ptr += self->value_size;
    }
  }

  if (self->length >= self->capacity) {
    void* new_keys;
    void* new_vals;
    size_t new_cap =
      self->capacity < (UB_FLATMAP_DEFAULT_SIZE)
      ? UB_FLATMAP_CAPACITY(self->capacity)
      : (UB_FLATMAP_DEFAULT_SIZE);

    new_keys = realloc(
      self->key_pointer,
      new_cap * self->key_size
    );
    new_vals = realloc(
      self->value_pointer,
      new_cap * self->value_size
    );
    if (new_keys && new_vals) {
      self->key_pointer = new_keys;
      self->value_pointer = new_vals;
      self->capacity = new_cap;
    } else {
      return NULL;
    }
  }

  {
    char* key_ptr =
      (char*)self->key_pointer
      + self->length * self->key_size;
    char* val_ptr =
      (char*)self->value_pointer
      + self->length * self->value_size;

    memcpy(key_ptr, key, self->key_size);
    memcpy(val_ptr, value, self->value_size);
    ++self->length;
    return key_ptr;
  }
}

void* ub_flatmap_get(
  ub_flatmap* self,
  void const* key
) {
  char* key_ptr = (char*)self->key_pointer;
  char const* key_ptr_end =
    (char*)self->key_pointer
    + self->length * self->key_size;

  char* value_ptr = (char*)self->value_pointer;

  while (key_ptr <= key_ptr_end) {
    if (self->equality(key, key_ptr)) {
      return value_ptr;
    }

    key_ptr += self->key_size;
    value_ptr += self->value_size;
  }

  return NULL;
}

#endif /* UB_FLATMAP_IMPLEMENTATION */

#if __cplusplus
}
#endif

#ifdef UB_FLATMAP_CXX_OVERLOADING
template <typename Key, typename Value>
void ub_flatmap_init(
  ub_flatmap* self,
  ub__kv_equal_func* keys_equal =
    [](void const* lhs, void const* rhs) {
      return static_cast<int>(
        *static_cast<Key const*>(lhs)
        == *static_cast<Key const*>(rhs)
      );
    }
) {
  ub_flatmap_init(self, keys_equal, sizeof(Key), sizeof(Value));
}
#endif

#ifdef UB_FLATMAP_CXX_OVERLOADING
template <typename T, typename U>
void* ub_flatmap_insert(
  ub_flatmap* self,
  T const& key,
  U const& value,
  U* old_value = nullptr
) {
  return ub_flatmap_insert(
    self,
    static_cast<void const*>(&key),
    static_cast<void const*>(&value),
    static_cast<void*>(old_value)
  );
}
#endif

#ifdef UB_FLATMAP_CXX_OVERLOADING
template <typename T>
void* ub_flatmap_get(
  ub_flatmap* self,
  T const& key
) {
  return ub_flatmap_get(
    self,
    static_cast<void const*>(&key)
  );
}
#endif

#undef UB__DEFAULT_ARG

#endif /* INCLUDE_UB_FLATMAP_H */
