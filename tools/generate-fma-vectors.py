#!/usr/bin/env python3
"""Generate deterministic exact-rational binary64 fused-operation vectors."""

from __future__ import annotations

import json
import math
import random
import struct
from fractions import Fraction


SEED = 0xF6A0992026
RANDOM_VECTOR_COUNT = 256
FRACTION_MASK = (1 << 52) - 1
MAX_FINITE = 0x7FEFFFFFFFFFFFFF


def value_from_bits(bits: int) -> float:
    """Return the binary64 value with the supplied representation."""
    return struct.unpack(">d", struct.pack(">Q", bits))[0]


def bits_from_value(value: float) -> int:
    """Return the binary64 representation of a Python float."""
    return struct.unpack(">Q", struct.pack(">d", value))[0]


def fraction_from_bits(bits: int) -> Fraction:
    """Return the exact rational value of one finite binary64 encoding."""
    sign = -1 if bits >> 63 else 1
    exponent = (bits >> 52) & 0x7FF
    significand = bits & FRACTION_MASK
    if exponent == 0:
        return Fraction(sign * significand, 1 << 1074)
    significand |= 1 << 52
    binary_exponent = exponent - 1023 - 52
    if binary_exponent >= 0:
        return Fraction(sign * significand * (1 << binary_exponent), 1)
    return Fraction(sign * significand, 1 << -binary_exponent)


def finite_rounding_bits(value: Fraction) -> int:
    """Round once to binary64, retaining WCRT's finite overflow result."""
    try:
        rounded = float(value)
    except OverflowError:
        return MAX_FINITE | ((1 << 63) if value < 0 else 0)
    if math.isinf(rounded):
        return MAX_FINITE | ((1 << 63) if value < 0 else 0)
    return bits_from_value(rounded)


def vector(lhs: int, middle: int, rhs: int) -> dict[str, str]:
    """Build one exact fused-operation vector from three finite encodings."""
    exact = (fraction_from_bits(lhs) * fraction_from_bits(middle) +
             fraction_from_bits(rhs))
    return {
        "lhs": f"0x{lhs:016x}",
        "middle": f"0x{middle:016x}",
        "rhs": f"0x{rhs:016x}",
        "expected": f"0x{finite_rounding_bits(exact):016x}",
    }


def finite_nonzero_bits(generator: random.Random) -> int:
    """Return a deterministic finite nonzero binary64 encoding."""
    while True:
        bits = generator.getrandbits(64)
        if ((bits >> 52) & 0x7FF) != 0x7FF and bits << 1 != 0:
            return bits


def main() -> None:
    """Emit the controlled vector set as stable JSON."""
    generator = random.Random(SEED)
    triples = [
        (0x3FF0000008000000, 0x3FEFFFFFF0000000,
         0xBFF0000000000000),
        (0x0010000000000000, 0x3FE0000000000000,
         0x0000000000000001),
        (0x7FEFFFFFFFFFFFFF, 0x4000000000000000,
         0xFFEFFFFFFFFFFFFF),
        (0x8000000000000001, 0x3FE0000000000000,
         0x0000000000000000),
    ]
    for _ in range(RANDOM_VECTOR_COUNT):
        triples.append(tuple(finite_nonzero_bits(generator)
                             for _ in range(3)))
    record = {
        "schemaVersion": 1,
        "source": "Python Fraction exact product-plus-addend",
        "seed": f"0x{SEED:x}",
        "rounding": "nearest-even with WCRT finite HUGE_VAL overflow",
        "vectorCount": len(triples),
        "vectors": [vector(*triple) for triple in triples],
    }
    print(json.dumps(record, indent=2, sort_keys=False))


if __name__ == "__main__":
    main()
