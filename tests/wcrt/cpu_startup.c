/** @file cpu_startup.c @brief Checks CPU discovery precedes main/WinMain. */

#include <stddef.h>
#include <wcrt/cpu.h>

static int startup_calls;
static struct wcrt_cpu_info startup_info;

/** @brief Substitutes a counting initializer in the startup-order test. */
const struct wcrt_cpu_info *wcrt_cpu_get_info(void)
{
    ++startup_calls;
    return &startup_info;
}

#ifndef TEST_GUI
/** @brief Observes startup without calling any lazy CPU query. */
int main(void)
{
    return startup_calls == 1 ? 0 : 1;
}

#else
/** @brief Observes GUI startup without calling any lazy CPU query. */
int __attribute__((stdcall)) WinMain(void *instance, void *previous,
    char *command_line, int show)
{
    (void)instance;
    (void)previous;
    (void)command_line;
    (void)show;
    return startup_calls == 1 ? 0 : 1;
}
#endif
