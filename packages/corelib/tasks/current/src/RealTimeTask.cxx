#include "corelib/RealTimeTask.hpp"
#include "corelib/StandardEventListener.hpp"
#include "corelib/StandardEventCompleter.hpp"
#include "corelib/CompletionProcessor.hpp"
#include "os/MutexLock.hpp"

#include <cmath>

namespace ORO_CoreLib
{
    
    RealTimeTask::RealTimeTask(Seconds period, RunnableInterface* r )
        :evHandler( listener(&ORO_CoreLib::RealTimeTask::doStep, this) ),
         taskCompleter( completer(&ORO_CoreLib::RealTimeTask::doStop, this)), 
        runner(r), running(false), inError(false)
    {
        if (runner)
            runner->setTask(this);
        per_ns = nsecs( rint( period * secs_to_nsecs(1) ) );
    }

    RealTimeTask::RealTimeTask(secs s, nsecs ns, RunnableInterface* r )
        :evHandler( listener(&ORO_CoreLib::RealTimeTask::doStep,this) ),
         taskCompleter( completer(&ORO_CoreLib::RealTimeTask::doStop, this)), 
        runner(r), running(false), inError(false), per_ns( secs_to_nsecs(s) + ns)
    {
        if (runner)
            runner->setTask(this);
    }

    RealTimeTask::~RealTimeTask()
    {
        stop();
        if (runner)
            runner->setTask(0);
        delete evHandler;
        delete taskCompleter;
    }
     
    bool RealTimeTask::run( RunnableInterface* r )
    {
        if ( isRunning() )
            return false;
        if (runner)
            runner->setTask(0);
        runner = r;
        if (runner)
            runner->setTask(this);
        return true;
    }

    bool RealTimeTask::start()
    {
        if ( isRunning() ) return false;

        if (runner != 0)
            inError = !runner->initialize();
        else
            inError = !initialize();

        if ( !inError )
            running = taskAdd();

        return running;
    }

    bool RealTimeTask::stop()
    {
        if ( !isRunning() ) return false;

        doStop();
        
        return true;
    }

    void RealTimeTask::doStop()
    {
        ORO_OS::MutexTryLock locker(stop_lock);
        if ( !locker.isSuccessful() )
            return; // stopping is in progress

        taskRemove();

        running = false;

        if (runner != 0)
            runner->finalize();
        else
            finalize();
    }

    bool RealTimeTask::isRunning()
    {
        return running;
    }

    Seconds RealTimeTask::periodGet()
    {
        return Seconds(per_ns) / (1000.0*1000.0*1000.0);
    }

    void RealTimeTask::doStep()
    {
        if ( isRunning() )
        {
            if (runner != 0)
                runner->step();
            else
                step();
        }
    }

}
