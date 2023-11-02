#!/usr/bin/env python3
import sys
import math
#import matplotlib
#matplotlib.use('Agg')
#import matplotlib.pyplot as plt
#import numpy as np
#import pylab as pl
#from matplotlib import collections as mc
from math import sqrt
import re
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
    #print(newlist)
    x1 = newlist[0][0]
    ##print("This is x1",x1)
    y1 = newlist[0][1]
    #print("This is y1", y1)
    x2 = newlist[1][0]
    #print("This is x2", x2)
    y2 = newlist[1][1]
    #print("This is y2", y2)
    x3 = newlist[2][0]
    #print("This is x3", x3)
    y3 = newlist[2][1]
    #print("This is y3", y3)
    x4 = newlist[3][0]
    #print("This is x4", x4)
    y4 = newlist[3][1]
    #print("This is y4", y4)
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
            px = float(x1) + tval * float(x2-x1)
            py = float(y1) + tval * float(y2-y1)
            intersect = (px, py)
    return intersect
def Vertex_generator(database):
    intersect = []
    Vertexes = []
    tuples = None
    newlist = []
    Edges = []
    keys_list= list(database.keys())
    #print("New keys",keys_list)
    for i in range(len(keys_list)):
        for j in range(i + 1, len(keys_list)):
            #print("This is i", i)
            #print("This is J", j)
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
                        intersect.append(tuples)
                        Edges.append((pt1,tuples))
                        Edges.append((pt2,tuples))
                        Edges.append((pt3,tuples))
                        Edges.append((pt4,tuples))
                    if (len(newlist) == 4):
                        newlist.clear()
    return Vertexes,intersect, Edges
def distance(x1,y1,x2,y2):
    distance = sqrt((x2-x1)**2 + (y2-y1)**2)
    return distance
def Edge_handler(V,I,E):
    #print(V)
    E_to_be_deleted = []
    for edge_index in range(len(E)):
        if (E[edge_index][0] == E[edge_index][1]):
            E_to_be_deleted.append(E[edge_index])
        trial_edge_src_x = E[edge_index][0][0]
        trial_edge_src_y = E[edge_index][0][1]
        trial_edge_dst_x = E[edge_index][1][0]
        trial_edge_dst_y = E[edge_index][1][1]
        #print ("Edge handler")
        #print(E[edge_index])
        #print(trial_edge_src_x)
        #print(trial_edge_src_y)
        #print(trial_edge_dst_x)
        #print(trial_edge_dst_y)
        trial_edge_dst = distance(trial_edge_src_x,trial_edge_src_y,trial_edge_dst_x,trial_edge_dst_y)
        #print(trial_edge_dst)
        #print(trial_edge_dst)
        for vertex_index in range(len(V)):
            #print("This vertex", V[vertex_index])
            if (V[vertex_index] == E[edge_index][0] or V[vertex_index] == E[edge_index][1]):
                continue
            else:
                Vertex_dst_x = V[vertex_index][0]
                Vertex_dst_y = V[vertex_index][1]
                Vertex_dst_src = distance(trial_edge_src_x,trial_edge_src_y,Vertex_dst_x,Vertex_dst_y)
                #print(Vertex_dst_src)
                Vertex_dst_dst = distance(Vertex_dst_x,Vertex_dst_y,trial_edge_dst_x,trial_edge_dst_y)
                #print(Vertex_dst_dst)
                dest = Vertex_dst_dst + Vertex_dst_src
                #print (dest)
                if (abs(dest - trial_edge_dst) <= 1e-6):
                    E_to_be_deleted.append(E[edge_index])
                    #print("Edges to be deleted",E_to_be_deleted)
                    break
    # https://www.geeksforgeeks.org/python-remove-all-values-from-a-list-present-in-other-list/
    E = [x for x in E if x not in E_to_be_deleted] 
    return E
def vertex_exist_within(intersect_src_x,intersect_src_y,intersect_dst_x,intersect_dst_y,V):
    vertex_exist_within = False
    for vertex_index in range(len(V)):
        if (V[vertex_index][0] == intersect_src_x and V[vertex_index][1] == intersect_src_y) or (V[vertex_index][0] == intersect_dst_x and V[vertex_index][1] == intersect_dst_y):
            continue
        else:
            trial_edge_dst = distance(intersect_src_x,intersect_src_y,intersect_dst_x,intersect_dst_y)
            Vertex_dst_x = V[vertex_index][0]
            Vertex_dst_y = V[vertex_index][1]
            Vertex_dst_src = distance(intersect_src_x,intersect_src_y,Vertex_dst_x,Vertex_dst_y)
          #      print(Vertex_dst_src)
            Vertex_dst_dst = distance(Vertex_dst_x,Vertex_dst_y,intersect_dst_x,intersect_dst_y)
           #     print(Vertex_dst_dst)
            if (abs(trial_edge_dst - (Vertex_dst_src + Vertex_dst_dst)) <= 1e-6):
                #print('vertex exist within, edge distance',trial_edge_dst)
                
                #print('vertex exist within, distance from edge to vertex',Vertex_dst_src + Vertex_dst_dst)
                vertex_exist_within = True
    return vertex_exist_within
def find_edges_among_intersects(database, I, E,V):
    new_Edges = []
    if (len(I) <= 1):
        return new_Edges
    else:
        for index_intersect in range(len(I)-1):
            for (index_next) in range(index_intersect, len(I)):
                #print(I[index_intersect])
                #print(I[index_next])
                #print(I[index_intersect][0],I[index_intersect][1],I[index_next][0],I[index_next][1])
                if (I[index_intersect] == I[index_next]):
                    continue
                ### check horizontal lines 
                if (I[index_intersect][1] == I[index_next][1]):
                    if (I[index_intersect][0] <= I[index_next][0]):
                        intersect_src_x = I[index_intersect][0]
                        intersect_src_y = I[index_intersect][1]
                        intersect_dst_x = I[index_next][0]
                        intersect_dst_y = I[index_next][1]
                        #print()
                    else:
                        intersect_src_x = I[index_next][0]
                        intersect_src_y = I[index_next][1]
                        intersect_dst_x = I[index_intersect][0]
                        intersect_dst_y = I[index_intersect][1]
                else:
                    if I[index_intersect][0] <= I[index_next][0]:
                        if I[index_intersect][1] <= I[index_next][1]:
                            intersect_src_x = I[index_intersect][0]
                            intersect_src_y = I[index_intersect][1]
                            intersect_dst_x = I[index_next][0]
                            intersect_dst_y = I[index_next][1]
                        else:
                            intersect_src_x = I[index_next][0]
                            intersect_src_y = I[index_next][1]
                            intersect_dst_x = I[index_intersect][0]
                            intersect_dst_y = I[index_intersect][1]
                    else:
                        if I[index_intersect][1] <= I[index_next][1]:
                            intersect_src_x = I[index_intersect][0]
                            intersect_src_y = I[index_intersect][1]
                            intersect_dst_x = I[index_next][0]
                            intersect_dst_y = I[index_next][1]
                        else:
                            intersect_src_x = I[index_next][0]
                            intersect_src_y = I[index_next][1]
                            intersect_dst_x = I[index_intersect][0]
                            intersect_dst_y = I[index_intersect][1]
                #print(intersect_src_x)
                #print(intersect_src_y)
                #print(intersect_dst_x)
                #print(intersect_dst_y)
                for key in database:
                    #print(database)
                    #print("Here are keys", key)
                    for value_index in range(0,len(database[key])-1):
                        #print("Here are the values",database[key])
                        if database[key][value_index][1] == database[key][value_index+1][1]:
                            if (database[key][value_index][0] <= database[key][value_index+1][0]):
                                database_src_x = database[key][value_index][0]
                                database_src_y = database[key][value_index][1]
                                database_dst_x = database[key][value_index+1][0]
                                database_dst_y = database[key][value_index+1][1]
                            else:
                                database_src_x = database[key][value_index+1][0]
                                database_src_y = database[key][value_index+1][1]
                                database_dst_x = database[key][value_index][0]
                                database_dst_y = database[key][value_index][1]     
                        else:
                            if database[key][value_index][0] <= database[key][value_index+1][0]:
                                if database[key][value_index][1] <= database[key][value_index+1][1]:
                                    database_src_x = database[key][value_index][0]
                                    database_src_y = database[key][value_index][1]
                                    database_dst_x = database[key][value_index+1][0]
                                    database_dst_y = database[key][value_index+1][1]
                                else:
                                    database_src_x = database[key][value_index+1][0]
                                    database_src_y = database[key][value_index+1][1]
                                    database_dst_x = database[key][value_index][0]
                                    database_dst_y = database[key][value_index][1]
                            else:
                                if database[key][value_index][1] < database[key][value_index+1][1]:
                                    database_src_x = database[key][value_index][0]
                                    database_src_y = database[key][value_index][1]
                                    database_dst_x = database[key][value_index+1][0]
                                    database_dst_y = database[key][value_index+1][1]    
                                else:
                                    database_src_x = database[key][value_index+1][0]
                                    database_src_y = database[key][value_index+1][1]
                                    database_dst_x = database[key][value_index][0]
                                    database_dst_y = database[key][value_index][1] 
                           
                        #print("Current database source", database[key][value_index])
                        #print("Current database dst", database[key][value_index+1])
                        #print("Current database source x",database_src_x)
                        #print("Current database source y",database_src_y)
                        #print("Current database dst x",database_dst_x)
                        #print("Current database dst y", database_dst_y)
                     ## edge case, when two of them is vertical
                        if (database_src_x == database_dst_x) and  (intersect_src_x == intersect_dst_x):
                            # overlap
                            if (database_src_x == intersect_src_x):
                                if (intersect_src_y >= database_src_y ) and (intersect_dst_y <= database_dst_y):
                                    if(vertex_exist_within(intersect_src_x,intersect_src_y,intersect_dst_x,intersect_dst_y,V) == False):
                                        tuple1 = (intersect_src_x, intersect_src_y)
                                        tuple2 = (intersect_dst_x, intersect_dst_y)
                                        newtuple = None
                                        newtuple =(tuple1,tuple2)
                                        #print("This is new tuple",newtuple)
                                        new_Edges.append(newtuple)
                        elif ((database_src_x == database_dst_x) and (intersect_dst_y == intersect_src_y)) or ((database_src_y == database_dst_y) and (intersect_src_x == intersect_dst_x)):
                            continue
                        else:
                            if database_src_x != database_dst_x and intersect_src_x != intersect_dst_x:
                                #print("Entered the condition when none of the two line are vertical")
                                if (intersect_src_x <= intersect_dst_x):
                                    if (intersect_src_y >= database_src_y ) and (intersect_dst_y <= database_dst_y):
                                        if (intersect_src_x >=  database_src_x) and (intersect_dst_x <= database_dst_x):
                                            rise_intersect = intersect_dst_y - intersect_src_y
                                            run_intersect = intersect_dst_x - intersect_src_x
                                            rise_db = database_dst_y - database_src_y
                                            run_db = database_dst_x - database_src_x
                                            #print("rise_intersect", rise_intersect)
                                            #print("run_intersect", run_intersect)
                                            #print("rise_db",rise_db)
                                            #print("run_db", run_db)
                                            if (abs(rise_intersect/run_intersect - rise_db / run_db) <=1e-6 ):
                                                if (vertex_exist_within(intersect_src_x,intersect_src_y,intersect_dst_x,intersect_dst_y,V) == False):
                                                    tuple1 = (intersect_src_x, intersect_src_y)
                                                    tuple2 = (intersect_dst_x, intersect_dst_y)
                                                    newtuple =(tuple1,tuple2)
                                                    #print("New tuple appended for rising lines going right", newtuple)
                                                    new_Edges.append(newtuple)
                                else:
                                    if (intersect_src_y >= database_src_y ) and (intersect_dst_y <= database_dst_y):
                                        if (database_src_x >= intersect_src_x) and (database_dst_x <= intersect_dst_x):
                                            rise_intersect = intersect_dst_y - intersect_src_y
                                            run_intersect = intersect_dst_x - intersect_src_x
                                            rise_db = database_dst_y - database_src_y
                                            run_db = database_dst_x - database_src_x
                                            #print("rise_intersect", rise_intersect)
                                           # print("run_intersect", run_intersect)
                                           # print("rise_db",rise_db)
                                           # print("run_db", run_db)
                                            if (abs(rise_intersect/run_intersect - rise_db / run_db) <=1e-6 ):
                                                if (vertex_exist_within(intersect_src_x,intersect_src_y,intersect_dst_x,intersect_dst_y,V) == False):
                                                    tuple1 = (intersect_src_x, intersect_src_y)
                                                    tuple2 = (intersect_dst_x, intersect_dst_y)
                                                    newtuple =(tuple1,tuple2)
                                                    #print("New tuple appended for rising lines going left", newtuple)
                                                    new_Edges.append(newtuple)
    return new_Edges           
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
            raise Exception('Error: street can\'t be removed because it does not exist')
        self.data.pop(StreetName)
    def modify(self, StreetName, tuples):
        exits = False
        for key,val in self.data.items():
            if (key == StreetName):
                exits = True    
        if (exits == False):
            raise Exception('Error: street can\'t be added because it does not exist')
        self.data[StreetName] = tuples
def parseLine(line):
    command = ""
    streetName = ""
    tuples = [] 
    line=line.strip()
    for c in line:
        if not c.isspace():
            command = command + c
        else:
            break
    line = line[len(command):] 
    if command == "add":
        ## delete white space
        line=line.strip()
        ###"C"(,lll)
        #print(line)
        if line[0] != '\"':
            raise Exception("Error: bad input1")
        else:
            ## get rid of starting quote
            line = line[1:]
            streetName = ""
            encounteredEndQuote = False
            for c in line:
                 #line = line[len(streetName):] does not delete the "
                if c == '\"':
                    encounteredEndQuote = True
                    line = line[1:]
                    break
                else:
                    streetName = streetName + c
                    
            if not encounteredEndQuote:
                raise Exception("Error: bad input2")
            ## parse line again
            line = line[len(streetName):]
            if (len(line) == 0) or (not line[0].isspace()):
                raise Exception("Error: bad input3")

            while len(line) > 0:
                if (not line[0].isspace()):
                    raise Exception("Error: bad input4")
                line=line.strip()
                currentCoordinate = ""
                #print(line)
                if line[0] != '(':
                    raise Exception("Error: bad input4")
                line = line [1:]
                ecnountedclosingbracket = False 
                for index in range(len(line)):
                    #print(line[index])
                    if  line[index] == ')':
                        ecnountedclosingbracket = True
                        line = line[:index] + line[index+1:]
                        #print(line)
                        break
                    else:
                        currentCoordinate = currentCoordinate + line[index]
                #print(currentCoordinate)
                if not ecnountedclosingbracket:
                    raise Exception("Error: bad input5")
                numberofspaces = re.findall("\s+", currentCoordinate)
                counter = 0
                for index in range(len(numberofspaces)):
                    for space in numberofspaces[index]:
                        counter = counter +1
                currentCoordinate = re.sub(r"\s+", '', currentCoordinate)
                #print(currentCoordinate)
                coordinateMatch = re.findall("^(-?[0-9]+),(-?[0-9]+)$", currentCoordinate)
                #print(len(coordinateMatch))
                if len(coordinateMatch) != 1:
                    raise Exception("Error: bad input5")
                firstNum = int(coordinateMatch[0][0])
                secondNum = int(coordinateMatch[0][1])
                tuples.append((firstNum, secondNum))
                #print(len(currentCoordinate))
                line = line[len(currentCoordinate)+counter:]
                #print(line)
            if len(tuples) < 2:
                raise Exception("Error: bad input6")
                                        
    elif command == "mod":
        ## delete white space
        line=line.strip()
        ###"C"(,lll)
        #print(line)
        if line[0] != '\"':
            raise Exception("Error: bad input1")
        else:
            ## get rid of starting quote
            line = line[1:]
            streetName = ""
            encounteredEndQuote = False
            for c in line:
                 #line = line[len(streetName):] does not delete the "
                if c == '\"':
                    encounteredEndQuote = True
                    line = line[1:]
                    break
                else:
                    streetName = streetName + c
                    
            if not encounteredEndQuote:
                raise Exception("Error: bad input2")
            ## parse line again
            line = line[len(streetName):]
            if (len(line) == 0) or (not line[0].isspace()):
                raise Exception("Error: bad input3")

            while len(line) > 0:
                if (not line[0].isspace()):
                    raise Exception("Error: bad input4")
                line=line.strip()
                currentCoordinate = ""
                #print(line)
                if line[0] != '(':
                    raise Exception("Error: bad input4")
                line = line [1:]
                ecnountedclosingbracket = False 
                for index in range(len(line)):
                    if  line[index] == ')':
                        ecnountedclosingbracket = True
                        line = line[:index] + line[index+1:]
                        #print(line)
                        break
                    else:
                        currentCoordinate = currentCoordinate + line[index]
                #print(currentCoordinate)
                if not ecnountedclosingbracket:
                    raise Exception("Error: bad input5")
                numberofspaces = re.findall("\s+", currentCoordinate)
                counter = 0
                for index in range(len(numberofspaces)):
                    for space in numberofspaces[index]:
                        counter = counter +1
                currentCoordinate = re.sub(r"\s+", '', currentCoordinate)
                coordinateMatch = re.findall("^(-?[0-9]+),(-?[0-9]+)$", currentCoordinate)
                if len(coordinateMatch) != 1:
                    raise Exception("Error: bad input5")
                firstNum = int(coordinateMatch[0][0])
                secondNum = int(coordinateMatch[0][1])
                tuples.append((firstNum, secondNum))
                #print(len(currentCoordinate))
                line = line[len(currentCoordinate)+counter:]
                #print(line)
            if len(tuples) < 2:
                raise Exception("Error: bad input6")
        #print(command)
        #print(streetName)
        #print(tuples)  
    elif command == "rm":
        ## delete white space
        line=line.strip()
        
        if line[0] != '\"':
            raise Exception("Error: bad input1")
        else:
            ## get rid of starting quote
            line = line[1:]
            streetName = ""
            encounteredEndQuote = False
            for c in line:
                 #line = line[len(streetName):] does not delete the "
                if c == '\"':
                    encounteredEndQuote = True
                    line = line[1:]
                    break
                else:
                    streetName = streetName + c
                    
            if not encounteredEndQuote:
                raise Exception("Error: bad input2")
            line = line[len(streetName):]
            line= line.strip()
            if (len(line)>0):
                raise Exception("Error: bad input2")
            ## parse line again

    elif command == 'gg':
        line = line.strip()
        if (len(line) >= 1):
            raise Exception("Error: bad input2")

    else:
        raise Exception("Error: bad input")  
    #print(command)
    #print(streetName)
    #print(tuples)                 
    return command, streetName, tuples
class Graph(object):
    def __init__(self,database, Vertex, E):
        self.D = database
        self.vertexdata = Vertex
        self.edgedata = E
        self.output_vertex ={}
        self.output_Edge = []
    def generate_outputvertex(self):
        for i in range(len(self.vertexdata)):
            self.output_vertex['{0}'.format(i+1)]=self.vertexdata[i]            
        return self.output_vertex
    def generate_outputEdge(self):
        element1 = None
        element2 = None
        count = 0
        for index in range(len(self.edgedata)):
            for i in range(len(self.edgedata[index])-1):
                for key,value in self.output_vertex.items():
                    if (self.edgedata[index][i] == value):
                        element1 = int(key)
                        count = count + 1
                    if (self.edgedata[index][i+1] == value):
                        element2 = int(key)
                        count = count + 1
                    if (count == 2):
                        tuple = (element1, element2)
                        self.output_Edge.append(tuple)
                        count = 0
        return self.output_Edge
def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    V = {}
    I=[]
    E = []
    database = {}
    a = street_Database()
    graphs = None
    while True:
        line = sys.stdin.readline()
        if line.strip() == "":
            break
        try:
            cmd, StreetName, tuples=parseLine(line)
            if (cmd == 'add'):
                a.add(StreetName,tuples)
            elif (cmd == 'mod'):
                a.modify(StreetName,tuples)
            elif (cmd == 'rm'):
                a.remove(StreetName,tuples)
            elif (cmd == 'gg'):
                database = a.generate()
                #print(database)
                
                #plot_function(database)  
                V,I,E=Vertex_generator(database)
                V=list(set(V))
                #print("This is Vertex", V)
                I=list(set(I))
                
                #print("This is Intercepts", I)
                E=list(set(E))
                #print("This is Edges", E)
                E=Edge_handler(V,I,E)
                
                #print("This is Edges", E)
                E2=find_edges_among_intersects(database,I, E,V)

                #print("needed to be added", (list(set(E2))))
                E2=list(set(E2))
                for value in E2:
                    E.append(value)
                #print (E)
                E = list(set(E))
                b = Graph(database, V, E)

                #print("new edges", E2)
                #graphs = Graph(database,V)
                d = b.generate_outputvertex()
                length = len(d)
                print('V {}'.format(length), flush=True)
                #for key,value in d.items():
                 #   v1 = "{0:.2f}".format(value[0])
                  #  v2 = "{0:.2f}".format(value[1])
                    #formatted_value = (v1,v2)
                   # print(f'  {int(key)}:  ({v1},{v2})')
                #print("}")
                c = b.generate_outputEdge()
                d = []
                for i in range(len(c)):
                    for j in range(i+1,len(c)):
                        if (c[i][0] == c[j][1]) and (c[i][1] == c[j][0]):
                            d.append(c[i])
                 # https://www.geeksforgeeks.org/python-remove-all-values-from-a-list-present-in-other-list/
                c = [x for x in c if x not in d]
                print("E = {", end='', flush=True)
                for index in range(len(c)):
                    if (index != len(c)-1):
                        print("<{0},{1}>,".format(c[index][0],c[index][1]),end='', flush=True)
                    else:
                        print("<{0},{1}>".format(c[index][0],c[index][1]),end='', flush=True)
                print("}", flush=True)

        except Exception as e:
            print('Error: ' + str(e), file=sys.stderr) 

        # method to access each individual element in tuple
        #print (tuples[0][1])
        #print("read a line:", line)
    #print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
