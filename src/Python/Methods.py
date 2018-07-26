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
