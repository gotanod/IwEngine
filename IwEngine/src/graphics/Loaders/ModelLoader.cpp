#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <string>

namespace IwGraphics {
	ModelData* ModelLoader::LoadAsset(
		const char* path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
		    | aiProcess_GenNormals);

		if (!scene) {
			LOG_WARNING << importer.GetErrorString();
			return nullptr;
		}

		ModelData* model = new ModelData {
			new MeshData[scene->mNumMeshes],
			scene->mNumMeshes
		};

		for (size_t i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* aimesh = scene->mMeshes[i];

			size_t indexBufferSize = 0;
			for (size_t t = 0; t < aimesh->mNumFaces; t++) {
				indexBufferSize += aimesh->mFaces[t].mNumIndices;
			}

			MeshData& mesh = model->Meshes[i];
			
			mesh.FaceCount = 0;
			mesh.Faces = new unsigned int[indexBufferSize];

			if (mesh.Faces) {
				for (size_t t = 0; t < aimesh->mNumFaces; t++) {
					const aiFace& face = aimesh->mFaces[t];
					for (size_t f = 0; f < face.mNumIndices; f++) {
						mesh.Faces[mesh.FaceCount] = face.mIndices[f];
						mesh.FaceCount++;
					}
				}
			}

			mesh.VertexCount = aimesh->mNumVertices;
			mesh.Vertices = new IwGraphics::Vertex[mesh.VertexCount];

			if (mesh.Vertices) {
				for (size_t i = 0; i < mesh.VertexCount; i++) {
					mesh.Vertices[i].Vertex.x = aimesh->mVertices[i].x;
					mesh.Vertices[i].Vertex.y = aimesh->mVertices[i].y;
					mesh.Vertices[i].Vertex.z = aimesh->mVertices[i].z;

					mesh.Vertices[i].Normal.x = aimesh->mNormals[i].x;
					mesh.Vertices[i].Normal.y = aimesh->mNormals[i].y;
					mesh.Vertices[i].Normal.z = aimesh->mNormals[i].z;
				}
			}
		}

		importer.FreeScene();

		return model;
	}
}