#pragma once

// On MSVC 2008 in 64 Bit <cmath> generates a lot of warnings (actually it is math.h, which is included by cmath)
PL_WARNING_PUSH()
PL_WARNING_DISABLE_MSVC(4985)

// include std header
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <new>

PL_WARNING_POP()

// redefine NULL to nullptr
#undef NULL
#define NULL nullptr

// include c++11 specific header
#include <type_traits>
#include <utility>

/// \brief Disallow the copy constructor and the assignment operator for this type.
#define PL_DISALLOW_COPY_AND_ASSIGN(type) \
  type(const type&) = delete;             \
  void operator=(const type&) = delete

#if PL_ENABLED(PL_COMPILE_FOR_DEVELOPMENT)
/// \brief Macro helper to check alignment
#  define PL_CHECK_ALIGNMENT(ptr, alignment) PL_ASSERT_DEV(((size_t)ptr & ((alignment) - 1)) == 0, "Wrong alignment.")
#else
/// \brief Macro helper to check alignment
#  define PL_CHECK_ALIGNMENT(ptr, alignment)
#endif

#define PL_WINCHECK_1 1          // PL_INCLUDED_WINDOWS_H defined to 1, _WINDOWS_ defined (stringyfied to nothing)
#define PL_WINCHECK_1_WINDOWS_ 1 // PL_INCLUDED_WINDOWS_H defined to 1, _WINDOWS_ undefined (stringyfied to "_WINDOWS_")
#define PL_WINCHECK_PL_INCLUDED_WINDOWS_H \
  0                              // PL_INCLUDED_WINDOWS_H undefined (stringyfied to "PL_INCLUDED_WINDOWS_H", _WINDOWS_ defined (stringyfied to nothing)
#define PL_WINCHECK_PL_INCLUDED_WINDOWS_H_WINDOWS_ \
  1                              // PL_INCLUDED_WINDOWS_H undefined (stringyfied to "PL_INCLUDED_WINDOWS_H", _WINDOWS_ undefined (stringyfied to "_WINDOWS_")

/// \brief Checks whether Windows.h has been included directly instead of through 'IncludeWindows.h'
///
/// Does this by stringifying the available defines, concatenating them into one long word, which is a known #define that evaluates to 0 or 1
#define PL_CHECK_WINDOWS_INCLUDE(PL_WINH_INCLUDED, WINH_INCLUDED)                               \
  static_assert(PL_PP_CONCAT(PL_WINCHECK_, PL_PP_CONCAT(PL_WINH_INCLUDED, WINH_INCLUDED)) == 1, \
    "Windows.h has been included but not through pl. #include <Foundation/Basics/Platform/Win/IncludeWindows.h> instead of Windows.h");

#if PL_ENABLED(PL_COMPILE_ENGINE_AS_DLL)

/// \brief The tool 'StaticLinkUtil' inserts this macro into each file in a library.
/// Each library also needs to contain exactly one instance of PL_STATICLINK_LIBRARY.
/// The macros create functions that reference each other, which means the linker is forced to look at all files in the library.
/// This in turn will drag all global variables into the visibility of the linker, and since it mustn't optimize them away,
/// they then end up in the final application, where they will do what they are meant for.
#  define PL_STATICLINK_FILE(LibraryName, UniqueName) PL_CHECK_WINDOWS_INCLUDE(PL_INCLUDED_WINDOWS_H, _WINDOWS_)

/// \brief Used by the tool 'StaticLinkUtil' to generate the block after PL_STATICLINK_LIBRARY, to create references to all
/// files inside a library. \see PL_STATICLINK_FILE
#  define PL_STATICLINK_REFERENCE(UniqueName)

/// \brief This must occur exactly once in each static library, such that all PL_STATICLINK_FILE macros can reference it.
#  define PL_STATICLINK_LIBRARY(LibraryName) void plReferenceFunction_##LibraryName(bool bReturn = true)

#else

struct plStaticLinkHelper
{
  using Func = void (*)(bool);
  plStaticLinkHelper(Func f) { f(true); }
};

/// \brief Helper struct to register the existence of statically linked plugins.
/// The macro PL_STATICLINK_LIBRARY will register a the given library name prepended with `pl` to the plPlugin system.
/// Implemented in Plugin.cpp.
struct PL_FOUNDATION_DLL plPluginRegister
{
  plPluginRegister(const char* szName);
};

/// \brief The tool 'StaticLinkUtil' inserts this macro into each file in a library.
/// Each library also needs to contain exactly one instance of PL_STATICLINK_LIBRARY.
/// The macros create functions that reference each other, which means the linker is forced to look at all files in the library.
/// This in turn will drag all global variables into the visibility of the linker, and since it mustn't optimize them away,
/// they then end up in the final application, where they will do what they are meant for.
#  define PL_STATICLINK_FILE(LibraryName, UniqueName)       \
    extern "C"                                              \
    {                                                       \
      void plReferenceFunction_##UniqueName(bool bReturn)   \
      {                                                     \
        (void)bReturn;                                      \
      }                                                     \
      void plReferenceFunction_##LibraryName(bool bReturn); \
    }                                                       \
    static plStaticLinkHelper StaticLinkHelper_##UniqueName(plReferenceFunction_##LibraryName);

/// \brief Used by the tool 'StaticLinkUtil' to generate the block after PL_STATICLINK_LIBRARY, to create references to all
/// files inside a library. \see PL_STATICLINK_FILE
#  define PL_STATICLINK_REFERENCE(UniqueName)                   \
    void plReferenceFunction_##UniqueName(bool bReturn = true); \
    plReferenceFunction_##UniqueName()

/// \brief This must occur exactly once in each static library, such that all PL_STATICLINK_FILE macros can reference it.
#  define PL_STATICLINK_LIBRARY(LibraryName)                                                         \
    plPluginRegister plPluginRegister_##LibraryName(PL_PP_STRINGIFY(PL_PP_CONCAT(pl, LibraryName))); \
    extern "C" void plReferenceFunction_##LibraryName(bool bReturn = true)

#endif

namespace plInternal
{
  template <typename T>
  constexpr bool AlwaysFalse = false;

  template <typename T>
  struct ArraySizeHelper
  {
    static_assert(AlwaysFalse<T>, "Cannot take compile time array size of given type");
  };

  template <typename T, size_t N>
  struct ArraySizeHelper<T[N]>
  {
    static constexpr size_t value = N;
  };

} // namespace plInternal

/// \brief Macro to determine the size of a static array
#define PL_ARRAY_SIZE(a) (plInternal::ArraySizeHelper<decltype(a)>::value)

/// \brief Template helper which allows to suppress "Unused variable" warnings (e.g. result used in platform specific block, ..)
template <class T>
void PL_IGNORE_UNUSED(const T&)
{
}

#if (__cplusplus >= 202002L || _MSVC_LANG >= 202002L)
#  undef PL_USE_CPP20_OPERATORS
#  define PL_USE_CPP20_OPERATORS PL_ON
#endif

#if PL_ENABLED(PL_USE_CPP20_OPERATORS)
// in C++ 20 we don't need to declare an operator!=, it is automatically generated from operator==
#  define PL_ADD_DEFAULT_OPERATOR_NOTEQUAL(...) /*empty*/
#else
#  define PL_ADD_DEFAULT_OPERATOR_NOTEQUAL(...)                                   \
    PL_ALWAYS_INLINE bool operator!=(PL_EXPAND_ARGS_COMMA(__VA_ARGS__) rhs) const \
    {                                                                             \
      return !(*this == rhs);                                                     \
    }
#endif
