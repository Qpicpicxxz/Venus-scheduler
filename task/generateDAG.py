import random

# total num of nodes
n = 10
# input p is probability
def prob_value(p):
    # converts the probability value to an integer
    q = int(10*p)
    l = [1]*q+[0]*(10-q)
    # randomly to select one element from the list "l"
    item = random.sample(l,1)[0]
    # 0 or 1
    return item
# input degree list
# node list of in/out-degree
in_degree = [0]*n
out_degree = [0]*n
# edge list
edges = []
# traverse nodes and add edges
for i in range(n-1):
    for j in range(i+1,n):
        # don't connect entrance and exit directly
        if i==0 and j==n-1:
            continue
        # the possibility of making edges is 0.4
        prob = prob_value(0.4)
        if prob:
            # limit the in/out-degree of the nodes <= 4
            if out_degree[i]<4 and in_degree[j]<4:
                # add edge
                edges.append((i,j))
                in_degree[j]+=1
                out_degree[i]+=1
# make all the nodes have no dependency to depend on root
for node,id in enumerate(in_degree):
    if node!=0:
        if id ==0:
            edges.append((0,node))
            out_degree[0]+=1
            in_degree[node]+=1
# make all the nodes have no successor to pass on sink
for node,od in enumerate(out_degree):
    if node!=n-1:
        if od ==0:
            edges.append((node,n-1))
            out_degree[node]+=1
            in_degree[n-1]+=1

def write_code(edges):
    with open('../src/DAG.c', 'w') as f:
        f.write('#include "task.h"\n\n')
        f.write('void random_DAG(void){\n')
        f.write('  /* means to specify a actor (node) */\n')
        for i in range (1, n+1):
            f.write(f'  actor_t* A{i} = actor_create(TASK1_START, TASK1_LEN, 4);\n')
        f.write('  /* meas to specify dependencies */\n')
        for u, v in edges:
            f.write(f'  edge_make(A{u+1}, A{v+1});\n')
        f.write('  /* means to specify root and sink node of DAG */\n')
        f.write('  assign_root(A1);\n')
        f.write(f'  assign_sink(A{n});\n')
        f.write('}')

write_code(edges)
