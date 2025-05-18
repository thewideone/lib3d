#include "../Inc/lib3d_mesh.h"
#include "stdlib.h"

/*
State how big scene can be. If we can't state that, we can't rasterize the scene.

Put all vertices, faces and edges together into their buffers.
Each mesh would then access the data by indexes in buffers.
This allows to avoid memory fragmentation.
*/
