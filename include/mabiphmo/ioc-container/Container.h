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

namespace mabiphmo::ioc_container{
	class ContainerException : public std::runtime_error
	{
	public:
		explicit ContainerException(std::string &&description) : std::runtime_error(description) {}
	};

	enum class Scope{
		Singleton,
		Factory
	};

	struct ITypeHolder {
		virtual const ioc_container::Scope & Scope() = 0;
	};
	template<class T>
	class TypeHolder : public ITypeHolder {
		template<typename TContainer>
		struct container_hash {
			std::size_t operator()(const TContainer &container) const{
				return boost::hash_range(container.begin(), container.end());
			}
		};

		std::shared_ptr<T> instance_;
		ioc_container::Scope scope_;
		std::unordered_map<std::vector<std::type_index>, std::shared_ptr<void>, container_hash<std::vector<std::type_index>>> factories_ =
		std::unordered_map<std::vector<std::type_index>, std::shared_ptr<void>, container_hash<std::vector<std::type_index>>>();

		template<typename ... Ts>
		static std::vector<std::type_index> CreateKey() {
			std::vector<std::type_index> key = std::vector<std::type_index>();
			(key.emplace_back(typeid(Ts)), ...);
			return key;
		}
	public:
		explicit TypeHolder(ioc_container::Scope && scope) : instance_(nullptr), scope_(std::move(scope)) {}

		TypeHolder(ioc_container::Scope && scope, std::shared_ptr<T> instance) : TypeHolder(std::move(scope)) {
			SetInstance(instance);
		}

		template<class ... TFactoryArgs>
		TypeHolder(ioc_container::Scope && scope, std::function<std::shared_ptr<T> (TFactoryArgs ...)> && factory) : TypeHolder(std::move(scope)) {
			SetFactory(std::move(factory));
		}

		template<typename ... TArgs>
		std::shared_ptr<T> Get(TArgs && ... args) {
			if(Scope() == Scope::Singleton) {
				if(instance_ == nullptr) {
					SetInstance((*(GetFactory<TArgs ...>()))(std::forward<TArgs>(args) ...));
				}
				return instance_;
			}
			else {
				return (*(GetFactory<TArgs ...>()))(std::forward<TArgs>(args) ...);
			}
		}

		template<typename ... TArgs>
		std::shared_ptr<std::function<std::shared_ptr<T> (TArgs ...)>> GetFactory() {
			std::shared_ptr<void> typeErasedFactory = factories_.at(CreateKey<TArgs ...>());
			return std::static_pointer_cast<std::function<std::shared_ptr<T> (TArgs ...)>>(typeErasedFactory);
		}

		template<typename ... TFactoryArgs>
		void SetFactory(std::function<std::shared_ptr<T> (TFactoryArgs ...)> && factory) {
			if(instance_ != nullptr)
				throw ContainerException("Can't change factories of singletons after instance is set");

			factories_[CreateKey<TFactoryArgs ...>()] = std::make_shared<std::function<std::shared_ptr<T> (TFactoryArgs ...)>>(std::move(factory));
		}

		void SetInstance(std::shared_ptr<T> instance) {
			if(Scope() != Scope::Singleton){
				throw ContainerException("Only singletons can only have managed instances");
			}

			instance_ = instance;
		}

		const ioc_container::Scope & Scope() override {
			return scope_;
		}
	};

	class Container : public std::enable_shared_from_this<Container>
	{
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
