

void tmp(bool onepass)
{
  Key lo0, hi0;
  Key lo1, hi1;
  Key oldLo, newLo;
  Key oldHi, newHi;
  uint32 level0;
  
  // Start at the beginning
  my_los->reset(); my_his->reset();

  //
  bool done    = false;
  bool changed = false;

  while(not done) {

    lo0 = my_los->getkey(); hi0 = my_his->getkey();

    if(lo0 < 0) { // Made it to the end
      if( not changed || onepass ) {
	done = true;
      } else {
	changed = false; // try again, unless one_pass is set
      }
    } else { // Got data to work on
      level0 = encoding->levelById(lo0);
      
      // Can we combine any intervals?
      bool done_defrag = false;
      while(not done_defrag) {
	// Peek ahead
	my_los->step(); my_his->step();
      
	lo1    = my_los->getkey();
	level1 = encoding->levelById(lo0);
	hi1    = my_his->getkey();

	// Do not change level? level0 .eq. level1 ?
	if( level0 != level1 ) {
	  done_defrag = true;
	} else {
	  Key hi_test = encoding->predecessorToLowerBound_NoDepthBit(lo1,level1);
	  if( hi_test != hi0 ) { // Is this too strict?
	    done_defrag = true;
	  } else {
	    // lo0..hi0 connects to lo1..hi1

	    // stitch them together
	    my_los->step();
	    Key lo_next = my_los->getkey();
	    my_los->free(lo1); // lo1 goes away.
	    if( lo_next >= 0 ) {
	      my_los->search(lo_next,1); // reset iter for stepping
	    }

	    my_his->step();
	    Key hi_next = my_his->getkey();
	    my_his->free(hi0); // hi0, its pred, goes away
	    if( hi_next ) {
	      my_his->search(hi_next,1); // reset iter for stepping (the '1' indicates setting iter)
	    }

	    // my_los->insert(my_los->getkey(),hi_next); Should be the following...
	    my_los->insert(lo0,hi1); // New interval is lo0..hi1.
	    my_his->insert(h1,3000000000+my_his->get_value()); // my_his at hi1...

	    // Get ready for the next iteration.
	    // lo1 is unchanged
	    hi0 = hi1;
	    done_defrag = false; // Still false, try again.

	    // hi0 will grow until we've eaten all of the contiguous intros
	  } // end hi_test == hi0
	} // end level0 == level1.
      } // end while not done_defrag

      // lo1..hi1 is the next interval, lo0..hi0 is the current one.
      lo1    = my_los->getkey();
      level1 = encoding->levelById(lo0);
      hi1    = my_his->getkey();

      // Check to see if we have enough here to coalesce.
      encoding->setId(lo0);
      uint64 bareLo = encoding->bareId();
      triangleNumber0 = encoding->getLocalTriangleNumber();
    
      Key hi0_id = encoding->idFromTerminatorAndLevel_NoDepthBit(hi0,level0);
      encoding->setId(hi0_id);
      uint64 bareHi = encoding->bareId();
      
      uint64 delta = bareHi - bareLo; // How many triangles are here (-1)?

      // There are no cycles of triangles in [bareLo..bareHi].
      if( delta >= (uint64) (3 + triangleNumber0) ) {
	// There are cycles of triangles in [bareLo..bareHi]!

	// Find the first tNum 0.
	uint64 delta_to_first = (4 - triangleNumber0) % 4;

	// Cut to tNum 0.
	
	Key lo_cut = lo0;
	for(int i = 0; i< delta_to_first; ++i ) {
	  lo_cut = encoding->increment(lo_cut,level0);
	}
	uint32 level_lo_cut = level0 - 1;

	// Are we dealing with the root polygon?
	if( level_lo_cut >= 0) {
	  Key hi0_cutm = encoding->predecessorToLowerBound_NoDepthBit(lo_cut,level_lo_cut);
	  // Make the changes
	  my_los->insert(lo0,hi0_cutm);         // Change l0..h0, l0..hi0_cutm
	  my_his->insert(hi0_cutm,4000000000);  // Change l0..h0, l0..hi0_cutm
	  my_los->insert(lo_cut,hi0);           // Add cut lo_cut..hi0
	  changed = true;
	} // my_los, my_his updated
      } // endif if "delta" >= 4
    } // end else there's work to do
  } // While not done
} // function def.


