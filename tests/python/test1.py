#!/usr/bin/env python

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

if __name__ == '__main__':

    unittest.main()

