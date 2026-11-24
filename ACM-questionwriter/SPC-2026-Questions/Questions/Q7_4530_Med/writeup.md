# Grandma's 100th Birthday

## Problem Description

Your grandma made it to 100 years old! To celebrate this amazing accomplishment,
your dad went to the farmers market to buy grandma a bunch of cakes for her 
party. He picked up N long rectangular cakes, each with varying lengths.

Every person at the party, to be fair, will receive exactly one slice, and all
cake sizes need to be exactly the same length. You are tasked to create all the 
fair slices. You are not allowed to combine any pieces of different cakes 
together. Any leftover cake is thrown away.

There is one major problem though. Your dad doesn't have a knife, so he went and
bought one at the gas station and its plastic and cheap. This knife is fragile 
and will snap after exactly M cuts.

Given the lengths of the N cakes, the number of people attending grandmas party 
K, and the maximum number of cuts M the knife can withstand, determine the 
maximum size of each cake so that everyone at the party can get one equal slice.

If it is impossible to give everyone one equal slice, return 0

## Input

The first line has two space seperated integers:
- N - number of cakes
- K - number of people coming to party
- M - maximum number of cuts the knife can make

The second line had N space seperated integers c1, c2, c3, .... , cn

## Output

Print a single integer of the maximum size of the slice which allows every 
family member to have one equal slice. Return 0 if it is impossible.

## Sample 1

Input = 
```
3 4 2
10 12 10
```

Output = 5

Explaination - We need 4 slices, but we can only make 2 cuts.
If we target cake size of 5, we can cut the 1st and 3rd cake once and make 4
equal slices. (5, 5, 5, 5).

Total = 4 slices, enough for the 4 people coming to the party

## Sample 2

Input = 
```
4 6 3
17 12 13