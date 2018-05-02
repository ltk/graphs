//
// graph_test.cpp
//

#define CATCH_CONFIG_MAIN

// in order to access private members, we have to defeat the safety
// mechanisms. This is acceptable for unit testing, but not much
// else. It is also not portable because it is not supported by the
// C++ spec, and therefore might not work with all compilers.
#define private public

#include "catch.hpp"
#include "graph.hpp"

using namespace std;

// forward declaration of helpers. read and learn from them!
Graph* buildGraphToClear();
Graph* mkgraph();
Node* find(Graph& graph, string label);

// Unit Tests

TEST_CASE("Graph: sanity check", "[sanity]") {
  REQUIRE(true);
}

TEST_CASE("Graph: edge set type", "[Edge.setType]") {
  Node* a = new Node("a");
  Node* b = new Node("b");
  Edge* ab = new Edge(*a, *b);
  ab->type = CROSS_EDGE;
  ab->setType(FORWARD_EDGE);
  REQUIRE(ab->type == FORWARD_EDGE);
  ab->setType(UNDISCOVERED_EDGE);
  REQUIRE(ab->type == UNDISCOVERED_EDGE);
}

TEST_CASE("Graph: clear node", "[Node.clear]") {
  Node* n = new Node("some node");
  Node* p = new Node("some other node");
  n->color = BLACK;
  n->discovery_time = 42;
  n->completion_time = 108;
  n->rank = 23;
  n->predecessor = p;
  n->clear();
  REQUIRE(n->color == WHITE);
  REQUIRE(n->discovery_time == -1);
  REQUIRE(n->completion_time == -1);
  REQUIRE(n->rank == -1);
  REQUIRE(n->predecessor == NULL);
}

TEST_CASE("Graph: clear graph", "[Graph.clear]") {
  // HINT: implement 'Node::clear' and 'Edge::setType', and then use
  // them to implement 'Graph::clear'.
  Graph* g = buildGraphToClear();
  g->clear();
  REQUIRE(g->clock == 0);
  for (auto it=g->nodes.begin(); it != g->nodes.end(); ++it) {
    Node* n = *it;
    REQUIRE(n->color == WHITE);
    REQUIRE(n->predecessor == NULL);
    REQUIRE(n->discovery_time == -1);
    REQUIRE(n->completion_time == -1);    
    REQUIRE(n->rank == -1);    
  }
  for (auto it=g->edges.begin(); it != g->edges.end(); ++it) {
    Edge* e = *it;
    REQUIRE(e->type == UNDISCOVERED_EDGE);
  }
}

TEST_CASE("Graph: set node color", "[Node.setColor]") {
  {
    Node* n = new Node("some node");
    n->color = BLACK;
    n->discovery_time = 10;
    n->completion_time = 11;
    n->setColor(WHITE, 18); // The 18 should be ignored
    REQUIRE(n->color == WHITE);
    REQUIRE(n->discovery_time == -1);
    REQUIRE(n->completion_time == -1);
  }
  {
    Node* n = new Node("some node");
    n->color = WHITE;
    n->discovery_time = -1;
    n->completion_time = -1;
    n->setColor(GRAY, 18);
    REQUIRE(n->color == GRAY);
    REQUIRE(n->discovery_time == 18);
    REQUIRE(n->completion_time == -1);
  }
  {
    Node* n = new Node("some node");
    n->color = GRAY;
    n->discovery_time = 10;
    n->completion_time = -1;
    n->setColor(BLACK, 18);
    REQUIRE(n->color == BLACK);
    REQUIRE(n->discovery_time == 10);
    REQUIRE(n->completion_time == 18);
  }  
}

TEST_CASE("Graph: node get disco info", "[Node.getDisco]") {
  Node* n = new Node("some node");
  n->color = GRAY;
  n->discovery_time = 10;
  n->completion_time = 18;
  n->rank = 4;
  int c, dt, ct, r;
  n->getDiscoveryInformation(c, dt, ct, r);
  REQUIRE(c == GRAY);
  REQUIRE(dt == 10);
  REQUIRE(ct == 18);
  REQUIRE(r == 4);
}

TEST_CASE("Graph: test if node is a spanning tree ancestor", "[Node.isAncestor]") {
  Node* a = new Node("a");
  Node* b = new Node("b");  
  Node* c = new Node("c");
  Node* e = new Node("e");
  a->predecessor = NULL;
  b->predecessor = a;
  c->predecessor = a;  
  e->predecessor = c;

  REQUIRE(a->isAncestor(*c) == false);
  REQUIRE(c->isAncestor(*a) == true);
  REQUIRE(e->isAncestor(*a) == true);
  REQUIRE(e->isAncestor(*b) == false);
}


TEST_CASE("Graph: set spanning tree predecessor", "[Node.setPredecessor]") {
  Node* a = new Node("a");
  Node* c = new Node("c");
  Node* e = new Node("e");
  c->setPredecessor(*a);
  e->setPredecessor(*c);

  REQUIRE(c->predecessor == a);
  REQUIRE(e->predecessor == c);
}

TEST_CASE("Graph: depth-first search, no target", "[Graph.dfs]") {
  Graph* g = mkgraph();
  g->clear();
  REQUIRE(g->clock == 0);
  Node* a = find(*g, "a");
  REQUIRE(a != NULL); // sanity check my find function
  g->dfs(*a);
  REQUIRE(g->clock > 0); // clock should increment beyond zero
  REQUIRE(a->color == BLACK); // start node should be fully explored
  REQUIRE(a->discovery_time == 0); // start node discovered at t=0
  REQUIRE(a->completion_time == 13); // should finish at exactly t=13
  int c, dt, ft, r;
  for (auto it=g->nodes.begin(); it != g->nodes.end(); ++it) {
    Node* n = *it;
    REQUIRE(n != NULL);
    n->getDiscoveryInformation(c, dt, ft, r);
    REQUIRE(c == BLACK); // all nodes should be fully explored
    REQUIRE(dt >= 0); // discovered between 0 and 6 inclusive;
    REQUIRE(dt <= 6);    
    REQUIRE(ft >= 7); // finish time between 7 and 13 inclusive
    REQUIRE(ft <= 13); 
  }
  Node* f = find(*g, "f");
  Node* d = find(*g, "d");
  REQUIRE(f->predecessor == d);

}

TEST_CASE("Graph: depth-first search edge types", "[Graph.dfs edges]") {
  Graph* g = mkgraph();
  g->clear();
  REQUIRE(g->clock == 0);
  Node* a = find(*g, "a");
  REQUIRE(a != NULL); // sanity check my find function
  g->dfs(*a); // run the DFS

  // declare vars to count number of each kind of edge
  int numFwd = 0;
  int numTree = 0;
  int numBack = 0;
  int numCross = 0;
  for (auto it=g->edges.begin(); it != g->edges.end(); ++it) {
    Edge* e = *it;
    REQUIRE(e != NULL);
    if (e->getType() == FORWARD_EDGE) {
      numFwd++;
    } else if (e->getType() == TREE_EDGE) {
      numTree++;
    } else if (e->getType() == BACK_EDGE) {
      numBack++;
    } else if (e->getType() == CROSS_EDGE) {
      numCross++;
    }
  }

  // The classification of edges depends on traversal order & can't be
  // predicted at write-time, so test sums and inequalities instead.
  REQUIRE(numTree + numFwd + numBack + numCross == 12);
  REQUIRE(numTree >= 6);
  REQUIRE(numFwd > 0);  
  REQUIRE(numBack > 0);  
  REQUIRE(numCross == 0);
}

TEST_CASE("Graph: breadth first search", "[Graph.bfs 1]") {
  Graph* graph = mkgraph();
  graph->clear();
  Node* a = find(*graph, "a");
  Node* b = find(*graph, "b");
  Node* c = find(*graph, "c");
  Node* d = find(*graph, "d");
  Node* e = find(*graph, "e");
  Node* f = find(*graph, "f");
  Node* g = find(*graph, "g");
  int color, dt, ft, rank;

  graph->bfs(*a);
  a->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 0);
  b->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 1);
  c->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 1);
  e->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 1);
  d->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 2);
  g->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 2);
  f->getDiscoveryInformation(color, dt, ft, rank);
  REQUIRE(rank == 3);
}

TEST_CASE("Graph: breadth first search with target", "[Graph.bfs 2]") {
  int color, dt, ft, rank;
  {
    Graph* graph = mkgraph();
    graph->clear();
    Node* c = find(*graph, "c");
    Node* f = find(*graph, "f");
    graph->bfs(*c, *f);
    f->getDiscoveryInformation(color, dt, ft, rank);
    REQUIRE(rank == 2);
  }
  {
    Graph* graph = mkgraph();
    graph->clear();
    Node* a = find(*graph, "a");
    Node* b = find(*graph, "b");
    Node* c = find(*graph, "c");
    Node* d = find(*graph, "d");
    Node* e = find(*graph, "e");
    Node* f = find(*graph, "f");
    Node* g = find(*graph, "g");
    graph->bfs(*c, *b);
    b->getDiscoveryInformation(color, dt, ft, rank);
    REQUIRE(rank == 3); // rank is 3 if graph is directed
  }
  {
    Graph* graph = mkgraph();
    graph->setDirected(false);
    graph->clear();
    Node* a = find(*graph, "a");
    Node* b = find(*graph, "b");
    Node* c = find(*graph, "c");
    Node* d = find(*graph, "d");
    Node* e = find(*graph, "e");
    Node* f = find(*graph, "f");
    Node* g = find(*graph, "g");
    graph->bfs(*c, *b);
    b->getDiscoveryInformation(color, dt, ft, rank);
    REQUIRE(rank == 1); // rank is 1 if graph is undirected
  }
  
}

TEST_CASE("Graph: thing", "[thing]") {

}


// ---------------------------------------------------------------- Helpers ---

Graph* buildGraphToClear() {
  // make a Graph with wonky metadata. it is intended test clear().
  Graph* g = new Graph();
  g->clock = 42;
  
  Node* a = new Node("a");
  a->color = GRAY;
  a->discovery_time = 1;
  a->completion_time = 4;
  a->rank = 0;
  Node* b = new Node("b");
  b->color = BLACK;
  b->discovery_time = 2;
  b->completion_time = 5;
  b->rank = 1;
  Node* c = new Node("c");
  c->color = WHITE;
  c->discovery_time = 3;
  c->completion_time = 6;
  c->rank=-1;

  Edge* ab = new Edge(*a, *b);
  ab->type = TREE_EDGE;
  Edge* bc = new Edge(*b, *c);
  bc->type = CROSS_EDGE;
  Edge* ac = new Edge(*a, *c);
  bc->type = BACK_EDGE;

  a->predecessor = NULL;
  b->predecessor = a;
  c->predecessor = b;
  
  g->addNode(*a);
  g->addNode(*b);
  g->addNode(*c);
  g->addEdge(*ab);
  g->addEdge(*bc);
  g->addEdge(*ac);

  return g;
}

Graph* mkgraph() {
  Graph* ret = new Graph();
  Node* a = new Node("a");
  Node* b = new Node("b");
  Node* c = new Node("c");
  Node* d = new Node("d");
  Node* e = new Node("e");
  Node* f = new Node("f");
  Node* g = new Node("g");

  Edge* ab = new Edge(*a, *b);
  Edge* ac = new Edge(*a, *c);
  Edge* bc = new Edge(*b, *c);
  Edge* ae = new Edge(*a, *e);
  Edge* ce = new Edge(*c, *e);
  Edge* cd = new Edge(*c, *d);
  Edge* dg = new Edge(*d, *g);
  Edge* eg = new Edge(*e, *g);
  Edge* df = new Edge(*d, *f);
  Edge* fb = new Edge(*f, *b);
  // if graph is undirected, these could cause trouble. see that it doesn't
  Edge* gd = new Edge(*g, *d);
  Edge* ge = new Edge(*g, *e);

  ret->addNode(*a);
  ret->addNode(*b);
  ret->addNode(*c);
  ret->addNode(*d);
  ret->addNode(*e);
  ret->addNode(*f);
  ret->addNode(*g);
  
  ret->addEdge(*ab);
  ret->addEdge(*ac);
  ret->addEdge(*bc);
  ret->addEdge(*ae);
  ret->addEdge(*ce);
  ret->addEdge(*cd);
  ret->addEdge(*dg);
  ret->addEdge(*eg);
  ret->addEdge(*df);
  ret->addEdge(*fb);
  ret->addEdge(*gd);
  ret->addEdge(*ge);

  ret->setDirected(true);

  return ret;
}

Node* find(Graph& graph, string label) {
  for (auto it=graph.nodes.begin(); it != graph.nodes.end(); ++it) {
    Node* n = *it;
    if (n->data == label) {
      return n;
    }
  }
  return NULL;
}

