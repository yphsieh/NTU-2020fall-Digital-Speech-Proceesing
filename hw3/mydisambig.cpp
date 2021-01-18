#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <stdio.h>
#include "Ngram.h"
#include "Vocab.h"
#include "File.h"

using namespace std;

Vocab voc;

double getUigramProb(Ngram& lm, const char *w1)
{
    VocabIndex wid1 = voc.getIndex(w1);

    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { Vocab_None };
    return lm.wordProb( wid1, context);
}

double getBigramProb(Ngram& lm, const char *w1, const char *w2)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if(wid1 == Vocab_None) wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None) wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

double getTrigramProb(Ngram& lm, const char *w1, const char *w2, const char *w3) 
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    VocabIndex wid3 = voc.getIndex(w3);

    if(wid1 == Vocab_None) wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None) wid2 = voc.getIndex(Vocab_Unknown);
    if(wid3 == Vocab_None)  //OOV
        wid3 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid2, wid1, Vocab_None };
    return lm.wordProb( wid3, context );
}

vector<string> split(string A)
{
    vector<string> word_list;

    // split chinese charaters and push into vector
    for(int j = 0; j < A.length(); j++)
    {
        if (A[j] == ' ') continue;
        else {
            string a = A.substr(j, 2);
            word_list.push_back(a);
            j++;
        }
    }
    return word_list;
}


int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Wrong argument format\n");
        printf("Usage: ./mydisambig testdata/$$i.txt ZhuYin-Big5.map bigram.lm result/$$i.txt\n");
        exit(1);
    }

    int ngram_order; 

    string ngramFile = argv[3];    
    if (!ngramFile.compare("unigram.lm")) ngram_order = 1;
    else if (!ngramFile.compare("bigram.lm")) ngram_order = 2;
    else if (!ngramFile.compare("trigram.lm")) ngram_order = 3;
    else{
    	cout << argv[3] << endl;
    	printf("error N-gram !\n");
    	exit(1);
    }

    // read in LM
    Ngram lm(voc, ngram_order);
    const char* lm_file_name = argv[3];
    File lm_file(lm_file_name, "r");
    lm.read(lm_file);
    lm_file.close();

    // read in Map
    string line;
    ifstream map_file(argv[2]);
    map <string, vector<string>> zhu2big_map;
    while(getline(map_file, line)) {
        string key = line.substr(0, 2);
        zhu2big_map[key] = split(line);
    }

    map_file.close();

    // open output file
    ofstream output_file(argv[4]);

    // read in test data
    char* texts = argv[1];
    ifstream text (texts, ifstream::in);

    // string line;
    while(getline(text, line)){
        
        vector <vector <string>> vec;
        vector<string> s = split(line);
        for(size_t i = 0; i < s.size(); i++)
        {
            vec.push_back(zhu2big_map[s[i]]);
        }

        vector<string> end_str;
        end_str.push_back(Vocab_SentEnd);
        vec.push_back(end_str);

        
        vector< vector <double> > score (vec.size());
        vector< vector <int> > path (vec.size());

        for(int t = 0; t < vec.size(); ++t) {

            if(ngram_order == 2){
                // Viterbi: Initialization
                if(t == 0){
                    for (size_t k = 0; k < vec[0].size(); ++k){
                        score[t].push_back(getBigramProb(lm, Vocab_SentStart, vec[0][k].c_str()));
                        path[t].push_back(0);
                    }
                }

                // Viterbi: Forward
                else{
                    for(size_t k = 0; k < vec[t].size(); ++k){
                        double max_prob = -2147483647;
                        int idx = 0;
                        for (size_t j = 0; j < vec[t-1].size(); ++j){
                            double prob = score[t - 1][j] + getBigramProb(lm, vec[t-1][j].c_str(), vec[t][k].c_str());
                            if (prob >= max_prob){
                                max_prob = prob;
                                idx = j;
                            }
                        }
                        score[t].push_back(max_prob);
                        path[t].push_back(idx);
                    }
                }
            }

        }

        int max_prob_index = 0;
        vector <string> result;
        
        // Viterbi: Backtracking
        for(int t = vec.size() - 1; t >= 0; t--){
            result.push_back(vec[t][max_prob_index]);
            max_prob_index = path[t][max_prob_index];
        }
        result.push_back(Vocab_SentStart);
        
        // Output
        for(int k = result.size() - 1; k >= 1; k--){
            output_file << result[k] << " ";
        }
        output_file << result[0] << endl;
        
    }

    output_file.close();
    text.close();
    
}
