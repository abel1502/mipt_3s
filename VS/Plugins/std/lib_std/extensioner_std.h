#ifndef P_EXTENSIONER_STD_H
#define P_EXTENSIONER_STD_H


#include <map>
#include <string_view>


namespace PUPPY {

class Extensioner {
    std::map<std::string_view, void*> extensions;

public:
    Extensioner() {}
    
    void *get(const std::string_view &name) const {
        auto ext = extensions.find(name);
        if (ext == extensions.end()) return nullptr;

        return (*ext).second;
    }

    void set(const std::string_view &name, void *ext) { extensions[name] = ext; }
};

}

#endif // P_EXTENSIONER_STD_H
