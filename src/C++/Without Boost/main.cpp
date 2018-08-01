#include "main.h"

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
  MCMFProblem P(0,0);
  std::string out = "mcmf.out";
  P.TrajectoryAlgorithm(filename, out, 10);
}
