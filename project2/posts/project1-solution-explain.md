### What is this solution?

1. A optimized brute force algorithm;
2. The analysis are all focus on the dictionary but not scheduling algorithm;
3. A modified Depth First Search algorithm, where:
   - depth = number of words
   - breadth of each leave = number of candidates in the dictionary of that word length

<!-- more -->

### Algorithm Analysis

**Average prune depth is the key.**

> simple calculation

For the simplicity of computation, consider a dictionary with n words, and of each word length
they all have m candidates.

1. Average case
2. Best case (Whenever the codebook is fully filled)

### Optimizations: Filling the codebook

> fill the most exclusive codes in advance;

1. words with very few candidates
2. words with less tolerance

### Further Improvement

> This algorithm is scalable

1. Parallel: multi-thread; adjust threads' nice value by evaluating the chance of fulfill the codebook
   in this subtree.
2. Randomness
