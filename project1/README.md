# Cryptanalysis of Permutation Ciphers

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
