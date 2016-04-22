// The MIT License (MIT) 

// Copyright (c) 2013-2016 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_TYPES_HPP
#define SOL_TYPES_HPP

#include "compatibility.hpp"
#include "traits.hpp"
#include "optional.hpp"
#include <string>

namespace sol {
namespace detail {
#ifdef SOL_NO_EXCEPTIONS
template <lua_CFunction f>
inline int static_trampoline (lua_State* L) {
    return f(L);
}

template <typename Fx, typename... Args>
inline int trampoline(lua_State* L, Fx&& f, Args&&... args) {
    return f(L, std::forward<Args>(args)...);
}

inline int c_trampoline(lua_State* L, lua_CFunction f) {
    return trampoline(L, f);
}
#else
template <lua_CFunction f>
inline int static_trampoline (lua_State* L) {
    try {
        return f(L);
    }
    catch (const char *s) {
        lua_pushstring(L, s);
    }
    catch (const std::exception& e) {
        lua_pushstring(L, e.what());
    }
    catch (...) {
        lua_pushstring(L, "caught (...) exception");
    }
    return lua_error(L);
}

template <typename Fx, typename... Args>
inline int trampoline(lua_State* L, Fx&& f, Args&&... args) {
    try {
        return f(L, std::forward<Args>(args)...);
    }
    catch (const char *s) {
        lua_pushstring(L, s);
    }
    catch (const std::exception& e) {
        lua_pushstring(L, e.what());
    }
    catch (...) {
        lua_pushstring(L, "caught (...) exception");
    }
    return lua_error(L);
}

inline int c_trampoline(lua_State* L, lua_CFunction f) {
    return trampoline(L, f);
}
#endif // Exceptions vs. No Exceptions
}
struct nil_t {};
const nil_t nil {};
inline bool operator==(nil_t, nil_t) { return true; }
inline bool operator!=(nil_t, nil_t) { return false; }

typedef std::add_lvalue_reference_t<std::remove_pointer_t<lua_CFunction>> lua_r_CFunction;

template <typename T, typename = void>
struct unique_usertype {};

template <typename T>
struct non_null {};

template<typename... Args>
struct function_sig {};

struct up_value_index {
    int index;
    up_value_index(int idx) : index(lua_upvalueindex(idx)) {}
    operator int() const { return index; }
};

struct absolute_index {
    int index;
    absolute_index(lua_State* L, int idx) : index(lua_absindex(L, idx)) {}
    operator int() const { return index; }
};

struct light_userdata_value {
    void* value;
    light_userdata_value(void* data) : value(data) {}
    operator void*() const { return value; }
};

struct userdata_value {
    void* value;
    userdata_value(void* data) : value(data) {}
    operator void*() const { return value; }
};

struct c_closure {
    lua_CFunction c_function;
    int upvalues;
    c_closure(lua_CFunction f, int upvalues = 0) : c_function(f), upvalues(upvalues) {}
};

enum class call_syntax {
    dot = 0,
    colon = 1
};

enum class call_status : int {
    ok      = LUA_OK,
    yielded = LUA_YIELD,
    runtime = LUA_ERRRUN,
    memory  = LUA_ERRMEM,
    handler = LUA_ERRERR,
    gc      = LUA_ERRGCMM
};

enum class thread_status : int {
    normal        = LUA_OK,
    yielded       = LUA_YIELD,
    error_runtime = LUA_ERRRUN,
    error_memory  = LUA_ERRMEM,
    error_gc      = LUA_ERRGCMM,
    error_handler = LUA_ERRERR,
    dead,
};

enum class type : int {
    none          = LUA_TNONE,
    nil           = LUA_TNIL,
    string        = LUA_TSTRING,
    number        = LUA_TNUMBER,
    thread        = LUA_TTHREAD,
    boolean       = LUA_TBOOLEAN,
    function      = LUA_TFUNCTION,
    userdata      = LUA_TUSERDATA,
    lightuserdata = LUA_TLIGHTUSERDATA,
    table         = LUA_TTABLE,
    poly          = none   | nil     | string   | number   | thread          |
                    table  | boolean | function | userdata | lightuserdata
};

inline int type_panic(lua_State* L, int index, type expected, type actual) {
    return luaL_error(L, "stack index %d, expected %s, received %s", index, 
        expected == type::poly ? "anything" : lua_typename(L, static_cast<int>(expected)),
        expected == type::poly ? "anything" : lua_typename(L, static_cast<int>(actual))
    );
}

// Specify this function as the handler for lua::check if you know there's nothing wrong
inline int no_panic(lua_State*, int, type, type) noexcept {
    return 0;
}

inline void type_error(lua_State* L, int expected, int actual) {
    luaL_error(L, "expected %s, received %s", lua_typename(L, expected), lua_typename(L, actual));
}

inline void type_error(lua_State* L, type expected, type actual) {
    type_error(L, static_cast<int>(expected), static_cast<int>(actual));
}

inline void type_assert(lua_State* L, int index, type expected, type actual) {
    if (expected != type::poly && expected != actual) {
           type_panic(L, index, expected, actual);
    }
}

inline void type_assert(lua_State* L, int index, type expected) {
    int actual = lua_type(L, index);
    if(expected != type::poly && static_cast<int>(expected) != actual) {
           type_error(L, static_cast<int>(expected), actual);
    }
}

inline std::string type_name(lua_State*L, type t) {
    return lua_typename(L, static_cast<int>(t));
}

class reference;
class stack_reference;
template<typename T>
class usertype;
template <bool, typename T>
class basic_table_core;
template <bool b>
using table_core = basic_table_core<b, reference>;
template <bool b>
using stack_table_core = basic_table_core<b, stack_reference>;
typedef table_core<false> table;
typedef table_core<true> global_table;
typedef stack_table_core<false> stack_table;
typedef stack_table_core<true> stack_global_table;
template <typename T>
class basic_function;
template <typename T>
class basic_protected_function;
using function = basic_function<reference>;
using protected_function = basic_protected_function<reference>;
using stack_function = basic_function<stack_reference>;
using stack_protected_function = basic_protected_function<stack_reference>;
template <typename base_t>
class basic_object;
template <typename base_t>
class basic_userdata;
template <typename base_t>
class basic_lightuserdata;
using object = basic_object<reference>;
using stack_object = basic_object<stack_reference>;
using userdata = basic_userdata<reference>;
using stack_userdata = basic_userdata<stack_reference>;
using lightuserdata = basic_lightuserdata<reference>;
using stack_lightuserdata = basic_lightuserdata<stack_reference>;
class coroutine;
class thread;

template <typename T, typename = void>
struct lua_type_of : std::integral_constant<type, type::userdata> {};

template <>
struct lua_type_of<std::string> : std::integral_constant<type, type::string> {};

template <std::size_t N>
struct lua_type_of<char[N]> : std::integral_constant<type, type::string> {};

template <>
struct lua_type_of<const char*> : std::integral_constant<type, type::string> {};

template <>
struct lua_type_of<bool> : std::integral_constant<type, type::boolean> {};

template <>
struct lua_type_of<nil_t> : std::integral_constant<type, type::nil> { };

template <>
struct lua_type_of<sol::error> : std::integral_constant<type, type::string> { };

template <>
struct lua_type_of<table> : std::integral_constant<type, type::table> { };

template <>
struct lua_type_of<global_table> : std::integral_constant<type, type::table> { };

template <>
struct lua_type_of<stack_table> : std::integral_constant<type, type::table> { };

template <>
struct lua_type_of<stack_global_table> : std::integral_constant<type, type::table> { };

template <>
struct lua_type_of<reference> : std::integral_constant<type, type::poly> {};

template <>
struct lua_type_of<stack_reference> : std::integral_constant<type, type::poly> {};

template <typename Base>
struct lua_type_of<basic_object<Base>> : std::integral_constant<type, type::poly> {};

template <typename... Args>
struct lua_type_of<std::tuple<Args...>> : std::integral_constant<type, type::poly> {};

template <typename A, typename B>
struct lua_type_of<std::pair<A, B>> : std::integral_constant<type, type::poly> {};

template <>
struct lua_type_of<light_userdata_value> : std::integral_constant<type, type::lightuserdata> {};

template <>
struct lua_type_of<userdata_value> : std::integral_constant<type, type::userdata> {};

template <typename Base>
struct lua_type_of<basic_lightuserdata<Base>> : std::integral_constant<type, type::lightuserdata> {};

template <typename Base>
struct lua_type_of<basic_userdata<Base>> : std::integral_constant<type, type::userdata> {};

template <>
struct lua_type_of<lua_CFunction> : std::integral_constant<type, type::function> {};

template <typename Base>
struct lua_type_of<basic_function<Base>> : std::integral_constant<type, type::function> {};

template <typename Base>
struct lua_type_of<basic_protected_function<Base>> : std::integral_constant<type, type::function> {};

template <>
struct lua_type_of<coroutine> : std::integral_constant<type, type::function> {};

template <>
struct lua_type_of<thread> : std::integral_constant<type, type::thread> {};

template <typename Signature>
struct lua_type_of<std::function<Signature>> : std::integral_constant<type, type::function>{};

template <typename T>
struct lua_type_of<optional<T>> : std::integral_constant<type, type::poly>{};

template <typename T>
struct lua_type_of<T*> : std::integral_constant<type, type::userdata> {};

template <typename T>
struct lua_type_of<T, std::enable_if_t<std::is_arithmetic<T>::value>> : std::integral_constant<type, type::number> {};

template <typename T>
struct lua_type_of<T, std::enable_if_t<std::is_enum<T>::value>> : std::integral_constant<type, type::number> {};

template <typename T>
struct is_lua_primitive : std::integral_constant<bool, 
    type::userdata != lua_type_of<meta::Unqualified<T>>::value
    || std::is_base_of<reference, meta::Unqualified<T>>::value
    || std::is_base_of<stack_reference, meta::Unqualified<T>>::value
    || meta::is_specialization_of<std::tuple, meta::Unqualified<T>>::value
    || meta::is_specialization_of<std::pair, meta::Unqualified<T>>::value
> { };

template <typename T>
struct is_lua_primitive<std::reference_wrapper<T>> : std::true_type { };
template <typename T>
struct is_lua_primitive<optional<T>> : std::true_type {};
template <typename T>
struct is_lua_primitive<T*> : std::true_type {};
template <>
struct is_lua_primitive<userdata_value> : std::true_type {};
template <>
struct is_lua_primitive<light_userdata_value> : std::true_type {};
template <typename T>
struct is_lua_primitive<non_null<T>> : is_lua_primitive<T*> {};

template <typename T>
struct is_proxy_primitive : is_lua_primitive<T> { };

template <typename T>
struct is_unique_usertype : std::false_type {};

template<typename T>
inline type type_of() {
    return lua_type_of<meta::Unqualified<T>>::value;
}

inline type type_of(lua_State* L, int index) {
    return static_cast<type>(lua_type(L, index));
}
} // sol

#endif // SOL_TYPES_HPP