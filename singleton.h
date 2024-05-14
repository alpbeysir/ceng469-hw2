#pragma once
template <class X>
X& singleton()
{
    static X x;
    return x;
}