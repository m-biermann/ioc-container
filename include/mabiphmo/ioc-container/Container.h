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
#include <boost/type_index.hpp>
#include "warnings.h"

namespace mabiphmo::ioc_container{
//region ContainerException
    /// Custom exception thrown by the container
	struct ContainerException : public std::runtime_error
	{
        /// Constructor
        /// \param description The description of the error
		explicit ContainerException( const std::string &description) : std::runtime_error(description) {}
	};
//endregion
//region FixedString
	template<unsigned N>
	struct FixedString {
		char buf[N + 1]{};
		constexpr FixedString(char const* s) { // NOLINT(google-explicit-constructor)
			for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
		}
		constexpr operator char const*() const { return buf; } // NOLINT(google-explicit-constructor)
		operator std::string() const { return buf; } // NOLINT(google-explicit-constructor)
	};
	template<unsigned N> FixedString(char const (&)[N]) -> FixedString<N - 1>;
//endregion
//region Container
    /// \brief IoC Container that stores Singleton- Instances and Factories for both Singletons and non- Singletons
    /// \details When factories are registered, dependencies will be resolved by using the arguments of the factory.
    /// All dependencies that should be resolved have to be of type std::shared_ptr<Dependency> and have to come before all other arguments to the factory.
    /// Also all dependencies cannot have any args or have to be a already-resolved singleton, otherwise the container itself should be used as a "dependency" and resolving should be done "manually".
	class Container : public std::enable_shared_from_this<Container>
	{
//region Structs
//region public
	public:
		template <class T, FixedString id = "">
		struct Injection{
			explicit Injection(std::shared_ptr<Container> container) : value(container->ResolveInterface<T, id>()) {}
			std::shared_ptr<T> value;
			operator const std::shared_ptr<T> &() {return value;}
		};

		template <class T>
		struct InjectionRuntimeResolved{
			InjectionRuntimeResolved(std::shared_ptr<Container> container, std::string id) : value(container->ResolveInterface<T>(id)) {}
			std::shared_ptr<T> value;
			operator const std::shared_ptr<T> &() {return value;}
		};

		template<class T>
		struct MultipleInjection{
			explicit MultipleInjection(std::shared_ptr<Container> container) : value(container->ResolveAll<T>()) {}
			std::vector<std::shared_ptr<T>> value;
			operator const std::vector<std::shared_ptr<T>> &() {return value;}
		};

		template <class T>
		struct Dependency{
			explicit Dependency(std::shared_ptr<Container> container) : value(container->Resolve<T>()) {}
			std::shared_ptr<T> value;
			operator const std::shared_ptr<T> &() {return value;}
		};
//endregion
//region private
	private:
		template<typename TContainer>
		struct container_hash {
			std::size_t operator()(const TContainer &container) const{
				return boost::hash_range(container.begin(), container.end());
			}
		};

		template<typename TContainer, typename TKey>
		inline bool container_contains(const TContainer& container, const TKey& key)
		{
			return container.find(key) != container.end();
		}

		/// Scopes of types in the container
		enum class Scope{
			/// Only a single instance should exist while running
			Singleton,
			/// Each time an instance is asked for, a new one will be created
			Transient,
			/// There will (cannot) be any instances of this, however instances of linked types will be resolved
			Interface
		};
//region Dependency Type Resolving
		template <typename... Ts>
		struct list {};

		template <typename, typename, typename, typename, typename>
		struct partition;

		template <typename Head, typename... Tail, typename DependencyList, typename RuntimeDependencyList, typename RuntimeDependencyStringList>
		struct partition<list<Head, Tail...>, DependencyList, RuntimeDependencyList, RuntimeDependencyStringList, list<>> :
				partition<list<>, DependencyList, RuntimeDependencyList, RuntimeDependencyStringList, list<Head, Tail...>>
		{};

		template <typename Head, typename ... Tail, typename DependencyList, typename ... RuntimeDependencies, typename ... RuntimeDependencyStrings>
		struct partition<list<InjectionRuntimeResolved<Head>, Tail...>, DependencyList, list<RuntimeDependencies...>, list<RuntimeDependencyStrings...>, list<>> :
				partition<list<Tail...>, DependencyList, list<RuntimeDependencies..., InjectionRuntimeResolved<Head>>, list<RuntimeDependencyStrings..., std::string>, list<>>
		{};

		template <typename Head, FixedString HeadId, typename... Tail, typename... Dependencies>
		struct partition<list<Injection<Head, HeadId>, Tail...>, list<Dependencies...>, list<>, list<>, list<>> :
				partition<list<Tail...>, list<Dependencies..., Injection<Head, HeadId>>, list<>, list<>, list<>>
		{};

		template <typename Head, typename... Tail, typename... Dependencies>
		struct partition<list<MultipleInjection<Head>, Tail...>, list<Dependencies...>, list<>, list<>, list<>> :
				partition<list<Tail...>, list<Dependencies..., MultipleInjection<Head>>, list<>, list<>, list<>>
		{};

		template <typename Head, typename... Tail, typename... Dependencies>
		struct partition<list<Dependency<Head>, Tail...>, list<Dependencies...>, list<>, list<>, list<>> :
				partition<list<Tail...>, list<Dependencies..., Dependency<Head>>, list<>, list<>, list<>>
		{};

		template <typename DependencyList, typename RuntimeDependencyList, typename RuntimeDependencyStringList, typename ArgsList>
		struct partition<list<>, DependencyList, RuntimeDependencyList, RuntimeDependencyStringList, ArgsList> { using type [[maybe_unused]] = list<DependencyList, RuntimeDependencyList, RuntimeDependencyStringList, ArgsList>; };

		template <typename... Ts>
		using register_traits = typename partition<list<Ts...>, list<>, list<>, list<>, list<>>::type;
//endregion
//endregion
//endregion
//region Member Variables
		std::unordered_map<std::type_index, Scope> registeredTypes_;
		std::unordered_map<std::type_index, std::unordered_map<std::vector<std::type_index>, std::shared_ptr<void>, container_hash<std::vector<std::type_index>>>> registeredFactories_;
		std::unordered_map<std::type_index, std::shared_ptr<void>> registeredInstances_;
		std::unordered_map<std::type_index, std::unordered_map<std::vector<std::type_index>, std::unordered_map<std::string, std::vector<std::shared_ptr<void>>>, container_hash<std::vector<std::type_index>>>> registeredLinks_;
//endregion
//region Functions
//region private
//region AddFactory

		/// Adds the factory after matching dependencies and creating a new factory based on that
		/// \tparam T Type that the factory creates
		/// \tparam F Type of the factory (needed in order to match dependencies)
		/// \tparam TMultipleDependencies Interface - dependencies that get resolved with ResolveAll()
		/// \tparam TDependencies Other dependencies
		/// \tparam TArgs Arguments that should be supplied when resolving
		/// \param pFactory Factory method
		template <typename T, typename F, typename... TDependencies, typename ... RuntimeDependencies, typename ... RuntimeDependencyStrings, typename... TArgs>
		void AddFactoryImpl(list<list<TDependencies...>, list<RuntimeDependencies...>, list<RuntimeDependencyStrings...>, list<TArgs...>>, F && pFactory)
		{
			if (container_contains(registeredInstances_, typeid(T))){
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " already has an instance registered. Cannot add another factory";
				throw ContainerException(ss.str());
			}

			auto new_factory = std::make_shared<std::function<std::shared_ptr<T>(RuntimeDependencyStrings &&..., TArgs &&...)>>(
					[self_weak = weak_from_this(), factory = std::make_shared<F>(pFactory)](RuntimeDependencyStrings &&...dependencyStrings, TArgs &&... args) {
						if(auto self = self_weak.lock())
							return (*factory)(TDependencies(self) ..., RuntimeDependencies(self, std::forward<RuntimeDependencyStrings>(dependencyStrings)) ..., std::forward<TArgs>(args) ...);
						throw ContainerException("Container is expired");
					});

			//add the factory
			registeredFactories_[typeid(T)][std::vector<std::type_index>{typeid(RuntimeDependencyStrings) ..., typeid(TArgs) ...}] = new_factory;
		}

		/// Needed for dependency matching; see <b>AddFactoryImpl</b>
		/// \tparam T Type that the factory creates
		/// \tparam TArgs All arguments of the supplied factory
		/// \param pFactory Factory method
		template <class T, typename... TArgs>
		void AddFactory(std::function<std::shared_ptr<T>(TArgs ...)> && pFactory)
		{
			return AddFactoryImpl<T>(register_traits<TArgs...>{}, std::forward<std::function<std::shared_ptr<T> (TArgs...)>>(pFactory));
		}
//endregion
//region AddLink
		/// Adds a link between TInterface and T
		/// \tparam T The type to link to
		/// \tparam TInterface The Interface to link
		/// \tparam TArgs Arguments that should be supplied when resolving
		template <class TInterface, class T, FixedString id, typename ... TRemainingArgs, typename ... TArgs>
		void AddLink(TArgs &&... args) {
			registeredLinks_[typeid(TInterface)][std::vector<std::type_index>{typeid(TRemainingArgs) ...}][id].insert(registeredLinks_[typeid(TInterface)][std::vector<std::type_index>{typeid(TRemainingArgs) ...}][id].cbegin(), std::make_shared<std::function<std::shared_ptr<TInterface>(TRemainingArgs &&...)>>(
					[self_weak = weak_from_this(), args = std::tuple<TArgs ...>(std::forward<TArgs>(args) ...)](TRemainingArgs &&... remainingArgs){
						return std::apply(
								[self_weak](TArgs &&...args, TRemainingArgs &&...remainingArgs)
								{
									if (auto self = self_weak.lock())
										return std::dynamic_pointer_cast<TInterface>(self->Resolve<T>(std::forward<TArgs>(args) ..., std::forward<TRemainingArgs>(remainingArgs)...));
									throw ContainerException("Container is expired");
								}, std::tuple_cat(args, std::tuple<TRemainingArgs ...>(std::forward<TRemainingArgs>(remainingArgs) ...)));
					}));
		}
//endregion
//region Resolveing
//region ResolveInterface
		template <class T, typename ... TArgs>
		std::shared_ptr<T> ResolveInterface(std::string interfaceId, TArgs &&... args){ // NOLINT(performance-unnecessary-value-param)
			if(!container_contains(registeredLinks_, typeid(T)) || registeredLinks_[typeid(T)].empty()){
				auto ss = std::ostringstream();
				ss << "Interface " << boost::typeindex::type_id<T>().pretty_name() << " has no associated, linked types";
				throw ContainerException(ss.str());
			}
			if(!container_contains(registeredLinks_[typeid(T)], std::vector<std::type_index>{typeid(TArgs) ...}) || registeredLinks_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}].empty()){
				auto ss = std::ostringstream();
				ss << "Interface " << boost::typeindex::type_id<T>().pretty_name() << " has no link with the supplied arguments";
				throw ContainerException(ss.str());
			}
			if(!container_contains(registeredLinks_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}], interfaceId)){
				auto ss = std::ostringstream();
				ss << "Interface " << boost::typeindex::type_id<T>().pretty_name() << " has no link with the supplied id";
				throw ContainerException(ss.str());
			}
			return (*std::static_pointer_cast<std::function<std::shared_ptr<T>(TArgs...)>>(registeredLinks_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}][interfaceId].front()))(std::forward<TArgs>(args) ...);
		}
		template <class T, FixedString id = "", typename ... TArgs>
		std::shared_ptr<T> ResolveInterface(TArgs &&... args){
			return ResolveInterface<T>((std::string)id, std::forward<TArgs>(args) ...);
		}
//endregion
//region All
		/// Resolves all available (parameterless) linked implementations of T (which has to be an interface)
		/// \tparam T The Interface to resolve
		/// \return All linked implementations of T
		template <class T>
		std::vector<std::shared_ptr<T>> ResolveAll()
		{
			if(!container_contains(registeredTypes_, typeid(T))){
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is not registered";
				throw ContainerException(ss.str());
			}
			if(registeredTypes_[typeid(T)] != Scope::Interface) {
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name()<< " is not registered as an Interface";
				throw ContainerException(ss.str());
			}
			if(!container_contains(registeredLinks_, typeid(T)) || registeredLinks_[typeid(T)].empty()){
				auto ss = std::ostringstream();
				ss << "Interface " << boost::typeindex::type_id<T>().pretty_name() << " has no associated, linked types";
				throw ContainerException(ss.str());
			}

			auto res = std::vector<std::shared_ptr<T>>();
			for (const auto& id_vector : registeredLinks_[typeid(T)][std::vector<std::type_index>()])
			{
				for(auto link : id_vector.second)
					res.insert(res.end(), (*std::static_pointer_cast<std::function<std::shared_ptr<T>()>>(link))());
			}
			return res;
		}
//endregion
//endregion
//endregion
//region public
	public:
//region Construction
        /// Default constructor
        Container() = default;
//endregion
//region Registering
//region T::Register
        /// \brief Registers a type as defined in T::Register
        /// \details The T::Register function should be like <b><tt>void Register(std::shared_ptr<Container> &)</tt></b>
        /// \tparam T Type to register
        template <class T>
        void Register()
        {
            T::Register(shared_from_this());
        }
//endregion
//region Singleton
//region Instance
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
			if (registeredTypes_[typeid(T)] != Scope::Singleton){
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is already registered as non - Singleton";
				throw ContainerException(ss.str());
			}
			if (container_contains(registeredInstances_, typeid(T))){
				auto ss = std::ostringstream();
				ss << "Singleton " << boost::typeindex::type_id<T>().pretty_name() << " already has a registered instance";
				throw ContainerException(ss.str());
			}

			//add the instance
			registeredInstances_[typeid(T)] = pInstance;
		}
//endregion
//region Factory
		/// Registers a type as Singleton with a factory
		/// \tparam T Type to register
		/// \tparam TArgs Arguments the factory takes (dependencies will be resolved according to these args)
		/// \param pFactory Transient method
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
			if (registeredTypes_[typeid(T)] != Scope::Singleton){
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is already registered as non - Singleton";
				throw ContainerException(ss.str());
			}

			//add the factory
			AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
		}
//endregion
//endregion
//region Transient
		/// Registers a type as non- Singleton with a factory
		/// \tparam T Type to register
		/// \tparam TArgs Arguments the factory takes (dependencies will be resolved according to these args)
		/// \param pFactory Transient method
		template <class T, typename... TArgs>
		void RegisterTransient(std::function<std::shared_ptr<T>(TArgs ...)> && pFactory)
		{
			//check whether the type is registered
			if (!container_contains(registeredTypes_, typeid(T))){
				//mark the type as registered as factory
				registeredTypes_[typeid(T)] = Scope::Transient;
				//add the factory
				AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
				return;
			}

			//assertions for a registered type
			if (registeredTypes_[typeid(T)] != Scope::Transient){
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is already registered as non - Transient";
				throw ContainerException(ss.str());
			}

			//add the factory
			AddFactory<T>(std::forward<std::function<std::shared_ptr<T>(TArgs ...)>>(pFactory));
		}
//endregion
//region Interface
		/// Registers TInterface to be resolvable by resolving via T
		/// \tparam TRemainingArgs The arguments that can be supplied when resolving TInterface
		template <class TInterface, class T, FixedString id = "", typename ... TRemainingArgs, typename ... TArgs>
		void RegisterOnInterface(TArgs &&... args)
		{
			static_assert(std::is_base_of<TInterface, T>::value, "T should be derived from the interface");

			//check whether the type is registered
			if (!container_contains(registeredTypes_, typeid(TInterface))){
				//mark the type as registered as factory
				registeredTypes_[typeid(TInterface)] = Scope::Interface;
				//add the link
				AddLink<TInterface, T, id, TRemainingArgs ...>(std::forward<TArgs>(args) ...);
				return;
			}

			//assertions for a registered type
			if (registeredTypes_[typeid(TInterface)] != Scope::Interface){
				auto ss = std::ostringstream();
				ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is already registered as non - Interface";
				throw ContainerException(ss.str());
			}

			//add the link
			AddLink<TInterface, T, id, TRemainingArgs ...>(std::forward<TArgs>(args) ...);
		}
//endregion
//endregion
//region Resolving

		/// Resolves the type T with the supplied arguments
		/// \tparam T The type to resolve
		/// \tparam TArgs The type of the arguments that will be used when resolving
		/// \param args The arguments that will be used when resolving
		/// \return The resolved instance
		template <class T, typename ... TArgs>
		std::shared_ptr<T> Resolve(TArgs &&... args)
		{
			if constexpr(std::is_same<T, Container>::value)
				return shared_from_this();
			else{
				if(!container_contains(registeredTypes_, typeid(T))){
					auto ss = std::ostringstream();
					ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is not registered";
					throw ContainerException(ss.str());
				}

				switch(registeredTypes_[typeid(T)]){
					case Scope::Singleton:
						if(container_contains(registeredInstances_, typeid(T))) return std::static_pointer_cast<T>(registeredInstances_[typeid(T)]);
						if(!container_contains(registeredFactories_, typeid(T))){
							auto ss = std::ostringstream();
							ss << "Singleton " << boost::typeindex::type_id<T>().pretty_name() << " has neither an instance nor a factory";
							throw ContainerException(ss.str());
						}
						if(!container_contains(registeredFactories_[typeid(T)], std::vector<std::type_index>{typeid(TArgs) ...})){
							auto ss = std::ostringstream();
							ss << "Singleton " << boost::typeindex::type_id<T>().pretty_name() << " has no factory with the supplied arguments";
							throw ContainerException(ss.str());
						}
						registeredInstances_[typeid(T)] = (*std::static_pointer_cast<std::function<std::shared_ptr<T>(TArgs...)>>(registeredFactories_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}]))(std::forward<TArgs>(args) ...);
						return std::static_pointer_cast<T>(registeredInstances_[typeid(T)]);
					case Scope::Transient:
						if(!container_contains(registeredFactories_, typeid(T))){
							auto ss = std::ostringstream();
							ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " has no registered factory methods";
							throw ContainerException(ss.str());
						}
						if(!container_contains(registeredFactories_[typeid(T)], std::vector<std::type_index>{typeid(TArgs) ...})){
							auto ss = std::ostringstream();
							ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " has no factory method with the supplied arguments";
							throw ContainerException(ss.str());
						}
						return (*std::static_pointer_cast<std::function<std::shared_ptr<T>(TArgs...)>>(registeredFactories_[typeid(T)][std::vector<std::type_index>{typeid(TArgs) ...}]))(std::forward<TArgs>(args) ...);
					case Scope::Interface:
						return ResolveInterface<T, "">(std::forward<TArgs>(args) ...);
					default:{
						auto ss = std::ostringstream();
						ss << "Type " << boost::typeindex::type_id<T>().pretty_name() << " is registered with an invalid Scope";
						throw ContainerException(ss.str());
					}
				}
			}
		}
//endregion
//endregion
//endregion
	};
//endregion
}

#endif //IOC_CONTAINER_H
