#ifndef Py_LIMITED_API
#ifndef Py_ASDL_H
#define Py_ASDL_H

typedef PyObject * identifier;
typedef PyObject * string;
typedef PyObject * object;
typedef PyObject * constant;

/* It would be nice if the code generated by asdl_c.py was completely
   independent of Python, but it is a goal the requires too much work
   at this stage.  So, for example, I'll represent identifiers as
   interned Python strings.
*/

#define _ASDL_SEQ_HEAD \
    Py_ssize_t size;   \
    void **elements;

typedef struct {
    _ASDL_SEQ_HEAD
} asdl_seq;

typedef struct {
    _ASDL_SEQ_HEAD
    void *typed_elements[1];
} asdl_generic_seq;

typedef struct {
    _ASDL_SEQ_HEAD
    PyObject *typed_elements[1];
} asdl_identifier_seq;

typedef struct {
    _ASDL_SEQ_HEAD
    int typed_elements[1];
} asdl_int_seq;

#ifdef __cplusplus
extern "C" {
#endif

asdl_generic_seq *_Py_asdl_generic_seq_new(Py_ssize_t size, PyArena *arena);
asdl_identifier_seq *_Py_asdl_identifier_seq_new(Py_ssize_t size, PyArena *arena);
asdl_int_seq *_Py_asdl_int_seq_new(Py_ssize_t size, PyArena *arena);

#ifdef __cplusplus
}
#endif
#define GENERATE_ASDL_SEQ_CONSTRUCTOR(NAME, TYPE) \
asdl_ ## NAME ## _seq *_Py_asdl_ ## NAME ## _seq_new(Py_ssize_t size, PyArena *arena) \
{ \
    asdl_ ## NAME ## _seq *seq = NULL; \
    size_t n; \
    /* check size is sane */ \
    if (size < 0 || \
        (size && (((size_t)size - 1) > (SIZE_MAX / sizeof(void *))))) { \
        PyErr_NoMemory(); \
        return NULL; \
    } \
    n = (size ? (sizeof(TYPE *) * (size - 1)) : 0); \
    /* check if size can be added safely */ \
    if (n > SIZE_MAX - sizeof(asdl_ ## NAME ## _seq)) { \
        PyErr_NoMemory(); \
        return NULL; \
    } \
    n += sizeof(asdl_ ## NAME ## _seq); \
    seq = (asdl_ ## NAME ## _seq *)PyArena_Malloc(arena, n); \
    if (!seq) { \
        PyErr_NoMemory(); \
        return NULL; \
    } \
    memset(seq, 0, n); \
    seq->size = size; \
    seq->elements = (void**)seq->typed_elements; \
    return seq; \
}

#define asdl_seq_GET_UNTYPED(S, I) ((S)->elements[(I)])
#define asdl_seq_GET(S, I) (S)->typed_elements[(I)]
#define asdl_seq_LEN(S) ((S) == NULL ? 0 : (S)->size)
#ifdef Py_DEBUG
#define asdl_seq_SET(S, I, V) \
    do { \
        Py_ssize_t _asdl_i = (I); \
        assert((S) != NULL); \
        assert(0 <= _asdl_i && _asdl_i < (S)->size); \
        (S)->typed_elements[_asdl_i] = (V); \
    } while (0)
#else
#define asdl_seq_SET(S, I, V) (S)->typed_elements[I] = (V)
#endif

#ifdef Py_DEBUG
#define asdl_seq_SET_UNTYPED(S, I, V) \
    do { \
        Py_ssize_t _asdl_i = (I); \
        assert((S) != NULL); \
        assert(0 <= _asdl_i && _asdl_i < (S)->size); \
        (S)->elements[_asdl_i] = (V); \
    } while (0)
#else
#define asdl_seq_SET_UNTYPED(S, I, V) (S)->elements[I] = (V)
#endif

#endif /* !Py_ASDL_H */
#endif /* Py_LIMITED_API */
