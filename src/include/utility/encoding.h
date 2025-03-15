/**
 * @file encoding.h
 * @brief Text encoding C interfaces (these interfaces are not implemented currently in C).
 */
#pragma once

// NOLINTBEGIN(modernize-macro-to-enum, cppcoreguidelines-macro-usage, performance-enum-size, modernize-use-using, modernize-deprecated-headers, modernize-avoid-c-arrays) C compatible code

#include <mts/runtime/definitions.h>
#include <mts/runtime/types.h>
#include <mts/runtime/visibility_control.h>

#include <uchar.h>

MTS_BEGIN_C_HEADER

//! The set of supported text encodings
typedef enum mts_text_encoding_t
{
    mts_text_encoding_none = 0,          //!< Unknown text encoding
    mts_text_encoding_utf32_le,          //!< UTF-32 using the little endian byte order
    mts_text_encoding_utf32_be,          //!< UTF-32 using the big endian byte order
    mts_text_encoding_utf16_le,          //!< UTF-16 using the little endian byte order
    mts_text_encoding_utf16_be,          //!< UTF-16 using the big endian byte order
    mts_text_encoding_utf8,              //!< UTF-8 format
    mts_text_encoding_ansi,              //!< ASCII (7-bit) character set
    mts_text_encoding_iso_8859_1,        //!< ISO/IEC 8859-1:1998 encoding
    mts_text_encoding_japanese_shift_jis //!< Shift JIS (Shift Japanese Industrial Standards) encoding
} mts_text_encoding_t;

/**
 * @brief Tests for byte order mark.
 *
 * @param buffer Buffer to test for byte order mark.
 * @param size Buffer length.
 * @param pattern Byte order mark pattern to search for.
 * @param bom_length Byte order mark pattern length.
 * @param result True if byte order mark was found, false otherwise.
 * @param error_details Optional error details.
 * @return MTS error code.
 */
MTS_SDK_SYMBOL_EXPORT
mts_error_t
mts_utility_encoding_has_bom(const uint8_t *buffer, size_t size, const uint8_t *pattern, std::size_t bom_length, bool *result, mts_error_details_handle_t *error_details);

/**
 * @brief Detects text encoding.
 * @param buffer Buffer to be analysed.
 * @param size Buffer length in bytes.
 * @param suggested Suggested encoding, provides a hint to this function.
 * @param encoding The detected encoding if this function succeeds.
 * @param bom_length Detected byte order mark pattern length.
 * @param error_details Optional error details.
 * @return MTS error code.
 */
MTS_SDK_SYMBOL_EXPORT
mts_error_t
mts_utility_encoding_detect(const uint8_t *buffer, size_t size, mts_text_encoding_t suggested, mts_text_encoding_t *encoding, size_t *bom_length, mts_error_details_handle_t *error_details);

/**
 * @brief Guess the encoding of the entity using the first bytes of the entity content
 * @brief accordingly of the non-normative appendix F of the XML-1.0 recommendation (https://www.w3.org/TR/xml/#sec-guessing).
 *
 * @param buffer A pointer to the first bytes of the XML entity, must be at least 4 bytes long.
 * @param size Buffer length in bytes.
 * @param encoding The detected encoding if this function succeeds.
 * @param error_details Optional error details.
 * @return MTS error code.
 */
MTS_SDK_SYMBOL_EXPORT
mts_error_t
mts_utility_encoding_detect_xml(const uint8_t *buffer, size_t size, mts_text_encoding_t *encoding, mts_error_details_handle_t *error_details);

/**
 * @brief Detects and retrieves text from a buffer. Any byte order mark detected will be removed from the resulting string.
 * @param buffer Buffer with text data.
 * @param size Buffer length in bytes.
 * @param suggested Suggested encoding, this value will be used if the function cannot detect the character encoding.
 * @param result A pointer to a variable that receives the converted string.
 * @param result_size A pointer to a variable that provides the size of the caller-provided buffer pointed to by the @ref result parameter, in bytes.
 * @param result_size When the function returns, this variable contains the size of the data copied to @ref result.
 * @param error_details Optional error details.
 * @return MTS error code.
 *
 * @remarks If the buffer specified by @ref result parameter is not large enough to hold the data,
 * @remarks the function returns @ref mts_error_no_buffer_space and stores the required buffer size in the variable pointed to by @ref result_size.
 * @remarks In this case, the contents of the @ref result buffer is undefined.
 */
MTS_SDK_SYMBOL_EXPORT
mts_error_t
mts_utility_encoding_convert_to_utf8(const uint8_t *buffer, size_t size, mts_text_encoding_t suggested, char *result, size_t *result_size, mts_error_details_handle_t *error_details);

/**
 * @brief Detects and retrieves text from a buffer. Any byte order mark detected will be removed from the resulting string.
 * @param buffer Buffer with text data.
 * @param size Buffer length in bytes.
 * @param suggested Suggested encoding, this value will be used if the function cannot detect the character encoding.
 * @param result A pointer to a variable that receives the converted string.
 * @param result_size A pointer to a variable that provides the size of the caller-provided buffer pointed to by the @ref result parameter, in bytes.
 * @param result_size When the function returns, this variable contains the size of the data copied to @ref result.
 * @param error_details Optional error details.
 * @return MTS error code.
 *
 * @remarks If the buffer specified by @ref result parameter is not large enough to hold the data,
 * @remarks the function returns @ref mts_error_no_buffer_space and stores the required buffer size in the variable pointed to by @ref result_size.
 * @remarks In this case, the contents of the @ref result buffer is undefined.
 */
MTS_SDK_SYMBOL_EXPORT
mts_error_t
mts_utility_encoding_convert_to_utf16(const uint8_t *buffer, size_t size, mts_text_encoding_t suggested, char16_t *result, size_t *result_size, mts_error_details_handle_t *error_details);

MTS_END_C_HEADER

// NOLINTEND(modernize-macro-to-enum, cppcoreguidelines-macro-usage, performance-enum-size, modernize-use-using, modernize-deprecated-headers, modernize-avoid-c-arrays) C compatible code
