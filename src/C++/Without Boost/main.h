#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdexcept>

#include "mcmf.cpp"

void PrintVector(const std::vector<int> & vec){
  for (auto citer = vec.cbegin(); citer < vec.cend(); citer++){
    std::cout << *citer << " ";
  }
  std::cout << std::endl;
}

std::vector<int> Unique(std::vector<int> vec){
  std::sort(vec.begin(), vec.end());
  std::vector<int>::iterator it;
  it = std::unique (vec.begin(), vec.end());
  vec.resize( std::distance(vec.begin(),it) );
  return vec;
}

int NumberOfLines(std::ifstream & file, char ignore = '#'){
  std::string line;
  int count = 0;
  if (file.is_open()){
    while ( getline(file,line) ){
      if (line.length() >= 1){
        if (*line.begin() != ignore){
          count += 1;
        }
      }
    }
    file.clear();
    file.seekg(0, ios::beg);
    return count;
  }
  else{
    std::cout << "File read error\n";
    return -1;
  }
}

int Characters(std::string & str){
  int count = 0;
  for (auto citer = str.cbegin(); citer < str.cend(); citer++){
    if (std::isspace(*citer)){
      // count each space
      count += 1;
    }
  }
  // add the last nunmber
  count += 1;
  return count;
}

std::vector<int> ReadData(std::string & str, MCMF_CS2 & mcmf){
  bool arc;
  std::vector<int> values;
  if (Characters(str) == 2){
    arc = 0;
  }
  else{
    arc = 1;
  }

  std::string CurrNum;
  for (auto citer = str.cbegin(); citer <= str.cend(); citer++){
    if (std::isspace(*citer) || citer == str.cend()){
      values.push_back(std::stoi(CurrNum));
      CurrNum.clear();
    }
    else{
      CurrNum += *citer;
    }
  }
  if (arc == 1){
    mcmf.set_arc(values[0], values[1], values[2], values[3], values[4]);
  }
  else{
    mcmf.set_supply_demand_of_node(values[0], values[1]);
  }
  return values;
}

std::vector<int> ProblemFromFile(MCMF_CS2 & mcmf, std::ifstream & file){
  // takes an empty MCMF_CS2 object e.g MCMF_CS2 mcmf(0,0);
  // and a file of the form example_problem.in
  std::string line;
  // source and sink id
  std::vector<int> values = {0,0};

  bool ReadNodeNumber = 0;
  if (file.is_open()){
    int Arcs = NumberOfLines(file)-3;
    while ( getline(file,line) ){
      if (line.length() >= 1){
        if (*line.begin() != '#'){
          if (ReadNodeNumber == 0){
            int Nodes = std::stoi(line);
            MCMF_CS2 m(Nodes, Arcs);
            mcmf = m;
            ReadNodeNumber = 1;
          }
          else{
            std::vector<int> vec = ReadData(line, mcmf);
            if (vec.size() == 2){
              if (vec[1] > 0){
                values[0] = vec[0];
              }
              else{
                values[1] = vec[0];
              }
            }
          }
        }
      }
    }
    file.close();
    return values;
  }
  else{
    std::cout << "File read error\n";
    return values;
  }
}

class MCMFProblem{
public:
  MCMFProblem(long nodes, long arcs)
    : mcmf(nodes,arcs), min_cost(1000000), flow(10)
    {}
  MCMFProblem()
    : mcmf(0,0), min_cost(1000000), flow(10)
    {}
  void LoadFromFile(std::ifstream & file){
    this->source_sink_id = ProblemFromFile(this->mcmf,file);
  }
  int Solve(bool debug, bool write_ans, std::string & out, int & min_cost, int & cost){
    return this->mcmf.run_cs2(debug, write_ans, out, min_cost, cost);
  }
  void TrajectoryAlgorithm(std::string & in, std::string & out, int flow_step = 10);
  void Flow(int & flow){
    this->flow = flow;
  }
protected:
  void SetFlow(int & flow){
    if (source_sink_id[0] > 0 && source_sink_id[1] > 0){
      this->mcmf.set_supply_demand_of_node(source_sink_id[0], flow);
      this->mcmf.set_supply_demand_of_node(source_sink_id[1], -flow);
    }
    else{
      std::cout << "Uninitialised source and sink\n";
      std::cout << "Or node label 0 is invalid start with 1";
    }
  }
  //std::vector<int> ReadData(std::string & line);
  std::vector< std::vector<int> > Arcs;
  std::vector<int> source_sink_id = {0,0};
  int flow = 0;
  int min_cost;
  int cost;
  bool read_data = false;
  int edges;
  int nodes;
  MCMF_CS2 mcmf;
};

void MCMFProblem::TrajectoryAlgorithm(std::string & in,std::string & out, int flow_step){
  std::ifstream file;
  file.open(in);
  LoadFromFile(file);
  // std::ofstream out_file;
  // out_file.open("mcmf.out");
  // termination is when the problem is
  // unfeasible which cause the whole program to exit
  int change = 0;
  int prev_cost = 0;
  while(true){
    SetFlow(this->flow);
    Solve(false,true,out,this->min_cost,this->cost);
    change = this->cost - prev_cost;
    if (change > 0){
      std::cout << "Reached convex minimum";
      return;
    }
    file.open(in);
    LoadFromFile(file);
    flow += flow_step;
    prev_cost = this->cost;
    std::cout << flow << std::endl;
  }
}
