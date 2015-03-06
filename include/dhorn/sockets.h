/*
 * sockets.h
 *
 * Duncan Horn
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>

#ifdef  WIN32

/* Force the project to link against the Windows Sockets library */
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#else

#error "Your compiler is not yet supported by the dhorn sockets API"

#endif

namespace dhorn
{
    /*
     * Socket Types and Constants
     */
#pragma region Socket Types and Constants

#ifdef  WIN32
    using socket_t = SOCKET;
    using socket_error_t = int;

    static const socket_t invalid_socket = INVALID_SOCKET;
    static const socket_error_t socket_error = SOCKET_ERROR;

    // Not defined using htonl since all values exposed to client code should be in host-byte-order. We do the
    // translation to network-byte-order when these values are used by the implementation.
    static const uint32_t any_address = INADDR_ANY;
    static const uint32_t loopback_address = INADDR_LOOPBACK;
    static const uint32_t broadcast_address = INADDR_BROADCAST;
    static const uint32_t invalid_address = INADDR_NONE;
#endif  /*WIN32*/

    enum class address_family : uint16_t
    {
        unspecified                     = AF_UNSPEC,
        unix                            = AF_UNIX,
        internetwork_version_4          = AF_INET,
        arpanet_imp                     = AF_IMPLINK,
        pup_protocols                   = AF_PUP,
        mit_chaos_protocols             = AF_CHAOS,
        xerox_ns_protocols              = AF_NS,
        ipx_protocols                   = AF_IPX,
        iso_protocols                   = AF_ISO,
        osi_protocols                   = AF_OSI,
        european_computer_manufacturers = AF_ECMA,
        data_kit_protocols              = AF_DATAKIT,
        ccitt_protocols                 = AF_CCITT,
        ibm_sna                         = AF_SNA,
        dec_net                         = AF_DECnet,
        direct_data_link_interface      = AF_DLI,
        lat                             = AF_LAT,
        nsc_hyperchannel                = AF_HYLINK,
        apple_talk                      = AF_APPLETALK,
        net_bios                        = AF_NETBIOS,
        voice_view                      = AF_VOICEVIEW,
        fire_fox_protocols              = AF_FIREFOX,
        unknown_1                       = AF_UNKNOWN1,
        banyan                          = AF_BAN,
        native_atm_services             = AF_ATM,
        internetwork_version_6          = AF_INET6,
        microsoft_wolfpack              = AF_CLUSTER,
        ieee_12844_4_wg_af              = AF_12844,
        irda                            = AF_IRDA,
        net_designers_osi_gateway       = AF_NETDES,
    };

    enum class socket_type : int
    {
        stream                      = SOCK_STREAM,
        datagram                    = SOCK_DGRAM,
        raw_protocol                = SOCK_RAW,
        reliably_delivered_message  = SOCK_RDM,
        sequenced_packet_stream     = SOCK_SEQPACKET,
    };

    enum class ip_protocol
    {
        ipv6_hop_by_hop_options                 = IPPROTO_HOPOPTS,
        internet_control_message_protocol       = IPPROTO_ICMP,
        internet_group_management_protocol      = IPPROTO_IGMP,
        gateway_to_gateway_protocol             = IPPROTO_GGP,
        version_4_header                        = IPPROTO_IPV4,
        stream_protocol                         = IPPROTO_ST,
        transmission_control_protocol           = IPPROTO_TCP,
        core_based_trees_protocol               = IPPROTO_CBT,
        exterior_gateway_protocol               = IPPROTO_EGP,
        private_interior_gateway_protocol       = IPPROTO_IGP,
        parc_universal_packet_protocol          = IPPROTO_PUP,
        user_datagram_protocol                  = IPPROTO_UDP,
        internet_datagram_protocol              = IPPROTO_IDP,
        reliable_data_protocol                  = IPPROTO_RDP,
        version_6_header                        = IPPROTO_IPV6,
        ipv6_routing_header                     = IPPROTO_ROUTING,
        ipv6_fragmentation_header               = IPPROTO_FRAGMENT,
        encapsulating_security_payload          = IPPROTO_ESP,
        authentication_header                   = IPPROTO_AH,
        ipv6_internet_control_message_protocol  = IPPROTO_ICMPV6,
        ipv6_no_next_header                     = IPPROTO_NONE,
        ipv6_destination_options                = IPPROTO_DSTOPTS,
        net_disk_protocol                       = IPPROTO_ND,
        wideband_monitoring                     = IPPROTO_ICLFXBM,
        protocol_independent_multicast          = IPPROTO_PIM,
        programatic_general_multicast           = IPPROTO_PGM,
        level_2_tunneling_protocol              = IPPROTO_L2TP,
        stream_control_transmission_protocol    = IPPROTO_SCTP,
        raw_ip_packets                          = IPPROTO_RAW,
    };

    enum class message_flags : int
    {
        dont_route      = MSG_DONTROUTE,
        interrupt       = MSG_INTERRUPT,
        out_of_band     = MSG_OOB,
        partial         = MSG_PARTIAL,
        peek            = MSG_PEEK,
        push_immediate  = MSG_PUSH_IMMEDIATE,
        wait_all        = MSG_WAITALL,
    };

    inline message_flags operator|(_In_ message_flags lhs, _In_ message_flags rhs)
    {
        return static_cast<message_flags>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline message_flags operator&(_In_ message_flags lhs, _In_ message_flags rhs)
    {
        return static_cast<message_flags>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    enum class shutdown_options : int
    {
        send    = SD_SEND,
        receive = SD_RECEIVE,
        both    = SD_BOTH,
    };

    enum class socket_level : int
    {
        socket  = SOL_SOCKET,
    };

    enum class socket_option : int
    {
        debug_info                      = SO_DEBUG,
        listening                       = SO_ACCEPTCONN,
        reuse_address                   = SO_REUSEADDR,
        keep_alive                      = SO_KEEPALIVE,
        dont_route                      = SO_DONTROUTE,
        broadcast                       = SO_BROADCAST,
        use_loopback                    = SO_USELOOPBACK,
        linger_on_close                 = SO_LINGER,
        oob_inline                      = SO_OOBINLINE,
        dont_linger_on_close            = SO_DONTLINGER,
        exclusive_address_use           = SO_EXCLUSIVEADDRUSE,

        send_buffer_size                = SO_SNDBUF,
        receive_buffer_size             = SO_RCVBUF,
        send_low_water_mark             = SO_SNDLOWAT,
        receive_low_water_mark          = SO_RCVLOWAT,
        send_timeout                    = SO_SNDTIMEO,
        receive_timeout                 = SO_RCVTIMEO,
        error_status                    = SO_ERROR,
        socket_type                     = SO_TYPE,

        group_id                        = SO_GROUP_ID,
        group_priority                  = SO_GROUP_PRIORITY,
        max_message_size                = SO_MAX_MSG_SIZE,
        protocol_info_ansi              = SO_PROTOCOL_INFOA,
        protocol_info_wide              = SO_PROTOCOL_INFOW,
        protocol_info                   = SO_PROTOCOL_INFO,
        service_provider_config_info    = PVD_CONFIG,
        conditional_accept              = SO_CONDITIONAL_ACCEPT,
    };

    inline socket_option operator|(_In_ socket_option lhs, _In_ socket_option rhs)
    {
        return static_cast<socket_option>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline socket_option operator&(_In_ socket_option lhs, _In_ socket_option rhs)
    {
        return static_cast<socket_option>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

#pragma endregion



    /*
     * socket_exception
     */
#pragma region socket_exception

    class socket_exception :
        public std::exception
    {
    public:
        socket_exception(_In_ socket_error_t error) :
            _error(error)
        {
        }

        virtual char *what(void)
        {
            return "socket_exception";
        }

        socket_error_t get_error(void)
        {
            return this->_error;
        }

    private:

        socket_error_t _error;
    };

    namespace garbage
    {
        inline void wsa_throw_last_error(void)
        {
            auto error = WSAGetLastError();
            if (error)
            {
                throw socket_exception(error);
            }
        }

        template <typename Ty>
        inline void wsa_throw_if_null(_In_ Ty *ptr)
        {
            if (!ptr)
            {
                wsa_throw_last_error();
            }
        }

        inline socket_error_t wsa_throw_if_error(_In_ socket_error_t val)
        {
            if (val == socket_error)
            {
                wsa_throw_last_error();
            }

            return val;
        }

        inline void wsa_throw_if_false(_In_ bool expr, _In_ socket_error_t error)
        {
            if (!expr)
            {
                throw socket_exception(error);
            }
        }

        inline void wsa_throw_if_true(_In_ bool expr, _In_ socket_error_t error)
        {
            if (expr)
            {
                throw socket_exception(error);
            }
        }
    }

#pragma endregion



    /*
     * Sockets Traits
     */
    namespace garbage
    {
        /*
         * address_family_traits
         */
        template <address_family Family>
        struct address_family_traits;

        template <>
        struct address_family_traits<address_family::internetwork_version_4>
        {
            using ip_addr = in_addr;
            using sock_addr = sockaddr_in;
            using create_type = unsigned long;
            static const address_family family = address_family::internetwork_version_4;
            static const int max_string_len = 16;

            static inline const ip_addr &create(_In_ const sock_addr &addr)
            {
                garbage::wsa_throw_if_false(addr.sin_family == static_cast<int>(family), WSAEINVAL);
                return addr.sin_addr;
            }

            static inline ip_addr create(_In_ create_type val)
            {
                ip_addr result;
                result.s_addr = htonl(val);
                return result;
            }
        };

        template <>
        struct address_family_traits<address_family::internetwork_version_6>
        {
            using ip_addr = in6_addr;
            using sock_addr = sockaddr_in6;
            using create_type = uint8_t[16];
            static const address_family family = address_family::internetwork_version_6;
            static const int max_string_len = 46;

            static inline const ip_addr &create(_In_ const sock_addr &addr)
            {
                garbage::wsa_throw_if_false(addr.sin6_family == static_cast<int>(family), WSAEINVAL);
                return addr.sin6_addr;
            }

            static inline ip_addr create(_In_ const create_type &val)
            {
                // create_type is already in network-byte-order, so we can just copy as-is
                ip_addr result;
                memcpy(result.s6_addr, val, sizeof(result.s6_addr));
                return result;
            }
        };



        /*
         * sockets_string_traits
         */
        template <typename CharType, address_family Family>
        struct sockets_string_traits;

        template <address_family Family>
        struct sockets_string_traits<char, Family>
        {
            using MyTraits = address_family_traits<Family>;
            using ip_addr = typename MyTraits::ip_addr;

            inline static std::string n_to_p(_In_ const ip_addr &addr)
            {
                char buff[MyTraits::max_string_len + 1];

                auto result = InetNtopA(static_cast<int>(Family), (PVOID)&addr, buff, sizeof(buff));
                garbage::wsa_throw_if_null(result);

                return buff;
            }
        };

        template <address_family Family>
        struct sockets_string_traits<wchar_t, Family>
        {
            using MyTraits = address_family_traits<Family>;
            using ip_addr = typename MyTraits::ip_addr;

            inline static std::wstring n_to_p(_In_ const ip_addr &addr)
            {
                wchar_t buff[MyTraits::max_string_len + 1];

                auto result = InetNtopW(static_cast<int>(Family), (PVOID)&addr, buff, sizeof(buff) / 2);
                garbage::wsa_throw_if_null(result);

                return buff;
            }
        };
    }



    /*
     * dhorn::ip_address
     */
#pragma region ip_address

    template <address_family Family>
    class ip_address
    {
        using MyTraits = garbage::address_family_traits<Family>;

    public:

        /*
         * Type/Value Definitions
         */
        using ip_addr = typename MyTraits::ip_addr;
        using sock_addr = typename MyTraits::sock_addr;
        static const address_family family = Family;



        /*
         * Constructor(s)/Destructor
         */
        ip_address(void) :
            _addr{}
        {
        }

        ip_address(_In_ const typename MyTraits::create_type &val) :
            _addr(MyTraits::create(val))
        {
        }

        ip_address(_In_ const ip_addr &ip) :
            _addr(ip)
        {
        }

        ip_address(_In_ const sock_addr &addr) :
            _addr(MyTraits::create(addr))
        {
        }

        template <typename CharType>
        ip_address(_In_ const CharType *ip)
        {
            this->Set(ip);
        }

        template <typename CharType>
        ip_address(_In_ const std::basic_string<CharType> &ip)
        {
            this->Set(ip.c_str());
        }



        /*
         * Assignment Operators
         */
        ip_address &operator=(_In_ const typename MyTraits::create_type &val)
        {
            this->_addr = MyTraits::create(val);
            return *this;
        }

        ip_address &operator=(_In_ const ip_addr &ip)
        {
            this->_addr = ip;
            return *this;
        }

        ip_address &operator=(_In_ const typename MyTraits::sock_addr &addr)
        {
            this->_addr = MyTraits::create(addr);
            return *this;
        }

        template <typename CharType>
        ip_address &operator=(_In_ const CharType *ip)
        {
            this->Set(ip);
            return *this;
        }

        template <typename CharType>
        ip_address &operator=(_In_ const std::basic_string<CharType> &ip)
        {
            this->Set(ip.c_str());
            return *this;
        }



        /*
         * Type Conversion Operators
         */
        operator const ip_addr &(void) const
        {
            return this->_addr;
        }

        template <typename CharType>
        operator std::basic_string<CharType>(void) const
        {
            return this->str<CharType>();
        }



        /*
         * Public Functions
         */
        const ip_addr &addr(void) const
        {
            return this->_addr;
        }

        template <typename CharType = char>
        std::basic_string<CharType> str(void) const
        {
            return garbage::sockets_string_traits<CharType, Family>::n_to_p(this->_addr);
        }



    private:

        void Set(_In_ const char *ip)
        {
            auto result = garbage::wsa_throw_if_error(InetPtonA(static_cast<int>(Family), ip, &this->_addr));
            if (result == 0)
            {
                // Result of an invalid string. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
        }

        void Set(_In_ const wchar_t *ip)
        {
            auto result = garbage::wsa_throw_if_error(InetPtonW(static_cast<int>(Family), ip, &this->_addr));
            if (result == 0)
            {
                // Result of an invalid string. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
        }



        ip_addr _addr;
    };

    using ipv4_address = ip_address<address_family::internetwork_version_4>;
    using ipv6_address = ip_address<address_family::internetwork_version_6>;

#pragma endregion



    /*
     * socket_address
     */
#pragma region socket_address

    class socket_address final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        socket_address(void) :
            _ipv6Addr{}         // Zero initialize the largest struct in the union
        {
        }

        socket_address(_In_ const ipv4_address &addr, _In_ uint16_t port)
        {
            this->assign(addr, port);
        }

        socket_address(
            _In_ const ipv6_address &addr,
            _In_ uint16_t port,
            _In_ uint32_t flowInfo,
            _In_ uint32_t scopeId)
        {
            this->assign(addr, port, flowInfo, scopeId);
        }



        /*
         * Operators
         */
        operator sockaddr *(void)
        {
            return &this->_addr;
        }

        operator const sockaddr *(void) const
        {
            return &this->_addr;
        }

        operator sockaddr &(void)
        {
            return this->_addr;
        }

        operator const sockaddr &(void) const
        {
            return this->_addr;
        }

        operator sockaddr_in &(void)
        {
            return this->_ipv4Addr;
        }

        operator const sockaddr_in &(void) const
        {
            return this->_ipv4Addr;
        }

        operator sockaddr_in6 &(void)
        {
            return this->_ipv6Addr;
        }

        operator const sockaddr_in6 &(void) const
        {
            return this->_ipv6Addr;
        }



        /*
         * Public Functions
         */
        void assign(_In_ const ipv4_address &addr, _In_ uint16_t port)
        {
            this->_ipv4Addr =
            {
                static_cast<int>(ipv4_address::family),     // sin_family
                htons(port),                                // sin_port
                addr                                        // sin_addr
            };

            this->_size = sizeof(this->_ipv4Addr);
        }

        void assign(_In_ const ipv6_address &addr, _In_ uint16_t port, _In_ uint32_t flowInfo, _In_ uint32_t scopeId)
        {
            this->_ipv6Addr =
            {
                static_cast<int>(ipv6_address::family),     // sin6_family
                htons(port),                                // sin6_port
                flowInfo,                                   // sin6_flowinfo
                addr,                                       // sin6_addr
                { scopeId }                                 // sin6_scope_id
            };

            this->_size = sizeof(this->_ipv6Addr);
        }

        size_t size(void) const
        {
            return this->_size;
        }

        size_t reset_size(void)
        {
            switch (static_cast<address_family>(this->_ipv4Addr.sin_family))
            {
            case address_family::internetwork_version_4:
                this->_size = sizeof(this->_ipv4Addr);
                break;

            case address_family::internetwork_version_6:
                this->_size = sizeof(this->_ipv6Addr);
                break;

            default:
                // Unknown
                this->_size = 0;
                break;
            }
        }

        address_family family(void) const
        {
            return static_cast<address_family>(this->_addr.sa_family);
        }

        ipv4_address as_ipv4(void) const
        {
            // Throws if sin_famliy does not equal AF_INET
            return this->_ipv4Addr;
        }

        ipv6_address as_ipv6(void) const
        {
            // Throws if sin6_family does not equal AF_INET6
            return this->_ipv6Addr;
        }



    private:

        union
        {
            sockaddr _addr;
            sockaddr_in _ipv4Addr;
            sockaddr_in6 _ipv6Addr;
        };

        size_t _size;
    };

#pragma endregion



    /*
     * socket_base
     */
#pragma region socket_base

    namespace garbage
    {
        /*
         * Only to be used by socket_base constructor to call WSAStartup to load WinSock.dll
         */
        struct socket_initializer
        {
            socket_initializer(void)
            {
                WSADATA data;
                auto result = WSAStartup(MAKEWORD(2, 2), &data);

                // If the dll load fails, we cannot call WSAGetLastError
                if (result)
                {
                    throw socket_exception(result);
                }
            }

            ~socket_initializer(void)
            {
                wsa_throw_if_error(WSACleanup());
            }
        };

    }


    /*
     * Base socket class
     */
    class socket_base
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        socket_base(void) :
            _socket(invalid_socket)
        {
            // Under the C++ standard, the construction of static function variables is not guaranteed to be thread
            // safe, so the constructor technically can get called more than once here. In the unlikely event that
            // this does happen, we'll just end up calling WSAStartup more than once while only making one call to
            // WSACleanup. Since the system will re-claim our resources, we just ignore the possibility
            static const garbage::socket_initializer _init;
        }

        socket_base(socket_t sock) :
            socket_base()
        {
            this->_socket = sock;
        }

        socket_base(_Inout_ socket_base &&other) :
            socket_base()
        {
            this->swap(other);
        }

        ~socket_base(void)
        {
            this->Destroy();
        }

        // Cannot copy
        socket_base(_In_ const socket_base &other) = delete;
        socket_base &operator=(_In_ const socket_base &other) = delete;



        /*
         * Operators
         */
        socket_base &operator=(_Inout_ socket_base &&other)
        {
            this->swap(other);
            return *this;
        }

        socket_base &operator=(_In_ socket_t sock)
        {
            this->Destroy();
            this->_socket = sock;

            return *this;
        }

        operator socket_t(void) const
        {
            return this->_socket;
        }

        operator bool(void) const
        {
            return this->_socket != invalid_socket;
        }



        /*
         * Socket functions
         */
        socket_t accept(_Inout_ socket_address &addr)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            int size = sizeof(sockaddr_in6);

            auto result = ::accept(this->_socket, addr, &size);
            if (result == invalid_socket)
            {
                garbage::wsa_throw_last_error();
            }

            addr.reset_size();
            return result;
        }

        void bind(_In_ const socket_address &addr)
        {
            this->InvokeThrowOnError(::bind, this->_socket, addr, addr.size());
        }

        void close(void)
        {
            this->InvokeThrowOnError(::closesocket, this->_socket);
            this->_socket = invalid_socket;
        }

        void connect(_In_ const socket_address &addr)
        {
            this->InvokeThrowOnError(::connect, this->_socket, addr, addr.size());
        }

        socket_address get_peer_name(void)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            socket_address result;
            int size = sizeof(sockaddr_in6);

            this->InvokeThrowOnError(::getpeername, this->_socket, result, &size);

            // Reset the socket_address size and assert that its size is the expected size
            result.reset_size();
            assert(result.size() == static_cast<size_t>(size));

            return result;
        }

        socket_address get_socket_name(void)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            socket_address result;
            int size = sizeof(sockaddr_in6);

            this->InvokeThrowOnError(::getsockname, this->_socket, result, &size);

            // Reset the socket_address size and assert that its size is the expected size
            result.reset_size();
            assert(result.size() == static_cast<size_t>(size));

            return result;
        }

        template <typename Ty>
        Ty get_socket_option(_In_ socket_option opt)
        {
            Ty value;
            int len = static_cast<int>(sizeof(Ty));

            this->InvokeThrowOnError(::getsockopt, socket_level::socket, opt, &val, &len);

            return value;
        }

        void listen(_In_ int backlog)
        {
            this->InvokeThrowOnError(::listen, this->_socket, backlog);
        }

        void open(_In_ address_family family, _In_ socket_type type, _In_ ip_protocol protocol)
        {
            garbage::wsa_throw_if_false(this->_socket == invalid_socket, WSAEISCONN);

            this->_socket = socket(static_cast<int>(family), static_cast<int>(type), static_cast<int>(protocol));
            if (this->_socket == invalid_socket)
            {
                garbage::wsa_throw_last_error();
            }
        }

        socket_error_t receive(
            _Out_writes_bytes_to_(length, return) void *buffer,
            _In_ int length,
            _In_ message_flags flags)
        {
            auto buff = static_cast<char *>(buffer);
            return this->InvokeThrowOnError(::recv, this->_socket, buff, length, static_cast<int>(flags));
        }

        socket_error_t receive_from(
            _Out_writes_bytes_to_(length, return) void *buffer,
            _In_ int length,
            _In_ message_flags flags,
            _Out_ socket_address &addr)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            int size = sizeof(sockaddr_in6);

            auto result = this->InvokeThrowOnError(
                ::recvfrom,
                this->_socket,
                static_cast<char *>(buffer),
                length,
                static_cast<int>(flags),
                addr,
                &size);

            // Reset the socket_address size and assert that its size is the expected size
            addr.reset_size();
            assert(addr.size() == static_cast<size_t>(size));

            return result;
        }

        socket_error_t send(_In_reads_bytes_(length) const void *buffer, _In_ int length, _In_ message_flags flags)
        {
            auto buff = static_cast<const char *>(buffer);
            return this->InvokeThrowOnError(::send, this->_socket, buff, length, static_cast<int>(flags));
        }

        socket_error_t send_to(
            _In_reads_bytes_(length) const void *buffer,
            _In_ int length,
            _In_ message_flags flags,
            _In_ const socket_address &addr)
        {
            return this->InvokeThrowOnError(
                ::sendto,
                this->_socket,
                static_cast<const char *>(buffer),
                length,
                static_cast<int>(flags),
                addr,
                addr.size());
        }

        void shutdown(_In_ shutdown_options options)
        {
            this->InvokeThrowOnError(::shutdown, this->_socket, static_cast<int>(options));
        }



        /*
         * Other functions
         */
        void swap(_Inout_ socket_base &other)
        {
            auto temp = this->_socket;
            this->_socket = other._socket;
            other._socket = temp;
        }

        socket_t detach(void)
        {
            auto value = this->_socket;
            this->_socket = invalid_socket;
            return value;
        }



    protected:

        // Invokes a function and closes the socket (if it's open) on failure. The destructor would otherwise take
        // care of closing the socket for us, but it would also throw another exception, which we don't want
        template <typename Func, typename... Args>
        inline socket_error_t InvokeThrowOnError(_In_ Func &func, _In_ Args&&... args)
        {
            auto result = func(std::forward<Args>(args)...);
            if (result == socket_error)
            {
                garbage::wsa_throw_last_error();
            }

            return result;
        }

        void Destroy(void)
        {
            // If the socket isn't already closed, we throw an exception (similar to std::thread if join or detach
            // has not been called). However, in order to leave the program in a good state, we go ahead and close
            // the socket for the user. Note that this function is called by the destructor, so we cannot rely on
            // the destructor doing the cleanup in such situations.
            if (this->_socket != invalid_socket)
            {
                this->close();
                throw socket_exception(WSAEFAULT);
            }
        }

        socket_t _socket;
    };

#pragma endregion



    /*
     * udp_socket
     */
    class udp_socket final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */


    private:

    };
}
