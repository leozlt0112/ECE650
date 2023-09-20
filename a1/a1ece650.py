#!/usr/bin/env python3
import sys
import math
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import pylab as pl
from matplotlib import collections as mc
# YOUR CODE GOES HERE
def plot_function(database):
    lines = []
    for key, value in database.items():
        lines.append(value)
   # print (lines)
    c = np.array([(1, 0, 0, 1), (0, 1, 0, 1), (0, 0, 1, 1)])
    lc = mc.LineCollection(lines, colors=c, linewidths=2)
    fig, ax = pl.subplots()
    ax.add_collection(lc)
    ax.autoscale()
    ax.margins(0.1)
    pl.savefig('plot.png')
def check_intersect(newlist):
    intersect = None
    print(newlist)
    x1 = newlist[0][0]
    print("This is x1",x1)
    y1 = newlist[0][1]
    print("This is y1", y1)
    x2 = newlist[1][0]
    print("This is x2", x2)
    y2 = newlist[1][1]
    print("This is y2", y2)
    x3 = newlist[2][0]
    print("This is x3", x3)
    y3 = newlist[2][1]
    print("This is y3", y3)
    x4 = newlist[3][0]
    print("This is x4", x4)
    y4 = newlist[3][1]
    print("This is y4", y4)
    ## please add corner case to check for slope = 0 case

    flag = True
    if ((x2 - x1 == 0) and (x4 - x3 ==0)):
        flag = False

    if (x2-x1 != 0) and (x4-x3 !=0):
        if ((y2-y1) / (x2-x1)) == ((y4-y3) / (x4-x3)):
             flag=False
    
    if (flag == True):
        tnum = (x1-x3)*(y3-y4)-(y1-y3)*(x3-x4) 
        tden = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4)
        tval = tnum / tden
        unum = (x1-x3)*(y1-y2)-(y1-y3)*(x1-x2)
        uden = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4)
        uval = unum / uden
        if (uval >= 0 and uval <=1 and tval >=0  and tval <=1):
            px = x1 + tval * (x2-x1)
            py = y1 + tval * (y2-y1)
            intersect = (px, py)
    return intersect
def Vertex_generator(database):
    Vertexes = []
    tuples = None
    newlist = []
    keys_list= list(database.keys())
    print("New keys",keys_list)
    for i in range(len(keys_list)):
        for j in range(i + 1, len(keys_list)):
            print("This is i", i)
            print("This is J", j)
            first_key = keys_list[i]
            second_key = keys_list[j]
            for k in range(len(database[first_key])-1):
                for l in range(len(database[second_key])-1):
                    pt1 = database[first_key][k]
                    newlist.append(pt1)
                    pt2 = database[first_key][k+1]
                    newlist.append(pt2)
                    pt3 = database[second_key][l]
                    newlist.append(pt3)
                    pt4 = database[second_key][l+1]
                    newlist.append(pt4)
                    tuples=check_intersect(newlist)
                    if (tuples!= None):
                        Vertexes.append(pt1)
                        Vertexes.append(pt2)
                        Vertexes.append(pt3)
                        Vertexes.append(pt4)
                        Vertexes.append(tuples)
                    if (len(newlist) == 4):
                        newlist.clear()
    return Vertexes
class street_Database(object):
    def __init__(self):
        self.data = {}
    def generate(self):
        return self.data
    def add(self, StreetName, tuples):
        for key,value in self.data.items():
            if StreetName == key:
                raise Exception('Error: duplicate street being added')
        self.data[StreetName] = tuples
    def remove(self, StreetName, tuples):
        exits = False
        for key,val in self.data.items():
            if (key == StreetName):
                exits = True    
        if (exits == False):
            raise Exception('Error: street can;t be removed because it does not exist')
        self.data.pop(StreetName)
    def modify(self, StreetName, tuples):
        exits = False
        for key,val in self.data.items():
            if (key == StreetName):
                exits = True    
        if (exits == False):
            raise Exception('Error: street can;t be removed because it does not exist')
        self.data[StreetName] = tuples
def parseLine(line):
    sp = line.strip().split('\"')
    cmd = None
    StreetName = None
    tuples = [] 
    cmd = sp[0].strip().split()
    if (cmd[0] == 'gg'):
        if (len(sp) > 1):
            raise Exception('Error: too many arguments')
    elif (cmd[0] == 'rm'):
        if (sp[2] !=''):
            raise Exception('Error: too many arguments')
        StreetName = sp[1]
    elif (cmd[0] == 'add'):
        StreetName = sp[1]
        temp_coor = sp[2].strip().split()
        for i in range(0,len(temp_coor)):
            tuples.append(eval(str(temp_coor[i])))
    elif (cmd[0] == 'mod'):
        StreetName = sp[1]
        temp_coor = sp[2].strip().split()
        for i in range(0,len(temp_coor)):
            tuples.append(eval(str(temp_coor[i])))
    else:
        raise Exception('Error: Unknown Command') 
    return cmd[0], StreetName, tuples
class Graph(object):
    def __init__(self,database, Vertex):
        self.D = database
        self.vertexdata = Vertex
        self.output_vertex ={}
    def generate_outputvertex(self):
        for i in range(len(self.vertexdata)):
            self.output_vertex['{0}'.format(i+1)]=self.vertexdata[i]
        return self.output_vertex
def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    V = {}
    database = {}
    a = street_Database()
    graphs = None
    while True:
        line = sys.stdin.readline()
        if line == "":
            break
        #try:
        cmd, StreetName, tuples=parseLine(line)
        if (cmd == 'add'):
            a.add(StreetName,tuples)
        elif (cmd == 'mod'):
            a.modify(StreetName,tuples)
        elif (cmd == 'rm'):
            a.remove(StreetName,tuples)
        elif (cmd == 'gg'):
            database = a.generate()
            print(database)
            plot_function(database)  
            V=Vertex_generator(database)
            V=list(set(V))
            print("This is Vertex", V)
            graphs = Graph(database,V)
            c = graphs.generate_outputvertex()
            print(c)
        #except Exception as e:
         #   print('Error: ' + str(e), file=sys.stderr) 

        # method to access each individual element in tuple
        #print (tuples[0][1])
        #print("read a line:", line)
    print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
