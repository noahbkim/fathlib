#ifndef COW_H
#define COW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

/// A data structure used to efficiently duplicate a string with minor changes.
///
/// The `Cow` is designed around path normalization, which will never produce a
/// longer path than the input string. Typical operations include fixing path
/// separators and removing redundant components.
typedef struct
{
    PyUnicodeObject *read;
    PyUnicodeObject *write;
    Py_ssize_t read_size;
    Py_ssize_t write_size;
    void *read_data;
    void *write_data;
    Py_ssize_t write_index;
    unsigned int read_kind;
    unsigned int write_kind;
} Cow;

/// Construct a `Cow` with a reference string.
///
/// Borrows `read`. Will not allocate `write` until `cow_advance` is called
/// with a divergent character.
void cow_construct(Cow *self, PyUnicodeObject *read);

/// Manually force the `Cow` to construct `write`.
///
/// In addition to allocating a `write` string with the same length as `read`
/// (if one is not present), this method copies all strings up to `read_index`
/// from `read` into `write`.
int cow_copy(Cow *self, Py_ssize_t resize);

/// Push a character into `Cow`.
///
/// If a `write` string has been allocated, `character` will be written to it
/// at `write_index`. Otherwise, the `write` string is only allocated if
/// `character` is different than the character at `read_index` in `read`
/// (or if `write_index` and `read_index` have diverged). On the happy path,
/// where each advanced `character` matches that of the `read` string, no
/// `write` string will be allocated and `write_index` will be incremented.
int cow_advance(Cow *self, Py_UCS4 character);

/// Manually write a character into `Cow`.
///
/// Prefer `cow_advance`. This function must be used if you `cow_resize` and
/// need to write past the length of `read`.
int cow_write(Cow *self, Py_UCS4 character);

/// Resize the `write` string, allocating if not present.
///
/// This function is Pandora's box and should be avoided if possible, as the
/// user must track whether they're writing past the length of `read` and call
/// `cow_write` accordingly.
int cow_resize(Cow *self, Py_ssize_t size);

/// Return the assembled string.
///
/// If `write` was allocated, return it. Otherwise, return either a substsring
/// of or a new reference to `read`, depending on how far we advanced.
PyUnicodeObject *cow_consume(Cow *self);

/// Deallocate `write` if present.
///
/// Does not zero other state, so be sure to call `cow_construct` again before
/// reusing an instance.
void cow_destroy(Cow *self);

#endif
