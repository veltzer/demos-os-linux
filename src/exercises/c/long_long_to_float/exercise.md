# Long Long to Float

Find the first integral long long number that cannot be accurately represented
as a floating point number.

*Bonus*
Can you improve the number of iterations? If yes, how? If not, why?

## Solution

It's harder than it looks to improve the number of iterations because for every number
greater than the first number where the conversion fails to be exact it does not
necessarily follow that the conversion will fail.

Take a loot at the improved solutions output to see why this fails.
What the failed improvement does find rather quickly is a SINGLE
long long number who's conversion to and from float is inaccurate.
