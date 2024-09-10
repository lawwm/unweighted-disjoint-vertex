// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <deque>
#include <tuple>
#include <functional>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Node {
  int x;
  int y;
  Node& operator=(const Node& other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  bool operator==(const Node& other) const {
    return x == other.x && y == other.y;
  }
};

int getCoor(Node node, int size) {
  return node.x * size + node.y;
}

Node getNode(int coor, int size) {
  return { coor / size, coor % size };
}

vector<tuple<int, int>> directions{
  {0, 1},
  {0, -1},
  {1, 0},
  {-1, 0}
};

enum State {
  UNVISITED,
  VISITED,
  VISITED_IN_PROCESS,
  VISITED_FAILED
};


void print(vector<int> parents, vector<Node> root, Node start, Node end, vector<vector<int>> grid) {
  int m = grid[0].size();
  auto getCoorGrid = std::bind(getCoor, placeholders::_1, m);
  auto getNodeGrid = std::bind(getNode, placeholders::_1, m);

  int currCount = 2;
  for (Node rt : root) {
    Node curr = rt;
    while (curr != start) {
      int currCoor = getCoorGrid(curr);
      grid[curr.x][curr.y] = currCount;
      curr = getNodeGrid(parents[currCoor]);
    }
    currCount += 1;
  }

  // Output the grid to check if it was parsed correctly
  for (int i = 0; i < grid.size(); ++i) {
    std::vector<int>& row = grid[i];
    for (int j = 0; j < grid[0].size(); ++j) {
      auto value = grid[i][j];
      if (Node{ i, j } == start || Node{ i, j } == end) {
        std::cout << "X ";
      }
      else if (value > 1) {
        char c = 'A' + value - 2;
        std::cout << c << " ";
      }
      else {
        std::cout << value << " ";
      }

    }
    std::cout << std::endl;
  }
  std::cout << endl;
}

int loops(int stride, vector<int>& parents, vector<Node>& root, Node start, Node end, const vector<vector<int>>& grid) {
  int count = 0;
  int n = grid.size(), m = grid[0].size();
  auto getCoorGrid = std::bind(getCoor, placeholders::_1, m);
  auto getNodeGrid = std::bind(getNode, placeholders::_1, m);

  vector<int> parentsCpy = parents;

  for (int i = 0; i < stride; ++i) {
    deque<Node> q;
    deque<tuple<Node, Node, Node>> stk;
    vector<int> visited(n * m, UNVISITED);

    q.push_back(start);
    visited[getCoorGrid(start)] = VISITED;

    while (!q.empty() || (q.empty() && !stk.empty())) {
      Node current;

      if (q.empty()) {
        auto [curr, next, parent] = stk.back();
        auto parentCoor = getCoorGrid(parent);

        if (visited[parentCoor] == UNVISITED || visited[parentCoor] == VISITED) {
          current = parent;
          visited[parentCoor] = VISITED_IN_PROCESS;
          cout << "Case 1: Start visit process for " << current.x << " " << current.y << endl;
        }
        else if (visited[parentCoor] == VISITED_IN_PROCESS) {
          stk.pop_back();
          visited[parentCoor] = VISITED_FAILED;
          cout << "Case 3: Visited FAILED" << endl;
          continue;
        }
        else {
          throw std::runtime_error("Invalid state");
        }
      }
      else {
        current = q.front();
        q.pop_front();
      }
      cout << current.x << " " << current.y << endl;

      for (auto [dx, dy] : directions) {
        Node next = { current.x + dx, current.y + dy };
        if (next.x < 0 || next.x >= n || next.y < 0 || next.y >= m || grid[next.x][next.y]) {
          continue;
        }
        int nextCoor = getCoorGrid(next);
        if (visited[nextCoor] == UNVISITED) {
          visited[nextCoor] = VISITED;

          if (next == end) {
            count += 1;
            root.push_back(current);
            goto endbfs;
          }
          else if (parents[nextCoor] == -1) {
            parents[nextCoor] = getCoorGrid(current);
            q.push_back(next);
          }
          else {
            int parentCoor = parents[nextCoor];
            if (parentCoor != -1) {
              Node parent = getNodeGrid(parentCoor);
              stk.push_back({ current, next, parent });
            }
          }
        }

      }

    }

  endbfs:

    while (!stk.empty()) {
      auto [curr, next, parent] = stk.back();
      stk.pop_back();
      auto parentCoor = getCoorGrid(parent);
      if (visited[parentCoor] == VISITED_IN_PROCESS) {
        parents[getCoorGrid(next)] = getCoorGrid(curr);
        continue;
      }
    }

    for (Node rt : root) {
      Node curr = rt;
      while (curr != start) {
        int currCoor = getCoorGrid(curr);
        parentsCpy[currCoor] = parents[currCoor];
        curr = getNodeGrid(parents[currCoor]);
      }
    }

    parents = parentsCpy;
    print(parents, root, start, end, grid);
  }

  return count == stride;
}

tuple<std::vector<std::vector<int>>, Node, Node> readGridFromFile(const std::string& filename) {
  std::ifstream infile(filename);
  std::vector<std::vector<int>> grid;

  if (!infile) {
    std::cerr << "Error opening file!" << std::endl;
    return { grid, Node{}, Node{} };
  }

  std::string line;
  Node src, dest;
  while (std::getline(infile, line)) {
    std::vector<int> row;
    std::istringstream iss(line);
    int value;
    while (iss >> value) {
      if (value == 2) {
        src.x = grid.size();
        src.y = row.size();
        row.push_back(0);
      }
      else if (value == 3) {
        dest.x = grid.size();
        dest.y = row.size();
        row.push_back(0);
      }
      else {
        row.push_back(value);  // Push each integer (0 or 1) to the row vector
      }
    }
    grid.push_back(row);  // Add the row to the grid
  }

  infile.close();
  return { grid, src, dest };
}


int main()
{
  std::string filename = "grid.txt";
  auto [grid, start, end] = readGridFromFile(filename);


  std::vector<int> parents(grid.size() * grid[0].size(), -1);
  std::vector<Node> root;
  bool canfind = loops(2, parents, root, start, end, grid);
  print(parents, root, start, end, grid);
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
