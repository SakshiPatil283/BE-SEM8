#include <iostream>
#include <omp.h>
#include <vector>
#include <queue>
using namespace std;

class Graph
{
private:
    int vertices;
    vector<vector<int>> adjList;

public:
    Graph(int vertices)
    {
        this->vertices = vertices;
        adjList.resize(vertices);
    }

    void addEdge(int u, int v)
    {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    void printGraph()
    {
        for (int i = 0; i < vertices; i++)
        {
            cout << i << " -> ";
            for (int j = 0; j < adjList[i].size(); j++)
            {
                cout << adjList[i][j] << " ";
            }
            cout << endl;
        }
    }

    void BFS(int start)
    {
        vector<bool> visited(vertices, false);
        queue<int> queue;
        queue.push(start);
        visited[start] = true;

        while (!queue.empty())
        {
            int current;

#pragma omp parallel shared(queue, visited)
            {
#pragma omp single
                {
                    current = queue.front();
                    queue.pop();
                    cout << "Thread " << omp_get_thread_num() << " visited " << current << endl;
                }

#pragma omp for
                for (int i = 0; i < adjList[current].size(); i++)
                {
                    if (!visited[adjList[current][i]])
                    {
#pragma omp critical
                        {
                            queue.push(adjList[current][i]);
                            visited[adjList[current][i]] = true;
                        }
                    }
                }
            }
        }
    }

    void DFS(int start)
    {
        vector<bool> visited(vertices, false);
        // DFSUtil(start, visited);
        #pragma omp parallel
        {
            #pragma omp single
            {
                DFSUtil(start, visited);
            }
        }
    }

    void DFSUtil(int v, vector<bool> &visited)
    {
        bool alreadyVisited = false;

        // Check and mark visited in a thread-safe way
        #pragma omp critical
        {
            if (visited[v])
                alreadyVisited = true;
            else
            {
                visited[v] = true;
                cout << "Thread " << omp_get_thread_num() << " visited " << v << endl;
            }
        }

        if (alreadyVisited) return; // Exit safely outside the critical section

        for (int i = 0; i < adjList[v].size(); i++)
        {
            int u = adjList[v][i];

            #pragma omp task firstprivate(u) shared(visited)
            {
                DFSUtil(u, visited);
            }
        }

        #pragma omp taskwait


    }
};

int main()
{
    int v, e;
    cout << "Enter number of vertices: ";
    cin >> v;

    Graph g(v);

    cout << "Enter number of edges: ";
    cin >> e;

    cout << "Enter " << e << " edges (u v format):" << endl;
    for (int i = 0; i < e; i++)
    {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    g.printGraph();

    int start;
    cout << "\nEnter start vertex for BFS and DFS: ";
    cin >> start;

    cout << "\nBFS traversal starting from vertex " << start << ":" << endl;
    g.BFS(start);

    cout << "\nDFS traversal starting from vertex " << start << ":" << endl;
    g.DFS(start);

    return 0;
}
