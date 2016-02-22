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
    friend std::ostream& operator<<(std::ostream&, const Scheduler&);
  public:

    // (plain, key, plain_pos, plain_len) => code
    using SchedulerType = std::function<int (char, const Key&, int, int)>;

    Scheduler(std::string name, SchedulerType schedule):name(name), schedule(schedule) {}

    std::vector<int> enc(const std::string &plaintext, const Key &k) const {
      std::vector<int> ciphercode;
      int pos = 0;
      for (char c : plaintext) {
        ciphercode.push_back(schedule(c, k, pos++, plaintext.size()));
      }
      return std::move(ciphercode);
    }

    std::string dec(const std::vector<int> &ciphercode, const Key &k) const {
      std::ostringstream out;
      for (int c : ciphercode) out << k.plain(c);
      return std::move(out.str());
    }

  private:
    std::string name;
    SchedulerType schedule;

  }; /* Scheduler */


  std::ostream& operator<<(std::ostream& o, const Scheduler &s) {o << s.name; return o;}

  static Scheduler mod_by_plain_position("Mod by Plaintext Position", [](char c, const Key &k, int pos, int) {
      return k.code(c, pos % slot_of(c));
    });



} /* cipher */

// generator of Scheduler
// randomly choose one scheduler
static auto gen_scheduler = cppqc::elements({
    cipher::mod_by_plain_position, // default scheduler
      // add new scheduler here
      });


#endif /* CRYPTO_PROJECT_1_YANG_LIU_SCHEDULER_0412 */
