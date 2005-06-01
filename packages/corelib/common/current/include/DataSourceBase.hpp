
#ifndef CORELIB_DATASOURCE_BASE_HPP
#define CORELIB_DATASOURCE_BASE_HPP

#include <boost/intrusive_ptr.hpp>
#include <map>
#include <string>

namespace ORO_CoreLib
{
  /**
   * @brief The base class for all DataSource's
   *
   * The DataSource is an object containing Data of any type. It's
   * interface is designed for dynamic build-up and destruction of
   * these objects and allowing Commands, Properties etc to use them
   * as 'storage' devices which have the dual copy()/clone() semantics
   * (which is heavily used by the Orocos Task Infrastructure).
   *
   * DataSource's are reference counted.  Use
   * DataSourceBase::shared_ptr or DataSource<T>::shared_ptr to deal
   * with this automatically, or don't forget to call ref and deref..
   *
   * @see DataSource
   */
  class DataSourceBase
  {
      /**
         We keep the refcount ourselves.  We aren't using
         boost::shared_ptr, because boost::intrusive_ptr is better,
         exactly because it can be used with refcounts that are stored
         in the class itself.  Advantages are that the shared_ptr's for
         derived classes use the same refcount, which is of course very
         much desired, and that refcounting happens in an efficient way,
         which is also nice :)
      */
    int refcount;
  protected:
      /** the destructor is private.  You are not allowed to delete this
       * class yourself, use a shared pointer !
       */
    virtual ~DataSourceBase();
  public:
      /**
       * Use this type to store a pointer to a DataSourceBase.
       */
      typedef boost::intrusive_ptr<DataSourceBase> shared_ptr;

      DataSourceBase() : refcount( 0 ) {};
      /**
       * Increase the reference count by one.
       */
      void ref() { ++refcount; };
      /**
       * Decrease the reference count by one and delete this on zero.
       */
      void deref() { if ( --refcount <= 0 ) delete this; };

      /**
       * Reset the data to initial values.
       */
      virtual void reset();

      /**
       * Force an evaluation of the DataSourceBase.
       */
      virtual void evaluate() const = 0;
      /**
       * Create a deep copy of this DataSource, unless it is already cloned and place the association (parent, clone) in \a alreadyCloned.
       */
      virtual DataSourceBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;

      /**
       * Return usefull type info in a human readable format.
       */
      virtual std::string getType() const = 0;
  };
}

void intrusive_ptr_add_ref( ORO_CoreLib::DataSourceBase* p );
void intrusive_ptr_release( ORO_CoreLib::DataSourceBase* p );

#endif
