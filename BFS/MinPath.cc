#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <limits>

using namespace std;

#define MAXVAL numeric_limits<int>::max()

vector<list<int> > readData(int V, int E) {
    vector<list<int> > data(V);
    int v1, v2;

    for(int i = 0; i < E; ++i){
        cin >> v1 >> v2;
        data[v1].push_back(v2);
    }
    return data;
}

void BFS(vector<list<int> > data, int V) {
    vector<bool> visited(V,false);
    vector<int> dist(V,MAXVAL);
    vector<int> ways(V,0);
    queue<int> PQ;

    dist[0] = 0;
    ways[0] = 1;
    PQ.push(0);

    while(!PQ.empty()) {
        int u = PQ.front();
        visited[u] = true;
        PQ.pop();

        list<int>::iterator it = data[u].begin();
        while(it != data[u].end()){

            if( dist[(*it)] >= (dist[u] + 1) ) dist[(*it)] = dist[u] + 1;

            if( (visited[(*it)] == false) || (dist[(*it)] == dist[u] + 1) ) ways[(*it)] += ways[u];

            if(visited[(*it)] == false) {
                visited[(*it)] = true;
                PQ.push(*it);
            }

            ++it;
        }
    }

    for (int i = 0; i < V; ++i) {
        cout << i << ": ";
        if(dist[i] != MAXVAL) cout << dist[i] << " " << ways[i];
        else cout << "-1";
        cout << endl;
    }
    cout << endl;
}

int main() {
    int V,E;
    while(cin >> V >> E){
        vector<list<int> > data = readData(V,E);
        BFS(data,V);
    }
}
