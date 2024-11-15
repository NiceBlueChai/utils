#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <type_traits>
#include <functional>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include "finally.hpp"

struct dll_caller
{
	template<typename Ftype, typename ...Args, typename RetType = std::invoke_result_t<Ftype, Args...>,
		typename = std::enable_if_t<std::is_function<Ftype>::value>>
		static std::optional<RetType> call(std::string_view dll_name, std::string_view function_name, Args&& ...args)
	{
#ifdef _WIN32
		auto dll_handle = ::LoadLibraryA(dll_name.data());
		auto end_clear = finally([&dll_handle] {
			if (dll_handle)
				::FreeLibrary(dll_handle);
			});
		if (!dll_handle)
			return {};

		auto function_handle = ::GetProcAddress(dll_handle, function_name.data());
		if (!function_handle)
			return {};
#else
		auto dll_handle = ::dlopen(dll_name.data(), RTLD_LAZY);
		auto end_clear = finally([&dll_handle] {
			if (dll_handle)
				::dlclose(dll_handle);
			});
		if (!dll_handle)
			return {};

		auto function_handle = ::dlsym(dll_handle, function_name.data());
		if (!function_handle)
			return {};
#endif
		return std::optional<RetType>{((Ftype*)function_handle)(std::forward<Args>(args)...)};
	}
};
