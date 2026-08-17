"""Generate the ordinary TC-0037 vectors with Decimal complex arithmetic."""

from decimal import Decimal as D
from decimal import getcontext
import json

getcontext().prec = 100

PI = D("3.141592653589793238462643383279502884197169399375105820974944592")
HALF_PI = PI / 2
QUARTER_PI = PI / 4
ONE = D(1)
TWO = D(2)
EPSILON = D(10) ** -95


class Complex:
    """Minimal high-precision complex value for independently retained data."""

    def __init__(self, real=0, imaginary=0):
        self.real = D(real)
        self.imaginary = D(imaginary)

    def __add__(self, other):
        other = as_complex(other)
        return Complex(self.real + other.real, self.imaginary + other.imaginary)

    __radd__ = __add__

    def __sub__(self, other):
        other = as_complex(other)
        return Complex(self.real - other.real, self.imaginary - other.imaginary)

    def __rsub__(self, other):
        other = as_complex(other)
        return other - self

    def __mul__(self, other):
        other = as_complex(other)
        return Complex(
            self.real * other.real - self.imaginary * other.imaginary,
            self.real * other.imaginary + self.imaginary * other.real,
        )

    __rmul__ = __mul__

    def __truediv__(self, other):
        other = as_complex(other)
        denominator = other.real * other.real + other.imaginary * other.imaginary
        return Complex(
            (self.real * other.real + self.imaginary * other.imaginary)
            / denominator,
            (self.imaginary * other.real - self.real * other.imaginary)
            / denominator,
        )


def as_complex(value):
    """Convert a real Decimal-compatible value to Complex when necessary."""
    return value if isinstance(value, Complex) else Complex(value)


def sine(value):
    """Evaluate sine after reduction to the primary interval."""
    value %= TWO * PI
    if value > PI:
        value -= TWO * PI
    term = value
    result = value
    index = 1
    while abs(term) > EPSILON:
        term *= -(value * value) / D((2 * index) * (2 * index + 1))
        result += term
        index += 1
    return result


def cosine(value):
    """Evaluate cosine after reduction to the primary interval."""
    value %= TWO * PI
    if value > PI:
        value -= TWO * PI
    term = ONE
    result = ONE
    index = 1
    while abs(term) > EPSILON:
        term *= -(value * value) / D((2 * index - 1) * (2 * index))
        result += term
        index += 1
    return result


def arctangent(value):
    """Evaluate real arctangent with reciprocal and pi/4 reductions."""
    if value < 0:
        return -arctangent(-value)
    if value > ONE:
        return HALF_PI - arctangent(ONE / value)
    if value > D(2).sqrt() - ONE:
        return QUARTER_PI + arctangent((value - ONE) / (value + ONE))
    term = value
    result = value
    square = value * value
    index = 1
    while abs(term) > EPSILON:
        term *= -square
        result += term / D(2 * index + 1)
        index += 1
    return result


def argument(value):
    """Return the principal argument for a nonzero finite Complex value."""
    angle = arctangent(abs(value.imaginary / value.real))
    if value.real < 0:
        angle = PI - angle
    if value.imaginary < 0:
        angle = -angle
    return angle


def exponential(value):
    """Evaluate the complex exponential."""
    magnitude = value.real.exp()
    return Complex(magnitude * cosine(value.imaginary),
                   magnitude * sine(value.imaginary))


def logarithm(value):
    """Evaluate the principal complex logarithm."""
    magnitude = (value.real * value.real +
                 value.imaginary * value.imaginary).sqrt()
    return Complex(magnitude.ln(), argument(value))


def square_root(value):
    """Evaluate the principal complex square root."""
    magnitude = (value.real * value.real +
                 value.imaginary * value.imaginary).sqrt()
    real = ((magnitude + value.real) / TWO).sqrt()
    imaginary = ((magnitude - value.real) / TWO).sqrt()
    if value.imaginary < 0:
        imaginary = -imaginary
    return Complex(real, imaginary)


def hyperbolic_sine(value):
    """Evaluate complex hyperbolic sine."""
    return Complex((value.real.exp() - (-value.real).exp()) / TWO *
                   cosine(value.imaginary),
                   (value.real.exp() + (-value.real).exp()) / TWO *
                   sine(value.imaginary))


def hyperbolic_cosine(value):
    """Evaluate complex hyperbolic cosine."""
    return Complex((value.real.exp() + (-value.real).exp()) / TWO *
                   cosine(value.imaginary),
                   (value.real.exp() - (-value.real).exp()) / TWO *
                   sine(value.imaginary))


def complex_sine(value):
    """Evaluate complex sine."""
    return Complex(sine(value.real) *
                   (value.imaginary.exp() + (-value.imaginary).exp()) / TWO,
                   cosine(value.real) *
                   (value.imaginary.exp() - (-value.imaginary).exp()) / TWO)


def complex_cosine(value):
    """Evaluate complex cosine."""
    return Complex(cosine(value.real) *
                   (value.imaginary.exp() + (-value.imaginary).exp()) / TWO,
                   -sine(value.real) *
                   (value.imaginary.exp() - (-value.imaginary).exp()) / TWO)


I = Complex(0, 1)


def arc_sine(value):
    """Evaluate principal complex inverse sine."""
    result = logarithm(I * value + square_root(ONE - value * value))
    return Complex(result.imaginary, -result.real)


def arc_cosine(value):
    """Evaluate principal complex inverse cosine."""
    result = arc_sine(value)
    return Complex(HALF_PI - result.real, -result.imaginary)


def arc_tangent(value):
    """Evaluate principal complex inverse tangent."""
    difference = logarithm(ONE - I * value) - logarithm(ONE + I * value)
    return Complex(-difference.imaginary / TWO, difference.real / TWO)


def arc_hyperbolic_sine(value):
    """Evaluate principal complex inverse hyperbolic sine."""
    return logarithm(value + square_root(value * value + ONE))


def arc_hyperbolic_cosine(value):
    """Evaluate principal complex inverse hyperbolic cosine."""
    return logarithm(value + square_root(value + ONE) *
                     square_root(value - ONE))


def arc_hyperbolic_tangent(value):
    """Evaluate principal complex inverse hyperbolic tangent."""
    return (logarithm(ONE + value) - logarithm(ONE - value)) / TWO


def render(value):
    """Render enough significant digits for the controlled binary64 vector."""
    return {
        "real": format(value.real, ".40g"),
        "imaginary": format(value.imaginary, ".40g"),
    }


def main():
    """Print deterministic vector evidence as JSON."""
    value = Complex("0.5", "-0.75")
    functions = {
        "cacos": arc_cosine,
        "casin": arc_sine,
        "catan": arc_tangent,
        "ccos": complex_cosine,
        "csin": complex_sine,
        "ctan": lambda item: complex_sine(item) / complex_cosine(item),
        "cacosh": arc_hyperbolic_cosine,
        "casinh": arc_hyperbolic_sine,
        "catanh": arc_hyperbolic_tangent,
        "ccosh": hyperbolic_cosine,
        "csinh": hyperbolic_sine,
        "ctanh": lambda item: hyperbolic_sine(item) / hyperbolic_cosine(item),
        "cexp": exponential,
        "clog": logarithm,
        "csqrt": square_root,
    }
    vectors = {name: render(function(value))
               for name, function in functions.items()}
    vectors["cpow"] = render(exponential(Complex("1.25", "-0.5") *
                                          logarithm(value)))
    evidence = {
        "schema": 1,
        "generator": "tools/generate-complex-vectors.py",
        "decimalPrecisionDigits": getcontext().prec,
        "rounding": str(getcontext().rounding),
        "input": {"real": "0.5", "imaginary": "-0.75"},
        "power": {"real": "1.25", "imaginary": "-0.5"},
        "vectors": vectors,
    }
    print(json.dumps(evidence, indent=2, sort_keys=True))


if __name__ == "__main__":
    main()
