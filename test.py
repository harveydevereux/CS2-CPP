from mcmf_ext import *
import numpy as np

edges = np.array([[1,2,0,40,30],[1,3,0,100,60],[2,4,0,90,10],[3,4,0,50,20]])
source_sink = np.array([1,4])

M = MCMFProblem(4,edges,source_sink)

a = M.PythonTrajectories(10)
print(a)
