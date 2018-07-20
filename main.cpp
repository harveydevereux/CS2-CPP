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

void ReadData(std::string & str, MCMF_CS2 & mcmf){
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
}

MCMF_CS2 ProblemFromFile(MCMF_CS2 & mcmf, std::ifstream & file){
  // takes an empty MCMF_CS2 object e.g MCMF_CS2 mcmf(0,0);
  // and a file of the form example_problem.in
  std::string line;

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
            ReadData(line, mcmf);
          }
        }
      }
    }
    file.close();
    return mcmf;
  }
  else{
    std::cout << "File read error\n";
    return mcmf;
  }
}

int main() {
  std:ifstream file;
  file.open("example_problem.in");

  MCMF_CS2 mcmf(0,0);
  mcmf = ProblemFromFile(mcmf, file);
  std::ofstream out;
  out.open("mcmf.out");
  mcmf.run_cs2(true,true,out);
}
