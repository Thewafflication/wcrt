/**
 * @file signal.c
 * @brief Implements the Windows default signal action.
 */

#if defined(_WIN32)
/** @brief Selects the Windows API calling convention. */
#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

/** @brief Windows process termination primitive. */
__declspec(dllimport) void WCRT_WINAPI ExitProcess(unsigned int exit_code);
#endif

void __wcrt_signal_terminate(int signal_number)
{
#if defined(_WIN32)
    ExitProcess((unsigned int)(128 + signal_number));
#else
    (void)signal_number;
#endif
}
