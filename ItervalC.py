import numpy as np
import scipy as sp
from scipy import stats
from array import array
from numpy import zeros
import sys
def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), sp.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    return h 
    


file = open(sys.argv[1],"r")

leng = len(file.readlines())



file.seek(0)




k = 0
a = zeros([leng])
for i in file.read().split('\n'):
	if len(i):
		a[k] = float(i)
		k = k + 1


print mean_confidence_interval(a)
