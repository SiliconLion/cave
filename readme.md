# Cave
Cave is an (in-progress) collection of small open source libraries written in C99
with no dependencies other than the C99 standard library.
These libraries are currently mostly related to computer graphics. 
Over time I may work to make these libraries more compatible with code that needs
full control over when and where memory is allocated. For now, malloc is called when I want. 

## Libraries Provided
- PolyTri : PolyTri is a library for dividing polygons into triangles.
- CaveWriter : A library for reading and writing 3D file formats. 
Works both with Cave types and user defined types (coming soon).
Currently, only supports binary STL files, but OBJ coming soon, and perhaps more in the future.
- Bedrock: Foundational data-structures for the rest of Cave.

## Building and Using Cave
The build story here is currently pretty rudimentary and somewhat manual, though I intend to 
make it more seamless in the future. In particular, everything builds right now as one library, but
I want to make it more composable/granular in the future.

### For use on Unix Systems
From inside the cave directory,
```bash
mkdir build
cd build
cmake ..
make
```

Then the copy all the files from `include` into the project you want to include
Cave into. Then copy `build/src/libCAVE.a` to somewhere the linker for your project
can see it. That should be all you need to get Cave to work in your project.

### For use on Windows 
I currently don't have a Windows machine to test building on, but it should be 
basic CMake and very similar to the Unix instructions

## Tests
Testing right now is also less smooth than ideal, but not so bad really.
Build Cave, and then in `build/cave/tests`, run the test executable you're interested in.
For example, 
```bash
cd build/tests
./CAVE_TESTS_WRITER
```
If the program exits with `0`, then the test passed! (it will also print out that it passed).
Otherwise it should try to give a *reasonably* good idea of what went wrong.