#pragma once
#include <cstdint>
#include <Windows.h>
#include <assert.h>
#include <algorithm>
#include <array>
#include <Psapi.h>
#pragma comment(lib,"psapi")
#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#ifdef __clang__
#define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE __forceinline
#endif

namespace signature
{
	namespace detail
	{
		template<typename chr = char>
		constexpr uint8_t hex(const chr n)
		{
			return static_cast<uint8_t>(9 * (n >> 6) + (n & 15));
		}

		template<typename chr = char>
		constexpr uint8_t eval(const chr* str, const size_t i)
		{
			return str[i * 3] == 63 ? 63u : hex(str[i * 3]) * 16 + hex(str[i * 3 + 1]);
		}

		template<typename chr = char, size_t... n>
		constexpr std::array<uint8_t, sizeof...(n)> stream(const chr* str, std::index_sequence<n...>)
		{
			return{ eval(str, n)... };
		}

		template<typename chr = char, size_t len>
		constexpr auto convert(const chr(&str)[len])
		{
			static_assert(!(len % 3), "Pattern length fail");
			return stream(str, std::make_index_sequence<len / 3>());
		}
	}

	template<typename chr = char, size_t len>
	uintptr_t search(const uint8_t* start, const size_t size, std::array<chr, len>& pattern)
	{
		// auto pattern = detail::convert(signature);
		auto result = std::search(start, start + size, pattern.begin(), pattern.end(), [](uint8_t lhs, uint8_t rhs)
		{
			return lhs == rhs || rhs == '?';
		});

		return result == start ? 0 : reinterpret_cast<uintptr_t>(result);
	}

	template<typename chr = char, size_t len>
	uintptr_t search(HMODULE module, std::array<chr, len> pattern)
	{
		assert(module);

		uint8_t* start = reinterpret_cast<uint8_t*>(module);
		auto	 hdr   = reinterpret_cast<PIMAGE_NT_HEADERS>(start + reinterpret_cast<PIMAGE_DOS_HEADER>(start)->e_lfanew);
		size_t   size  = hdr->OptionalHeader.SizeOfImage;

		// auto pattern = detail::convert(signature);
		auto result = std::search(start, start + size, pattern.begin(), pattern.end(), [](uint8_t _, uint8_t __)
		{
			return _ == __ || __ == 63;
		});

		return result == start ? 0 : reinterpret_cast<uintptr_t>(result);
	}
}