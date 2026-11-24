# Tower building competition

## Problem Description

It's move-in day at FSU, and two freshman roommates, Kyle and Dylan, are locked in a fierce, 
completely unnecessary competition to see who can build the tallest tower out of their moving 
boxes before one of their parents tells them to stop.

Kyle piles his boxes into tower K, and Dylan piles his into tower D. Neither of them is being 
particularly strategic about it; they keep piling boxes on and then changing their minds and 
pulling them back off, all while arguing about whose side of the room is bigger.

You, the exhausted RA, are watching this unfold from the doorway with a clipboard. Your job is 
to log every PLACE and REMOVE command, and at the end of the chaos report the tallest each tower 
ever got and the longest streak each freshman achieved during the madness. These are needed for 
the inevitable argument at the end about who was winning. 

## Input

The first line contains an integer N, the number of commands.
The next N lines each contain one of two commands:
- PLACE t x - push a block of height x onto tower t (where t is K or D)
- REMOVE t - pop the top block off tower t (guaranteed non-empty when called)

## Output

Print 4 lines:
- Max height ever reached by tower K alone
- Max height ever reached by tower D alone
- Longest Streak ever held by Kyle alone
- Longest Streak ever held by Dylan alone

## Sample 1

Input = 
```
6
PLACE K  5
PLACE D 3
PLACE K 2
REMOVE B
PLACE D 8
REMOVE A
```

Output =
```
Max Height K: 7
Max Height D: 8
Longest Streak K: 2
Longest Streak D: 1
```

Explanation - 
- 6 (Number of commands = 6)
- PLACE K  5 (Max Height K  = 5; Longest Streak K  = 1)
- PLACE D 3 (Max Height D = 3; Longest Streak D = 1)
- PLACE K  2 (Max Height K  = 7; Longest Streak K  = 2)
- REMOVE D (Max Height D = 3; Longest Streak D = 1)
- PLACE D 8 (Max Height D = 8; Longest Streak D = 1)
- REMOVE K   (Max Height K  = 7; Longest Streak K  = 2)