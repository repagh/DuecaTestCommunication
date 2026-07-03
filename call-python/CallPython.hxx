/* ------------------------------------------------------------------   */
/*      item            : CallPython.hxx
        made by         : repa
        from template   : DuecaModuleTemplate.hxx (2026.05)
        date            : Fri Jul  3 15:24:43 2026
        category        : header file
        description     :
        changes         : Fri Jul  3 15:24:43 2026 first version
        language        : C++
        copyright       : (c)
*/

#pragma once

// This includes headers for the objects that are sent over the channels
#include "comm-objects.h"

// include the dueca common classes header
#include <dueca/dueca.h>

// include headers for functions/classes you need in the module
#include <boost/python.hpp>
namespace bpy = boost::python;


/** A test module, checking the collaboration between Python scripting and
    C++ DUECA.

    The instructions to create an module of this class from the start
    script are:

    \verbinclude call-python.scm
 */
class CallPython: public dueca::Module
{
  /** self-define the module type, to ease writing the parameter table */
  typedef CallPython _ThisModule_;

private: // simulation data
  /// Flag to remember python set-up
  bool python_linked;

  /// Python identy
  bpy::object helper;

public:
  /// Python dict with the current variable settings
  /// The dict is created in the ModuleCreation call; that is after the
  /// dueca_mod.py script has been read and processed.
  /// If correct, gil is held.
  bpy::dict confirmed_settings;

private: // channel access

private: // activity allocation
  /** You might also need a clock. Don't mis-use this, because it is
      generally better to trigger on the incoming channels */
  dueca::PeriodicAlarm        myclock;

  /** Callback object for simulation calculation. */
  dueca::Callback<CallPython>  cb1;

  /** Activity for simulation calculation. */
  dueca::ActivityCallback      do_calc;

public: // class name and trim/parameter tables
  /** Name of the module. */
  static const char* const           classname;

  /** Return the parameter table. */
  static const dueca::ParameterTable*       getMyParameterTable();

public: // construction and further specification
  /** Constructor. Is normally called from scheme/the creation script. */
  CallPython(dueca::Entity* e, const char* part, const dueca::PrioritySpec& ts);

  /** Continued construction. This is called after all script
      parameters have been read and filled in, according to the
      parameter table. Your running environment, e.g. for OpenGL
      drawing, is also prepared. Any lengthy initialisations (like
      reading the 4 GB of wind tables) should be done here.
      Return false if something in the parameters is wrong (by
      the way, it would help if you printed what!) May be deleted. */
  bool complete();

  /** Destructor. */
  ~CallPython();

  // add here the member functions you want to be called with further
  // parameters. These are then also added in the parameter table
  // The most common one (addition of time spec) is given here.
  // Delete if not needed!

  /** Specify a time specification for the simulation activity. */
  bool setTimeSpec(const dueca::TimeSpec& ts);

  /** Request check on the timing. */
  bool checkTiming(const std::vector<int>& i);

public: // member functions for cooperation with DUECA
  /** indicate that everything is ready. */
  bool isPrepared();

  /** start responsiveness to input data. */
  void startModule(const dueca::TimeSpec &time);

  /** stop responsiveness to input data. */
  void stopModule(const dueca::TimeSpec &time);

public: // the member functions that are called for activities
  /** the method that implements the main calculation. */
  void doCalculation(const dueca::TimeSpec& ts);
};
