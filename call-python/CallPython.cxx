/* ------------------------------------------------------------------   */
/*      item            : CallPython.cxx
        made by         : repa
        from template   : DuecaModuleTemplate.cxx (2026.05)
        date            : Fri Jul  3 15:24:43 2026
        category        : body file
        description     :
        changes         : Fri Jul  3 15:24:43 2026 first version
        language        : C++
        copyright       : (c)
*/

#define CallPython_cxx

// include the definition of the module class
#include "CallPython.hxx"

// include additional files needed for your calculation here

// the standard package for DUSIME, including template source
#define DO_INSTANTIATE
#include <dueca/dueca.h>
using namespace dueca;

// include the debug writing header, by default, write warning and
// error messages
#define W_MOD
#define E_MOD
#include <dueca/debug.h>

/** Gil state helper */
class RunWithGIL
{
  PyGILState_STATE _state;

public:
    /// get GIL lock (or block)
  RunWithGIL() { _state = PyGILState_Ensure(); }
    /// Automatic release
  ~RunWithGIL() { PyGILState_Release(_state); }
  RunWithGIL(const RunWithGIL &) = delete;
  RunWithGIL &operator=(const RunWithGIL &) = delete;
};

// class/module name
const char *const CallPython::classname = "call-python";

// Parameters to be inserted
const ParameterTable *CallPython::getMyParameterTable()
{
  static const ParameterTable parameter_table[] = {
    { "set-timing",
      new MemberCall<_ThisModule_, TimeSpec>(&_ThisModule_::setTimeSpec),
      set_timing_description },

    { "check-timing",
      new MemberCall<_ThisModule_, std::vector<int>>(
        &_ThisModule_::checkTiming),
      check_timing_description },

    /* You can extend this table with labels and MemberCall or
       VarProbe pointers to perform calls or insert values into your
       class objects. Please also add a description (c-style string).

       Note that for efficiency, set_timing_description and
       check_timing_description are pointers to pre-defined strings,
       you can simply enter the descriptive strings in the table. */

    /* The table is closed off with NULL pointers for the variable
       name and MemberCall/VarProbe object. The description is used to
       give an overall description of the module. */
    { NULL, NULL, "please give a description of this module" }
  };

  return parameter_table;
}

// constructor
CallPython::CallPython(Entity *e, const char *part, const PrioritySpec &ps) :
  /* The following line initialises the SimulationModule base class.
     You always pass the pointer to the entity, give the classname and the
     part arguments. */
  Module(e, classname, part),

  // initialize the data you need in your simulation or process
  python_linked(false),
  helper(),
  confirmed_settings(),

  // create a clock
  myclock(),

  // a callback object, pointing to the main calculation function
  cb1(this, &_ThisModule_::doCalculation),
  // the module's main activity
  do_calc(getId(), "check comm with Python", &cb1, ps)
{
  // connect the triggers for simulation
  do_calc.setTrigger(myclock);
}

bool CallPython::complete()
{
  /* All your parameters have been set. You may do extended
     initialisation here. Return false if something is wrong. */

  if (!python_linked) {

    try {

    // obtain the python lock
      RunWithGIL gillock;

    // import the main python module, and find the python namespace
      bpy::object main_module = bpy::import("__main__");
      bpy::object main_namespace = main_module.attr("__dict__");

    // put this object in the dictionary
      bpy::object self(bpy::ptr(this));
      main_namespace["call_python"] = self;

    // create and link the helper
      bpy::exec("call_helper = CallHelper(call_python.confirmed_settings)",
                main_namespace, main_namespace);
      helper = main_namespace["call_helper"];

    // run the login sequence, login should be a member function by now
      bpy::object result = helper.attr("login")();
      if (!result) {
        E_MOD("Could not login/connnect to the helper class");
        return false;
      }
    }
    catch (const std::exception &e) {

      E_MOD("Cannot initialize python side of CallPython module: " << e.what());
      return false;
    }
  }
  return true;
}

// destructor
CallPython::~CallPython()
{
  RunWithGIL gillock;
  bpy::object res = helper.attr("logout")();
  if (!res) {
    W_MOD("Failed Moog device control logout");
  }
}

// as an example, the setTimeSpec function
bool CallPython::setTimeSpec(const TimeSpec &ts)
{
  // a time span of 0 is not acceptable
  if (ts.getValiditySpan() == 0)
    return false;

  // specify the timespec to the activity
  do_calc.setTimeSpec(ts);
  // or do this with the clock if you have it (don't do both!)
  myclock.changePeriodAndOffset(ts);

  // do whatever else you need to process this in your model
  // hint: ts.getDtInSeconds()

  // return true if everything is acceptable
  return true;
}

// the checkTiming function installs a check on the activity/activities
// of the module
bool CallPython::checkTiming(const std::vector<int> &i)
{
  if (i.size() == 3) {
    new TimingCheck(do_calc, i[0], i[1], i[2]);
  }
  else if (i.size() == 2) {
    new TimingCheck(do_calc, i[0], i[1]);
  }
  else {
    return false;
  }
  return true;
}

// tell DUECA you are prepared
bool CallPython::isPrepared()
{
  bool res = true;

  // Example checking a token:
  // CHECK_TOKEN(w_somedata);

  // Example checking anything
  // CHECK_CONDITION(myfile.good());
  // CHECK_CONDITION2(sometest, "some test failed");

  // return result of checks
  return res;
}

// start the module
void CallPython::startModule(const TimeSpec &time) { do_calc.switchOn(time); }

// stop the module
void CallPython::stopModule(const TimeSpec &time) { do_calc.switchOff(time); }

// this routine contains the main simulation process of your module. You
// should read the input channels here, and calculate and write the
// appropriate output
void CallPython::doCalculation(const TimeSpec &ts)
{
  // get python lock
  RunWithGIL gillock;

  bpy::object res = helper.attr("checkup")();
  if (!res) {
    W_MOD("checkup failed");
  }
}

#if defined(SCRIPT_PYTHON)
// this contains the interface to the scripting language
#include <dueca/ScriptInterpret.hxx>
#include <memory>

static void startfunc()
{
  bpy::class_<CallPython, std::shared_ptr<CallPython>,
              boost::noncopyable>("CallPython", bpy::no_init)

    // expose the setting dictionary
    .def_readwrite("confirmed_settings", &CallPython::confirmed_settings);
}
static AddInitFunction addit("python link CallPython", startfunc);
#endif

// Make a TypeCreator object for this module, the TypeCreator
// will check in with the script code, and enable the
// creation of modules of this type
static TypeCreator<CallPython> a(CallPython::getMyParameterTable());
