/** @file bool.c @brief Probes compiler-owned C99 Boolean support. */
typedef char wcrt_bool_size[sizeof(_Bool) == 1 ? 1 : -1];
static _Bool wcrt_bool_value = (_Bool)1;

