# Longest increasing subsequence

## Definition: a substring
Given a string, a substring is a string where every letter is taken
from the original string and the letters appear in the order they appear
in the original string.

## Examples of a substring
'cat' is a substring of the string 'nice art'
'cent' is not a substring of the string 'nice art' since 'n' appears before the letter 'e'.

Definition: an increasing substring
Is a substring (according to the previous definition) where every characters value is greater
than the value of the preceding character.

## Examples of increasing substring
'cat' is not an increasing substring of 'nice art' since 'a' appears before 'c' in the English
alphabet and not vise versa.
'cet' is an increasing substring of 'nice art'.

## Definition: Longest increasing substring
Is the longest increasing substring of that string.

## Examples of a longest increasing substring
both 'cert' and 'bert' are the longest increasing substring of the string 'nicbe art'.
They are both of length 4. We will prefer the one that starts earlier in the original
string. In this case we this means we will prefer 'cert' since 'c' appears before 'b'
in the original string.

## The exercise

* Find the longest increasing substring of a given string. You may not use loops (for, do/while, while).
* Use only recursion.
* The recursive function must be of the following prototype:

```c
void maximum_subset(char str[], int index, char curr[], char best[]);
```

* Make the solution as simple as possible.

## References
* [definition](https://en.wikipedia.org/wiki/Longest_increasing_subsequence)
