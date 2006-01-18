/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  BufferPolicy.hpp 

                        BufferPolicy.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_CORELIB_BUFFER_POLICY_HPP
#define ORO_CORELIB_BUFFER_POLICY_HPP
#include <os/Semaphore.hpp>

namespace ORO_CoreLib
{

    /**
     * Use this policy to indicate that you \b do \b not want
     * to block on an \a empty or \a full buffer, queue, fifo,...
     */
    struct NonBlockingPolicy
    {
        NonBlockingPolicy(unsigned int ) {}
        void push(int /*i*/ = 1 ) {
        }
        void pop(int /*i*/ = 1 ) {
        }
        void reset( int /*i*/ ) {
        }
    };

    /**
     * Use this policy to indicate that you \b do want
     * to block on an \a empty or \a full buffer, queue, fifo,...
     */
    struct BlockingPolicy
    {
        BlockingPolicy(unsigned int c) : count(c) {}
        void push(int add = 1) {
            while (add-- > 0) {
                count.signal();
            }
        }
        void pop(int sub = 1) {
            while (sub-- > 0) {
                count.wait();
            }
        }
        void reset( int c ) {
            while( c > count.value() )
                count.signal();
            while( c < count.value() )
                count.wait();
        }
    private:
        ORO_OS::Semaphore count;
    };
}
#endif
