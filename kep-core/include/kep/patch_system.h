#pragma once
#include <string>
#include <cstdint>
#include <ogre/OgreVector3.h>
#include <ogre/OgreQuaternion.h>

namespace KEP
{
	namespace PatchSystem
	{
		void init();

		class ModReader
		{
		public:
			ModReader(const char* path)
			{
				m_ifs.open(path, std::ios::in | std::ios::binary);
			}

			ModReader(const std::string& path)
			{
				m_ifs.open(path, std::ios::in | std::ios::binary);
			}

			~ModReader()
			{
				if (m_ifs.is_open())
					m_ifs.close();
			}

			bool isOk() const { return !m_ifs.fail(); }
			bool isErr() const { return m_ifs.fail(); }

			bool readBoolean()
			{
				bool buf = false;
				m_ifs.read((char*)&buf, sizeof(bool));
				return buf;
			}

			int32_t readInt32()
			{
				int32_t buf = 0;
				m_ifs.read((char*)&buf, sizeof(int32_t));
				return buf;
			}

			uint32_t readUInt32()
			{
				uint32_t buf = 0;
				m_ifs.read((char*)&buf, sizeof(uint32_t));
				return buf;
			}

			float readFloat()
			{
				float buf = 0.0f;
				m_ifs.read((char*)&buf, sizeof(float));
				return buf;
			}

			std::string readString()
			{
				size_t count = readUInt32();
				size_t ignoreSize = 0;
				if (512 < count)
				{
					ignoreSize = count - 512;
					count = 512;
				}

				char buf[512] = {};
				m_ifs.read((char*)&buf, sizeof(char) * count);

				if (ignoreSize > 0)
					m_ifs.seekg(ignoreSize, std::ios_base::cur);

				std::string str(buf, count);
				return str;
			}

			Ogre::Vector3 readVector3()
			{
				Ogre::Vector3 buf(0.0f, 0.0f, 0.0f);
				m_ifs.read((char*)&buf, sizeof(Ogre::Vector3));
				return buf;
			}

			Ogre::Quaternion readQuaternion()
			{
				Ogre::Quaternion buf(1.0f, 0.0f, 0.0f, 0.0f);
				m_ifs.read((char*)&buf, sizeof(Ogre::Quaternion));
				return buf;
			}

			void readHeader(int32_t fileVersion)
			{
				if (fileVersion > 16)
				{
					auto headerSize = readUInt32();
					m_ifs.ignore(headerSize);
				}
				else if (fileVersion == 16)
				{
					auto version = readInt32();
					auto offset = readUInt32();
					m_ifs.ignore(offset);
					offset = readUInt32();
					m_ifs.ignore(offset);
					offset = readUInt32();
					m_ifs.ignore(offset);
					offset = readUInt32();
					m_ifs.ignore(offset);
				}
			}

			void ignore(int32_t offset)
			{
				m_ifs.ignore(offset);
			}

		private:
			std::ifstream m_ifs;
		};
	}
}