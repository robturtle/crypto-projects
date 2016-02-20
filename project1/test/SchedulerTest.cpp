#include "testing.hpp"
#include "Key.hpp"
#include "Scheduler.hpp"
using namespace std;
using namespace cppqc;
using namespace cipher;

// The scheduling algorithm is a deterministic algorithm (that is, it does not use new random bits)
// that may depend on:
// 1. order of the letter within the ciphertext
// 2. length of ciphertext
// 3. the length of the list on that row (slot number)
//
// NOTE the ciphertext above is refering to a single word


/********************************************************************************
* Generator of Key
********************************************************************************/
namespace cipher {
  ostream& operator<<(ostream& o, const Scheduler &s) {
    o << s.name;
    return o;
  }
}

// randomly choose one scheduler
static auto schedulerGen = elements({
    mod_by_cipher_position, // default scheduler
      // add new scheduler here
      });

struct SchedulerProperty: Property<Scheduler, Key, string> {
  SchedulerProperty(): Property(schedulerGen,
                                Arbitrary<Key>(),
                                // pick one dictionary, and pick one word from it
                                elements(elements(dictionaries)));
};


/********************************************************************************
* Properties
********************************************************************************/

// Dec(Enc(m)) = m
struct SchedulerInvertCorrectly: SchedulerProperty {
  bool check(const Scheduler &s, const Key &k, const string &m) {
    return s.dec(s.enc(m, k), k) == m;
  }
};
