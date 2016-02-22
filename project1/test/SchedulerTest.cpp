#include "config.hpp"
#include "setup.hpp"
#include "testing.hpp"
#include "Key.hpp"
#include "Scheduler.hpp"
using namespace std;
using namespace cppqc;
using namespace cipher;

// The scheduling algorithm is a deterministic algorithm (that is, it does not use new random bits)
// that may depend on:
// 1. order of the letter within the plaintext
// 2. length of plaintext
// 3. the length of the list on that row (slot number)
//
// NOTE the plaintext above is refering to a single word

/********************************************************************************
* Data
********************************************************************************/
static vector<vector<string>> dictionaries = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));


/********************************************************************************
* Generator of Scheduler
********************************************************************************/
struct SchedulerProperty: Property<Scheduler, Key, string> {
  SchedulerProperty(): Property(gen_scheduler,
                                Arbitrary<Key>(),
                                // pick one dictionary, and pick one word from it
                                sample_of<vector<vector<string>>, string>(dictionaries)) {}
};


/********************************************************************************
* Properties
********************************************************************************/

// Dec(Enc(m)) = m
struct Correctness: SchedulerProperty {
  bool check(const Scheduler &s, const Key &k, const string &m) const override {
    return s.dec(s.enc(m, k), k) == m;
  }
};
RUN_QUICK_CHECK(ScheduleTest, Correctness)
