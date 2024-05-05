#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include "glm/gtx/hash.hpp"
#include <glm/glm.hpp> 

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/normal.hpp> 
#include "mesh.h"

// ChatGPT get quad normal

Mesh subdivide_tri(Mesh mesh);

Mesh subdivide_quad(Mesh mesh);

Mesh subdivide(Mesh mesh);

Mesh subdivide_level(Mesh mesh, int level);
