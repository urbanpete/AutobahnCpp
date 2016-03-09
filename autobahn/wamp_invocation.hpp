///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) Tavendo GmbH
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBAHN_WAMP_INVOCATION_HPP
#define AUTOBAHN_WAMP_INVOCATION_HPP

#include "wamp_arguments.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <msgpack.hpp>
#include <string>

namespace autobahn {

class wamp_message;

class wamp_invocation_impl
{
public:
    wamp_invocation_impl();
    wamp_invocation_impl(wamp_invocation_impl&&) = delete; // copy wamp_invocation instead

    //add URI and details
    /*!
    * Invocatition procedure URI.  Used by prefix & wildcard registered procedures
    */
    const std::string& uri() const;
    /*!
     * The number of positional arguments passed to the invocation.
     */
    std::size_t number_of_arguments() const;

    /*!
     * The number of keyword arguments passed to the invocation.
     */
    std::size_t number_of_kw_arguments() const;

    /*!
     * The positional argument passed to the invocation with the given @p index, converted to type T.
     *
     * Example:
     * `std::string id = invocation->argument<std::string>(0); // first positional argument`
     *
     * @throw std::out_of_range
     * @throw std::bad_cast
     */
    template <typename T>
    T argument(std::size_t index) const;

    /*!
     * The positional arguments passed to the invocation, converted to a list type.
     *
     * Example:
     * `auto args = invocation->arguments<std::tuple<std::string>>();`
     *
     * @throw std::bad_cast
     */
    template <typename List>
    List arguments() const;

    /*!
     * Convert and assign the positional arguments to the given @p args list.
     *
     * Example:
     * ```
     * std::tuple<std::string> args;
     * invocation->get_arguments(args);
     * ```
     *
     * @throw std::bad_cast
     */
    template <typename List>
    void get_arguments(List& args) const;

    /*!
     * Convert and assign the positional arguments to a given list of individual parameters.
     *
     * Enables a syntax that lets you declare variables individually, but list them in a
     * single space to empathize parameter order. This will also throw if the number of
     * arguments to the invocation doesn't match the number of given parameters.
     *
     * Example:
     * ```
     * uint64_t id;
     * std::string name;
     * invocation->get_each_argument(id, name);
     * ```
     *
     * @throw std::bad_cast
     */
    template <typename... T>
    inline void get_each_argument(T&... args) const;

    /*!
     * The keyword argument passed to the invocation with the given @p key, converted to type T.
     *
     * Overloads are provided for `std::string` and `char*` as @p key type.
     *
     * This function uses key string comparisons to find the matching value, O(n) with n being
     * the number of map elements. Memory allocation for keys is avoided though. For larger maps,
     * you might want to prioritize look-up performance by using `std::map`, `std::unordered_map`
     * or custom types with custom deserialization. To do this, use kw_arguments<Map>() or
     * get_kw_arguments<Map>(map), then access the items from there.
     *
     * Example:
     * `std::string id = invocation->kw_argument<std::string>("id");`
     *
     * @throw std::out_of_range
     * @throw std::bad_cast
     */
    template <typename T>
    T kw_argument(const std::string& key) const;

    template <typename T>
    T kw_argument(const char* key) const;

    /*!
     * The keyword argument passed to the invocation with the given @p key, converted to type T,
     * or the given @p fallback if no such key was passed.
     *
     * Overloads are provided for `std::string` and `char*` as @p key type.
     *
     * This function uses key string comparisons to find the matching value, O(n) with n being
     * the number of map elements. Memory allocation for keys is avoided though. For larger maps,
     * you might want to prioritize look-up performance by using `std::map`, `std::unordered_map`
     * or custom types with custom deserialization. To do this, use kw_arguments<Map>() or
     * get_kw_arguments<Map>(map), then access the items from there.
     *
     * Example:
     * `std::string id = invocation->kw_argument_or("id", std::string());`
     *
     * @throw std::bad_cast
     */
    template <typename T>
    T kw_argument_or(const std::string& key, const T& fallback) const;

    template <typename T>
    T kw_argument_or(const char* key, const T& fallback) const;

    /*!
     * The keyword arguments passed to the invocation, converted to a map type.
     *
     * Example:
     * `auto kw_args = invocation->kw_arguments<std::unordered_map<std::string, msgpack::object>>();`
     *
     * @throw std::bad_cast
     */
    template <typename Map>
    Map kw_arguments() const;

    /*!
     * Convert and assign the keyword arguments to the given @p kw_args map.
     *
     * Example:
     * ```
     * std::unordered_map<std::string, msgpack::object> kw_args;
     * invocation->get_kw_arguments(kw_args);
     * ```
     *
     * @throw std::bad_cast
     */
    template <typename Map>
    void get_kw_arguments(Map& kw_args) const;

    /*!
    * Checks if caller expects progressive results.
    */
    bool progressive_results_expected() const;

    /*!
     * Reply to the invocation with an empty result.
     */
    void empty_result();

    /*!
    * Send progressive/partial result with positional arguments.
    */
    template <typename List>
    void progress(const List& arguments);

    /*!
    * Send progressive/partial result with positional and keyword arguments.
    */
    template <typename List, typename Map>
    void progress(const List& arguments, const Map& kw_arguments);

    /*!
     * Reply to the invocation with positional arguments.
     */
    template <typename List>
    void result(const List& arguments);

    /*!
     * Reply to the invocation with positional and keyword arguments.
     */
    template <typename List, typename Map>
    void result(const List& arguments, const Map& kw_arguments);

    /*!
     * Reply to the invocation with an error and no further details.
     */
    void error(const std::string& error_uri);

    /*!
     * Reply to the invocation with an error, including positional arguments.
     */
    template <typename List>
    void error(const std::string& error_uri, const List& arguments);

    /*!
     * Reply to the invocation with an error, including positional and keyword arguments.
     */
    template <typename List, typename Map>
    void error(
            const std::string& error_uri,
            const List& arguments, const Map& kw_arguments);

    //
    // functions only called internally by wamp_session

    using result_type = enum {
        final = 0,
        intermediary
    } ;

    using send_result_fn = std::function<void(const std::shared_ptr<wamp_message>&)>;
    void set_send_result_fn(send_result_fn&&);
    void set_details(const msgpack::object& details);
    void set_request_id(std::uint64_t);
	std::uint64_t get_request_id();
	void set_zone(msgpack::zone&&);
    void set_arguments(const msgpack::object& arguments);
    void set_kw_arguments(const msgpack::object& kw_arguments);
    bool sendable() const;

private:
    void throw_if_not_sendable() const;

    template <typename List>
    void send_result(const List& arguments, result_type resultType);

    template <typename List, typename Map>
    void send_result(const List& arguments, const Map& kw_arguments, result_type resultType);
private:


    msgpack::zone m_zone;
    msgpack::object m_arguments;
    msgpack::object m_kw_arguments;
    send_result_fn m_send_result_fn;
    std::uint64_t m_request_id;
    std::string m_uri;
    bool m_progressive_results_expected;
};

using wamp_invocation = std::shared_ptr<wamp_invocation_impl>;

} // namespace autobahn

#include "wamp_invocation.ipp"

#endif // AUTOBAHN_WAMP_INVOCATION_HPP
