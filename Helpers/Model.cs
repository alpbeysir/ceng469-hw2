// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using Silk.NET.Assimp;
using Silk.NET.OpenGL;
using AssimpMesh = Silk.NET.Assimp.Mesh;

namespace HW2.Helpers
{
    public class Model : IDisposable
    {
        public Model(GL gl, string path, bool gamma = false)
        {
            var assimp = Assimp.GetApi();
            _assimp = assimp;
            _gl = gl;
            LoadModel(path);
        }

        private readonly GL _gl;
        private readonly Assimp _assimp;
        public string Directory { get; protected set; } = string.Empty;
        public List<Mesh> Meshes { get; protected set; } = [];

        private unsafe void LoadModel(string path)
        {
            var scene = _assimp.ImportFile(path, (uint)PostProcessSteps.Triangulate);

            if (scene == null || scene->MFlags == Assimp.SceneFlagsIncomplete || scene->MRootNode == null)
            {
                var error = _assimp.GetErrorStringS();
                throw new Exception(error);
            }

            Directory = path;

            ProcessNode(scene->MRootNode, scene);
        }

        private unsafe void ProcessNode(Node* node, Scene* scene)
        {
            for (var i = 0; i < node->MNumMeshes; i++)
            {
                var mesh = scene->MMeshes[node->MMeshes[i]];
                Meshes.Add(ProcessMesh(mesh, scene));
            }

            for (var i = 0; i < node->MNumChildren; i++)
            {
                ProcessNode(node->MChildren[i], scene);
            }
        }

        private unsafe Mesh ProcessMesh(AssimpMesh* mesh, Scene* scene)
        {
            // data to fill
            List<Vertex> vertices = [];
            List<uint> indices = [];

            // walk through each of the mesh's vertices
            for (uint i = 0; i < mesh->MNumVertices; i++)
            {
                Vertex vertex = new()
                {
                    Position = mesh->MVertices[i]
                };

                // normals
                if (mesh->MNormals != null)
                    vertex.Normal = mesh->MNormals[i];

                vertices.Add(vertex);
            }

            for (uint i = 0; i < mesh->MNumFaces; i++)
            {
                Face face = mesh->MFaces[i];
                for (uint j = 0; j < face.MNumIndices; j++)
                    indices.Add(face.MIndices[j]);
            }

            // return a mesh object created from the extracted mesh data
            var result = new Mesh(_gl, BuildVertices(vertices), BuildIndices(indices));
            return result;
        }

        private float[] BuildVertices(List<Vertex> vertexCollection)
        {
            var vertices = new List<float>();

            foreach (var vertex in vertexCollection)
            {
                vertices.Add(vertex.Position.X);
                vertices.Add(vertex.Position.Y);
                vertices.Add(vertex.Position.Z);
                vertices.Add(vertex.Normal.X);
                vertices.Add(vertex.Normal.Y);
                vertices.Add(vertex.Normal.Z);
            }

            return vertices.ToArray();
        }

        private uint[] BuildIndices(List<uint> indices)
        {
            return indices.ToArray();
        }

        public void Dispose()
        {
            foreach (var mesh in Meshes)
            {
                mesh.Dispose();
            }
        }
    }
}
