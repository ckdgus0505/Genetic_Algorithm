#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#define NUM_OF_ANSWERSHEET 10
#define NUM_OF_QUESTIONS 10
#define GENERATION 10000
#define TRIAL 20
using namespace std;



class OMR {
private:

protected:
    int score; // 정답과 비교했을때의 점수
    int marking[NUM_OF_QUESTIONS]; // 표기한 답안
public:

    OMR(){
            for (int i = 0; i < NUM_OF_QUESTIONS; i++) {    
                marking[i] = rand()%4;
            }
            score = 0;
        }

    OMR(OMR* clone){
            for (int i = 0; i < NUM_OF_QUESTIONS; i++) {    
                marking[i] = clone->get_mark(i);
            }
            score = clone->get_score();
        }

    bool operator<(OMR a) const{
        if (this->score <= a.get_score()) return this->score<= a.get_score();
        else return this->score > a.get_score();
    }

    void change_marking(OMR*& new_marking) { // marking 전체를 변경하는 함수
        for (int i = 0; i < NUM_OF_QUESTIONS; i++) {
            this->set_mark(i, new_marking->get_mark(i));            
        }
    }
    void set_mark(int index, int num) { // marking 의 index 부분의 값만 변경하는 함수
        marking[index] = num;
    }

    int get_mark(int index) { // marking 의 index 부분의 값을 얻어오는 함수
        return marking[index];
    }
    

    void set_score(int s){ // score을 변경하는 함수
        score = s;
    }

    int  get_score(){ // score을 얻어오는 함수
        return score;
    }

};

bool compare (OMR* a, OMR* b) {
    if (a->get_score() > b->get_score()) return true;
    else return false;
}

class Teacher : public OMR {
public:
    Teacher() {
        score = NUM_OF_QUESTIONS;
    }
    void scoring(OMR* st){
        int score = 0;
        for (int i = 0; i < NUM_OF_QUESTIONS; i++) {
            if( st->get_mark(i) == this->get_mark(i) ) {
                score++;
            }
        }
        st->set_score(score);
    }
};

class TESTS{
private:
public:
    

    pair<OMR*, OMR*> make_crossover(vector<OMR*>& this_generation, bool proportion_crossover_point = false) {

        // crossover 후보로 2개의 서로 다른 답안을 선택한다.
        vector<int> random_roulette;
        for (int i = 0; i < NUM_OF_QUESTIONS; i++) {
            for (int j = 0; j < this_generation[i]->get_score(); j++) {
                random_roulette.push_back(i);
            }
        }
        
        int random_pick = rand() % random_roulette.size();
        int first_idx = random_roulette[random_pick];

        random_roulette.clear();

        for (int i = 0; i < NUM_OF_QUESTIONS; i++) {
            if (i == first_idx) continue;
            for (int j = 0; j < this_generation[i]->get_score(); j++) {
                random_roulette.push_back(i);
            }
        }

        random_pick = rand() % random_roulette.size();
        int second_idx = random_roulette[random_pick];

        int crossover_point;
        if (proportion_crossover_point = true) {
            double a = (double)this_generation[first_idx]->get_score();
            double b = (double)this_generation[second_idx]->get_score();
            a /= a+b;

            crossover_point = (int)( round(a*10) );

        } else {
            crossover_point = NUM_OF_QUESTIONS / 2;
        }

        // crossover
        OMR *crossover1;
        crossover1 = new OMR();

        for (int i = 0; i < crossover_point; i++) {
            crossover1->set_mark(i, this_generation[first_idx]->get_mark(i));
        }
        for (int i = crossover_point; i < NUM_OF_QUESTIONS; i++) {
            crossover1->set_mark(i, this_generation[second_idx]->get_mark(i));
        }

        // crossover
        OMR *crossover2;
        crossover2 = new OMR();

        for (int i = 0; i < NUM_OF_QUESTIONS - crossover_point; i++) {
            crossover2->set_mark(i, this_generation[second_idx]->get_mark(i));
        }
        for (int i = NUM_OF_QUESTIONS - crossover_point; i < NUM_OF_QUESTIONS; i++) {
            crossover2->set_mark(i, this_generation[first_idx]->get_mark(i));
        }
        return make_pair(crossover1, crossover2);
    }

    void make_mutation(vector<OMR*> &next_generation_pool, double mutation_probability = 0.0005) {
        for(int i = 0; i < next_generation_pool.size(); i++) {
            for (int j = 0; j < NUM_OF_QUESTIONS; j++) {
                if (rand()/(double)RAND_MAX < mutation_probability ) {
                    next_generation_pool[i]->set_mark(j, rand()%4);
                }
            }
            
        }
    }

    void make_next_generation_pool(vector<OMR*> &this_generation, vector<OMR*> &next_generation_pool, bool proportion_crossover_point = false, int num_of_crossover = 50, double mutation_probability = 0.0005) {
        // 여러번 crossover을 하여 next generation 후보들을 만든다
        for (int i = 0; i < num_of_crossover; i++) {
            pair<OMR*, OMR*> crossover = this->make_crossover(this_generation, proportion_crossover_point);
            next_generation_pool.push_back(crossover.first);
            next_generation_pool.push_back(crossover.second);
        }

        // 돌연변이도 생성 한다
        for (int i = 0; i < NUM_OF_QUESTIONS; i++) {
            make_mutation(next_generation_pool, mutation_probability);
        }
    }

    void select_next_generation(vector<OMR*> &this_generation, vector<OMR*> &next_generation_pool) {
        vector<int> random_roulette;
        
        for (int i = 0; i < NUM_OF_ANSWERSHEET; i++) {
            random_roulette.clear();
            for (int i = 0; i < next_generation_pool.size(); i++) {
                for (int j = 0; j < next_generation_pool[i]->get_score(); j++) {
                    random_roulette.push_back(i);
                }
            }
            int random_pick = rand() % random_roulette.size();
            int index = random_roulette[random_pick];
            this_generation[i]->change_marking(next_generation_pool[index]);
            this_generation[i]->set_score(next_generation_pool[index]->get_score());
            next_generation_pool.erase(next_generation_pool.begin()+index, next_generation_pool.begin() + index+1);
        }
        next_generation_pool.clear();
    }
};


int main()
{
    srand(time(NULL));
    ofstream file;
    time_t timer = time(NULL);
    struct tm* t = localtime(&timer);
    string file_name = to_string(t->tm_mday)+"_"+to_string(t->tm_hour)+"_"+to_string(t->tm_min);
    file.open(file_name+".csv");

    /*
    * 
    * 다른 GENERATION ALGORITHM 전략을 정하는 flag
    * this_generation_to_next_generation_pool
    * proportion_crossover_point
    * 
    * 실험에 사용할 변수 
    * mutation_probability
    * num_of_crossover
    * 
    */

    bool proportion_crossover_point = false;
    bool this_generation_to_next_generation_pool = false;

    double mutation_probability = 0.0005;
    int num_of_crossover = 50;



    for (int trial = 0; trial < TRIAL; trial++){
        file << "trial" + to_string(trial+1)+",";
        Teacher teacher;
        vector<OMR*> this_generation;
        vector<OMR*> next_generation_pool;
        for (int i = 0; i < NUM_OF_ANSWERSHEET; i++) { // make first answersheets
            this_generation.push_back(new OMR());
            teacher.scoring(this_generation[i]);
        }
        TESTS tests;
        
        int total_max_score = -1;
        int total_max_epoch = -1;

        for (int gen = 0; gen < GENERATION; gen++) {
            int max_score = -1;
            int mean_score = 0;
            std::cout << "=== Generation " << gen << " ===" << endl;
            // print this generation's score
            sort(this_generation.begin(), this_generation.end(), compare);
            for (int i = 0; i < NUM_OF_QUESTIONS; i++) {
                int score = this_generation[i]->get_score();
                std::cout << score << ' ';
                if (score > max_score) max_score = score;
                mean_score += score;
            }
            std::cout << endl << "max score : " << max_score << ", mean score : " << mean_score / (double)NUM_OF_ANSWERSHEET << endl;
            file << mean_score / (double)NUM_OF_ANSWERSHEET << ",";
            if (total_max_score < max_score) {
                total_max_score = max_score;
                total_max_epoch = gen;

            }

            // make next generation's answersheet candidates

            if (this_generation_to_next_generation_pool) { // this_generation의 유전자를 그대로 다음 generation에 넘기는 코드
                for (int i = 0; i < NUM_OF_ANSWERSHEET; i++) {
                    OMR* tmp = new OMR(this_generation[i]);
                    next_generation_pool.push_back(tmp);
                }
            }
            
            tests.make_next_generation_pool(this_generation, next_generation_pool, proportion_crossover_point, num_of_crossover, mutation_probability);
            for (int i = 0; i < next_generation_pool.size(); i++) {
                teacher.scoring(next_generation_pool[i]);
            }

            // select answersheet for next generation
            tests.select_next_generation(this_generation, next_generation_pool);

            if (max_score == NUM_OF_QUESTIONS) break;
        }
        std::cout << "total max score = " << total_max_score << " at " << total_max_epoch << " epochs" << endl;
        file << endl;
    }
    file.close();
    return 0;
}