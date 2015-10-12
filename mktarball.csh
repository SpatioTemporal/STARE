#!/bin/csh -f
cd ../
set n=`/bin/date +"htm_%Y_%m%d_%H%M".tar.gz`
/usr/bin/tar -zvcf $n ./htm
