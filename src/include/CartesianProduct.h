#pragma once

#include <vector>
#include <list>
#include <set>
#include <any>
#include <string>

class CartesianProduct {
private:
    static bool inAnyVector(int val, const std::vector<std::any>& vec);
    static bool inAnyVector(float val, const std::vector<std::any>& vec);
    static bool inAnyVector(const std::string& val, const std::vector<std::any>& vec);
public:
    static void testCartesianProduct();
    static std::vector<std::vector<std::any>> product(std::vector<std::vector<std::any>> vecs, int depth = 0);
    static std::vector<std::any> toUniqueAny(const std::vector<int>& vec);
    static std::vector<std::any> toUniqueAny(const std::vector<float>& vec);
    static std::vector<std::any> toUniqueAny(const std::vector<std::string>& vec);
};
