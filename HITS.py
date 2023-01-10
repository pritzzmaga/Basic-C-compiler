import networkx as nx
import numpy as np
import re

web_graph = nx.read_gpickle('web_graph.gpickle')


def rootSet(query):
    """
    Finds the root set for a given query
    Parameters
    -----------------
    query -  The string which whose root set is to be found

    Returns
    ---------------
    rootset - The list of nodes in the root set
    """
    n = len(web_graph.nodes)
    rootset = []
    for node in web_graph.nodes:
        nc = web_graph.nodes[node]['page_content'].lower()
        nodeContent = re.split(' |:|,', nc)
        if query in nodeContent:
            rootset.append(node)
    return rootset


def makeBaseSet(rootset):
    """
    Function to find the base set for a given root set
    Parameters
    ---------------
    rootset - The list of nodes for which the base set is required
    Returns
    ---------------
    baseSet - The list of nodes in the base set
    """
    baseSet = list(rootset)

    for edge in web_graph.edges:
        if edge[0] in rootset:
            if edge[1] not in baseSet:
                baseSet.append(edge[1])
        elif edge[1] in rootset:
            if edge[0] not in baseSet:
                baseSet.append(edge[0])
        np.sort(baseSet)
    return baseSet


def makeAdjacencyMatrix(baseSet):
    """
    Function to convert a base set ot adjancency matrix
    Parameters
    -------------
    baseeSet - List of nodes for which adjacency matrix needs to be found

    Returns
    -------------
    adjancencyMatrix - adjacency matrix
    """
    subgraph = nx.subgraph(web_graph, sorted(baseSet))
    adjacencyMatrix = nx.to_numpy_array(subgraph)
    print(adjacencyMatrix)
    return adjacencyMatrix, list(subgraph.nodes)


def findHandA(adjacencyMatrix, baseSet):
    """
     Function to find and print the Authority and HUb scores of the nodes
    The Runtime of this HITS Algorithm is given as follows:
    ---------
    O(k*m)
        k:
            Total number edges present in the graph
        m: 
            Maximum number of iterations to get the probability vector
    """
    n = len(baseSet)
    hubValues = np.ones(n) / n
    authValues = np.ones(n) / n

    for i in range(2022):
        hubValues = np.dot(adjacencyMatrix, authValues)
        authValues = np.dot(adjacencyMatrix.T, hubValues)
        hubSum = sum(hubValues)
        authSum = sum(authValues)
        hubValues /= hubSum
        authValues /= authSum

    HubScores = []
    AuthorityScores = []
    for i in range(len(baseSet)):
        HubScores.append((baseSet[i], hubValues[i]))
        AuthorityScores.append((baseSet[i], authValues[i]))

    HubScores.sort(key=lambda x: x[1], reverse=True)
    AuthorityScores.sort(key=lambda x: x[1], reverse=True)

    print("\nHub Scores: ")
    for i in range(len(baseSet)):
        print("Node", HubScores[i][0], " : ", HubScores[i][1])

    print("\nAuthority Scores: ")
    for i in range(len(baseSet)):
        print("Node", AuthorityScores[i][0], " : ", AuthorityScores[i][1])


"""
    
"""
query = input("Enter a query word: ")
query = query.lower()
rootset = rootSet(query)
baseset = makeBaseSet(rootset)
adj, listNodes = makeAdjacencyMatrix(baseset)
print(rootset)
print(baseset)
findHandA(adj, listNodes)
