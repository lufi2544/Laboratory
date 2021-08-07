

template<class T, T val>
struct identity_value
{
	static constexpr auto value = val;
};

template<class T, T val>
static constexpr auto identity_value_v = identity_value<T, val>::value;

template<class T>
struct identity_type
{
	using type = T;
};

template<class T>
using identity_type_t = typename identity_type<T>::type;

template<class T, T val>
struct integral_constant
{

	using value_type = T;
	using type = integral_constant<T, val>;
	static constexpr T value = val;


	operator value_type () noexcept
	{
		return value;
	}

	value_type operator ()(void) noexcept
	{
		return value;
	}

};

template <bool b>
using bool_integral = integral_constant<bool, b>;

using true_type = bool_integral<true>;
using false_type = bool_integral<false>;


template<class T>
struct remove_const : identity_type<T>
{};

template<class T>
struct remove_const<T const> :identity_type<T>
{};


template <class T>
using remove_const_t = typename remove_const<T>::type;


template<class T>
struct remove_volatile :identity_type<T>
{};


template<class T>
struct remove_volatile<T volatile> :identity_type<T>
{};

template<class T>
using remove_volatile_t = typename remove_volatile<T>::type;

template<class T>
struct remove_cv :remove_const<typename remove_volatile<T>::type>
{};

template <class T>
using remove_cv_t = remove_const_t<remove_volatile_t<T>>;

template<class A, class B >
struct is_same :false_type
{};

template<class A>
struct is_same<A, A> :true_type
{};

template<class A, class B>
static const auto is_same_v = is_same<A, B>::value;

template<class A, class B>
using is_same_raw = is_same<remove_cv_t<A>, remove_cv_t<B>>;

template<class A, class B>
static constexpr bool is_same_raw_v = is_same<remove_cv_t<A>, remove_cv_t<B>>::value;

template<class T>
using is_floting_point = bool_integral<

	is_same_raw_v<float, T>
	|| is_same_raw_v<double, T>
	|| is_same_raw_v<long double, T>>;

template<class TargetT, class ... Ts>
using is_type_in_pack = ...;

template <class T>
using is_integral = is_type_in_pack<remove_cv_t<T>, bool, char, int, long long>;