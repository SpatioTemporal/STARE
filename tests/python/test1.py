#!/usr/bin/env python

import numpy   as np
import pystare as ps
import unittest

def throw_test():
    raise Exception('test_a')
    return



class test0(unittest.TestCase):
    
    def test_a(self):
        self.assertRaises(Exception,throw_test)
        # throw_test()
        return

    def test_b(self):
        expected = \
        [(    0, 4430603050402447369 )
        ,(  295, 4430595237856935950 )
        ,(  590, 4430626891765907470 )
        ,(  885, 4430691958372958222 )
        ,( 1180, 4430869121478950926 )]
        cover = ps.to_circular_cover(1.5,0.5,0.25,14)
        for i in list(expected):
            self.assertEqual(i[1],cover[i[0]])
        return

    def test_ng1(self):
        a = [4114022797720682508, 4505997421712506892, 4505997834029367308, 4505997868389105676, 4505998418144919564]
        b = [4528191461944221900, 4505997456072245260]
        for i in range(1,10000):        
            ps.intersect(a, b)
        self.assertTrue(True)
        return

    def test_intersect_single_res(self):
        resolution = 6
        resolution0 = resolution; ntri0 = 1000
        lat0 = np.array([ 10, 5, 60,70], dtype=np.double)
        lon0 = np.array([-30,-20,60,10], dtype=np.double)
        hull0 = ps.to_hull_range_from_latlon(lat0,lon0,resolution0,ntri0)
                                              
        resolution1 = resolution; ntri1 = 1000
        lat1 = np.array([10,  20, 30, 20 ], dtype=np.double)
        lon1 = np.array([-60, 60, 60, -60], dtype=np.double)
        hull1 = ps.to_hull_range_from_latlon(lat1,lon1,resolution1,ntri1)

        intersectedFalse = np.full([1000],-1,dtype=np.int64)
        intersectedTrue  = np.full([1000],-1,dtype=np.int64)
        intersectedFalse = ps.intersect(hull0,hull1,multiresolution=False)
        intersectedTrue  = ps.intersect(hull0,hull1,multiresolution=True)

        self.assertEqual(328,len(intersectedFalse))
        self.assertEqual(172,len(intersectedTrue))
        
        # print('false\n',len(intersectedFalse))
        # print('true\n',len(intersectedTrue))
        
        return

if __name__ == '__main__':

    unittest.main()

