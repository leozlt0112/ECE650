#!/usr/bin/env python3
import sys
import math
# YOUR CODE GOES HERE
class street_Database(object):
    def __init__(self, data={}):
        self.data = data
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
def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    a = street_Database()
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
            print(a.generate())        
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
