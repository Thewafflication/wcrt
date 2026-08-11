/**
 * @file fenv.c
 * @brief Implements the C99 environment over x87/MXCSR or ARM64 FPCR/FPSR.
 */

#include <fenv.h>

#if defined(__i386__) || defined(__x86_64__)

/** @brief Native 28-byte x87 environment image used by FNSTENV/FLDENV. */
struct wcrt_x87_environment {
    unsigned short control;
    unsigned short reserved0;
    unsigned short status;
    unsigned short reserved1;
    unsigned short tag;
    unsigned short reserved2;
    unsigned int instruction_offset;
    unsigned short instruction_selector;
    unsigned short opcode;
    unsigned int data_offset;
    unsigned short data_selector;
    unsigned short reserved3;
};

static unsigned short wcrt_x87_control(void)
{
    unsigned short value;
    __asm__ volatile ("fnstcw %0" : "=m" (value));
    return value;
}

static void wcrt_x87_set_control(unsigned short value)
{
    __asm__ volatile ("fldcw %0" : : "m" (value));
}

static unsigned short wcrt_x87_status(void)
{
    unsigned short value;
    __asm__ volatile ("fnstsw %0" : "=m" (value));
    return value;
}

static void wcrt_x87_set_status(unsigned short flags, unsigned short mask)
{
    struct wcrt_x87_environment environment;

    __asm__ volatile ("fnstenv %0" : "=m" (environment));
    environment.status = (unsigned short)((environment.status & ~mask) |
        (flags & mask));
    __asm__ volatile ("fldenv %0" : : "m" (environment));
}

#if defined(__x86_64__)
static unsigned int wcrt_mxcsr(void)
{
    unsigned int value;
    __asm__ volatile ("stmxcsr %0" : "=m" (value));
    return value;
}

static void wcrt_set_mxcsr(unsigned int value)
{
    __asm__ volatile ("ldmxcsr %0" : : "m" (value));
}
#endif

static unsigned int wcrt_native_flags(void)
{
    unsigned int flags = (unsigned int)wcrt_x87_status();
#if defined(__x86_64__)
    flags |= wcrt_mxcsr();
#endif
    return flags & FE_ALL_EXCEPT;
}

static void wcrt_write_flags(unsigned int flags, unsigned int mask)
{
    unsigned short narrow_mask = (unsigned short)(mask & FE_ALL_EXCEPT);

    wcrt_x87_set_status((unsigned short)flags, narrow_mask);
#if defined(__x86_64__)
    {
        unsigned int mxcsr = wcrt_mxcsr();
        mxcsr = (mxcsr & ~mask) | (flags & mask);
        wcrt_set_mxcsr(mxcsr);
    }
#endif
}

static unsigned int wcrt_native_round(void)
{
#if defined(__x86_64__)
    return (wcrt_mxcsr() >> 3) & 0x0c00U;
#else
    return (unsigned int)wcrt_x87_control() & 0x0c00U;
#endif
}

static void wcrt_write_round(unsigned int round)
{
    unsigned short control = wcrt_x87_control();
    control = (unsigned short)((control & ~0x0c00U) | round);
    wcrt_x87_set_control(control);
#if defined(__x86_64__)
    {
        unsigned int mxcsr = wcrt_mxcsr();
        mxcsr = (mxcsr & ~0x6000U) | (round << 3);
        wcrt_set_mxcsr(mxcsr);
    }
#endif
}

static void wcrt_capture(fenv_t *environment)
{
    environment->__control = wcrt_native_round();
    environment->__status = wcrt_native_flags();
    environment->__reserved[0] = (unsigned int)wcrt_x87_control();
#if defined(__x86_64__)
    environment->__reserved[1] = wcrt_mxcsr();
#else
    environment->__reserved[1] = 0U;
#endif
}

static void wcrt_install(const fenv_t *environment)
{
    unsigned short x87_control;

    x87_control = environment->__reserved[0] != 0U ?
        (unsigned short)environment->__reserved[0] : (unsigned short)0x027fU;
    x87_control = (unsigned short)((x87_control & ~0x0c00U) |
        (environment->__control & 0x0c00U));
    wcrt_x87_set_control(x87_control);
#if defined(__x86_64__)
    {
        unsigned int mxcsr = environment->__reserved[1] != 0U ?
            environment->__reserved[1] : 0x00001f80U;
        mxcsr = (mxcsr & ~(0x6000U | FE_ALL_EXCEPT)) |
            ((environment->__control & 0x0c00U) << 3) |
            (environment->__status & FE_ALL_EXCEPT);
        wcrt_set_mxcsr(mxcsr);
    }
#endif
    wcrt_write_flags(environment->__status, FE_ALL_EXCEPT);
}

static void wcrt_install_default(void)
{
    fenv_t environment;
    environment.__control = FE_TONEAREST;
    environment.__status = 0U;
    environment.__reserved[0] = 0x027fU;
#if defined(__x86_64__)
    environment.__reserved[1] = 0x00001f80U;
#else
    environment.__reserved[1] = 0U;
#endif
    wcrt_install(&environment);
}

static void wcrt_mask_traps(void)
{
    unsigned short control = wcrt_x87_control();
    wcrt_x87_set_control((unsigned short)(control | 0x003fU));
#if defined(__x86_64__)
    wcrt_set_mxcsr(wcrt_mxcsr() | 0x00001f80U);
#endif
}

#elif defined(__aarch64__)

static unsigned long long wcrt_fpcr(void)
{
    unsigned long long value;
    __asm__ volatile ("mrs %0, fpcr" : "=r" (value));
    return value;
}

static void wcrt_set_fpcr(unsigned long long value)
{
    __asm__ volatile ("msr fpcr, %0" : : "r" (value));
}

static unsigned long long wcrt_fpsr(void)
{
    unsigned long long value;
    __asm__ volatile ("mrs %0, fpsr" : "=r" (value));
    return value;
}

static void wcrt_set_fpsr(unsigned long long value)
{
    __asm__ volatile ("msr fpsr, %0" : : "r" (value));
}

static unsigned int wcrt_flags_from_fpsr(unsigned long long status)
{
    unsigned int flags = 0U;
    if (status & 0x01U) flags |= FE_INVALID;
    if (status & 0x02U) flags |= FE_DIVBYZERO;
    if (status & 0x04U) flags |= FE_OVERFLOW;
    if (status & 0x08U) flags |= FE_UNDERFLOW;
    if (status & 0x10U) flags |= FE_INEXACT;
    return flags;
}

static unsigned long long wcrt_flags_to_fpsr(unsigned int flags)
{
    unsigned long long status = 0U;
    if (flags & FE_INVALID) status |= 0x01U;
    if (flags & FE_DIVBYZERO) status |= 0x02U;
    if (flags & FE_OVERFLOW) status |= 0x04U;
    if (flags & FE_UNDERFLOW) status |= 0x08U;
    if (flags & FE_INEXACT) status |= 0x10U;
    return status;
}

static unsigned int wcrt_round_from_fpcr(unsigned long long control)
{
    switch ((unsigned int)((control >> 22) & 3U)) {
    case 1U: return FE_UPWARD;
    case 2U: return FE_DOWNWARD;
    case 3U: return FE_TOWARDZERO;
    default: return FE_TONEAREST;
    }
}

static unsigned long long wcrt_round_to_fpcr(unsigned int round)
{
    if (round == FE_UPWARD) return 1ULL << 22;
    if (round == FE_DOWNWARD) return 2ULL << 22;
    if (round == FE_TOWARDZERO) return 3ULL << 22;
    return 0ULL;
}

static unsigned int wcrt_native_flags(void)
{
    return wcrt_flags_from_fpsr(wcrt_fpsr());
}

static void wcrt_write_flags(unsigned int flags, unsigned int mask)
{
    unsigned long long native_mask = wcrt_flags_to_fpsr(mask);
    unsigned long long status = wcrt_fpsr();
    status = (status & ~native_mask) |
        (wcrt_flags_to_fpsr(flags) & native_mask);
    wcrt_set_fpsr(status);
}

static unsigned int wcrt_native_round(void)
{
    return wcrt_round_from_fpcr(wcrt_fpcr());
}

static void wcrt_write_round(unsigned int round)
{
    unsigned long long control = wcrt_fpcr();
    control = (control & ~(3ULL << 22)) | wcrt_round_to_fpcr(round);
    wcrt_set_fpcr(control);
}

static void wcrt_capture(fenv_t *environment)
{
    unsigned long long control = wcrt_fpcr();
    unsigned long long status = wcrt_fpsr();
    environment->__control = wcrt_round_from_fpcr(control);
    environment->__status = wcrt_flags_from_fpsr(status);
    environment->__reserved[0] = (unsigned int)control;
    environment->__reserved[1] = (unsigned int)(control >> 32);
}

static void wcrt_install(const fenv_t *environment)
{
    unsigned long long control =
        (unsigned long long)environment->__reserved[0] |
        ((unsigned long long)environment->__reserved[1] << 32);
    control = (control & ~(3ULL << 22)) |
        wcrt_round_to_fpcr(environment->__control);
    wcrt_set_fpcr(control);
    wcrt_write_flags(environment->__status, FE_ALL_EXCEPT);
}

static void wcrt_install_default(void)
{
    wcrt_set_fpcr(0ULL);
    wcrt_write_flags(0U, FE_ALL_EXCEPT);
}

static void wcrt_mask_traps(void)
{
    wcrt_set_fpcr(wcrt_fpcr() & ~(0x1fULL << 8));
}

#else
#error WCRT fenv requires an x86, x64, or ARM64 target
#endif

int feclearexcept(int excepts)
{
    wcrt_write_flags(0U, (unsigned int)excepts & FE_ALL_EXCEPT);
    return 0;
}

int fegetexceptflag(fexcept_t *flagp, int excepts)
{
    if (flagp == 0) return 1;
    *flagp = wcrt_native_flags() & ((unsigned int)excepts & FE_ALL_EXCEPT);
    return 0;
}

int feraiseexcept(int excepts)
{
    unsigned int mask = (unsigned int)excepts & FE_ALL_EXCEPT;
    wcrt_write_flags(wcrt_native_flags() | mask, mask);
    return 0;
}

int fesetexceptflag(const fexcept_t *flagp, int excepts)
{
    unsigned int mask;
    if (flagp == 0) return 1;
    mask = (unsigned int)excepts & FE_ALL_EXCEPT;
    wcrt_write_flags(*flagp, mask);
    return 0;
}

int fetestexcept(int excepts)
{
    return (int)(wcrt_native_flags() &
        ((unsigned int)excepts & FE_ALL_EXCEPT));
}

int fegetround(void)
{
    return (int)wcrt_native_round();
}

int fesetround(int round)
{
    if (round != FE_TONEAREST && round != FE_DOWNWARD &&
        round != FE_UPWARD && round != FE_TOWARDZERO) return 1;
    wcrt_write_round((unsigned int)round);
    return 0;
}

int fegetenv(fenv_t *envp)
{
    if (envp == 0) return 1;
    wcrt_capture(envp);
    return 0;
}

int feholdexcept(fenv_t *envp)
{
    if (envp == 0) return 1;
    wcrt_capture(envp);
    feclearexcept(FE_ALL_EXCEPT);
    wcrt_mask_traps();
    return 0;
}

int fesetenv(const fenv_t *envp)
{
    if (envp == FE_DFL_ENV) {
        wcrt_install_default();
        return 0;
    }
    if (envp == 0) return 1;
    wcrt_install(envp);
    return 0;
}

int feupdateenv(const fenv_t *envp)
{
    unsigned int raised = wcrt_native_flags();
    if (fesetenv(envp) != 0) return 1;
    return feraiseexcept((int)raised);
}
