#pragma once


/**
 * TIsLValueReferenceType
 */
template<typename T> struct TIsLValueReferenceType	   { enum { Value = false }; };
template<typename T> struct TIsLValueReferenceType<T&> { enum { Value = true  }; };

template<typename T, typename K> struct TAreTypesEqual { enum { Value = false }; };
template<typename T> struct TAreTypesEqual<T, T>	   { enum { Value = true; }; };

