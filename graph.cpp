//
// graph.cpp
//

#include <iostream>
#include <sstream>
#include <queue>

#include "graph.hpp"

using namespace std;

// fwd declarations
string make_dot(Graph* g);
string what(int& v);

Graph::Graph() {
  // DONE FOR YOU
  directed = false; // graphs are undirected by default
}

Graph::~Graph() {
  // "DONE" FOR YOU
  // no implementation needed
}

vector<Node*> Graph::getNodes() {
  // DONE FOR YOU
  return nodes;
}

vector<Edge*> Graph::getEdges() {
  // DONE FOR YOU
  return edges;
}

int Graph::getClock() {
  // DONE FOR YOU
  return clock;
}

void Graph::addNode(Node& n) {
  // DONE FOR YOU
  nodes.push_back(&n);
}

void Graph::addEdge(Edge& e) {
  // DONE FOR YOU
  edges.push_back(&e);
}
 
void Graph::removeNode(Node& n) {
  // DONE FOR YOU
  for (vector<Node*>::iterator it = nodes.begin();
       it != nodes.end(); 
       it++) {
    if (&n == *it) {
      nodes.erase(it);
      break;
    }
  }
}
 
void Graph::removeEdge(Edge& e) {
  // DONE FOR YOU
  for (vector<Edge*>::iterator it = edges.begin();
       it != edges.end(); 
       it++) {
    if (&e == *it) {
      edges.erase(it);
      break;
    }
  }
}

void Graph::setDirected(bool val) {
  // DONE FOR YOU
  directed = val;
}

bool Graph::isDirected() {
  // DONE FOR YOU
  return directed;
}

set<Edge*> Graph::getAdjacentEdges(Node& n) {
  // DONE FOR YOU
  set<Edge*> ret;
  for (vector<Edge*>::iterator it = edges.begin(); it != edges.end(); it++) {
    Edge* edge = *it;
    if (edge->getStart() == &n) {
      ret.insert(edge);
    }
    if (!directed && edge->getEnd() == &n) {
      ret.insert(edge);
    }
  }
  return ret;
}

void Graph::clear() {
  // Clear all nodes.
  for (int i = 0; i < this->nodes.size(); i++) {
    this->nodes[i]->clear();
  }
  // Set undiscovered type for all edges.
  for (int i = 0; i < this->edges.size(); i++) {
    this->edges[i]->setType(UNDISCOVERED_EDGE);
  }
  // Reset graph clock.
  this->clock = 0;
}

void Graph::tick(string message) {
  // DONE FOR YOU
  //
  // optional debugging function, disabled with if(false) to prevent
  // unused var warning. Change this however you want.
  //
  // recommended use: call it just after incrementing the clock.
  if (false) {
    cout << "// " << message << endl << make_dot(this) << endl;
  }
}

void Graph::dfs(Node& start) {
  // mark node gray
  start.setColor(GRAY, this->clock);
  this->clock += 1;

  //  visit node
  set<Edge*> related_edges = this->getAdjacentEdges(start);
  for(auto related_edge : related_edges) {
    Node* edge_start = related_edge->getStart();
    Node* edge_end = related_edge->getEnd();

    Node* related_node;
    if (edge_start == &start) {
      related_node = edge_end;
    } else {
      related_node = edge_start;
    }

    int related_color;
    int related_disco_time;
    int related_finish_time;
    int related_bfs_rank;
    related_node->getDiscoveryInformation(related_color, related_disco_time, related_finish_time, related_bfs_rank);

    if (related_color == WHITE) {
      edge_end->setPredecessor(*edge_start);
      related_edge->setType(TREE_EDGE);
      this->dfs(*related_node);
    } else if (related_color == GRAY)  {
      related_edge->setType(BACK_EDGE);
    } else {
      if (edge_end->isAncestor(*edge_start)) {
        related_edge->setType(FORWARD_EDGE);
      } else {
        related_edge->setType(CROSS_EDGE);
      }
    }
  }  

  start.setColor(BLACK, this->clock);
  this->clock += 1;
}

void Graph::bfs(Node& start) {
  queue<Node*> search_queue;
  int rank = 0;

  search_queue.push(&start);
  start.setColor(GRAY, this->clock);
  start.setRank(rank);
  this->clock += 1;

  while (!search_queue.empty()) {
    Node* node = search_queue.front();
    search_queue.pop();
    node->setColor(BLACK, this->clock);
    this->clock += 1;

    int color;
    int disco_time;
    int finish_time;
    int bfs_rank;
    node->getDiscoveryInformation(color, disco_time, finish_time, bfs_rank);

    set<Edge*> related_edges = this->getAdjacentEdges(*node);
    for(auto related_edge : related_edges) {
      Node* other_node = related_edge->getEnd();

      int related_color;
      int related_disco_time;
      int related_finish_time;
      int related_bfs_rank;
      other_node->getDiscoveryInformation(related_color, related_disco_time, related_finish_time, related_bfs_rank);

      if (related_color == WHITE) {
        other_node->setColor(GRAY, this->clock);
        this->clock += 1;
        search_queue.push(other_node);
        // other_node->setRank(bfs_rank + 1);
      }

      if (related_bfs_rank == -1 || related_bfs_rank > bfs_rank + 1) {
        cout << other_node->getData() << " RANK IS NOW " << (bfs_rank + 1) << endl;
        other_node->setRank(bfs_rank + 1);
      }
    }
  }
}

void Graph::bfs(Node& start, Node& finish) {
  bool stop = false;
  queue<Node*> search_queue;
  int rank = 0;

  search_queue.push(&start);
  start.setColor(GRAY, this->clock);
  start.setRank(rank);
  this->clock += 1;

  while (!search_queue.empty()) {
    Node* node = search_queue.front();
    search_queue.pop();
    node->setColor(BLACK, this->clock);
    this->clock += 1;

    int color;
    int disco_time;
    int finish_time;
    int bfs_rank;
    node->getDiscoveryInformation(color, disco_time, finish_time, bfs_rank);
    
    set<Edge*> related_edges = this->getAdjacentEdges(*node);
    for(auto related_edge : related_edges) {
      Node* other_node = related_edge->getEnd();

      

      int related_color;
      int related_disco_time;
      int related_finish_time;
      int related_bfs_rank;
      other_node->getDiscoveryInformation(related_color, related_disco_time, related_finish_time, related_bfs_rank);

      if (!stop && related_color == WHITE) {
        other_node->setColor(GRAY, this->clock);
        this->clock += 1;
        search_queue.push(other_node);
      }

      if (related_bfs_rank == -1 || related_bfs_rank > bfs_rank + 1) {
        cout << other_node->getData() << " RANK IS NOW " << (bfs_rank + 1) << endl;
        if (this->isDirected()) {
          other_node->setRank(bfs_rank + 1);
        } else {
          other_node->setRank(1);
        }
        
      }

      if (other_node->getData() == finish.getData()) {
        stop = true;
      }
    }
  }
}

Node::Node(string s) {
  // DONE FOR YOU
  data = s;
}

Node::~Node() {
  // "DONE" FOR YOU
  //
  // This is equivalent to the default destructor the compiler will
  // generate for you. As a rule of thumb, you only need to `delete`
  // member variables that were created with `new` inside this
  // class. So, you don't need to do anything here. It could have been
  // left out of the header/impl file entirely.
}

string Node::getData() {
  // DONE FOR YOU
  return data;
}

void Node::setRank(int r) {
  // DONE FOR YOU
  rank = r;
}

void Node::clear() {
  this->discovery_time = -1;
  this->completion_time = -1;
  this->rank = -1;
  this->color = WHITE;
  this->predecessor = NULL;
}

void Node::setColor(int search_color, int time) {
  this->color = search_color;
  if (search_color == BLACK) {
    this->completion_time = time;
  } else if (search_color == WHITE) {
    this->completion_time = -1;
    this->discovery_time = -1;
  } else {
    this->discovery_time = time;
  }
}

void Node::getDiscoveryInformation(int& color, int& disco_time, 
				   int& finish_time, int& bfs_rank) {
  color = this->color;
  disco_time = this->discovery_time;
  finish_time = this->completion_time;
  bfs_rank = this->rank;
}

bool Node::isAncestor(Node& other) {
  if (&other == this) {
    return true;
  } else if (this->predecessor != NULL) {
    return this->predecessor->isAncestor(other);
  } else {
    return false;
  }
}

void Node::setPredecessor(Node& other) {
  this->predecessor = &other;
}

Edge::Edge(Node& n1, Node& n2) {
  // DONE FOR YOU
  a = &n1;
  b = &n2;
}

Edge::~Edge() {
  // "DONE" FOR YOU
}


Node* Edge::getStart() {
  // DONE FOR YOU
  return a;
}

Node* Edge::getEnd() {
  // DONE FOR YOU
  return b;
}

int Edge::getType() {
  // DONE FOR YOU
  return type;
}

void Edge::setType(int edge_type) {
  // TODO
  this->type = edge_type;
}

// overloading operator << lets you put a Graph object into an output
// stream.
ostream &operator << (ostream& out, Graph graph) {
  // DONE FOR YOU
  out << graph.nodes.size() << " Nodes:" << endl;
  out << "[";
  for (vector<Node*>::iterator it = graph.nodes.begin(); it != graph.nodes.end(); it++) {
    Node* n = *it;
    out << *n << ", ";
  }
  out << "]" << endl;
  out << graph.edges.size() << " Edges:" << endl;
  out << "[";
  for (vector<Edge*>::iterator it = graph.edges.begin(); it != graph.edges.end(); it++) {
    Edge* e = *it;
    out << *e << ", ";
  }
  out << "]";
  return out;
}

// overloading operator << lets you put a Node object into an output
// stream.
ostream &operator << (std::ostream& out, Node node) {
  // DONE FOR YOU
  out << node.data;
  return out;
}

// overloading operator << lets you put an Edge object into an output
// stream.
ostream &operator << (std::ostream& out, Edge edge) {
  // DONE FOR YOU
  out << *edge.a << " -- " << *edge.b;
  return out;
}

// what is a helper function that turns a constant (e.g. WHITE or
// FORWARD_EDGE) into a string for debugging goodness. 
string what(int& v) {
  // DONE FOR YOU
  string ret = "Unknown";
  if (v == WHITE) {
    ret = "white";
  } else if (v == GRAY) {
    ret = "gray";
  } else if (v == BLACK) {
    ret = "black";
  } else if (v == UNDISCOVERED_EDGE) {
    ret = "undiscovered";
  } else if (v == TREE_EDGE) {
    ret = "tree";
  } else if (v == BACK_EDGE) {
    ret = "back";
  } else if (v == FORWARD_EDGE) {
    ret = "forward";
  } else if (v == CROSS_EDGE) {
    ret = "cross";
  }
  return ret;
}


// make_dot generates a dotfile string for viewing your graph. Have
// tick() print out the results of this function, and then use
// http://www.webgraphviz.com/ (or get a copy of GraphViz) to view the
// result. If you're stuck, this can help you get un-stuck.
string make_dot(Graph* g) {
  // DONE FOR YOU
  stringstream ss;
  vector<Node*> nodes = g->getNodes();
  vector<Edge*> edges = g->getEdges();
  string gt = "graph";
  string con = "--";
  if (g->isDirected()) {
    con = "->";
    gt = "digraph";
  }
  ss << gt << " homework {" << endl;
  int c, dt, ft, r;
  string textColor = "black";
  for (auto it=nodes.begin(); it != nodes.end(); ++it) {
    Node* n = *it;
    n->getDiscoveryInformation(c, dt, ft, r);
    if (c == BLACK) {
      textColor = "white";
    } else {
      textColor = "black";
    }
    ss << "  " << n->getData() << " [ style=\"filled\" fontcolor=\"" << textColor << "\" fillcolor=\"" << what(c) << "\"" << " ]" << endl;
  }
  
  string edgeColor = "black";
  for (auto it=edges.begin(); it != edges.end(); ++it) {
    Edge* e = *it;
    if (e->getType() == TREE_EDGE) {
      edgeColor = "black";
    } else if (e->getType() == FORWARD_EDGE) {
      edgeColor = "purple";
    } else if (e->getType() == BACK_EDGE) {
      edgeColor = "blue";
    } else if (e->getType() == CROSS_EDGE) {
      edgeColor = "green";
    } else {
      edgeColor = "gray";
    }
    ss << "  " << e->getStart()->getData() << con << e->getEnd()->getData() << " [color=\"" << edgeColor << "\"]" << endl;
  }
  ss << "}" << endl;
  return ss.str();
}
