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

#ifndef SOL_STACK_PUSH_HPP
#define SOL_STACK_PUSH_HPP

#include "stack_core.hpp"
#include "raii.hpp"
#include "optional.hpp"
#include <memory>

namespace sol {
namespace stack {
template<typename T, typename>
struct pusher {
    template <typename... Args>
    static int push(lua_State* L, Args&&... args) {
        // Basically, we store all user-data like this:
        // If it's a movable/copyable value (no std::ref(x)), then we store the pointer to the new
        // data in the first sizeof(T*) bytes, and then however many bytes it takes to
        // do the actual object. Things that are std::ref or plain T* are stored as 
        // just the sizeof(T*), and nothing else.
        T** pointerpointer = static_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(T)));
        T*& referencereference = *pointerpointer;
        T* allocationtarget = reinterpret_cast<T*>(pointerpointer + 1);
        referencereference = allocationtarget;
        std::allocator<T> alloc{};
        alloc.construct(allocationtarget, std::forward<Args>(args)...);
        luaL_getmetatable(L, &usertype_traits<T>::metatable[0]);
        lua_setmetatable(L, -2);
        return 1;
    }
};

template <>
struct pusher<detail::as_reference_tag> {
    template <typename T>
    static int push(lua_State* L, T&& obj) {
        return stack::push(L, detail::ptr(obj));
    }
};

template<typename T>
struct pusher<T*> {
    static int push(lua_State* L, T* obj) {
        if (obj == nullptr)
            return stack::push(L, nil);
        T** pref = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
        *pref = obj;
        luaL_getmetatable(L, &usertype_traits<T*>::metatable[0]);
        lua_setmetatable(L, -2);
        return 1;
    }
};

template<typename T, typename Real>
struct pusher<unique_usertype<T, Real>> {
    template <typename... Args>
    static int push(lua_State* L, Args&&... args) {
        T** pref = static_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(detail::special_destruct_func) + sizeof(Real)));
        detail::special_destruct_func* fx = static_cast<detail::special_destruct_func*>(static_cast<void*>(pref + 1));
        Real* mem = static_cast<Real*>(static_cast<void*>(fx + 1));
        *fx = detail::special_destruct<T, Real>;
        detail::default_construct::construct(mem, std::forward<Args>(args)...);
        *pref = std::addressof(detail::deref(*mem));
        if (luaL_newmetatable(L, &usertype_traits<unique_usertype<T>>::metatable[0]) == 1) {
            set_field(L, "__gc", detail::unique_destruct<T>);
        }
        lua_setmetatable(L, -2);
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<is_unique_usertype<T>::value>> {
    template <typename... Args>
    static int push(lua_State* L, Args&&... args) {
        typedef typename is_unique_usertype<T>::metatable_type meta_type;
        return stack::push<unique_usertype<meta_type, T>>(L, std::forward<Args>(args)...);
    }
};

template<typename T, typename D>
struct pusher<std::unique_ptr<T, D>> {
    static int push(lua_State* L, std::unique_ptr<T, D> obj) {
        if (obj == nullptr)
            return stack::push(L, nil);
        return stack::push<unique_usertype<T, std::unique_ptr<T, D>>>(L, std::move(obj));
    }
};

template<typename T>
struct pusher<std::shared_ptr<T>> {
    template <typename S>
    static int push(lua_State* L, S&& s) {
        if (s == nullptr)
            return stack::push(L, nil);
        return stack::push<unique_usertype<T, std::shared_ptr<T>>>(L, std::forward<S>(s));
    }
};


template<typename T>
struct pusher<std::reference_wrapper<T>> {
    static int push(lua_State* L, const std::reference_wrapper<T>& t) {
        return stack::push(L, std::addressof(detail::deref(t.get())));
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<std::is_floating_point<T>::value>> {
    static int push(lua_State* L, const T& value) {
        lua_pushnumber(L, value);
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<std::is_integral<T>, std::is_signed<T>>::value>> {
    static int push(lua_State* L, const T& value) {
        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<std::is_integral<T>, std::is_unsigned<T>>::value>> {
    static int push(lua_State* L, const T& value) {
         typedef std::make_signed_t<T> signed_int;
         return stack::push(L, static_cast<signed_int>(value));
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<meta::has_begin_end<T>, meta::Not<meta::has_key_value_pair<T>>, meta::Not<std::is_base_of<reference, T>>>::value>> {
    static int push(lua_State* L, const T& cont) {
        lua_createtable(L, static_cast<int>(cont.size()), 0);
        int tableindex = lua_gettop(L);
        unsigned index = 1;
        for(auto&& i : cont) {
            set_field(L, index++, i, tableindex);
        }
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<meta::has_begin_end<T>, meta::has_key_value_pair<T>, meta::Not<std::is_base_of<reference, T>>>::value>> {
    static int push(lua_State* L, const T& cont) {
        lua_createtable(L, static_cast<int>(cont.size()), 0);
        int tableindex = lua_gettop(L);
        for(auto&& pair : cont) {
            set_field(L, pair.first, pair.second, tableindex);
        }
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<std::is_base_of<reference, T>::value>> {
    static int push(lua_State*, T& ref) {
        return ref.push();
    }

    static int push(lua_State*, T&& ref) {
        return ref.push();
    }
};

template<>
struct pusher<stack_reference> {
    static int push(lua_State*, const stack_reference& ref) {
        return ref.push();
    }
};

template<>
struct pusher<bool> {
    static int push(lua_State* L, bool b) {
        lua_pushboolean(L, b);
        return 1;
    }
};

template<>
struct pusher<nil_t> {
    static int push(lua_State* L, nil_t) {
        lua_pushnil(L);
        return 1;
    }
};

template<>
struct pusher<std::remove_pointer_t<lua_CFunction>> {
    static int push(lua_State* L, lua_CFunction func, int n = 0) {
        lua_pushcclosure(L, func, n);
        return 1;
    }
};

template<>
struct pusher<lua_CFunction> {
    static int push(lua_State* L, lua_CFunction func, int n = 0) {
        lua_pushcclosure(L, func, n);
        return 1;
    }
};

template<>
struct pusher<c_closure> {
    static int push(lua_State* L, c_closure closure) {
        lua_pushcclosure(L, closure.c_function, closure.upvalues);
        return 1;
    }
};

template<>
struct pusher<void*> {
    static int push(lua_State* L, void* userdata) {
        lua_pushlightuserdata(L, userdata);
        return 1;
    }
};

template<>
struct pusher<light_userdata_value> {
    static int push(lua_State* L, light_userdata_value userdata) {
        lua_pushlightuserdata(L, userdata);
        return 1;
    }
};

template<>
struct pusher<userdata_value> {
    static int push(lua_State* L, userdata_value data) {
        void** ud = static_cast<void**>(lua_newuserdata(L, sizeof(void*)));
        *ud = data.value;
        return 1;
    }
};

template<>
struct pusher<const char*> {
    static int push(lua_State* L, const char* str) {
        lua_pushlstring(L, str, std::char_traits<char>::length(str));
        return 1;
    }
};

template<size_t N>
struct pusher<char[N]> {
    static int push(lua_State* L, const char (&str)[N]) {
        lua_pushlstring(L, str, N - 1);
        return 1;
    }
};

template<>
struct pusher<std::string> {
    static int push(lua_State* L, const std::string& str) {
        lua_pushlstring(L, str.c_str(), str.size());
        return 1;
    }
};

template<typename... Args>
struct pusher<std::tuple<Args...>> {
    template <std::size_t... I, typename T>
    static int push(std::index_sequence<I...>, lua_State* L, T&& t) {
        int pushcount = 0;
        (void)detail::swallow{ 0, (pushcount += stack::push(L, 
              detail::forward_get<I>(t)
        ), 0)... };
        return pushcount;
    }

    template <typename T>
    static int push(lua_State* L, T&& t) {
        return push(std::index_sequence_for<Args...>(), L, std::forward<T>(t));
    }
};

template<typename A, typename B>
struct pusher<std::pair<A, B>> {
    template <typename T>
    static int push(lua_State* L, T&& t) {
        int pushcount = stack::push(L, detail::forward_get<0>(t));
        pushcount += stack::push(L, detail::forward_get<1>(t));
        return pushcount;
    }
};

template<typename O>
struct pusher<optional<O>> {
    template <typename T>
    static int push(lua_State* L, T&& t) {
        if (t == nullopt) {
            return stack::push(L, nullopt);
        }
        return stack::push(L, t.value());
    }
};

template<>
struct pusher<nullopt_t> {
    static int push(lua_State* L, nullopt_t) {
        return stack::push(L, nil);
    }
};
} // stack
} // sol

#endif // SOL_STACK_PUSH_HPP
