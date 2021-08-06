#pragma once

/*
 * TRemoveReference<type> will remove any reference from a type.
 */
 template <typename T>struct TRemoveReference      { typedef T Type };
 template <typename T>struct TRemoveReference<T&>  { typedef T Type };
 template <typename T>struct TRemoveReference<T&&> { typedef T Type };