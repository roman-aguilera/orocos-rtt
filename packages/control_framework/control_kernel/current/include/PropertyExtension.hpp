/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  PropertyExtension.hpp 

                        PropertyExtension.hpp -  description
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
 
#ifndef MOTIONPROPERTYEXTENSION_HPP
#define MOTIONPROPERTYEXTENSION_HPP

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include "KernelInterfaces.hpp"
#include "ComponentInterfaces.hpp"
#include "ComponentConfigurator.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    
    class PropertyExtension;

    /**
     * @brief A Component using properties can be extended with this interface
     * to export them to the user or update them with user defined values.
     * @see PropertyExtension
     * @ingroup compinterf
     */
    class PropertyComponentInterface
        :public detail::ComponentFacetInterface<PropertyExtension>
    {
        friend class PropertyExtension;
    protected:
        using detail::ComponentFacetInterface<PropertyExtension>::enableFacet;
        /**
         * The Default constructor 
         */
        PropertyComponentInterface( const std::string& _name );

        virtual bool enableFacet( PropertyExtension* ext);

        virtual void disableFacet();
    public:
        /**
         * Destructor.
         */
        virtual ~PropertyComponentInterface();

        /**
         * Update the Component's properties with a bag.
         */
        virtual bool updateProperties( const PropertyBag& bag) = 0;
        
        /**
         * Export the Component's properties into a bag.
         */
        virtual void exportProperties( PropertyBag& bag ) = 0;

    private:
        /**
         * Used by the PropertyExtension to access the components property bag.
         */
        Property<PropertyBag>& getLocalStore() { return localStore; }

        /**
         * The Kernel we belong to.
         */
        PropertyExtension* master;

        /**
         * This bag is used by the PropertyExtension to store
         * the properties of the component itself. It is as such
         * not owned by the component, but used by the PropertyExtension as
         * temporary storage. Virtually anything may be stored in here.
         */
        Property<PropertyBag> localStore;
    };

    /**
     * @brief A ControlKernel Extension for Properties.
     * It will configure all components in the Kernel with the properties found in the
     * cpf files.
     * 
     * The properties of the PropertyExtension itself contain the filenames 
     * of the component property files.
     * @see PropertyComponentInterface
     * @ingroup kernelint
     */
    class PropertyExtension
        : public detail::ExtensionInterface
    {
        public:
        typedef PropertyComponentInterface CommonBase;
        typedef unsigned int KeyType;

        /**
         * Create a PropertyExtension.
         */
        PropertyExtension(ControlKernelInterface* _base );

        virtual ~PropertyExtension();

        virtual bool updateProperties(const PropertyBag& bag);

        /**
         * Used by the PropertyComponentInterface to register itself to
         * this Extension.
         */
        bool addComponent(PropertyComponentInterface* comp);

        /**
         * Used by the PropertyComponentInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(PropertyComponentInterface* comp);

        bool configureComponent(const std::string& filename, PropertyComponentInterface* target);

        /**
         * Return a bag with the properties of a Component or an
         * empty bag if no such component is present.
         */
        PropertyBag getComponentProperties(const std::string& comp_name);
    protected:
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        virtual bool exportProperties( ORO_Execution::AttributeRepository& bag );
        
        virtual ORO_Execution::MethodFactoryInterface* createMethodFactory();
#endif

        bool readAllProperties();
        bool readProperties( const std::string& compname );
        bool writeAllProperties();
        bool writeProperties( const std::string& compname );

        /**
         * Read the XML cpf files and try to configure all the components.
         */
        virtual bool initialize();

        virtual void step();

        /**
         * If needed, save the components properties.
         */
        virtual void finalize();

        private:
        /**
         * Mapping of Component name to component instances.
         */
        typedef std::map<std::string, PropertyComponentInterface*> CompMap;
        
        /**
         * A vector of Component names.
         */
        typedef std::vector< Property<std::string>* >              CompNames;
        
        /**
         * A mapping of every registered components name to its instance.
         */
        CompMap     myMap;
        /**
         * A list of all the component filenames.
         */
        CompNames   componentFileNames;
        
        /**
         * Property. True if components properties must be saved afterwards.
         */
        Property<bool> save_props;

        /**
         * Set to true if a component needs to be configured when loaded,
         * Set to false if a component needs to be configured when the kernel is started.
         */
        Property<bool> configureOnLoad;

        /**
         * Set to true to ignore missing property files. If set to false (default),
         * each missing property file will be replaced by an empty property bag upon
         * configuration, allowing an updateProperties() to fail ( and thus not start
         * the ControlKernel).
         */
        Property<bool> ignoreMissingFiles;

        Property<std::string> saveFilePrefix;
        Property<std::string> saveFileExtension;
        /**
         * The base kernel of this extension.
         */
        ControlKernelInterface* base;
    };
}        
#endif
