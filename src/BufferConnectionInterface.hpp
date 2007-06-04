/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  BufferConnectionInterface.hpp 

                        BufferConnectionInterface.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_EXECUTION_BUFFER_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_BUFFER_CONNECTION_INTERFACE_HPP

#include "ConnectionInterface.hpp"
#include "BufferInterface.hpp"

namespace RTT
{
    /**
     * This is the interface of a connection which
     * transfers buffered data from a read Port a TaskContext.
     */
    template<class T>
    class ReadConnectionInterface
        : public virtual ConnectionInterface
    {
    public:
        typedef boost::intrusive_ptr< ReadConnectionInterface<T> > shared_ptr;

        /**
         * Get the buffer's read interface.
         */
        virtual ReadInterface<T>* read() = 0;

        /**
         * Let the connection use a new Buffer implementation.
         * This may lead to information loss if called when the connection
         * is in use.
         * @param doi The new buffer to use.
         * @post \a doi is acquired by this connection.
         */
        virtual void setImplementation( BufferInterface<T>* doi ) = 0;
    };

    /**
     * This is the interface of a connection which
     * transfers buffered data to a write Port of a TaskContext.
     */
    template<class T>
    class WriteConnectionInterface
        : public virtual ConnectionInterface
    {
    public:
        typedef boost::intrusive_ptr< WriteConnectionInterface<T> > shared_ptr;
        /**
         * Get the buffer's write interface.
         */
        virtual WriteInterface<T>* write() = 0;

        /**
         * Let the connection use a new Buffer implementation.
         * This may lead to information loss if called when the connection
         * is in use.
         * @param doi The new buffer to use.
         * @post \a doi is acquired by this connection.
         */
        virtual void setImplementation( BufferInterface<T>* doi ) = 0;
    };

    /**
     * This is the interface of a connection which transfers buffered
     * data bidirectionally from a port of a TaskContext.
     */
    template<class T>
    class BufferConnectionInterface
        : public ReadConnectionInterface<T>, public WriteConnectionInterface<T>
    {
    public:
        typedef boost::intrusive_ptr< BufferConnectionInterface<T> > shared_ptr;

        /**
         * Get the buffer interface.
         */
        virtual BufferInterface<T>* buffer() = 0;

        /**
         * Let the connection use a new Buffer implementation.
         * This may lead to information loss if called when the connection
         * is in use.
         * @param doi The new buffer to use.
         * @post \a doi is acquired by this connection.
         */
        virtual void setImplementation( BufferInterface<T>* doi ) = 0;
    };

}

#endif
