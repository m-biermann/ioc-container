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
#include "warnings.h"

namespace mabiphmo::ioc{
	class ContainerException : public std::runtime_error
	{
	public:
		explicit ContainerException(std::string &&description) : std::runtime_error(description) {}
	};

	class Container : public std::enable_shared_from_this<Container>
	{
		struct ICreatorHolder{
			virtual ~ICreatorHolder() = default;
		};
		template<typename ...TArgs>
		class CreatorHolder : public ICreatorHolder{
			std::function<std::shared_ptr<void>(TArgs&&...)> lambda_;
		public:
			explicit CreatorHolder(std::function<std::shared_ptr<void>(TArgs&&...)> &&lambda) : lambda_(std::move(lambda)){}
			~CreatorHolder() override = default;
			std::shared_ptr<void> operator()(TArgs&&...args){
				return lambda_(std::forward<TArgs>(args)...);
			}
		};
		template<class T> struct wrapper {};

		std::unordered_map<std::type_index, std::shared_ptr<void>> instanceMap_;
		std::unordered_map<std::type_index, std::shared_ptr<ICreatorHolder>> creatorMap_;
		std::set<std::type_index> lookupSet_;

		template<class T,
				template<typename...> class TWrapper, typename...TAdditionalArgs,
				template<typename...> class TDependencyWrapper, typename...TDependencies,
				typename...TArgs>
		std::function<std::shared_ptr<void>(TAdditionalArgs&&...)>
		CreateFactory(wrapper<TWrapper<TAdditionalArgs...>> unused, wrapper<TDependencyWrapper<TDependencies...>> unused2, TArgs&&...args)
		{
			(void)unused;
			(void)unused2;
CLANG_DISABLE_WARNING_PUSH
CLANG_DISABLE_WARNING(-Wunused-lambda-capture)
			return [this, forwardedArgs = std::tuple<TArgs...>(std::forward<TArgs>(args)...)]
					(TAdditionalArgs&&...additionalArgs) mutable -> std::shared_ptr<void>
				{
					return std::apply(
						[this, &additionalArgs...](auto&&...args) -> std::shared_ptr<void>
						{
							return std::move(std::make_shared<T>(
								GetInstance<TDependencies>()...,
						        std::forward<TArgs>(args)...,
				                std::forward<TAdditionalArgs>(additionalArgs)...));
						}, std::move(forwardedArgs));
				};
CLANG_DISABLE_WARNING_POP
		}

		template<class T>
		void SetInstance(std::shared_ptr<void> &&instance){
			std::shared_ptr<void> &holder = instanceMap_[typeid(T)];
			if(holder != nullptr && !holder.unique()){
				throw ContainerException("Already registered AND in use");
			}

			holder = std::move(instance);
		}

		template <class T,
				template<typename...> class TWrapper, typename...TAdditionalArgs,
				template<typename...> class TDependencyWrapper, typename...TDependencies,
				typename ...TArgs,
				class = typename std::enable_if<!std::is_reference<T>::value>::type,
				class = typename std::enable_if<!(std::is_reference<TDependencies>::value || ...)>::type>
		void RegisterClassFactoryImpl(wrapper<TWrapper<TAdditionalArgs...>> wrapper1, wrapper<TDependencyWrapper<TDependencies...>> wrapper2, TArgs &&...args)
		{
			instanceMap_.erase(typeid(T));

			creatorMap_[typeid(T)] = std::dynamic_pointer_cast<ICreatorHolder>(
					std::make_shared<CreatorHolder<TAdditionalArgs...>>(
							CreateFactory<T>(
									wrapper1, wrapper2,
									std::forward<TArgs>(args)...)));
		}

		template <class TInterface, class T,
				template<typename...> class TWrapper, typename...TAdditionalArgs,
				template<typename...> class TDependencyWrapper, typename...TDependencies,
				typename ...TArgs,
				class = typename std::enable_if<std::is_base_of<TInterface, T>::value>::type,
				class = typename std::enable_if<!std::is_reference<TInterface>::value>::type,
				class = typename std::enable_if<!std::is_reference<T>::value>::type,
				class = typename std::enable_if<!(std::is_reference<TDependencies>::value || ...)>::type>
		void RegisterClassFactoryOnInterfaceImpl(wrapper<TWrapper<TAdditionalArgs...>> wrapper1, wrapper<TDependencyWrapper<TDependencies...>> wrapper2, TArgs &&...args)
		{
			instanceMap_.erase(typeid(TInterface));

			creatorMap_[typeid(TInterface)] = std::dynamic_pointer_cast<ICreatorHolder>(
					std::make_shared<CreatorHolder<TAdditionalArgs...>>(
							CreateFactory<T>(
									wrapper1, wrapper2,
									std::forward<TArgs>(args)...)));
		}

	public:
		template <class T,
				typename... TDependencies,
				typename ...TArgs,
				class = typename std::enable_if<!std::is_reference<T>::value>::type,
				class = typename std::enable_if<!(std::is_reference<TDependencies>::value || ...)>::type>
		std::shared_ptr<T> RegisterSingletonClass(TArgs &&...args)
		{
			std::shared_ptr<T> res = std::make_shared<T>(GetInstance<TDependencies>()..., std::forward<TArgs>(args)...);

			SetInstance<T>(std::static_pointer_cast<void>(res));

			creatorMap_.erase(typeid(T));

			return res;
		}

		template <class TInterface, class T,
				typename... TDependencies,
				typename ...TArgs,
				class = typename std::enable_if<std::is_base_of<TInterface, T>::value>::type,
				class = typename std::enable_if<!std::is_reference<TInterface>::value>::type,
				class = typename std::enable_if<!std::is_reference<T>::value>::type,
				class = typename std::enable_if<!(std::is_reference<TDependencies>::value || ...)>::type>
		std::shared_ptr<TInterface> RegisterSingletonClassOnInterface(TArgs &&...args)
		{
			std::shared_ptr<T> res = std::make_shared<T>(GetInstance<TDependencies>()..., std::forward<TArgs>(args)...);

			SetInstance<TInterface>(std::static_pointer_cast<void>(res));

			creatorMap_.erase(typeid(TInterface));

			return res;
		}

		template <class T,
				typename... TDependencies,
				typename ...TArgs,
				class = typename std::enable_if<!std::is_reference<T>::value>::type,
				class = typename std::enable_if<!(std::is_reference<TDependencies>::value || ...)>::type>
		void RegisterSingletonClassFactory(TArgs &&...args)
		{
			SetInstance<T>(nullptr);

			creatorMap_[typeid(T)] = std::dynamic_pointer_cast<ICreatorHolder>(
				std::make_shared<CreatorHolder<>>(
					CreateFactory<T>(
						wrapper<std::tuple<>>(), wrapper<std::tuple<TDependencies...>>(),
				        std::forward<TArgs>(args)...)));
		}

		template <class TInterface, class T,
				typename... TDependencies,
				typename ...TArgs,
				class = typename std::enable_if<std::is_base_of<TInterface, T>::value>::type,
				class = typename std::enable_if<!std::is_reference<TInterface>::value>::type,
				class = typename std::enable_if<!std::is_reference<T>::value>::type,
				class = typename std::enable_if<!(std::is_reference<TDependencies>::value || ...)>::type>
		void RegisterSingletonClassFactoryOnInterface(TArgs &&...args)
		{
			SetInstance<TInterface>(nullptr);

			creatorMap_[typeid(TInterface)] = std::dynamic_pointer_cast<ICreatorHolder>(
				std::make_shared<CreatorHolder<>>(
					CreateFactory<T>(
						wrapper<std::tuple<>>(), wrapper<std::tuple<TDependencies...>>(),
				        std::forward<TArgs>(args)...)));
		}

		template <class T,
				class TWrapper,
				class TDependencyWrapper,
				typename ...TArgs,
				class = typename std::enable_if<!std::is_reference<T>::value>::type>
		void RegisterClassFactory(TArgs &&...args)
		{
			RegisterClassFactoryImpl<T>(wrapper<TWrapper>(), wrapper<TDependencyWrapper>(), std::forward<TArgs>(args)...);
		}

		template <class TInterface, class T,
				class TWrapper,
				class TDependencyWrapper,
				typename ...TArgs,
				class = typename std::enable_if<std::is_base_of<TInterface, T>::value>::type,
				class = typename std::enable_if<!std::is_reference<TInterface>::value>::type,
				class = typename std::enable_if<!std::is_reference<T>::value>::type>
		void RegisterClassFactoryOnInterface(TArgs &&...args)
		{
			RegisterClassFactoryOnInterfaceImpl<TInterface, T>(wrapper<TWrapper>(), wrapper<TDependencyWrapper>(), std::forward<TArgs>(args)...);
		}

		template <class T, typename ...TArgs,
				class = typename std::enable_if<!std::is_reference<T>::value>::type>
		std::shared_ptr<T> GetInstance(TArgs &&...args)
		{
			try{
				if constexpr(std::is_same<T, Container>::value)
					return shared_from_this();
				if(lookupSet_.find(typeid(T)) != lookupSet_.end())
					throw ContainerException("Cyclic reference");
				if(instanceMap_.find(typeid(T)) != instanceMap_.end()){
					std::shared_ptr<void> &holder = instanceMap_.at(typeid(T));
					if(holder == nullptr){
						std::shared_ptr<CreatorHolder<TArgs...>> creatorHolder = std::dynamic_pointer_cast<CreatorHolder<TArgs...>>(creatorMap_.at(typeid(T)));
						if(creatorHolder == nullptr){
							throw ContainerException("Arguments don't match the registered factory.");
						}
						lookupSet_.insert(typeid(T));
						holder = (*creatorHolder)(std::forward<TArgs>(args)...);
						lookupSet_.erase(typeid(T));
						creatorMap_.erase(typeid(T));
					}

					return std::static_pointer_cast<T>(holder);
				}
				std::shared_ptr<CreatorHolder<TArgs...>> creatorHolder = std::dynamic_pointer_cast<CreatorHolder<TArgs...>>(creatorMap_.at(typeid(T)));
				if(creatorHolder == nullptr){
					if constexpr ((std::is_class<TArgs>::value || ...)){
						std::ostringstream oss;
						oss << "Arguments ";
						((oss << typeid(TArgs).name()) , ...);
						oss << " don't match the registered factory.";
						throw ContainerException(oss.str());
					}
					else{
						throw ContainerException("No Arguments provided, does not match any constructor");
					}
				}
				lookupSet_.insert(typeid(T));
				std::shared_ptr<T> result = std::static_pointer_cast<T>((*creatorHolder)(std::forward<TArgs>(args)...));
				lookupSet_.erase(typeid(T));
				return result;
			}
			catch (const ContainerException&e) {
				throw;
			}
			catch (const std::out_of_range&){
				throw ContainerException("Type lookup failed (not registered)");
			}
		}
	};
}

#endif //IOC_CONTAINER_H
