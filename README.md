# Graph Homework README

## Go in order

I recommend that you implement the functions in the same order that
they appear in the `points.json` file. This is the same order reported
by `python grade.py graph_test`.

## Pseudocode

This is the pseudocode to get you going. You'll need to augment it
(e.g. by setting ancestors, incrementing the clock, classifying edges,
and so on).

```
bfs(start):
  clear graph
  mark start gray
  Q = empty queue
  add start to Q
  while Q has stuff in it:
    v = popped element from Q
    mark v black
    visit v
    for each unmarked neighbor w
        mark w gray
        add w to Q
```

```
# clear graph before initial call
dfs(node): 
  mark node gray
  visit node
  E = edges related to node
  for all edges e in E:
    a = end of e that isn't node
    if a is white:
      dfs(a)
  mark node black
```

## Two versions of bfs

There are two signatures for bfs. I recommend that you implement the
two-arg version fully, and then have the one-arg version simply call
the two-arg version with a target that doesn't exist in the graph.

Remember: laziness is one of the three virtues of a great programmer.

## Use a queue, maybe

The header file includes vectors to store nodes and edges during a
search. You can use these if you like, or ignore them and use
something else instead.

In my implementation I did an interative BFS using a queue (so,
`#include <queue>` atop implementation file), and a recursive DFS
using the `getAdjacentEdges` set.

## The auto keyword

The 'auto' keyword can make life easier when iterating through a
collection. My solution is littered with code like the following:

```
for (auto it=g->nodes.begin(); it != g->nodes.end(); ++it) {
  Node* n = *it;
    
}
```

(and I do mean _littered_, it just looks awful)

## Use `make_dot`

Check out the `tick` and `make_dot` functions that is provided for you
in the implementation file. If you call `tick` after each interesting
event (start, end, and every time you increment the clock) you can use
this to watch the progress of your dfs or bfs.

## Make helper functions

You'll likely want to create and use helper functions. The details are
up to you. For example, I made a `otherEnd` function that returned the
start/end of an edge that wasn't the node I'm looking at currently.

Helpers aren't strictly necessary but they will make your life easier
and reduce the volume of code where bugs can lurk.
