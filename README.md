The running of the simulation is two phase. 
First run read_graph in separate tab to read your graph to the shared memory.
Run simulate and use appropriate shared_name. Use ./simulate -h and read_graph -h for more details.

In order to build library run:
mkdir build
cd build
cmake  -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT='your boost installation directory'  ..
make 

One can omit the BOOST_ROOT if the boost libraries are installed in the system directories.


Graph should be file with edges in  format (follower -> followee).
