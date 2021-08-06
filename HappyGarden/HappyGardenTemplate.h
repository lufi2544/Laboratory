#pragma once
#include "Platform.h"
#include "RemoveReference.h"

/**
 * Forward will cast a reference to an rvalue reference.
 */
template< typename T >
FORCEINLINE T&& Forward( (typename TRemoveReference<T>::Type)& Object)
{
	return (T&&)Object;
};

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move except that it will not compile when passed an rvalue or
 * const object, because we would prefer to be informed when MoveTemp will have no effect.
 */
 template<typename T>
 FORCEINLINE typename TRemoveReference<T>::Type&& MoveTemp(T&& Obj)
 {
	typedef typename TRemoveReference<T>::Type CastType;

	static_assert(TIsLValueReferenceType<T>::Value ,"MoveTemp called on an rvalue." );
	static_assert(!TAreTypesEqual<CastType&, const CastType&>::Value, "MoveTemp called on a const object.");

 }