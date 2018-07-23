// TODO create an object to provide the
//      file input/output interface
//      MCMFProblem?

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

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

int main(int argc, char ** argv) {
  std::string filename;
  if (argc >= 2){
    filename = argv[1];
  }
  else{
    filename = "mcmf.in";
  }
  int flow_step;
  if ( argc >= 3){
    flow_step = std::stoi(argv[2]);
  }
  else{
    flow_step = 10;
  }
  std:ifstream file;
  file.open(filename);
  MCMF_CS2 mcmf(0,0);
  MCMF_CS2 temp(0,0);
  MCMF_CS2 t(0,0);
  std::vector<int> source_sink = ProblemFromFile(mcmf, file);
  file.open(filename);
  ProblemFromFile(temp, file);
  std::ofstream out;
  out.open("mcmf.out");
  //mcmf.run_cs2(true,true,out);
  // termination is when the problem is
  // unfeasible which cause the whole program to exit
  int min_cost = 1000000;
  int flow = 10;
  while(true){
    mcmf.set_supply_demand_of_node(source_sink[0], flow);
    mcmf.set_supply_demand_of_node(source_sink[1], -flow);
    mcmf.run_cs2(false,true,out,min_cost);
    file.open(filename);
    ProblemFromFile(mcmf, file);
    out.open("mcmf.out");
    flow += flow_step;
    std::cout << flow << std::endl;
  }
}
