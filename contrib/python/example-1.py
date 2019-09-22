
import numpy as np
import pystare as ps
a = np.array([0x0000000000000008, 0x000030000000000a, 0x000067ffffffffff, 0x000070000000000a, 0x0000907fffffffff],dtype=np.int64)
starts,ends = ps.from_intervals(a)
for i in range(starts.size):
  print(i,hex(starts[i]),hex(ends[i]))

