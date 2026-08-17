/**
 * @file tinycc_complex_abi.c
 * @brief Completes TinyCC ARM64 scalar complex-arithmetic helper calls.
 *
 * Waughtal C Run Time
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#if defined(__aarch64__) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

#include <math.h>

/** Stores a scaled C99 complex product for the TinyCC ARM64 ABI bridge. */
void __wcrt_tinycc_muldc3(double a, double b, double c, double d,
    double *output)
{
    double ac = a * c;
    double bd = b * d;
    double ad = a * d;
    double bc = b * c;
    double real = ac - bd;
    double imaginary = ad + bc;

    if (isfinite(a) && isfinite(b) && isfinite(c) && isfinite(d)) {
        double left_scale = fmax(fabs(a), fabs(b));
        double right_scale = fmax(fabs(c), fabs(d));
        if (left_scale != 0.0 && right_scale != 0.0) {
            int left_exponent = ilogb(left_scale);
            int right_exponent = ilogb(right_scale);
            double scaled_a = scalbn(a, -left_exponent);
            double scaled_b = scalbn(b, -left_exponent);
            double scaled_c = scalbn(c, -right_exponent);
            double scaled_d = scalbn(d, -right_exponent);
            double scaled_real = scaled_a * scaled_c -
                scaled_b * scaled_d;
            double scaled_imaginary = scaled_a * scaled_d +
                scaled_b * scaled_c;
            int result_exponent = left_exponent + right_exponent;
            real = scalbn(scaled_real, result_exponent);
            imaginary = scalbn(scaled_imaginary, result_exponent);
        }
    }

    if (isnan(real) && isnan(imaginary)) {
        int recalculate = 0;
        if (isinf(a) || isinf(b)) {
            a = copysign(isinf(a) ? 1.0 : 0.0, a);
            b = copysign(isinf(b) ? 1.0 : 0.0, b);
            if (isnan(c)) c = copysign(0.0, c);
            if (isnan(d)) d = copysign(0.0, d);
            recalculate = 1;
        }
        if (isinf(c) || isinf(d)) {
            c = copysign(isinf(c) ? 1.0 : 0.0, c);
            d = copysign(isinf(d) ? 1.0 : 0.0, d);
            if (isnan(a)) a = copysign(0.0, a);
            if (isnan(b)) b = copysign(0.0, b);
            recalculate = 1;
        }
        if (!recalculate &&
            (isinf(ac) || isinf(bd) || isinf(ad) || isinf(bc))) {
            if (isnan(a)) a = copysign(0.0, a);
            if (isnan(b)) b = copysign(0.0, b);
            if (isnan(c)) c = copysign(0.0, c);
            if (isnan(d)) d = copysign(0.0, d);
            recalculate = 1;
        }
        if (recalculate) {
            real = INFINITY * (a * c - b * d);
            imaginary = INFINITY * (a * d + b * c);
        }
    }
    output[0] = real;
    output[1] = imaginary;
}

/** Stores a scaled C99 complex quotient for the TinyCC ARM64 ABI bridge. */
void __wcrt_tinycc_divdc3(double a, double b, double c, double d,
    double *output)
{
    double ratio;
    double denominator;
    double real;
    double imaginary;

    if (isfinite(a) && isfinite(b) && isfinite(c) && isfinite(d) &&
        (c != 0.0 || d != 0.0)) {
        double denominator_scale = fmax(fabs(c), fabs(d));
        double numerator_scale = fmax(fabs(a), fabs(b));
        int denominator_exponent = ilogb(denominator_scale);
        double scaled_c = scalbn(c, -denominator_exponent);
        double scaled_d = scalbn(d, -denominator_exponent);
        denominator = scaled_c * scaled_c + scaled_d * scaled_d;
        if (numerator_scale == 0.0) {
            real = scalbn((a * scaled_c + b * scaled_d) / denominator,
                -denominator_exponent);
            imaginary = scalbn((b * scaled_c - a * scaled_d) /
                denominator, -denominator_exponent);
        } else {
            int numerator_exponent = ilogb(numerator_scale);
            double scaled_a = scalbn(a, -numerator_exponent);
            double scaled_b = scalbn(b, -numerator_exponent);
            int result_exponent = numerator_exponent -
                denominator_exponent;
            real = scalbn((scaled_a * scaled_c +
                scaled_b * scaled_d) / denominator, result_exponent);
            imaginary = scalbn((scaled_b * scaled_c -
                scaled_a * scaled_d) / denominator, result_exponent);
        }
    } else if (fabs(c) < fabs(d)) {
        ratio = c / d;
        denominator = c * ratio + d;
        real = (a * ratio + b) / denominator;
        imaginary = (b * ratio - a) / denominator;
    } else {
        ratio = d / c;
        denominator = d * ratio + c;
        real = (b * ratio + a) / denominator;
        imaginary = (b - a * ratio) / denominator;
    }

    if (isnan(real) && isnan(imaginary)) {
        if (c == 0.0 && d == 0.0 && (!isnan(a) || !isnan(b))) {
            double infinity = copysign(INFINITY, c);
            real = infinity * a;
            imaginary = infinity * b;
        } else if ((isinf(a) || isinf(b)) &&
            isfinite(c) && isfinite(d)) {
            a = copysign(isinf(a) ? 1.0 : 0.0, a);
            b = copysign(isinf(b) ? 1.0 : 0.0, b);
            real = INFINITY * (a * c + b * d);
            imaginary = INFINITY * (b * c - a * d);
        } else if ((isinf(c) || isinf(d)) &&
            isfinite(a) && isfinite(b)) {
            c = copysign(isinf(c) ? 1.0 : 0.0, c);
            d = copysign(isinf(d) ? 1.0 : 0.0, d);
            real = 0.0 * (a * c + b * d);
            imaginary = 0.0 * (b * c - a * d);
        }
    }
    output[0] = real;
    output[1] = imaginary;
}

#endif
