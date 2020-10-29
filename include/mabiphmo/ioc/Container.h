//
// Created by max on 8/13/20.
//

#ifndef IOC_CONTAINER_H
#define IOC_CONTAINER_H

#include <string>
#include <type_traits>
#include <memory>
#include <map>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <typeindex>
#include <set>
#include <boost/functional/hash.hpp>
#include "warnings.h"

namespace mabiphmo::ioc{
	class ContainerException : public std::runtime_error
	{
	public:
		explicit ContainerException(std::string &&description) : std::runtime_error(description) {}
	};

	class Container : public std::enable_shared_from_this<Container>
	{
	public:
	    enum class Scope{
	        Singleton,
	        Factory
	    };
	private:
	    template<typename TContainer>
	    struct container_hash {
	        std::size_t operator()(const TContainer &container) const{
	            return boost::hash_range(container.begin(), container.end());
	        }
	    };

	public:
        struct ITypeHolder {
            virtual const Container::Scope & Scope() = 0;
        };
        template<class T>
        class TypeHolder : public ITypeHolder {
            std::shared_ptr<T> instance_;
            Container::Scope scope_;
            std::unordered_map<std::vector<std::type_index>, std::shared_ptr<void>, container_hash<std::vector<std::type_index>>> factories_ =
                    std::unordered_map<std::vector<std::type_index>, std::shared_ptr<void>, container_hash<std::vector<std::type_index>>>();

            template<typename ... Ts>
            static std::vector<std::type_index> CreateKey() {
                std::vector<std::type_index> key = std::vector<std::type_index>();
                (key.emplace_back(typeid(Ts)), ...);
                return key;
            }

            template<typename ... TArgs>
            std::shared_ptr<std::function<T (TArgs ...)>> GetFactoryInternal() {
                if(Scope() == Scope::Singleton && (std::is_same<TArgs, TArgs>::value || ...)){
                    throw ContainerException("Singletons can only have parameterless factories");
                }

                std::shared_ptr<void> typeErasedFactory = factories_.at(CreateKey<TArgs ...>());
                return std::static_pointer_cast<std::function<T (TArgs ...)>>(typeErasedFactory);
            }
        public:
            explicit TypeHolder(Container::Scope && scope) : instance_(nullptr), scope_(scope) {}

            TypeHolder(Container::Scope && scope, T && instance) : TypeHolder(std::move(scope)) {
                SetInstance(std::move(instance));
            }

            template<class ... TFactoryArgs>
            TypeHolder(Container::Scope && scope, std::function<T (TFactoryArgs ...)> && factory) : TypeHolder(std::move(scope)) {
                AddFactory(std::move(factory));
            }

            template<typename ... TArgs>
            std::shared_ptr<T> Get(TArgs && ... args) {
                if(Scope() == Scope::Singleton) {
                    if(instance_ == nullptr) {
                        SetInstance((*GetFactoryInternal<>())());
                    }
                    return instance_;
                }
                else {
                    return std::make_shared<T>((*GetFactoryInternal<TArgs ...>())(std::forward<TArgs>(args) ...));
                }
            }

            template<typename ... TFactoryArgs>
            void AddFactory(std::function<T (TFactoryArgs ...)> && factory) {
                if(Scope() == Scope::Singleton){
                    if((std::is_same<TFactoryArgs, TFactoryArgs>::value || ...)){
                        throw ContainerException("Singletons can only have parameterless factories");
                    }

                }

                factories_[CreateKey<TFactoryArgs ...>()] = std::make_shared<std::function<T (TFactoryArgs ...)>>(factory);
            }

            void SetInstance(T && instance) {
                if(Scope() != Scope::Singleton){
                    throw ContainerException("Only singletons can only have managed instances");
                }

                instance_ = std::make_shared<T>(instance);
            }

            const Container::Scope & Scope() override {
                return scope_;
            }
        };
	private:
        std::unordered_map<std::type_index, std::shared_ptr<ITypeHolder>> typeMap_ = std::unordered_map<std::type_index, std::shared_ptr<ITypeHolder>>();
	public:
	    template<typename T>
	    std::shared_ptr<TypeHolder<T>> GetTypeHolder() const{
	        return std::dynamic_pointer_cast<TypeHolder<T>>(typeMap_.at(typeid(T)));
	    }

	    template<typename T>
	    std::shared_ptr<TypeHolder<T>> RegisterType(TypeHolder<T> && holder){
	        if(typeMap_.find(typeid(T)) != typeMap_.end())
	            throw ContainerException("Type already registered");

	        std::shared_ptr<TypeHolder<T>> res = std::make_shared<TypeHolder<T>>(holder);

	        typeMap_[typeid(T)] = std::dynamic_pointer_cast<ITypeHolder>(res);

	        return res;
	    }
	};
}

#endif //IOC_CONTAINER_H
