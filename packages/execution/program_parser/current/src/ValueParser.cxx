/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ValueParser.cxx

                        ValueParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
#include "execution/ValueParser.hpp"

#include "execution/ParsedValue.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace ORO_Execution
{
  using boost::bind;

    ValueParser::ValueParser( ValueRepository& repos)
        : _repos(repos)
  {
    BOOST_SPIRIT_DEBUG_RULE( constant );
    BOOST_SPIRIT_DEBUG_RULE( const_double );
    BOOST_SPIRIT_DEBUG_RULE( const_int );
    BOOST_SPIRIT_DEBUG_RULE( const_bool );
    BOOST_SPIRIT_DEBUG_RULE( const_string );
    BOOST_SPIRIT_DEBUG_RULE( named_constant );

    // note the order is important: commonparser.identifier throws a
    // useful "cannot use x as identifier" error if it fails, so we
    // must first show all non-identifier rules.
    constant =
        const_double
      | const_int
      | const_bool
      | const_string
      | named_constant;

    const_double =
      strict_real_p [
        bind( &ValueParser::seendoubleconstant, this, _1 ) ];
    const_int =
      int_p [
        bind( &ValueParser::seenintconstant, this, _1 ) ];
    const_bool =
      ( str_p( "true" ) | "false" )[
        bind( &ValueParser::seenboolconstant, this, _1, _2 ) ];
    const_string = confix_p(
      ch_p( '"' ), *(
        c_escape_ch_p[ bind( &ValueParser::push_str_char, this, _1 ) ]
        ), '"' )[ bind( &ValueParser::seenstring, this ) ];
    named_constant =
      ( "done" | commonparser.identifier ) [
        bind( &ValueParser::seennamedconstant, this, _1, _2 ) ];
  }

  void ValueParser::seenboolconstant( iter_t begin, iter_t end )
  {
    std::string value( begin, end );
    assert( value == "true" || value == "false" );
    if ( value == "true" )
      ret = new ParsedAliasValue<bool>(
        new VariableDataSource<bool>( true ) );
    else
      ret = new ParsedAliasValue<bool>(
        new VariableDataSource<bool>( false ) );
    deleter.reset( ret );
  }

  void ValueParser::seennamedconstant( iter_t begin, iter_t end )
  {
    std::string name( begin, end );
    if ( !_repos.isDefined( name ) )
        throw_(begin, "Value " + name + " not defined.");
        //      throw parse_exception_undefined_value( name );
    else
      ret = _repos.getValue(name);
    deleter.reset( 0 );
  }

  void ValueParser::seenintconstant( int i )
  {
    ret = new ParsedAliasValue<int>( new VariableDataSource<int>( i ) );
    // make the new ParsedValue managed by the auto_ptr..
    deleter.reset( ret );
  }

  void ValueParser::seendoubleconstant( double i )
  {
    ret = new ParsedAliasValue<double>( new VariableDataSource<double>( i ) );
    // make the new ParsedValue managed by the auto_ptr..
    deleter.reset( ret );
  }

  ValueParser::~ValueParser()
  {
    clear();
  }

  void ValueParser::clear()
  {
    ret = 0;
    deleter.reset( 0 );
  }

  rule_t& ValueParser::parser()
  {
    return constant;
  }

  void ValueParser::push_str_char( char c )
  {
    mcurstring += c;
  }

  void ValueParser::seenstring()
  {
    // due to a problem in Boost.spirit, the '"' terminating a
    // string will be in mcurstring, and we don't want it, so we
    // remove it..
    mcurstring.erase( mcurstring.end() - 1 );
    ret = new ParsedAliasValue<std::string>(
      new VariableDataSource<std::string>( mcurstring ) );
    deleter.reset( ret );
    mcurstring.clear();
  }
}
