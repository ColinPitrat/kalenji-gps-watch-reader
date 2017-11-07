#ifndef _REGISTRY_H
#define _REGISTRY_H

#include <string>
#include <map>

template <typename LayerType>
class LayerRegistry
{
	public:
		static LayerRegistry* getInstance()
		{
			if(_instance == nullptr)
			{
				_instance = new LayerRegistry();
			}
			return _instance;
		}
		~LayerRegistry()
		{
			for(typename std::map<std::string, LayerType*>::iterator it = _objects.begin(); it != _objects.end(); ++it)
			{
				delete it->second;
			}
			_objects.clear();
		}
		void registerObject(std::string objectName, LayerType *object)
		{
			std::pair<typename std::map<std::string, LayerType*>::iterator, bool> result;
			result = _objects.insert(std::pair<std::string, LayerType*>(objectName, object));
			if(!result.second)
			{
				// Insert failed because a object of this type with this name is already registered
				// TODO: Throw an exception ?
				delete object;
			}
		}
		// TODO: What if filter is not registered ?
		LayerType *getObject(std::string objectName) { return _objects[objectName]; };
		std::map<std::string, LayerType*> &getObjects() { return _objects; };

	private:
		LayerRegistry() = default;
		static LayerRegistry* _instance;
		std::map<std::string, LayerType*> _objects;
};

template <typename LayerType, typename LayerClass>
class LayerRegistrer
{
	public:
		LayerRegistrer()
		{
			LayerType *object = new LayerClass();
			LayerRegistry<LayerType>::getInstance()->registerObject(object->getName(), object);
		}
};

template <typename LayerType>
LayerRegistry<LayerType> *LayerRegistry<LayerType>::_instance = nullptr;

#endif
