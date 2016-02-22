### Installation ###

In order to build library run:
```bash
mkdir build
cd build
cmake  -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT='your boost installation directory'  ..
make 
```
One can omit the BOOST_ROOT if the boost libraries are installed in the system directories.

### Usage ###

The running of the simulation is two phase. 
First run read_graph in one process to read your graph to the shared memory.
Next run binary simulate and use appropriate shared_name. For more details on usage:

```bash
./simulate -h
./read_graph -h
```
Graph should be a file with edges in format (follower -> followee).
