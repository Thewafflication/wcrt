/**
 * @file ctype.h
 * @brief Declares C89 character classification and case mapping.
 */

#ifndef WCRT_CTYPE_H
#define WCRT_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Tests whether a character is alphabetic or decimal numeric. */
int isalnum(int character);

/** @brief Tests whether a character is alphabetic. */
int isalpha(int character);

/** @brief Tests whether a character is a control character. */
int iscntrl(int character);

/** @brief Tests whether a character is a decimal digit. */
int isdigit(int character);

/** @brief Tests whether a character has a visible representation. */
int isgraph(int character);

/** @brief Tests whether a character is a lowercase letter. */
int islower(int character);

/** @brief Tests whether a character is printable, including space. */
int isprint(int character);

/** @brief Tests whether a character is printable punctuation. */
int ispunct(int character);

/** @brief Tests whether a character is white space. */
int isspace(int character);

/** @brief Tests whether a character is an uppercase letter. */
int isupper(int character);

/** @brief Tests whether a character is a hexadecimal digit. */
int isxdigit(int character);

/** @brief Converts an uppercase letter to lowercase when possible. */
int tolower(int character);

/** @brief Converts a lowercase letter to uppercase when possible. */
int toupper(int character);

#ifdef __cplusplus
}
#endif

#endif
