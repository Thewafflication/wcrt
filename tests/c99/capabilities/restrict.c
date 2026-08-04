/** @file restrict.c @brief Probes C99 restrict-qualified declarators. */
static int wcrt_restrict_sum(const int * restrict left,
    const int * restrict right)
{
    return *left + *right;
}

