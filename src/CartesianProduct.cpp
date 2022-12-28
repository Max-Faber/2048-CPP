#include <CartesianProduct.h>

void CartesianProduct::testCartesianProduct()
{
    std::vector<std::vector<std::any>> vectors {
            toUniqueAny(std::vector<int> { 1, 2, 3, 2 }),
            toUniqueAny(std::vector<std::string> { "A", "B", "C", "A" }),
            toUniqueAny(std::vector<float> { .3, .5 })
    };
    std::vector<std::vector<std::any>> res = product(vectors);

    for (std::vector<std::any> vals : res)
    {
        printf(
            "(%d, %s, %f)\n",
            std::any_cast<int>(vals[0]),
            std::any_cast<std::string>(vals[1]).c_str(),
            std::any_cast<float>(vals[2])
       );
    }
}

std::vector<std::vector<std::any>> CartesianProduct::product(std::vector<std::vector<std::any>> vecs, int depth)
{
    std::vector<std::any> vals;
    std::vector<std::vector<std::any>> res;

    if (vecs.empty())
    {
        if (depth == 0) return { };
        return { { } };
    }
    vals = vecs[0];
    vecs.erase(vecs.begin());
    for (const std::vector<std::any>& p : product(vecs, depth + 1))
    {
        for (const std::any& val : vals)
        {
            std::vector<std::any> l = { p };

            l.insert(l.begin(), val);
            res.emplace_back(l);
        }
    }
    return res;
}

std::vector<std::any> CartesianProduct::toUniqueAny(const std::vector<int>& vec)
{
    std::vector<std::any> vecUniqueAny;

    for (int val : vec)
    {
        if (inAnyVector(val, vecUniqueAny)) continue;
        vecUniqueAny.emplace_back(val);
    }
    return vecUniqueAny;
}

std::vector<std::any> CartesianProduct::toUniqueAny(const std::vector<float>& vec)
{
    std::vector<std::any> vecUniqueAny;

    for (float val : vec)
    {
        if (inAnyVector(val, vecUniqueAny)) continue;
        vecUniqueAny.emplace_back(val);
    }
    return vecUniqueAny;
}

std::vector<std::any> CartesianProduct::toUniqueAny(const std::vector<std::string>& vec)
{
    std::vector<std::any> vecUniqueAny;

    for (const std::string& val : vec)
    {
        if (inAnyVector(val, vecUniqueAny)) continue;
        vecUniqueAny.emplace_back(val);
    }
    return vecUniqueAny;
}

bool CartesianProduct::inAnyVector(int val, const std::vector<std::any>& vec)
{
    for (std::any v : vec) if (val == std::any_cast<int>(v)) return true;
    return false;
}

bool CartesianProduct::inAnyVector(float val, const std::vector<std::any>& vec)
{
    for (std::any v : vec) if (val == std::any_cast<float>(v)) return true;
    return false;
}

bool CartesianProduct::inAnyVector(const std::string& val, const std::vector<std::any>& vec)
{
    for (std::any v : vec) if (val == std::any_cast<std::string>(v)) return true;
    return false;
}
