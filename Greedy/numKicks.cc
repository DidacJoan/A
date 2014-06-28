#include <iostream>
#include <vector>

using namespace std;

vector<int> readData(int numDealers){
    vector<int> result (numDealers);
    for(int i = 0; i < numDealers; ++i)
        cin >> result[i];

    return result;
}

int algorithm(vector<int> data, int numDealers){
    int max;
    vector<int> aux(numDealers,1);
    max = 1;

    for(int i = 1; i < numDealers; ++i){
        for(int j = i-1; j >= 0; --j) {
            if(data[j]+10 <= data[i] && aux[j] >= aux[i])
                aux[i] = aux[j]+1;
        }

        if(aux[i] > max)
            max = aux[i];
    }
    return max;
}

int main(){
    int numTests, numDealers;
    vector<int> data;
    cin >> numTests;
    for(int i = 0; i < numTests; ++i){
        cin >> numDealers;
        data = readData(numDealers);
        int kicks = algorithm(data,numDealers);
        cout << kicks << endl;
    }
}
