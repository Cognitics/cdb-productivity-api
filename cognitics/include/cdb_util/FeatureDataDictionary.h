#pragma once

#include <map>
#include <string>
#include <sstream>

namespace cognitics {
namespace cdb {

class FeatureDataDictionary
{
public:
    struct Type
    {
        int Key;
        std::string Name;
    };

    struct Subcategory
    {
        char Key;
        std::string Name;
        std::map<int, Type> Types;
        void AddType(int key, const std::string& name);
    };

    struct Category
    {
        char Key;
        std::string Name;
        std::map<char, Subcategory> Subcategories;
        void AddSubcategory(char key, const std::string& name);
    };

    std::map<char, Category> Categories;

    void AddCategory(char key, const std::string& name);

    FeatureDataDictionary();

    std::string Subdirectory(const std::string& facc);

};

}
}
