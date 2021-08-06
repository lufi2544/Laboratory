#pragma once

#include "Platform.h" 
#include "RemoveReference.h"
#include "HappyGardenTemplate.h"

#define FORCE_THREADSAFE_SHAREDPTRS PLATFORM_WEAKLY_CONSISTENT_MEMORY


/**
 * ESPMode is used select between either 'fast' or 'thread safe' shared pointer types.
 * This is only used by templates at compile time to generate one code path or another.
 */
enum class ESPMode
{
	// This is not thread safe
	NotThreadSakfe = 0 , 
	/**
		*	Fast, doesn't ever use atomic interlocks.
		*	Some code requires that all shared pointers are thread-safe.
		*	It's better to change it here, instead of replacing ESPMode::Fast to ESPMode::ThreadSafe throughout the code.
		*/
	Fast = FORCE_THREADSAFE_SHAREDPTRS ? 1 : 0,

	ThreadSafe = 1
	 
};

// Forward declarations. Nothe that accessing to the Object quickly has priprity With ESPMode::Fast, but if you want to 
// acces your object in different threads, you have to use ESPMode::ThreadSafe.

template< typename ObjectType, ESPMode Mode = ESPMode::Fast > class TSharedRef;
template< typename ObjectType, ESPMode Mode = ESPMode::Fast > class TSharedPtr;
template< typename ObjectType, ESPMode Mode = ESPMode::Fast > class TWeakPtr;
template< typename ObjectType, ESPMode Mode = ESPMode::Fast > class TSharedFromThis;

/**
*SharedPointerInternals contains internal workings of sharedand weak pointers.You should
* hopefully never have to use anything inside this namespace directly.
*/
namespace SharedPointerInternals
{
	//Forward declarations
	template< ESPMode Mode > class FWeakReferencer;

	/** Dummy structures used internally as template arguments for typecasts */
	struct FStaticCastTag {};
	struct FConstCastTag  {};

	struct FNullTag{};


	class FReferenceControllerBase
	{
		public:

			FORCEINLINE explicit FReferenceControllerBase():
				SharedReferenceCount(1),
				WeakRefereceCount(1)
				{}

			/** Number of shared references to this object.  When this count reaches zero, the associated object
				will be destroyed (even if there are still weak references!) */
			int32 SharedReferenceCount; 


			/** Number of weak references to this object.  If there are any shared references, that counts as one
			   weak reference too. */
			int32 WeakRefereceCount;

			/** Destroy the object associated with this object. */
			virtual void DestroyObject() = 0;

			virtual ~FReferenceControllerBase()
			{}
		 
		private:

			FReferenceControllerBase( FReferenceControllerBase const& );
			FReferenceControllerBase& operator=(FReferenceControllerBase const&){}
	};

	template<typename ObjectType, typename DeleterType>
	class TReferenceControllerWithDeleter : private DeleterType, public FReferenceControllerBase
	{

		private:
		ObjectType* Object;

		public:
			explicit TReferenceControllerWithDeleter(ObjectType* InObject, DeleterType&& Deleter):
					 DeleterType(MoveTemp(Deleter)),
				     Object(InObject)
					 {
					 }
			
			virtual void DestroyObject()override
			{
				(*static_cast<DeleterType*>(this))(Object);
			}


		// Non-copyable
		TReferenceControllerWithDeleter(const TReferenceControllerWithDeleter&) = delete;
		TReferenceControllerWithDeleter& operator =  (const TReferenceControllerWithDeleter&) = delete;

	};
	template<typename ObjectType>
	class TIntrusiveReferenceController : public FReferenceControllerBase
	{

	private:

		/**  The Object associated with this reference counter. */
		mutable TTypeCompatibleBytes<ObjectType> ObjectStorage;

	public:

		// Non- copyable
		TIntrusiveReferenceController(const TIntrusiveReferenceController&) = delete;
		TIntrusiveReferenceController& operator = (const TIntrusiveReferenceController&) = delete;


		template<typename... ArgTypes>
		explicit TIntrusiveReferenceController(ArgTypes&&... Args)
		{
			new ((void*)&ObjectStorage) ObjectType(Forward<ArgTypes>(Args)...);
		}
		 
		ObjectType* GetObjectPtr() const
		{
			return ( ObjectType* )&ObjectStorage;
		}

	};


	/** Deletes an object via the standard delete operator. */
	template <typename Type>
	struct DefaultDeleter
	{
		FORCEINLINE void operator () ( Type* Object ) const
		{
			delete Object;
		}
	};

	/** Creates a reference controller which just calls delete(take a look at the DefaultDeleter class () operator override funciton) */
	template <typename ObjectType>
	inline FReferenceControllerBase* NewDefaultReferenceController(ObjectType* Object)
	{
		return new TReferenceControllerWithDeleter<ObjectType, DefaultDeleter<ObjectType>>( Object, DefaultDeleter<ObjectType>() ); 
	}
		
	/** Creates a custom reference controller with a specified deleter. */
	template <typename ObjectType, typename DeleterType>
	inline FReferenceControllerBase* NewCustomReferenceController(ObjectType* Object, DeleterType&& Deleter)
	{
		//TODO add remove reference.h
		new TReferenceControllerWithDeleter<ObjectType, typename TRemoveReference<DeleterType>::Type>(Object, Forward<DeleterType>(Deleter))
	}

	/** Creates an intrusive reference controller */
	template <typename ObjectType, typename... ArgTypes>
	inline TIntrusiveReferenceController<ObjectType>* NewIntrusiveReferenceController(ArgTypes&&... Args)
	{
		return new TIntrusiveReferenceController<ObjectType>(Forward<ArgTypes>(Args)...);
	}
	
}