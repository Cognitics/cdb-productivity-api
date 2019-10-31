#include <fstream>
#include <iomanip>
#include <ccl/FileInfo.h>
#include "scenegraphfbx/scenegraphfbx.h"
#include <fbxsdk.h>
#include <fbxsdk\scene\shading\fbxtexture.h>
#include <fbxsdk\core\fbxmanager.h>
#include <vector>
#include <memory>

namespace scenegraph
{
    struct SceneFbxBuilder
    {
        std::string outputNameFbx;
        std::string tilename;
		std::string texturename;
        scenegraph::Scene* inScene;

        FbxManager* sdkManager;
        FbxScene* fbxScene;
        FbxNode* rootNode;   
        FbxIOSettings* iosettings; 
        FbxExporter* exporter;    

		double north;
		double east;
		double south;
		double west;

        int counter;

        SceneFbxBuilder(std::string outputName, scenegraph::Scene* scene, double n, double s, double e, double w)
        {
            ccl::FileInfo fi(outputName);
            outputNameFbx = fi.getFileName();
            tilename = fi.getBaseName(true);
			texturename = tilename + ".jpg";
            inScene = scene;
			north = n;
			south = s;
			east = e;
			west = w;
            counter = 0;

            sdkManager = FbxManager::Create();            

            // Import/export settings
            iosettings = FbxIOSettings::Create(sdkManager, IOSROOT);
            sdkManager->SetIOSettings(iosettings);

            // true - media embedded within export. Only binary | false - relative references to media files
            bool embedMedia = true;
            (*(sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_MATERIAL, true);
            (*(sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_TEXTURE, true);
            (*(sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, embedMedia);
            (*(sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_SHAPE, true);
            (*(sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_GOBO, true);
            (*(sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

            exporter = FbxExporter::Create(sdkManager, "Exporter");

            fbxScene = FbxScene::Create(sdkManager, "Scene");
            rootNode = fbxScene->GetRootNode();

            // Set up axis
            FbxAxisSystem sceneAxis = fbxScene->GetGlobalSettings().GetAxisSystem();
            FbxAxisSystem unrealAxis(FbxAxisSystem::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded);
            
            if (sceneAxis != unrealAxis)
            {
                unrealAxis.ConvertScene(fbxScene);
            }
        }

        ~SceneFbxBuilder()
        {            
        }

        bool BuildSceneWithFeatures(GsBuildings buildings, cognitics::TerrainGenerator *tg, scenegraph::Scene* scene)
        {
            bool exportStatus = exporter->Initialize(outputNameFbx.c_str(), -1, sdkManager->GetIOSettings());
            if (!exportStatus)
            {
                std::cout << "Call to FbxExporter::Initialize() failed" << std::endl;
                std::cout << "Error: " << exporter->GetStatus().GetErrorString() << std::endl;
                return false;
            }

            // Mesh
            std::string sm = "SM_" + tilename;
            FbxNode *terrainNode = FbxNode::Create(fbxScene, sm.c_str());
            FbxMesh *terrainMesh = BuildScene(inScene, "Terrain", true);
            terrainNode->SetNodeAttribute(terrainMesh);

            // Material and texture 
            AddMaterial(terrainNode, "M_Terrain", "Lambert", terrainMesh);

            rootNode->AddChild(terrainNode);

            // Features 
            FbxNode *featuresNode = FbxNode::Create(fbxScene, "Features");
            int counter = 0;
            ProcessFeatures(buildings, tg, featuresNode, scene);
            terrainNode->AddChild(featuresNode);

            exporter->Export(fbxScene);

            sdkManager->Destroy();

            return true;
        }

		// Builds an FbxScene using the working points.
		FbxMesh* BuildScene(scenegraph::Scene* scene, std::string i, bool UVNegate)
		{
			// Set up the fbx sdk stuff 
            std::string sm = "SM_Mesh_" + i;
			FbxMesh *mesh = FbxMesh::Create(fbxScene, sm.c_str());            
			
			int vertexCount = 0;
			int size = scene->faces.size();
			mesh->InitControlPoints(size * 3);
			FbxVector4* controlPoints = mesh->GetControlPoints();

			for (auto face : scene->faces)
			{				
				for (int i = 0; i < face.verts.size(); i++)
				{
					double x = face.verts.at(i).X();
					double y = face.verts.at(i).Y();
					double z = face.verts.at(i).Z();
					FbxVector4 vertex(x, y, z);
					controlPoints[vertexCount] = vertex;
					vertexCount++;                    
				}
			}   

            FbxLayer* layer = mesh->GetLayer(0);
			if (layer == nullptr)
			{
				mesh->CreateLayer();
				layer = mesh->GetLayer(0);
			}

            // Normals
            FbxLayerElementNormal* normalLayerElement = mesh->CreateElementNormal();
            normalLayerElement->SetMappingMode(FbxLayerElementNormal::eByControlPoint);
            normalLayerElement->SetReferenceMode(FbxLayerElementNormal::eDirect);            

            // UV
            FbxLayerElementUV* UVLayerElement = mesh->CreateElementUV("UVLayerElement");
            UVLayerElement->SetMappingMode(FbxLayerElementUV::eByPolygonVertex);
            UVLayerElement->SetReferenceMode(FbxLayerElementUV::eDirect);

            // Smoothing
            FbxLayerElementSmoothing* smoothingLayerElement = mesh->CreateElementSmoothing();
            smoothingLayerElement->SetMappingMode(FbxLayerElement::eByPolygon);
            smoothingLayerElement->SetReferenceMode(FbxLayerElement::eDirect);

			vertexCount = 0;
			for (auto face : scene->faces)
			{
				mesh->BeginPolygon(-1, -1, false);
				// loop through vertex indices 

				for (int i = 0; i < face.verts.size(); i++)
				{
					mesh->AddPolygon(vertexCount);

                    float u = face.textures[0].uvs[i].X();
                    float v = face.textures[0].uvs[i].Y();                    
                    if (UVNegate == true)
                    {
                        FbxVector2 vertex(u, -v);
                        UVLayerElement->GetDirectArray().Add(vertex);
                    }  
                    else
                    {
                        FbxVector2 vertex(u, v);
                        UVLayerElement->GetDirectArray().Add(vertex);
                    }                    

                    float x = face.vertexNormals[i].X();
                    float y = face.vertexNormals[i].Y();
                    float z = face.vertexNormals[i].Z();
                    FbxVector4 vertexNorm(x, y, z);
                    normalLayerElement->GetDirectArray().Add(vertexNorm);

                    smoothingLayerElement->GetDirectArray().SetAt(vertexCount, i);

					vertexCount++;
				}
                layer->SetUVs(UVLayerElement, FbxLayerElement::eTextureDiffuse);
                layer->SetNormals(normalLayerElement);
                layer->SetSmoothing(smoothingLayerElement);
				mesh->EndPolygon();
			}

			return mesh;
		}   

		FbxNode* BuildMeshWithMultipleTextures(scenegraph::Scene* scene, std::string meshName, bool UVNegate)
		{
			// Set up sdk 
			std::string nodename = meshName;
			FbxNode *node = FbxNode::Create(fbxScene, nodename.c_str());

			// determine the faces that have different textures 
			std::map<std::string, std::vector<scenegraph::Face*>> seperatedFaces;
			int vertexCount = 0;

			for (auto& face : scene->faces)
			{
				std::string texName = face.getMappedTextureN(0).GetTextureName();
				if (seperatedFaces.find(texName) == seperatedFaces.end())
				{
					seperatedFaces[texName] = std::vector<scenegraph::Face*>();
				}
				seperatedFaces[texName].push_back(&face);
				vertexCount += face.verts.size();
			}

			// for each seperated face, call BuildMeshFromFaceList and add the material
			for (auto faceList : seperatedFaces)
			{
				FbxNode *smallNode = FbxNode::Create(fbxScene, (meshName + "_node_" + std::to_string(counter)).c_str());
				FbxMesh *mesh = BuildMeshFromFaceList(faceList.second, meshName, faceList.first, false);
				SetTextureFileName(faceList.first);
				smallNode->SetNodeAttribute(mesh);
				std::string m = "M_Feature_" + std::to_string(counter);
				counter++;
				AddMaterial(smallNode, m.c_str(), "Lambert", mesh);
				node->AddChild(smallNode);
			}

			return node;
		}

		FbxMesh* BuildMeshFromFaceList(std::vector<scenegraph::Face*> faceList, std::string name, std::string textureName, bool UVNegate)
		{
			// set up mesh 
			std::string meshname = name;
			FbxMesh *mesh = FbxMesh::Create(fbxScene, meshname.c_str());

			// Set up control points 
			int vertexCount = 0;
			int numFaces = faceList.size();
			int numVertices = 0;
			for (auto face : faceList)
			{
				numVertices += face->verts.size();
			}
			mesh->InitControlPoints(numVertices);
			FbxVector4* controlPoints = mesh->GetControlPoints();

			// Add control points
			for (auto face : faceList)
			{
				for (int i = 0; i < face->verts.size(); i++)
				{
					double x = face->verts.at(i).X();
					double y = face->verts.at(i).Y();
					double z = face->verts.at(i).Z();
					FbxVector4 vertex(x, y, z);
					controlPoints[vertexCount] = vertex;
					vertexCount++;
				}
			}

			// Create the material layer 
			FbxLayer* layer = mesh->GetLayer(0);
			if (layer == nullptr)
			{
				mesh->CreateLayer();
				layer = mesh->GetLayer(0);
			}

			// Normals 
			FbxLayerElementNormal* normalLayerElement = mesh->CreateElementNormal();
			normalLayerElement->SetMappingMode(FbxLayerElementNormal::eByControlPoint);
			normalLayerElement->SetReferenceMode(FbxLayerElementNormal::eDirect);

			// UV 
			FbxLayerElementUV* UVLayerElement = mesh->CreateElementUV("UVLayerElement");
			UVLayerElement->SetMappingMode(FbxLayerElementUV::eByPolygonVertex);
			UVLayerElement->SetReferenceMode(FbxLayerElementUV::eDirect);

			// Smoothing
			FbxLayerElementSmoothing* smoothingLayerElement = mesh->CreateElementSmoothing();
			smoothingLayerElement->SetMappingMode(FbxLayerElement::eByPolygon);
			smoothingLayerElement->SetReferenceMode(FbxLayerElement::eDirect);

			// Add UV and normal and smoothing points and create polygons
			vertexCount = 0;
			for (auto face : faceList)
			{
				mesh->BeginPolygon(-1, -1, false);
				for (int i = 0; i < face->verts.size(); i++)
				{
					mesh->AddPolygon(vertexCount);

					float u = face->textures[0].uvs[i].X();
					float v = face->textures[0].uvs[i].Y();
					if (UVNegate == true)
					{
						FbxVector2 vertex(u, -v);
						UVLayerElement->GetDirectArray().Add(vertex);
					}
					else
					{
						FbxVector2 vertex(u, v);
						UVLayerElement->GetDirectArray().Add(vertex);
					}

					float x = face->vertexNormals[i].X();
					float y = face->vertexNormals[i].Y();
					float z = face->vertexNormals[i].Z();
					FbxVector4 vertexNorm(x, y, z);
					normalLayerElement->GetDirectArray().Add(vertexNorm);

					smoothingLayerElement->GetDirectArray().SetAt(vertexCount, i);

					vertexCount++;
				}
				layer->SetUVs(UVLayerElement, FbxLayerElement::eTextureDiffuse);
				layer->SetNormals(normalLayerElement);
				layer->SetSmoothing(smoothingLayerElement);
				mesh->EndPolygon();
			}

			return mesh;
		}

        void ProcessFeatures(GsBuildings buildings, cognitics::TerrainGenerator *tg, FbxNode* featuresNode, scenegraph::Scene* scene)
        {
            int count = buildings.Count();

            for (int i = 0; i < count; ++i)
            {
				// check if feature is within the tile 
				FeatureInfo building = buildings.GetBuilding(i);
				if (building.lat < south || building.lat > north || building.lon < west || building.lon > east)
				{
					continue;
				}

				// set up the feature node
                std::string f = "SM_Feature_" + std::to_string(i) + "_" + std::to_string(counter);
                FbxNode* featureNode = FbxNode::Create(fbxScene, f.c_str());
                
				// check if the feature model exists
                ccl::FileInfo fi(building.modelpath);
                std::string sModelPath = building.modelpath;
                if (!fi.fileExists(sModelPath))
                {
                    continue;
                }

				// build the scene from the model's JSON
                scenegraph::Scene buildingScene;
                tg->ParseJSON(sModelPath, "", buildingScene);

                float lat = building.lat;
                float lon = building.lon;
                std::string featurefilename = fi.getBaseName(true) + ".fbx";
                std::string textureFilename = buildingScene.faces[0].getMappedTextureN(0).GetTextureName();

				// Build the feature mesh with multiple textures
                SetTextureFileName(textureFilename);
                std::string s = std::to_string(i);
				featureNode = BuildMeshWithMultipleTextures(&buildingScene, f, false);

                // build the material using this information 
                std::string m = "M_Feature_" + std::to_string(counter) + std::to_string(i);
                counter++;

                // add the mesh node to the scene using the position 
                featuresNode->AddChild(featureNode);
                float x = tg->ConvertGeoToLocalX(lon);
                float y = tg->ConvertGeoToLocalY(lat);
                float z = building.elev;
                PositionFeature(x, y, z, featureNode, building.AO1);
            }
        }

        void PositionFeature(float x, float y, float z, FbxNode* feature, float ao1)
        {
            feature->LclTranslation.Set(FbxVector4(x, y, z));
            feature->SetRotationOrder(FbxNode::eDestinationPivot, FbxEuler::eOrderZYX);
            feature->SetPreRotation(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0, 0));
            feature->SetPostRotation(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0, 0));
            feature->SetRotationOffset(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0, 0));
            feature->SetScalingOffset(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0, 0));
            feature->SetRotationPivot(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0, 0));
            feature->SetScalingPivot(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0, 0));

            feature->LclRotation.Set(FbxDouble3(0, 0, (double)(360.0 - ao1)));
        }

        void AddMaterial(FbxNode* node, FbxString materialName, FbxString shadingName, FbxMesh* mesh)
		{
			FbxGeometryElementMaterial *materialElement = mesh->CreateElementMaterial();
			materialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
			materialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

            FbxSurfaceLambert* material = FbxSurfaceLambert::Create(fbxScene, materialName);

            FbxDouble3 white(1.0, 1.0, 1.0);
            FbxDouble3 red(1.0, 0.0, 0.0);
            material->Emissive.Set(white);
            material->Ambient.Set(red);
            material->TransparencyFactor.Set(1.0);
            material->ShadingModel.Set(shadingName);

            node->AddMaterial(material);

			materialElement->GetIndexArray().SetCount(mesh->GetPolygonCount());
			int polyCount = mesh->GetPolygonCount();            
            for (int i = 0; i < polyCount; ++i)
			{
				materialElement->GetIndexArray().SetAt(i, 0);                
			}		
                        
            FbxFileTexture * texture = AddTexture(node, material);
            material->Diffuse.ConnectSrcObject(texture);
        }		

        FbxFileTexture* AddTexture(FbxNode* node, FbxSurfaceLambert* material)
        {
            std::string t = "T_" + texturename;
            FbxFileTexture * texture = FbxFileTexture::Create(fbxScene, t.c_str());
            node->SetShadingMode(FbxNode::eTextureShading);

            texture->SetFileName(texturename.c_str());
            texture->SetTextureUse(FbxTexture::eStandard);
            texture->SetMappingType(FbxFileTexture::eUV);
            texture->SetMaterialUse(FbxFileTexture::eModelMaterial);
            texture->SetSwapUV(false);
            texture->SetTranslation(0.0, 0.0);
            texture->SetScale(1.0, 1.0);
            texture->SetRotation(0.0, 0.0);
            texture->SetAlphaSource(FbxTexture::eNone);
            texture->Alpha.Set(0.0);
            texture->SetDefaultAlpha(0.0);
            texture->SetBlendMode(FbxFileTexture::eTranslucent);

            //material->Diffuse.ConnectSrcObject(texture);

            return texture;
        }  

        void SetTextureFileName(std::string textureFileName)
        {
            texturename = textureFileName;
        }			
    };

	bool buildFbxWithFeatures(const std::string &outputName, scenegraph::Scene* scene, GsBuildings buildings, cognitics::TerrainGenerator *tg, double north, double south, double east, double west, std::string imageFileName, bool setTextureFilename)
	{
		SceneFbxBuilder fbxBuilder(outputName, scene, north, south, east, west);
		if (setTextureFilename)
		{
			fbxBuilder.SetTextureFileName(imageFileName);
		}
		fbxBuilder.BuildSceneWithFeatures(buildings, tg, scene);
		return true;
	}
}
