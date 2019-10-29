#pragma once

#include "IwAsset.h"
#include "IAssetLoader.h"
#include <unordered_map>

namespace IW {
	inline namespace Asset {
		class AssetManager {
		private:
			std::unordered_map<size_t, IAssetLoader*> m_loaders;

		public:
			AssetManager() {}

			~AssetManager() {
				for (auto pair : m_loaders) {
					delete pair.second;
				}
			}

			template<
				typename _l,
				typename... _args>
			void SetLoader(
				_args&&... args)
			{
				_l* loader = new _l(*this, args...);
				m_loaders[loader->GetType()] = loader;
			}

			template<
				typename _t>
			_t* Load(
				const char* filepath)
			{
				auto itr = m_loaders.find(typeid(_t).hash_code());
				if (itr != m_loaders.end()) {
					return (_t*)itr->second->Load(filepath);
				}

				return nullptr;
			}

			template<
				typename _t>
			void Give(
				const char* filepath,
				_t* asset)
			{
				auto itr = m_loaders.find(typeid(_t).hash_code());
				if (itr != m_loaders.end()) {
					itr->second->Give(filepath, asset);
				}
			}
		};
	}
}
