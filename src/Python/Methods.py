import networkx
import mcmf_ext

def FindNode(G,attribute,value):
    '''
    Find a node in networkx graph G with attribute==value
    '''
    found = []
    for v in G.nodes:
        if (G.nodes[v][attribute]==value):
            found.append(v)
    return found

def GraphToMCMFProblem(G,source='source',sink='sink'):
    '''
    Takes a networkx graph and returns an initialised
    MCMFProblem
    '''
    K = nx.convert_node_labels_to_integers(G,1,'sorted','old')
    edges = np.zeros((len(K.edges),5),dtype=np.int64)
    e = 0
    for u,v in K.edges:
        edges[e,0:2] = u,v
        edges[e,2] = 0
        edges[e,3] = K.edges[u,v]['capacity']
        edges[e,4] = K.edges[u,v]['weight']

        e += 1
    ss = np.array([FindNode(K,'old',source)[0],FindNode(K,'old',sink)[0]])
    M = mcmf_ext.MCMFProblem(int(len(K.nodes)),edges,ss)
    return M

def CorrectAssignments(X,Predictions):
    '''
    A utility function mostly for use in PiecewiseTrajectories

    This takes the original data X and the predictions and returns
    a copy of X with elements re-order as dictated by the predictions.

    If the size of predictions < X the remaining elements are left untouched.
    '''
    Beetles = np.arange(0,Predictions.shape[0])
    N = Predictions.shape[1]
    Pred = np.zeros((N,len(Beetles),3))
    for n in range(0,N):
        for ind,b in enumerate(Predictions[:,n]):
            if (n > N-1):
                break
            Pred[n,ind,:] = X[n,b,:]
    return Pred

def PiecewiseTrajectories(X,endframe,framestep=10,flow_step_values=np.array([1000,100,10,1]),params=np.ones(4)):
    '''
    Wraps around the whole mcmf_ext module to solve for the trajectories of the data
    X in a piecwise fashion (online I suppose?).

    TODO
        add some kind of rate sampler to more efficiently solve, currenty only have the
        variable step from flow_step_values. User must make sure these are sensible the defualts
        are correct for a Beetle data set I have used.
    '''
    Predictions = []
    T = X.copy()
    for i in tqdm(range(0,int(np.floor(endframe/framestep)))):
        if (i == 0):
            # choose the values to build the flow model
            f1 = framestep*i
            f2 = framestep*(i+1)
            x = X[f1:f2,:,:]
        else:
            # -1 in order to link the trajectories up
            # then the fact th solution is correct
            # follows by induction
            f1 = framestep*i-1
            f2 = framestep*(i+1)
            x = T[f1:f2,:,:]

        G = BuildFlowModel(x,step,T.shape[1],N=step,b=0.01,a=params[2],c=1.0,t=params[4])
        # will change for the variable step
        starting_flow = 0
        for flow_step in flow_step_values:
            M,K = GraphToMCMFProblem(G)
            a = M.Trajectories(flow_step,1,False,starting_flow)
            # the algorithm goes 'one past the end' to determine
            # if the convex minimum has been passed.
            starting_flow = M.GetFinalFlow() - flow_step
        pred,trunc = MCMFPostProcessing(a,K)
        # get the corrected values (T unchanged)
        t = CorrectAssignments(T[f1:f2,:,:],pred)
        # handle missing beetles
        while t.shape[1] < T.shape[1]:
            t = np.append(t,np.zeros((framestep,1,T.shape[2])),axis=1)
        # Update T with the corrections
        if (i == 0):
            T[f1:f2,:,:] = t
        else:
            # +1 to account for the link values
            T[f1+1:f2,:,:] = t
        Predictions.append(pred)
    # T is the corrected matrix up to endframe
    # Predictions is the individual predictions
    return T,Predictions
