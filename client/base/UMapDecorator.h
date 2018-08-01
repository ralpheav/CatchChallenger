#ifndef UMAPDECORATOR_H
#define UMAPDECORATOR_H

#include <unordered_map>

namespace CatchChallenger {

    template<class Key, class Value>
    class UMapDecorator : public std::unordered_map<Key, Value>
    {
        public:
            bool exists(const Key& key) const
            {
                return this->find(key) != this->cend();
            }
    };

}

#endif // UMAPDECORATOR_H

