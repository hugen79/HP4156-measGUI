#!/usr/bin/env python 
import matplotlib.pyplot as plt 
import numpy
import sys

from matplotlib import rc
rc('xtick', labelsize=20) 
rc('ytick', labelsize=20) 
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica'],'size':20})
plt.rcParams.update({'legend.fontsize':16,'legend.linewidth':2})
rc('text', usetex=True)

def stream_lines(file_name):
    _file = open(file_name)
    while True:
      line = _file.readline()
      if not line:
        _file.close()
        break
      yield line

def _getdata(file_name):
    gen  = stream_lines(file_name)
    keys = gen.next().split()
    data = dict((k, []) for k in keys)
    while True:
        try:
            line = gen.next().split()
            for i,k in enumerate(keys):
                data[k].append(float(line[i]))
        except:
            return data

if __name__ == "__main__": 
    data = []
    for pathname in sys.argv:
        if pathname == sys.argv[0]:
            continue
        data.append(_getdata(pathname))
    for D in data:
        plt.plot(D["VG"], [i for i in D["ID"]])
            
    plt.xlabel("Gate Voltage $(V_g)$")
    plt.ylabel("Drain Current $(I_d)$")
    plt.title("$(V_d = 100mV)$")
    plt.show()
