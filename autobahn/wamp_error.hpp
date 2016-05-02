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
#ifndef WAMP_ERROR_HPP
#define WAMP_ERROR_HPP

#include "wamp_message.hpp"
#include "autobahn/exceptions.hpp"
#include <stdexcept>

namespace autobahn {

class wamp_error : public std::runtime_error {
public:
    wamp_error(
            message_type type,
            uint64_t id,
            const std::string& uri,
            const msgpack::object& details,
            const msgpack::object& args,
            const msgpack::object& kwargs,
            const msgpack::zone& zone);

    wamp_error(const wamp_error &other);

    message_type type();

    uint64_t id() const;

    const char *uri() const;

    template<typename List>
    inline List args() const;

    template<typename Map>
    inline Map kw_args() const;

    template<typename T>
    inline T details() const;

private:
    message_type m_type;
    uint64_t m_id;
    msgpack::object m_details;
    msgpack::object m_args;
    msgpack::object m_kw_args;
    mutable msgpack::zone m_zone;
};

} // namespace autobahn

#include "wamp_error.ipp"

#endif // WAMP_ERROR_HPP
