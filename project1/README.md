# Cryptanalysis of Permutation Ciphers

## Test framework
First, build the project like the normal CMake project:

```
mkdir -p /some/where/to/build/
cd /some/where/to/build/
cmake /path/to/project1
make
```

Simply execute `./test/CodeBreakTest` to test the correctness of the implementation.
The test suite will:
- generate a random key
- select one scheduling algorithm from all pre-defined ones (currently only the "Mod by Plaintext Position" is valid)
- randomly pick one dicionary from 5, and randomly pick some words to build up the plaintext
- encrypt the plaintext to yield ciphertext (numbers separated by ',', words separated by ' ')
- execute `CodeBreaker().solve(ciphertext)` and see whether the inverted text is equal to plaintext

Once the test failed, a detailed message will be output like this:

![](resources/code-breaker-test.png)

To make the test passed, go to `src/CodeBreaker.cpp` and implement the `_solve()` method correctly:
``` cpp
vector<string> _solve(vector<string> ciphers);
```
It accepts an vector of cipher (example: ["118,58,23", "78,29,4,102,112,25"]), and it should crack
the cipher and return the corresponding plaintext word as an vector. (NOTE: the order of each element must be
preserved).

### Facilities
Dictionaries are stored in `cipher::dictionaries` as `vector<vector<string>`, each element represents one line
of words in the `plaintext_dictionaries.txt`.

Function `vector<string> split(content, separator)` can be used to split words.

Function `string join(vector<T>, separator)` can be used to join an vector into a single string.

Once a test is passed, a csv record will be logged into `resources/break.log` with fields:
"break time in seconds", "plaintext", and "scheduling algorithm". This can be used for further
analysis.

## Observations

The [dictionary](resources/plaintext_dictionary.txt) is now published. From the contents we can tell
that the size of the dictionary is pretty small. And since each word is independent. We could try to
start it from the easiest one, i.e. shortest word.

``` python
def parse_input(ciphertext):
    words = ciphertext.split().sort_by_len()
    for word in words:
      break(word) # break word and determine partial mapping from it
```

### Mod by Plaintext Position Scheduling Algorithm
It's obviously that the heading elements in the list for each letter is more likely appear in the ciphertext.
That makes the shortest word to longest strategy more efficient.

## Iteration
The search space in the dictionary is very small. So let's just do Depth-First Search to find the key.

P.S. I tried the excercise in the KL book which request you break a mono substitution cipher where there's
no space serparating words. That search space is so huge that the DFS and BFS just not worked. In that case,
a randomized iterating method should be better.

``` python
def break(word):
    # placeholder for recursion base here
    candidates = select from dictionary where plaintext.len == word.len
    for candidate in candidates:
        make each letter in candidate map to each integer in word
        break(next word) # if all words can be mapped correctly, job done; otherwise trace back to this for loop
        revoke the mapping of this candidate
```

## Potential Additional Work
Some guess of the mechanism of the scheduler might be helpful. And we need to figure out some scheduling method
to complete the test suites. But let's just put this analysis after the first implementation.
