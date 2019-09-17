		STARE_ArrayIndexSpatialValue siv1[2] = { 0x0000000000000008, 0x000067ffffffffff };
		STARE_ArrayIndexSpatialValue siv2[2] = { 0x000030000000000a, 0x0000907fffffffff };


import numpy as np
import pystare as ps
a = np.array([0x0000000000000008, 0x000030000000000a, 0x000067ffffffffff, 0x000070000000000a, 0x0000907fffffffff],dtype=np.int64)
ps.from_intervals(a)

