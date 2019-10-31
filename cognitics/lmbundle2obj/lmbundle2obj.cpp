#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/FileInfo.h>
#include "fstream"
#include "ccl/StringUtils.h"
#include "sfa/Point.h"
#include <stdio.h>
typedef std::vector<unsigned short> face_vec_t;
;

void writeMtlFile(const std::string& textureName, const std::string& outputPath)
{
    ccl::FileInfo fi(textureName);
    const std::string textureBase = fi.getBaseName(true);
    std::string mtlPath = ccl::joinPaths(outputPath, textureBase);
    mtlPath += ".mtl";
    std::ofstream outfile(mtlPath.c_str());
    outfile << "newmtl " << textureBase << std::endl;
    outfile << "Ka 1 1 1" << std::endl;
    outfile << "Kd 1 1 1" << std::endl;
    outfile << "d 1" << std::endl;
    outfile << "Ns 0" << std::endl;
    outfile << "illum 1" << std::endl;
    outfile << "map_Kd " << textureName << std::endl;
    outfile.close();
}

void writeOBJ(const std::string& objFileName,
              const std::string& mtfFile,
              const std::vector<sfa::Point>& verts,
              const std::vector<sfa::Point>& uvs,
              const std::vector<face_vec_t>& faces)
{
    ccl::FileInfo fi(mtfFile);
    const std::string textureID = fi.getBaseName(true);

    std::ofstream outfile(objFileName.c_str());
    outfile << "mtllib " << mtfFile << std::endl;
    for (auto&& vert : verts)
    {
        outfile << "v " << vert.X() << " " << vert.Y() << " " << vert.Z() << std::endl;
    }

    for (auto&& uv : uvs)
    {
        outfile << "vt " << uv.X() << " " << uv.Y() << std::endl;
    }
    outfile << "usemtl " << textureID << std::endl;

    for (auto&& face : faces)
    {
        outfile << "f ";
        outfile << face[2] << "/" << face[2] << " ";
        outfile << face[1] << "/" << face[1] << " ";
        outfile << face[0] << "/" << face[0] << "\n";
    }

    outfile.close();
}

std::string getTileString(const std::string& fileName)
{
    const ccl::FileInfo fi(fileName);
    const std::string baseName = fi.getBaseName();
    //Extract the name without the LOD part.
    //Tile_+006_+015_L19_0.obj -> Tile_+006_+015
    const std::string::size_type pos = baseName.rfind("_L");
    if (std::string::npos != pos && pos > 0)
    {
        const std::string tileName = baseName.substr(0, pos);
        return tileName;
    }
    return ".";
}

void convert(FILE* f, const std::string& texturePath, std::string outputPath)
{
    //Read texture length
    unsigned char texture_name_len;
    fread(&texture_name_len, 1, 1, f);

    //Read texture name
    char* texname = new char[texture_name_len + 1];
    texname[texture_name_len] = 0;
    fread(texname, 1, texture_name_len, f);

    const std::string subdir = getTileString(texname);
    outputPath = ccl::joinPaths(outputPath, subdir);
    ccl::makeDirectory(outputPath);

    ccl::FileInfo textureFileInfo(texname);
    const std::string objPath = ccl::joinPaths(outputPath, textureFileInfo.getBaseName(true) + ".obj");

    //Build mtl file
    writeMtlFile(texname, outputPath);

    //Copy dds file
    ccl::copyFile(ccl::joinPaths(texturePath, texname), ccl::joinPaths(outputPath, texname));
    std::cout << "Copying texture: " << texname << std::endl;

    //Read vert count
    unsigned short num_verts = 0;

    //Read verts
    fread(&num_verts, 2, 1, f);

    std::vector<sfa::Point> verts;
    for (int i = 0; i < num_verts; i++)
    {
        float x, y, z = 0;
        fread(&x, 4, 1, f);
        fread(&y, 4, 1, f);
        fread(&z, 4, 1, f);

        //Flip z and y, since the ENU projection expects Z to be up
        verts.push_back(sfa::Point(z, x, y));
    }

    //Read UVs
    std::vector<sfa::Point> uvs;
    for (int i = 0; i < num_verts; i++)
    {
        float u, v;
        fread(&u, 4, 1, f);
        fread(&v, 4, 1, f);
        uvs.push_back(sfa::Point(u, v));
    }

    //Read faces
    typedef std::vector<unsigned short> face_vec_t;
    std::vector<face_vec_t> faces;
    short num_faces;
    fread(&num_faces, 2, 1, f);

    for (int i = 0; i < num_faces; i++)
    {
        face_vec_t face;
        for (int b = 0; b < 3; b++)
        {
            unsigned short idx;
            fread(&idx, 2, 1, f);
            face.push_back(idx + 1); //Verts start at 1, not 0
        }
        faces.push_back(face);
    }

    std::string textureBase = textureFileInfo.getBaseName(true);
    std::string mtlPath = textureBase + ".mtl";
    std::cout << "Creating : " << objPath << std::endl;
    writeOBJ(objPath, mtlPath, verts, uvs, faces);
}


int main(int argc, char** argv)
{
    //E:\TestData\ATLAS v2 Latest\ATLAS v2 Latest\TerrainsAvailable\USC_LMAB_SEGT_ENU_5cm_170520\terrainMesh.lmab
    if (argc < 4)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "lmbundle2obj <lmab file path> <source texture path> <output_directory>" << std::endl;
        return 1;
    }

    const std::string output_path = argv[3];
    const std::string input_lmab = argv[1];
    const std::string texture_path = argv[2];
    ccl::makeDirectory(output_path, true);
    ccl::FileInfo fi(input_lmab);
    const std::string metadataFilename = ccl::joinPaths(fi.getDirName(), "metadata.xml");
    if (ccl::fileExists(metadataFilename))
    {
        ccl::copyFile(metadataFilename, ccl::joinPaths(output_path, "metadata.xml"));
    }
    else
    {
        std::cout << "Warning: No metadata.xml exists in input path.\n";
    }

    FILE* f = NULL;
    fopen_s(&f, input_lmab.c_str(), "rb");
    if (!f)
    {
        std::cout << "Can't open the file.\n";
        return 1;
    }
    unsigned short num_meshes = 0;
    fread(&num_meshes, 2, 1, f);

    for (int i = 0; i < num_meshes; i++)
    {
        convert(f, texture_path,
                output_path);
    }
    fclose(f);
    return 0;
}
