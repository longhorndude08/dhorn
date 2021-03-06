/*
 * Duncan Horn
 *
 * numeric.h
 *
 * Various functions that deal with numeric values (either integral or not). The input of each function is expected to
 * be of the form:
 *
 *  [+/-][digits...][.][digits...][e/E][+/-][digits...]
 */
#pragma once

#pragma warning(push)
#pragma warning(disable:4702) // Unreachable code -- seems to be a bug with this...

#include <string>

#include "utf_string.h"

namespace dhorn
{
    namespace experimental
    {
        namespace details
        {
            template <typename NumericType>
            inline constexpr std::enable_if_t<std::is_signed_v<NumericType>, NumericType> negate(NumericType val)
            {
                return -val;
            }

            template <typename NumericType>
            inline constexpr std::enable_if_t<std::is_unsigned_v<NumericType>, NumericType> negate(NumericType)
            {
                throw std::invalid_argument("Cannot negate an unsigned type");
            }

            template <typename CharT>
            inline constexpr bool is_digit(CharT ch) noexcept
            {
                return (ch >= '0') && (ch <= '9');
            }

            template <typename CharT>
            inline const CharT *skip_digits(const CharT *str) noexcept
            {
                for (; is_digit(*str); ++str);
                return str;
            }

            template <typename NumericType, typename CharT>
            inline constexpr NumericType to_digit(CharT ch) noexcept
            {
                return static_cast<NumericType>(ch - '0');
            }

            template <typename CharT>
            inline constexpr bool is_sign(CharT ch) noexcept
            {
                return (ch == '+') || (ch == '-');
            }

            template <typename CharT>
            inline const CharT *skip_sign(CharT *str) noexcept
            {
                if (is_sign(*str))
                {
                    ++str;
                }
                return str;
            }

            template <typename CharT>
            inline constexpr bool is_decimal(CharT ch) noexcept
            {
                return ch == '.';
            }

            template <typename CharT>
            inline const CharT *skip_decimal(CharT *str) noexcept
            {
                if (is_decimal(*str))
                {
                    ++str;
                }
                return str;
            }

            template <typename CharT>
            inline constexpr bool is_exponential(CharT ch) noexcept
            {
                return (ch == 'e') || (ch == 'E');
            }

            template <typename CharT>
            inline const CharT *skip_exponential(CharT *str) noexcept
            {
                if (is_exponential(*str))
                {
                    ++str;
                }
                return str;
            }

            template <typename NumericType, typename CharT>
            const CharT *read_integer_value(const CharT *front, const CharT *back, NumericType *value)
            {
                for (; (front != back) && is_digit(*front); ++front)
                {
                    *value = ((*value) * 10) + to_digit<NumericType>(*front);
                }

                return front;
            }

            template <typename NumericType, typename CharT>
            const CharT *read_fractional_value(const CharT *front, const CharT *back, NumericType *value, bool* isNonZero)
            {
                if (back == nullptr)
                {
                    for (back = front; is_digit(*back); ++back);
                }

                bool nonZero = false;
                for (; back > front; --back)
                {
                    nonZero = nonZero || (*(back - 1) != '0');
                    *value = (*value + to_digit<NumericType>(*(back - 1))) / 10;
                }

                *isNonZero = *isNonZero || nonZero;

                return front;
            }
        }



        /*
         * String literal conversion
         */
#pragma region String Literal Conversion

        template <typename NumericType, typename CharT>
        NumericType numeric_cast(const CharT *str)
        {
            // Since the string could be of some crazy form like 10000000000000e-13, we need to keep the value in string
            // form for as long as possible and shift the decimal via string pointers
            const CharT *integer_start;
            const CharT *integer_end;
            const CharT *fraction_start;
            const CharT *fraction_end;
            const CharT *string_front = str;

            str = details::skip_sign(str);

            // This may yield a string of zero length, which is okay
            integer_start = str;
            str = details::skip_digits(str);
            integer_end = str;

            str = details::skip_decimal(str);

            // This may yield a string of zero length, which is okay
            fraction_start = str;
            str = details::skip_digits(str);
            fraction_end = str;

            // We initially read the exponent as intmax_t since any value whose exponent is greater than
            // std::numeric_limits<intmax_t>::max() is guaranteed to not be representable
            intmax_t exponent = 0;
            if (details::is_exponential(*str))
            {
                ++str;

                bool isExponentNegative = (*str == '-');
                str = details::skip_sign(str);

                str = details::read_integer_value<intmax_t>(str, static_cast<const CharT *>(nullptr), &exponent);
                if (isExponentNegative)
                {
                    exponent = -exponent;
                }
            }

            // We now read an integer part and a fractional part in the resulting part. Since we know the exponent, we can
            // correctly shift the decimal so that if we get integer overflow we know that it is unavoidable and the
            // undefined behavior is okay
            NumericType integer_part = {};
            NumericType fractional_part = {};
            bool fractionNonZero = false;

            if (exponent >= 0)
            {
                // All of the integer remains. Note that we still need to multiply by some power of 10. That will come
                // when we read the fraction
                details::read_integer_value(integer_start, integer_end, &integer_part);

                // Some of fraction goes to integer
                auto fraction_shift = fraction_start + exponent;
                if (fraction_shift > fraction_end)
                {
                    // The whole fraction and then some needs to go to the integer part
                    details::read_integer_value(fraction_start, fraction_end, &integer_part);

                    auto power = fraction_shift - fraction_end;
                    integer_part *= static_cast<NumericType>(std::pow(static_cast<NumericType>(10), power));
                }
                else
                {
                    // Only part of the fraction needs to go to the integer part
                    details::read_integer_value(fraction_start, fraction_shift, &integer_part);
                    details::read_fractional_value(fraction_shift, fraction_end, &fractional_part, &fractionNonZero);
                }
            }
            else
            {
                details::read_fractional_value(fraction_start, fraction_end, &fractional_part, &fractionNonZero);

                // Some of the integer needs to go to the fraction
                auto integer_shift = integer_end + exponent;
                if (integer_start > integer_shift)
                {
                    // The whole integer and then some needs to go to the fractional part. We've already accounted for the
                    // "and then some" above with the fractional_divide
                    details::read_fractional_value(integer_start, integer_end, &fractional_part, &fractionNonZero);

                    // Need to account for the leading zeros
                    auto power = integer_start - integer_shift;
                    fractional_part /= static_cast<NumericType>(std::pow(static_cast<NumericType>(10), power));
                }
                else
                {
                    // Only part of the integer needs to go to the fractional part
                    details::read_integer_value(integer_start, integer_shift, &integer_part);
                    details::read_fractional_value(integer_shift, integer_end, &fractional_part, &fractionNonZero);
                }
            }

            auto result = static_cast<NumericType>(integer_part + fractional_part);

            // Negate if necessary; ignore if zero
            if ((*string_front == '-') && ((integer_part != 0) || fractionNonZero))
            {
                result = details::negate(result);
            }

            return result;
        }

#pragma endregion



#pragma region std::basic_string Conversion

        template <typename NumericType, typename CharT, typename Traits, typename Alloc>
        NumericType numeric_cast(const std::basic_string<CharT, Traits, Alloc> &str)
        {
            return numeric_cast<NumericType>(str.c_str());
        }

#pragma endregion



#pragma region dhorn::utf_string Conversion

        template <typename NumericType, typename CharT>
        NumericType numeric_cast(const utf_string<CharT> &str)
        {
            return numeric_cast<NumericType>(str.c_str());
        }

#pragma endregion
    }
}

#pragma warning(pop)
