/*
 * Duncan Horn
 *
 * com_utility.h
 *
 * Helpers and type traits for dealing with COM types and objects.
 */
#pragma once

#include <Unknwn.h>

#include "../type_traits.h"

namespace dhorn::com
{
    /*
     * is_unknown
     */
#pragma region is_unknown

    template <typename... IFaces>
    using is_unknown = any_base_of<IUnknown, IFaces...>;

    template <typename... IFaces>
    constexpr bool is_unknown_v = is_unknown<IFaces...>::value;

#pragma endregion



    /*
     * all_unknown
     */
#pragma region all_unknown

    template <typename... IFaces>
    using all_unknown = all_base_of<IUnknown, IFaces...>;

    template <typename... IFaces>
    constexpr bool all_unknown_v = all_unknown<IFaces...>::value;

#pragma endregion
}