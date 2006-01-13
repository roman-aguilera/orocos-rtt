#ifndef EVENT_SERVICE_HPP
#define EVENT_SERVICE_HPP

#include "TemplateEventFactory.hpp"
#include "EventC.hpp"
#include "ConnectionC.hpp"

namespace ORO_Execution
{

    namespace detail {

        /**
         * Factory interface creating intermediate EventHookBase objects.
         */
        struct EventStubInterface
        {
            virtual ~EventStubInterface() {}
            virtual int arity() const = 0;
            virtual EventHookBase* createReceptor(const std::vector<DataSourceBase::shared_ptr>& args ) = 0;
            virtual DataSourceBase* createEmittor(const std::vector<DataSourceBase::shared_ptr>& args ) = 0;
        };

    
        template< class EventT>
        struct EventStub :
            public EventStubInterface
        {
            EventT* me;
            detail::TemplateFactoryPart<EventT, EventHookBase*>*  mrfact;
            detail::TemplateFactoryPart<EventT, DataSourceBase*>* mefact;
            EventStub( EventT* e,
                       detail::TemplateFactoryPart<EventT, EventHookBase*>* rfact,
                       detail::TemplateFactoryPart<EventT, DataSourceBase*>* efact)
                : me(e), mrfact(rfact), mefact(efact) {}
            virtual ~EventStub() {
                delete mrfact;
                delete mefact;
            }

            virtual int arity() const { return me->arity(); }

            virtual EventHookBase* createReceptor(const std::vector<DataSourceBase::shared_ptr>& args ) {
                return mrfact->produce( me, args );
            }

            virtual DataSourceBase* createEmittor(const std::vector<DataSourceBase::shared_ptr>& args ) {
                return mefact->produce( me, args );
            }
        };

    }

    class ExecutionEngine;

    /**
     * The function of this service is twofold.  As a factory and
     * event producer, it stores Events of any type and connects
     * SYN/ASYN Event handlers to them.  As an event consumer, it also
     * offers access to an EventProcessor which may process (external
     * or own) events for this service.
     */
    class EventService
    {
        typedef std::map<std::string, detail::EventStubInterface* > Factories;
        Factories fact;

        ExecutionEngine* eeproc;
        ORO_CoreLib::EventProcessor* eproc;
    public:
        /**
         * Create an EventService with an associated ExecutionEngine.
         * If you want the owner task of this object to process an event use
         * EventService::getEventProcessor() in the \a setup functions below.
         */
        EventService( ExecutionEngine* ee );

        /**
         * Create an EventService with an associated EventService.
         * This EventProcessor optional and defaults to the CompletionProcessor.
         * If you want the owner task of this object to process an event use
         * EventService::getEventProcessor() in the \a setup functions below.
         */
        EventService( ORO_CoreLib::EventProcessor* ep = 0 );

        ORO_CoreLib::EventProcessor* getEventProcessor();
            
        /**
         * Add an arbitrary Event to this Service.
         */
        template< class EventT>
        bool addEvent( const std::string& ename, EventT* e ) {
            if ( fact.count(ename) != 0 )
                return false;
            fact[ename] = new detail::EventStub<EventT>( e,
                                                         detail::EventHookFactoryGenerator<EventT>().receptor(),
                                                         detail::EventHookFactoryGenerator<EventT>().emittor() );
            return true;
        }

        /**
         * Query for the existence of an Event in this Service.
         */
        bool hasEvent(const std::string& ename) const;

        /**
         * Return the number of arguments a given event has.
         * @retval -1 The event does not exist.
         * @return The number of arguments (may be zero).
         */
        int arity(const std::string& name) const;            
        
        /**
         * Add an added Event from this Service.
         */
        bool removeEvent( const std::string& ename );
        
        ~EventService();

        /**
         * Setup a 'handle' to emit events with arguments.
         * Use this method as in
         @verbatim
         createEmit("EventName").arg(2.0).arg(1.0).emit();
         // or:
         EventC em = createEmit("EventName").arg(2.0).arg(1.0);
         em.emit();
         @endverbatim
         * Also variables or reference to variables may be given
         * within arg().
         * @see EventC
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
        */
        EventC setupEmit(const std::string& ename) const;

        /**
         * Setup a 'handle' to connect a \b synchronous callback to an event.
         * Use this method as in
         @verbatim
         Handle h = createSynConnection("EventName", &function ).arg( &a ).arg( &b ).handle();
         h.connect();
         @endverbatim
         * Only reference to variables may be given within arg(), to these variables,
         * the event data is written.
         * @see ConnectionC
         * @see Handle
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         * @throw non_lvalue_args_exception
        */
        ConnectionC setupSynConnection(const std::string& ename,
                                        boost::function<void(void)> func) const;

        /**
         * Create a 'handle' to connect a \b asynchronous callback to an event.
         * Use this method as in
         @verbatim
         EventProcessor = // e.g.: taskc.events();
         Handle h = createAsynConnection("EventName", &function, eproc ).arg( &a ).arg( &b ).handle();
         h.connect();
         @endverbatim
         * Only reference to variables may be given within arg(), to these variables,
         * the event data is written.
         * @see ConnectionC
         * @see Handle
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         * @throw non_lvalue_args_exception
        */
        ConnectionC setupAsynConnection(const std::string& ename,
                                         boost::function<void(void)> func,
                                         ORO_CoreLib::EventProcessor* ep) const;

        /**
         * Setup a synchronous Event handler which will set \a args and
         * then call \a func synchronously when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param func  A function object which will be called.
         * @param args  The arguments which will be set before \a func is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         */
        ORO_CoreLib::Handle setupSyn(const std::string& ename,
                                     boost::function<void(void)> func,          
                                     std::vector<DataSourceBase::shared_ptr> args ) const;
        
        /**
         * Setup a asynchronous Event handler which will set \a args and
         * call \a afunc asynchronously (in task \a t) when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param afunc  A function object which will be called.
         * @param args  The arguments which will be set before \a afunc is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         * @param t The task in which the \a args will be set and \a afunc will be called.
         * @param ep The EventProcessor in which the \a args will be set and \a afunc will be called.
         * @{
         */
        ORO_CoreLib::Handle setupAsyn(const std::string& ename,
                                      boost::function<void(void)> afunc,          
                                      const std::vector<DataSourceBase::shared_ptr>& args,
                                      ORO_CoreLib::TaskInterface* t) const;
        
        ORO_CoreLib::Handle setupAsyn(const std::string& ename,
                                      boost::function<void(void)> afunc,          
                                      const std::vector<DataSourceBase::shared_ptr>& args,
                                      ORO_CoreLib::EventProcessor* ep = ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor() ) const;
        //!@}
        
        /**
         * Setup a synchronous and asynchronous Event handler which will set \a args and
         * call \a sfunc synchronously, and call \a afunc asynchronously (in task \a t)
         * when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param sfunc  A function object which will be called synchronously
         * @param afunc  A function object which will be called asynchronously
         * @param args  The arguments which will be set before \a sfunc
         * \em and before \a afunc is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         * @param t The task in which the \a args will be set and \a afunc will be called.
         * @param ep The EventProcessor in which the \a args will be set and \a afunc will be called.
         * @{
         */
        ORO_CoreLib::Handle setupSynAsyn(const std::string& ename,
                                         boost::function<void(void)> sfunc,
                                         boost::function<void(void)> afunc,
                                         const std::vector<DataSourceBase::shared_ptr>& args,
                                         ORO_CoreLib::TaskInterface* t) const;

        ORO_CoreLib::Handle setupSynAsyn(const std::string& ename,
                                         boost::function<void(void)> sfunc,
                                         boost::function<void(void)> afunc,
                                         const std::vector<DataSourceBase::shared_ptr>& args,
                                         ORO_CoreLib::EventProcessor* ep = ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor() ) const;
        //! @}

        /**
         * Setup an Event::emit() invocation wrapped in a DataSource.
         * Call \a result.evaluate() to emit the event with the given \a args.
         *
         * @param ename The name of the previously added Event.
         * @param args  DataSources holding the values for each parameter of the event's emit().
         * They be read at the moment of emit().
         * @see DataSourceGenerator for creating the arguments from variables or plain literals.
         */
        DataSourceBase::shared_ptr setupEmit(const std::string& ename,const std::vector<DataSourceBase::shared_ptr>& args) const;

        /*
         * The Global EventService, in which global Events are stored.
         */
        //static EventService Global;
    };


}

#endif
