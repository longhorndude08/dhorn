/*
 * Duncan Horn
 *
 * socket_stream.h
 *
 * Implements a std::streambuf class that is powered by a dhorn::tcp_socket for use with std::iostream, etc. There are
 * also simple wrappers that implement std::iostream, etc. for simpler use. E.g. without them, you'd have to do
 * something like:
 *
 *      dhorn::socket_streambuf buf(&this->_socket);
 *      std::iostream stream(&buf);
 *
 * But with them, this simplifies slightly to something easier on the eyes:
 *
 *      dhorn::socket_stream stream(&this->_socket);
 *
 * By default, the SocketStorageType template argument type is dhorn::tcp_socket *. If the client wishes to use
 * something different (e.g. std::shared_ptr<dhorn::tcp_socket>), that can be specified in its place.
 */

#include <iostream>
#include <memory>
#include <streambuf>

#include "sockets.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * streambuf implementation
         */
#pragma region basic_socket_streambuf

        template <
            typename CharT,
            typename CharTraits = std::char_traits<CharT>,
            std::size_t ReceiveBufferSize = 2048,
            std::size_t SendBufferSize = 2048,
            typename SocketStorageType = tcp_socket *>
        class basic_socket_streambuf final :
            public std::basic_streambuf<CharT, CharTraits>
        {
            using my_base = std::basic_streambuf<CharT, CharTraits>;

        public:
            basic_socket_streambuf(SocketStorageType socket) :
                _socket(socket),
                _receiveBuffer(std::make_unique<CharT[]>(ReceiveBufferSize)),
                _sendBuffer(std::make_unique<CharT[]>(SendBufferSize))
            {
                // Init get/put buffers
                this->setg(this->_receiveBuffer.get(), this->_receiveBuffer.get(), this->_receiveBuffer.get());
                this->setp(this->_sendBuffer.get(), this->_sendBuffer.get(), this->_sendBuffer.get() + SendBufferSize);
            }



            /*
             * basic_streambuf functions
             */
            virtual int underflow(void)
            {
                if (this->gptr() == this->egptr())
                {
                    // We're out of data; try and read more from the socket
                    auto len = this->_socket->receive(this->_receiveBuffer.get(), ReceiveBufferSize * sizeof(CharT));
                    this->setg(
                        this->_receiveBuffer.get(),
                        this->_receiveBuffer.get(),
                        this->_receiveBuffer.get() + len);
                }

                return (this->gptr() == this->egptr()) ?
                    typename my_base::traits_type::eof() :
                    typename my_base::traits_type::to_int_type(*this->gptr());
            }

            virtual int overflow(int ch)
            {
                bool isEof = (ch == typename my_base::traits_type::eof());

                if (this->pptr() == this->epptr())
                {
                    this->Flush();
                }

                if (isEof)
                {
                    assert(ch < 256);
                    this->sputc(static_cast<char>(ch));
                }

                return ch;
            }

            virtual int sync()
            {
                this->Flush();
                return 0;
            }



        private:

            void Flush(void)
            {
                auto len = static_cast<int>(this->pptr() - this->pbase());
                this->_socket->send(this->pbase(), len);
                this->setp(this->_sendBuffer.get(), this->_sendBuffer.get(), this->_sendBuffer.get() + SendBufferSize);
            }

            SocketStorageType _socket;
            std::unique_ptr<CharT[]> _receiveBuffer;
            std::unique_ptr<CharT[]> _sendBuffer;
        };



        /*
         * Type definitions
         */
        using socket_streambuf = basic_socket_streambuf<char>;
        using wsocket_streambuf = basic_socket_streambuf<wchar_t>;

#pragma endregion



        /*
         * iostream implementation
         */
#pragma region basic_socket_stream

        template <
            typename CharT,
            typename CharTraits = std::char_traits<CharT>,
            std::size_t ReceiveBufferSize = 2048,
            std::size_t SendBufferSize = 2048,
            typename SocketStorageType = tcp_socket *>
            class basic_socket_stream final :
            public std::basic_iostream<CharT, CharTraits>
        {
            using BufferType =
                basic_socket_streambuf<CharT, CharTraits, ReceiveBufferSize, SendBufferSize, SocketStorageType>;

        public:
            basic_socket_stream(SocketStorageType socket) :
                _buffer(socket),
                std::basic_iostream<CharT, CharTraits>(&this->_buffer)
            {
            }

        private:

            BufferType _buffer;
        };



        /*
         * Type Definitions
         */
        using socket_stream = basic_socket_stream<char>;
        using wsocket_stream = basic_socket_stream<wchar_t>;

#pragma endregion
    }
}
