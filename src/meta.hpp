#pragma once

namespace cppio::meta
{
    template <class T, class U>
    concept derived = std::is_base_of<U, T>::value;
} // namespace cppio
