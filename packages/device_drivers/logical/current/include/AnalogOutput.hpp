#ifndef ANALOGOUTPUT_HPP
#define ANALOGOUTPUT_HPP

#include <device_interface/AnalogOutInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an analog output channel.
     *
     * @param OutputType The type (int, unsigned int, double,...)
     * in which data is sent to the board.
     */
    template< class OutputType >
    class AnalogOutput
    {
    public:
        /**
         * Create an analog output object to read the state of a channel.
         *
         * @param ana_out     The analog output device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         */
        AnalogOutput( AnalogOutInterface<OutputType>* ana_out, unsigned int channel_nr )
            :board(ana_out), channel(channel_nr)
        {
        }

        /**
         * Destruct the AnalogOutput.
         */
        ~AnalogOutput() {};

        /**
         * Write the value of this channel.
         */
        void value(double v)
        {
            d_cache = v;
            i_cache = board->binaryLowest() + OutputType( ( v - board->lowest(channel) ) * board->resolution(channel) );
            board->write(channel, i_cache);
        }

        /**
         * Write the raw value of this channel.
         */
        void rawValue(OutputType i)
        {
            i_cache = i;
            board->write(channel, i);
        }

        /**
         * Read the value of this channel.
         */
        double value()
        {
             return d_cache;
        }

        /**
         * Read the raw value of this channel.
         */
        OutputType rawValue()
        {
            return i_cache;
        }

        /**
         * Return the highest output.
         */
        double highest()
        {
            return board->highest(channel);
        }

        /**
         * Return the lowest output.
         */
        double lowest()
        {
            return board->lowest(channel);
        }

    private:
        AnalogOutInterface<OutputType> *board;
        int channel;
        double d_cache;
        OutputType i_cache;
    };
}

#endif // ANALOGOUTPUT_HPP
