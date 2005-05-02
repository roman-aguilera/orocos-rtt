/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Marshaller.hpp 

                        Marshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
#ifndef PI_PROPERTIES_MARSHALLER
#define PI_PROPERTIES_MARSHALLER

#include <string>

namespace ORO_CoreLib
{

    class PropertyBag;
	template<typename T> class Property;

	/**
     * @brief Used for converting properties to a format.
	 * An interface which all classes which serialize property bags
	 * should implement.
	 *
	 * @see Demarshaller
	 * @see Property
	 * @see PropertyBag
	 */
    class Marshaller
    {
        public:
        virtual ~Marshaller() {}

			/**
			 * Serialize a property of type bool.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<bool> &v) = 0;
			
			/**
			 * Serialize a property of type char.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<char> &v) = 0;
			
			/**
			 * Serialize a property of type int.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<int> &v) = 0;
			
			/**
			 * Serialize a property of type unsigned int.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<unsigned int> &v) = 0;
			
			/**
			 * Serialize a property of type double.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<double> &v) = 0;
			
			/**
			 * Serialize a property of type string.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<std::string> &v) = 0;
			
			/**
			 * Serialize a property of type property bag. This enables
			 * nested property bags.
			 * @param v The property to be serialized.
			 */
            virtual void serialize(const Property<PropertyBag> &v) = 0;
			
			/**
			 * Serialize the contents of a property bag with headers and footers. Use this method
             * on your 'root' PropertyBag, such that headers and footers
             * are written.
			 * @param v The property bag to be serialized.
			 */
            virtual void serialize(const PropertyBag &v) = 0;

            /**
             * Flush all buffers.
             * Instructs the Marshaller to flush any remaining buffered
             * data.
             */
            virtual void flush() = 0;

            /** 
             * Unknown types must decompose theirselves into the primitives.
             * For each type you define, you need to provide the function
             * 
             *    decomposeProperty( Marshaller*, const Property<YourType>& )
             *
             * An link time error will be generated
             * when this function is not found.
             */
            template< class T >
            void serialize( const Property<T> &v )
            {
                decomposeProperty(this, v);
            }
	};

	/**
     * @brief Used for extracting properties from a format.
	 * An interface which all classes which deserialize data
	 * should implement.
	 *
	 * @see Marshaller
	 * @see Property
	 * @see PropertyBag
	 */
	class Demarshaller
	{
		public:
        virtual ~Demarshaller() {}
			/**
			 * Deserialize data to a property bag.
			 * @param v The property bag which will contain the results
             *          of the deserialization.
			 * @see PropertyBag
			 */
            virtual bool deserialize(PropertyBag &v) = 0;
    };
} // Namespace ORO_CoreLib
#endif
