// fms_iterable.h - iterators with operator bool() sentinel
#pragma once
#include <compare>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace fms::iterable {

	template<class I>
	concept input_iterable = std::input_iterator<I> &&
		requires(I i) {
		{ i.operator bool() } -> std::same_as<bool>;
	};
	template<class I, class T>
	concept output_iterable = std::output_iterator<I, T> &&
		requires(I i) {
			{ i.operator bool() } -> std::same_as<bool>;
	};
	template<class I>
	concept forward_iterable = std::forward_iterator<I> &&
		requires(I i) {
			{ i.operator bool() } -> std::same_as<bool>;
	};
	template<class I>
	concept bidirectional_iterable = std::bidirectional_iterator<I> &&
		requires(I i) {
			{ i.operator bool() } -> std::same_as<bool>;
	};
	template<class I>
	concept random_access_iterable = std::random_access_iterator<I> &&
		requires(I i) {
			{ i.operator bool() } -> std::same_as<bool>;
	};
	template<class I>
	concept contiguous_iterable = std::contiguous_iterator<I> &&
		requires(I i) {
			{ i.operator bool() } -> std::same_as<bool>;
	};

	// Compare elements of two iterables
	template<input_iterable I, input_iterable J>
	constexpr auto compare(I i, J j)
	{
		while (i && j) {
			const auto cmp = *i++ <=> *j++;
			if (cmp != 0) {
				return cmp;
			}
		}

		return !!i <=> !!j;
	}


	// Possibly unsafe pointer iterable with std::span/view semantics.
	template<class T>
	class ptr {
	protected:
		T* p;
	public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using difference_type = std::ptrdiff_t;
		
		constexpr ptr(T* p = nullptr)
			: p(p)
		{ }
		constexpr ptr(const ptr&) = default;
		constexpr ptr& operator=(const ptr&) = default;
		constexpr ptr(ptr&&) = default;
		constexpr ptr& operator=(ptr&&) = default;
		constexpr ~ptr() = default;

		constexpr auto operator<=>(const ptr& _p) const = default;

		constexpr ptr begin() const
		{
			return *this;
		}
		constexpr ptr end() const
		{
			return ptr(nullptr);
		}

		constexpr explicit operator bool() const noexcept
		{
			return p != nullptr;
		}
		//??? why not value_type
		// indirectly readable
		constexpr reference operator*() const noexcept
		{
			return *p;
		}
		// indirectly writable
		constexpr reference operator*() noexcept
		{
			return *p;
		}
		// weakly incrementable
		constexpr ptr& operator++() noexcept
		{
			++p;

			return *this;
		}
		constexpr ptr operator++(int) noexcept
		{
			auto tmp{ *this };
			
			operator++();

			return tmp;
		}
		// bidirectional
		constexpr ptr& operator--() noexcept
		{
			--p;

			return *this;
		}
		constexpr ptr operator--(int) noexcept
		{
			auto tmp{ *this };
			
			operator--();

			return tmp;
		}
		// random access
		constexpr ptr& operator+=(difference_type d) noexcept
		{
			p += d;

			return *this;
		}
		constexpr ptr operator+(difference_type d) const noexcept
		{
			return ptr(p + d);
		}
		constexpr ptr& operator-=(difference_type d) noexcept
		{
			p -= d;

			return *this;
		}
		constexpr ptr operator-(difference_type d) const noexcept
		{
			return ptr(p - d);
		}
		constexpr difference_type operator-(const ptr& i) const noexcept
		{
			return p - i.p;
		}
		reference operator[](difference_type i) const noexcept
		{
			return p[i];
		}
		reference operator[](difference_type i) noexcept
		{
			return p[i];
		}
		// contiguous
		constexpr auto operator->() const noexcept
		{
			return p;
		}
	};
	template<class T>
	constexpr ptr<T> operator+(std::ptrdiff_t d, ptr<T> p) noexcept
	{
		return p + d;
	}
	template<class T>
	constexpr ptr<T> operator-(std::ptrdiff_t d, ptr<T> p) noexcept
	{
		return p - d;
	}

	template<std::input_or_output_iterator I>
	class interval {
		I b, e;
	public:
		using iterator_category = typename I::iterator_category;
		using value_type = typename I::value_type;
		using reference = typename I::reference;
		using pointer = typename I::pointer;
		using difference_type = typename I::difference_type;

		constexpr interval(I b, I e)
			: b(b), e(e)
		{ }
		constexpr interval(const interval&) = default;
		constexpr interval& operator=(const interval&) = default;
		constexpr interval(interval&&) = default;
		constexpr interval& operator=(interval&&) = default;
		constexpr ~interval() = default;

		constexpr auto operator<=>(const interval& i) const = default;

		constexpr interval begin() const
		{
			return *this;
		}
		constexpr interval end() const
		{
			return interval(e, e);
		}

		constexpr explicit operator bool() const noexcept
		{
			return b != e;
		}
		// indirectly readable
		constexpr reference operator*() const noexcept
		{
			return *b;
		}
		// indirectly writable
		constexpr reference operator*() noexcept
			requires std::indirectly_writable<I,value_type>
		{
			return *b;
		}
		// weakly incrementable
		constexpr interval& operator++() noexcept
		{
			if (b != e) {
				++b;
			}

			return *this;
		}
		constexpr interval operator++(int) noexcept
		{
			auto tmp{ *this };

			operator++();

			return tmp;
		}
		// bidirectional
		constexpr interval& operator--() noexcept
			requires std::bidirectional_iterator<I>
		{
			--b;

			return *this;
		}
		constexpr interval operator--(int) noexcept
			requires std::bidirectional_iterator<I>
		{
			auto tmp{ *this };

			operator--();

			return tmp;
		}
		// random access
		constexpr interval& operator+=(difference_type i) noexcept
			requires std::random_access_iterator<I>
		{
			b += i;

			return *this;
		}
		constexpr interval operator+(difference_type i) const noexcept
			requires std::random_access_iterator<I>
		{
			return interval(b + i, e);
		}
		constexpr interval& operator-=(difference_type i) noexcept
			requires std::random_access_iterator<I>
		{
			b -= i;

			return *this;
		}
		constexpr interval operator-(difference_type i) const noexcept
			requires std::random_access_iterator<I>
		{
			return interval(b - i, e);
		}
		reference operator[](difference_type i) const noexcept
			requires std::random_access_iterator<I>
		{
			return b[i];
		}
		reference operator[](difference_type i) noexcept
			requires std::random_access_iterator<I>
		{
			return b[i];
		}
	};
	template<class I>
		requires std::random_access_iterator<I>
	constexpr interval<I> operator+(std::ptrdiff_t d, interval<I> p) noexcept
	{
		return p + d;
	}
	template<class I>
		requires std::random_access_iterator<I>
	constexpr interval<I> operator-(std::ptrdiff_t d, interval<I> p) noexcept
	{
		return p - d;
	}

	// Assumes lifetime of container.
	template<class C>
	constexpr auto make_interval(C& c)
	{
		return interval(std::begin(c), std::end(c));
	}

	// Assumes lifetime of a.
	template<class T, std::size_t N>
	constexpr auto array(T(&a)[N]) noexcept
	{
		return interval(ptr(a), ptr(a + N));
	}

	template<std::input_or_output_iterator I>
	constexpr auto take(I i, std::size_t n)
	{
		return interval(i, std::next(i, n));
	}

	// i0, ..., in, i0, ...
	template<std::input_or_output_iterator I>
	class cyclic {
		I i, i0;
	public:
		using iterator_category = std::common_type<std::forward_iterator_tag, typename I::iterator_category>;
		using value_type = typename I::value_type;
		using reference = typename I::reference;
		using pointer = typename I::pointer;
		using difference_type = typename I::difference_type;

		constexpr cyclic(I i)
			: i(i), i0(i)
		{ }
		constexpr cyclic(const cyclic&) = default;
		constexpr cyclic& operator=(const cyclic&) = default;
		constexpr cyclic(cyclic&&) = default;
		constexpr cyclic& operator=(cyclic&&) = default;
		constexpr ~cyclic() = default;

		constexpr auto operator<=>(const cyclic& c) const = default;

		constexpr cyclic begin() const
		{
			return *this;
		}
		// no end

		constexpr explicit operator bool() const noexcept
		{
			return true;
		}
		constexpr reference operator*() const noexcept
		{
			return *i;
		}
		constexpr reference operator*() noexcept
			requires std::indirectly_writable<I,value_type>
		{
			return *i;
		}
		constexpr cyclic& operator++() noexcept
		{
			if (!++i) {
				i = i0;
			}

			return *this;
		}
		constexpr cyclic operator++(int) noexcept
		{
			auto tmp{ *this };

			operator++();

			return tmp;
		}
	};

} // namespace fms
