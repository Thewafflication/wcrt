/**
 * @file ctype.c
 * @brief Implements C89 character handling for the C locale.
 */

#include <ctype.h>

/** @brief Implements the public islower contract. */
int islower(int character)
{
    return character >= 'a' && character <= 'z';
}

/** @brief Implements the public isupper contract. */
int isupper(int character)
{
    return character >= 'A' && character <= 'Z';
}

/** @brief Implements the public isalpha contract. */
int isalpha(int character)
{
    return islower(character) || isupper(character);
}

/** @brief Implements the public isdigit contract. */
int isdigit(int character)
{
    return character >= '0' && character <= '9';
}

/** @brief Implements the public isalnum contract. */
int isalnum(int character)
{
    return isalpha(character) || isdigit(character);
}

/** @brief Implements the public iscntrl contract. */
int iscntrl(int character)
{
    return (character >= 0 && character <= 0x1f) || character == 0x7f;
}

/** @brief Implements the public isgraph contract. */
int isgraph(int character)
{
    return character >= 0x21 && character <= 0x7e;
}

/** @brief Implements the public isprint contract. */
int isprint(int character)
{
    return character >= 0x20 && character <= 0x7e;
}

/** @brief Implements the public isspace contract. */
int isspace(int character)
{
    return character == ' ' ||
        (character >= '\t' && character <= '\r');
}

/** @brief Implements the public ispunct contract. */
int ispunct(int character)
{
    return isgraph(character) && !isalnum(character);
}

/** @brief Implements the public isxdigit contract. */
int isxdigit(int character)
{
    return isdigit(character) ||
        (character >= 'a' && character <= 'f') ||
        (character >= 'A' && character <= 'F');
}

/** @brief Implements the public tolower contract. */
int tolower(int character)
{
    if (isupper(character)) {
        return character + ('a' - 'A');
    }
    return character;
}

/** @brief Implements the public toupper contract. */
int toupper(int character)
{
    if (islower(character)) {
        return character - ('a' - 'A');
    }
    return character;
}
