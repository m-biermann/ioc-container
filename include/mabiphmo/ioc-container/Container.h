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
    /// Custom exception thrown by the container
	struct ContainerException : public std::runtime_error
	{
        /// Constructor
        /// \param description The description of the error
		explicit ContainerException( const std::string &description) : std::runtime_error(description) {}
	};

    /// Scopes of types in the container
	enum class Scope{
		/// Only a single instance should exist while running
		Singleton,
		/// Each time an instance is asked for, a new one will be created
		Factory,
		/// There will (cannot) be any instances of this, however instances of linked types will be resolved
        Interface
	};

    /// \brief IoC Container that stores Singleton- Instances and Factories for both Singletons and non- Singletons
    /// \details When factories are registered, dependencies will be resolved by using the arguments of the factory.
    /// All dependencies that should be resolved have to be of type std::shared_ptr<Dependency> and have to come before all other arguments to the factory.
    /// Also all dependencies cannot have any args or have to be a already-resolved singleton, otherwise the container itself should be used as a "dependency" and resolving should be done "manually".
	class Container : public std::enable_shared_from_this<Container>
	{
//region Helper Stuff
//region Structs
        template<typename TContainer>
        struct container_hash {
            std::size_t operator()(const TContainer &container) const{
                return boost::hash_range(container.begin(), container.end());
            }
        };

		template<typename TContainer, typename TKey>
		bool container_contains(const TContainer& container, const TKey& key)
		{
			return container.find(key) != container.end();
		}
//endregion
//region Functions
//region AddFactory Helpers
		template <typename... Ts>
		struct list {};

		template <typename T>
		struct is_shared_ptr : std::false_type {};

		template <typename T>
		struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

		template <typename L, typename T, typename F, typename = void>
		struct partition;

		template <typename Next, typename... Ls, typename T, typename... Fs>
		struct partition<list<Next, Ls...>, T, list<Fs...>, std::enable_if_t<!is_shared_ptr<Next>::value>> :
				partition<list<Ls...>, T, list<Fs..., Next>>
		{};

		template <typename Next, typename... Ls, typename... Ts, typename F>
		struct partition<list<std::shared_ptr<Next>, Ls...>, list<Ts...>, F> :
				partition<list<Ls...>, list<Ts..., std::shared_ptr<Next>>, F>
		{};

		template <typename T, typename F>
		struct partition<list<>, T, F> { using type = list<T, F>; };

		template <typename... Ts>
		using register_traits = typename partition<list<Ts...>, list<>, list<>>::type;

		template <typename T, typename F, typename... TDependencies, typename... TArgs>
		void AddFactoryImpl(list<list<std::shared_ptr<TDependencies>...>, list<TArgs...>>, F && pFactory)
		{
			if (container_contains(registeredInstances_, typeid(T))) throw ContainerException("An instance is already registered");

			auto new_factory = std::make_shared<std::function<std::shared_ptr<T>(TArgs ...)>>(
					[self = shared_from_this(), factory = std::make_shared<F>(pFactory)](TArgs &&... args) {
						return (*factory)(self->Resolve<TDependencies>() ..., std::forward<TArgs>(args) ...);
					});

			//add the factory
			registeredFactories_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}] = new_factory;
		}
//endregion
		template <class T, typename... TArgs>
		void AddFactory(std::function<std::shared_ptr<T>(TArgs ...)> && pFactory)
		{
			return AddFactoryImpl<T>(register_traits<TArgs...>{}, std::forward<std::function<std::shared_ptr<T> (TArgs...)>>(pFactory));
		}

		template<class T, class TInterface, typename... TArgs>
		void AddLink() {
			registeredLinks_[typeid(TInterface)][std::vector<std::type_index>{typeid(TArgs) ...}].insert(registeredLinks_[typeid(TInterface)][std::vector<std::type_index>{typeid(TArgs) ...}].cbegin(), std::make_shared<std::function<std::shared_ptr<TInterface>(TArgs &&...)>>(
				[self = shared_from_this()](TArgs &&... args){
					return std::dynamic_pointer_cast<TInterface>(self->Resolve<T>(std::forward<TArgs>(args)...));
				}));
		}
//endregion
//endregion
        std::unordered_map<std::type_index, Scope> registeredTypes_;
        std::unordered_map<std::type_index, std::unordered_map<std::vector<std::type_index>, std::shared_ptr<void>, container_hash<std::vector<std::type_index>>>> registeredFactories_;
        std::unordered_map<std::type_index, std::shared_ptr<void>> registeredInstances_;
        std::unordered_map<std::type_index, std::unordered_map<std::vector<std::type_index>, std::vector<std::shared_ptr<void>>, container_hash<std::vector<std::type_index>>>> registeredLinks_;

	public:
        /// Default constructor
        Container() = default;

        /// \brief Registers a type as defined in T::Register
        /// \details The T::Register function should be like <b><tt>void Register(std::shared_ptr<Container> &)</tt></b>
        /// \tparam T Type to register
        template <class T>
        void Register()
        {
            T::Register(shared_from_this());
        }
//region Resolving
		template <class T>
		std::vector<std::shared_ptr<T>> ResolveAll()
		{
			if(!container_contains(registeredTypes_, typeid(T))) throw ContainerException("Type is not registered");
			if(registeredTypes_[typeid(T)] != Scope::Interface) throw ContainerException("Type is not registered as an Interface");
			if(!container_contains(registeredLinks_, typeid(T)) || registeredLinks_[typeid(T)].empty()) throw ContainerException("No Links are registered for this Interface");

			auto res = std::vector<std::shared_ptr<T>>();
			for (auto link : registeredLinks_[typeid(T)][std::vector<std::type_index>()])
			{
				res.insert(res.end(), (*std::static_pointer_cast<std::function<std::shared_ptr<T>()>>(link))());
			}
			return res;
		}

		template <class T, typename ... TArgs>
		std::shared_ptr<T> Resolve(TArgs &&... args)
		{
			if constexpr(std::is_same<T, Container>::value)
				return shared_from_this();
			else{
				if(!container_contains(registeredTypes_, typeid(T))) throw ContainerException("Type is not registered");

				switch(registeredTypes_[typeid(T)]){
					case Scope::Singleton:
						if(container_contains(registeredInstances_, typeid(T))) return std::static_pointer_cast<T>(registeredInstances_[typeid(T)]);
						if(!container_contains(registeredFactories_, typeid(T))) throw ContainerException("Type has neither an instance nor a factory");
						if(!container_contains(registeredFactories_[typeid(T)], std::vector<std::type_index>{typeid(TArgs) ...})) throw ContainerException("Type has no factory with the supplied arguments");
						registeredInstances_[typeid(T)] = (*std::static_pointer_cast<std::function<std::shared_ptr<T>(TArgs...)>>(registeredFactories_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}]))(std::forward<TArgs>(args) ...);
						return std::static_pointer_cast<T>(registeredInstances_[typeid(T)]);
					case Scope::Factory:
						if(!container_contains(registeredFactories_, typeid(T))) throw ContainerException("Type has neither an instance nor a factory");
						if(!container_contains(registeredFactories_[typeid(T)], std::vector<std::type_index>{typeid(TArgs) ...})) throw ContainerException("Type has no factory with the supplied arguments");
						return (*std::static_pointer_cast<std::function<std::shared_ptr<T>(TArgs...)>>(registeredFactories_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}]))(std::forward<TArgs>(args) ...);
					case Scope::Interface:
						if(!container_contains(registeredLinks_, typeid(T)) || registeredLinks_[typeid(T)].empty()) throw ContainerException("No Links are registered for this Interface");
						if(!container_contains(registeredLinks_[typeid(T)], std::vector<std::type_index>{typeid(TArgs) ...})) throw ContainerException("The Interface has no link with the supplied arguments");
						return (*std::static_pointer_cast<std::function<std::shared_ptr<T>(TArgs...)>>(*registeredLinks_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}].begin()))(std::forward<TArgs>(args) ...);
					default:
						throw ContainerException("The type is registered with an invalid Scope");
				}
			}
		}

		template <class T, typename ... TArgs>
		std::shared_ptr<T> Get(TArgs &&... args)
		{
			return Resolve<T>(std::forward<TArgs>(args) ...);
		}
//endregion
//region Singleton
		/// Registers a type as Singleton with the predefined instance
		/// \tparam T Type to register
		/// \param pInstance Predefined Instance
		template <class T>
		void RegisterSingleton(std::shared_ptr<T> pInstance)
		{
			//check whether the type is registered
			if (!container_contains(registeredTypes_, typeid(T))) {
				//mark the type as registered as singleton
				registeredTypes_[typeid(T)] = Scope::Singleton;
				//add the instance
				registeredInstances_[typeid(T)] = pInstance;
				return;
			}

			//assertions for a registered type
			if (registeredTypes_[typeid(T)] != Scope::Singleton) throw ContainerException("Already registered as non- Singleton");
			if (container_contains(registeredInstances_, typeid(T))) throw ContainerException("An instance is already registered");

			//add the instance
			registeredInstances_[typeid(T)] = pInstance;
		}

		/// Registers a type as Singleton with a factory
		/// \tparam T Type to register
		/// \tparam TArgs Arguments the factory takes (dependencies will be resolved according to these args)
		/// \param pFactory Factory method
		template <class T, typename... TArgs>
		void RegisterSingleton(std::function<std::shared_ptr<T>(TArgs ...)> && pFactory)
		{
			//check whether the type is registered
			if (!container_contains(registeredTypes_, typeid(T))) {
				//mark the type as registered as singleton
				registeredTypes_[typeid(T)] = Scope::Singleton;
				//add the factory
				AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
				return;
			}

			//assertions for a registered type
			if (registeredTypes_[typeid(T)] != Scope::Singleton) throw ContainerException("Already registered as non- Singleton");

			//add the factory
			AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
		}
//endregion
//region Factory
		/// Registers a type as non- Singleton with a factory
		/// \tparam T Type to register
		/// \tparam TArgs Arguments the factory takes (dependencies will be resolved according to these args)
		/// \param pFactory Factory method
		template <class T, typename... TArgs>
		void RegisterFactory(std::function<std::shared_ptr<T>(TArgs ...)> && pFactory)
		{
			//check whether the type is registered
			if (!container_contains(registeredTypes_, typeid(T))){
				//mark the type as registered as factory
				registeredTypes_[typeid(T)] = Scope::Factory;
				//add the factory
				AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
				return;
			}

			//assertions for a registered type
			if (registeredTypes_[typeid(T)] != Scope::Factory) throw ContainerException("Already registered as non- Factory");

			//add the factory
			AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
		}
//endregion
//region Interface
		template <class TInterface, class T, typename ... TArgs> void RegisterOnInterface()
		{
			static_assert(std::is_base_of<TInterface, T>::value, "T should be derived from the interface");

			//check whether the type is registered
			if (!container_contains(registeredTypes_, typeid(TInterface))){
				//mark the type as registered as factory
				registeredTypes_[typeid(TInterface)] = Scope::Interface;
				//add the link
				AddLink<T, TInterface, TArgs ...>();
				return;
			}

			//assertions for a registered type
			if (registeredTypes_[typeid(TInterface)] != Scope::Interface) throw ContainerException("Already registered as non- Interface");

			//add the link
			AddLink<T, TInterface, TArgs ...>();
		}
//endregion
	};
}

#endif //IOC_CONTAINER_H
