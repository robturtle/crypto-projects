#ifndef CRYPTO_PROJECT_1_YANG_LIU_SCHEDULER_0412
#define CRYPTO_PROJECT_1_YANG_LIU_SCHEDULER_0412
#include "Key.hpp"
#include <vector>
#include <string>
#include <algorithm>

// The scheduling algorithm is a deterministic algorithm (that is, it does not use new random bits)
// that may depend on:
// 1. order of the letter within the plaintext
// 2. length of plaintext
// 3. the length of the list on that row (slot number)
//

namespace cipher {

  class Scheduler {
  public:

    // (plain, key, plain_pos, plain_len) => code
    using SchedulerType = std::function<int (char, const Key&, int, int)>;

    std::string name;
    SchedulerType schedule;

    Scheduler(std::string name, SchedulerType schedule):name(name), schedule(schedule) {}

    char dec(int code, const Key &k) const {return k.plain(code);}

    std::vector<int> enc(const std::string &plaintext, const Key &k) const {
      std::vector<int> ciphercode;
      int pos = 0;
      std::for_each(plaintext.begin(), plaintext.end(), [&](char c) {
          ciphercode.push_back(schedule(c, k, pos++, plaintext.size()));
        });
      return std::move(ciphercode);
    }

    std::string dec(const std::vector<int> &ciphercode, const Key &k) const {
      std::ostringstream out;
      std::for_each(ciphercode.begin(), ciphercode.end(), [&](int c) {
          out << dec(c, k);
        });
      return std::move(out.str());
    }

  }; /* Scheduler */

  std::ostream& operator<<(std::ostream& o, const Scheduler &s) {o << s.name; return o;}

  static Scheduler::SchedulerType _s = [](char c, const Key &k, int pos, int) {
    return k.code(c, pos % slot_of(c));
  };
  static Scheduler mod_by_plain_position("Mod by Plaintext Position", _s);


} /* cipher */

// generator of Scheduler
// randomly choose one scheduler
static auto gen_scheduler = cppqc::elements({
    cipher::mod_by_plain_position, // default scheduler
      // add new scheduler here
      });


#endif /* CRYPTO_PROJECT_1_YANG_LIU_SCHEDULER_0412 */
