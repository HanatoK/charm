/*****************************************************************************
 * $Source$
 * $Author$
 * $Date$
 * $Revision$
 *****************************************************************************/

/**
 * \addtogroup CkPerf
*/
/*@{*/

#ifndef __trace_counter_h__
#define __trace_counter_h__

#include <stdio.h>
#include <errno.h>
#include "trace.h"
#include "ck.h"
#include "trace-common.h"
#include "conv-mach.h"
#ifdef CMK_ORIGIN2000
#include <sys/hwperftypes.h>
#endif

#define MAX_ENTRIES 500

//! track statistics for all entry points
class StatTable {
  public:
    StatTable(char** name, char** desc, int argc);
    ~StatTable();
    //! one entry is called for 'time' seconds, value is counter reading 
    void setEp(int epidx, int stat, long long value, double time);
    //! write three lines for each stat:
    //!   1. number of calls for each entry
    //!   2. average count for each entry
    //!   3. total time in us spent for each entry
    void write(FILE* fp);
    void clear();
    inline int numStats() { return numStats_; }

  private:
    //! struct to maintain statistics
    struct Statistics {
      char*  name;                    // name of stat being tracked
      char*  desc;                    // description of stat being tracked
      UInt   numCalled[MAX_ENTRIES];  // total number times called
      double avgCount[MAX_ENTRIES];   // track average of value
      double totTime[MAX_ENTRIES];    // total time associated with counter

      Statistics(): name(NULL) { }
    };

    Statistics* stats_;             // track stats for each entry point
    int         numStats_;          // size of statistics being tracked
    // bool        error_;             // will be set to true if error writing info
};

// counter log pool
class CountLogPool {
  public:
    CountLogPool(char* pgm, char** name, char** desc, int argc);
    ~CountLogPool();
    void write(int phase=-1) ;
    void writeSts(int phase=-1);
    void openFile(int phase=-1);
    void setEp(int epidx, long long count1, long long count2, double time);
    void clearEps() { stats_.clear(); }
    void setTrace(bool on) { traceOn_ = on; }

  private:
    char*     pgm_;
    FILE*     fp_;
    StatTable stats_;
    bool      traceOn_;
};

//! For each processor, TraceCounter calculates mean, stdev, etc of 
//! CPU performance counters for each entry point.
class TraceCounter : public Trace {
  public:
    TraceCounter();
    ~TraceCounter();
    void traceBegin();
    void traceEnd();
    int traceRegisterUserEvent(const char* userEvent) { 
      // CmiPrintf("%d/%d traceRegisterUserEvent(%s)\n", 
      // CkMyPe(), CkNumPes(), userEvent);
      return 0;
    }
    void userEvent(int e) { 
      // CmiPrintf("%d/%d userEvent %d\n", CkMyPe(), CkNumPes(), e); 
    }
    void creation(envelope *e, int num=1) { }
    void messageRecv(char *env, int pe) { }
    void beginExecute(envelope *e);
    void beginExecute(int event, int msgType, int ep, int srcPe, int mlen=0);
    void endExecute(void);
    void beginIdle(void) { }
    void endIdle(void) { }
    void beginPack(void);
    void endPack(void);
    void beginUnpack(void);
    void endUnpack(void);
    void enqueue(envelope *e) { }
    void dequeue(envelope *e) { }
    void beginComputation(void);
    void endComputation(void) { }

    void traceInit(char **argv);
    void traceClearEps();
    void traceWriteSts();
    void traceClose();

    //! CounterArg is a linked list of strings that allows
    //! processing of command line args
    struct CounterArg {
      int         code;
      char*       arg;
      char*       desc;
      CounterArg* next;

      CounterArg(): code(-1), arg(NULL), desc(NULL), next(NULL) { }
      CounterArg(int c, char* a, char* d):
        code(c), arg(a), desc(d), next(NULL) { }
      void setValues(int _code, char* _arg, char* _desc) {
        code = _code;  arg = _arg;  desc = _desc;
      }
    };

  private:
    int         execEP_;       // id currently executing entry point
    double      startEP_;      // start time of currently executing ep
    double      startPack_;    // start time of pack operation
    double      startUnpack_;  // start time of unpack operation
    CounterArg* firstArg_;     // pointer to start of linked list of args
    CounterArg* lastArg_;      // pointer to end of linked list of args
    int         argStrSize_;   // size of maximum arg string (formatted output)
    int         phase_;        // current phase
    bool        traceOn_;      // true if trace is turned on

    int         counter1_;     // 1st counter to track
    int         counter2_;     // 2nd counter to track

    #ifdef CMK_ORIGIN2000
    enum Status { IDLE, WORKING };
    Status      status_;       // to prevent errors
    int         genStart_;     // track value of start_counters
    int         fileDesc_;     // file descriptor for accessing hw counters
    hwperf_profevctrarg_t evctrArgs_;  // tells what type of counter to profile
    hwperf_cntr_t         cnts_;       // reads the counters values
    // starts hw profiling for these counters
    // returns -1 if error, generation number if no error
    int startCounter(int counter1, int counter2);
    // gets hw profile values for these counters
    // returns -1 if error, generation number if no error
    int readCounters
    (
      int        counter1,
      long long* value1,
      int        counter2,
      long long* value2
    );
    #endif // CMK_ORIGIN2000

    //! add the argument parameters to the linked list of args choices
    void registerArg(CounterArg* arg);
    //! see if the arg (str or code) matches any in the linked list of choices
    //! and sets arg->code to the SGI code
    //! return true if arg matches, false otherwise
    bool matchArg(CounterArg* arg);
    //! print out all arguments in the linked-list of choices
    void printHelp();
};

#endif  // __trace_counter_h__

/*@}*/


