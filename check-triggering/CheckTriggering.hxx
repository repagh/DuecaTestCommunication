/* ------------------------------------------------------------------   */
/*      item            : CheckTriggering.hxx
        made by         : repa
        from template   : DuecaModuleTemplate.hxx (2022.06)
        date            : Thu Nov  9 09:08:52 2023
        category        : header file
        description     :
        changes         : Thu Nov  9 09:08:52 2023 first version
        language        : C++
        copyright       : (c)
*/

#define CheckTriggering_hxx
#pragma once

// include the dusime header
#include <dueca.h>
USING_DUECA_NS;

// This includes headers for the objects that are sent over the channels
#include "comm-objects.h"

// include headers for functions/classes you need in the module

/** Channel triggering test module.

    The instructions to create an module of this class from the start
    script are:

    \verbinclude check-triggering.scm
 */
class CheckTriggering : public Module
{
  /** self-define the module type, to ease writing the parameter table */
  typedef CheckTriggering _ThisModule_;

private: // simulation data
  /// Latest trigger time for the regular sending
  DataTimeSpec ts_1_1;

  /// Number of mishaps
  unsigned nfault;

  /// counter for switching
  unsigned switchcounter;

private:
  /// @group Regular trigger and receive
  /// Stream, regular at ticker rate
  ChannelWriteToken w_s1_1;

  /// Event, regular at ticker rate
  ChannelWriteToken w_e1_1;

  /// Stream read, regular at ticker rate
  ChannelReadToken r_s1_1;

  /// Event read, regular at ticker rate
  ChannelReadToken r_e1_1;

  /// Stream read, regular at ticker rate, higher priority
  ChannelReadToken r_s1_1_pp;

  /// Event read, regular at ticker rate, higher priority
  ChannelReadToken r_e1_1_pp;

  /// Stream read, regular at ticker rate, lower priority
  ChannelReadToken r_s1_1_pm;

  /// Event read, regular at ticker rate, lower priority
  ChannelReadToken r_e1_1_pm;

  /// Stream, channel with multiple entries
  ChannelWriteToken w_sm_1;

  /// Stream, channel with multiple entries, second entry
  ChannelWriteToken w_sm_2;

  /// Stream, reader for channel with multiple entries
  ChannelReadToken r_sm;

  /// Callback
  Callback<_ThisModule_> cb0;

  /// Regular stream read check
  ActivityCallback do_1_1;

  /// Callback
  Callback<_ThisModule_> cb0b;

  /// Regular stream read check
  ActivityCallback do_1_sm;

  /// Callback
  Callback<_ThisModule_> cb1;

  /// Regular stream read check
  ActivityCallback do_s1_1;

  /// Callback
  Callback<_ThisModule_> cb2;

  /// Regular event read check
  ActivityCallback do_e1_1;

  /// Callback
  Callback<_ThisModule_> cb3;

  /// Regular stream read check, higher priority
  ActivityCallback do_s1_1_pp;

  /// Callback
  Callback<_ThisModule_> cb4;

  /// Regular event read check, higher priority
  ActivityCallback do_e1_1_pp;

  /// Callback
  Callback<_ThisModule_> cb5;

  /// Regular stream read check, lower priority
  ActivityCallback do_s1_1_pm;

  /// Callback
  Callback<_ThisModule_> cb6;

  /// Regular event read check, lower priority
  ActivityCallback do_e1_1_pm;

  /// Callback
  Callback<_ThisModule_> cb7;

  /// multiple entry trigger
  ActivityCallback do_sm;

  /// Callback
  Callback<_ThisModule_> cb8;

  /// multiple entry trigger
  ActivityCallback do_switchtrig;

private: // activity allocation
  /** You might also need a clock. Don't mis-use this, because it is
      generally better to trigger on the incoming channels */
  PeriodicAlarm myclock;

public: // class name and trim/parameter tables
  /** Name of the module. */
  static const char *const classname;

  /** Return the parameter table. */
  static const ParameterTable *getMyParameterTable();

public: // construction and further specification
  /** Constructor. Is normally called from scheme/the creation script. */
  CheckTriggering(Entity *e, const char *part, const PrioritySpec &ts);

  /** Continued construction. This is called after all script
      parameters have been read and filled in, according to the
      parameter table. Your running environment, e.g. for OpenGL
      drawing, is also prepared. Any lengthy initialisations (like
      reading the 4 GB of wind tables) should be done here.
      Return false if something in the parameters is wrong (by
      the way, it would help if you printed what!) May be deleted. */
  bool complete();

  /** Destructor. */
  ~CheckTriggering();

  // add here the member functions you want to be called with further
  // parameters. These are then also added in the parameter table
  // The most common one (addition of time spec) is given here.
  // Delete if not needed!

  /** Specify a time specification for the simulation activity. */
  bool setTimeSpec(const TimeSpec &ts);

  /** Request check on the timing. */
  bool checkTiming(const std::vector<int> &i);

public: // member functions for cooperation with DUECA
  /** indicate that everything is ready. */
  bool isPrepared();

  /** start responsiveness to input data. */
  void startModule(const TimeSpec &time);

  /** stop responsiveness to input data. */
  void stopModule(const TimeSpec &time);

public: // the member functions that are called for activities
  /** Send basic rate stream and event data. */
  void doSend_1_1(const TimeSpec &ts);

  /** Second send activity */
  void doSend_sm2(const TimeSpec &ts);

  /** Check stream coming in at regular priority. */
  void doCheck_s1_1(const TimeSpec &ts);

  /** Check event coming in at regular priority. */
  void doCheck_e1_1(const TimeSpec &ts);

  /** Check stream coming in at higher priority. */
  void doCheck_s1_1_pp(const TimeSpec &ts);

  /** Check event coming in at higher priority. */
  void doCheck_e1_1_pp(const TimeSpec &ts);

  /** Check stream coming in at lower priority. */
  void doCheck_s1_1_pm(const TimeSpec &ts);

  /** Check event coming in at lower priority. */
  void doCheck_e1_1_pm(const TimeSpec &ts);

  /** Check triggering on channel with multiple entries. */
  void doCheck_sm(const TimeSpec &ts);

  /** Check switching the trigger from channel to clock and back */
  void doCheck_switchtrigger(const TimeSpec &ts);
};
