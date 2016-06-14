#pragma once
#include <sndfile.hh>

#include <io/inputs/InputManagerBase.h>
#include <mathutils/math_util.h>

template <typename data_type>
/**
 * @brief The FileInput class
 *
 * Lit un fichier à l'aide de libsndfile
 */
class FileInput : public InputManagerBase<data_type>
{
	public:
		using InputManagerBase<data_type>::InputManagerBase;

		FileInput(std::string filename, Parameters<data_type>& cfg):
			InputManagerBase<data_type>(cfg)
		{
			readFile(filename);
		}

		FileInput(std::string filename, InputCopy<data_type>* icp, Parameters<data_type>& cfg):
			InputManagerBase<data_type>(icp, cfg)
		{
			readFile(filename);
		}

		virtual void readFile(std::string& str)
		{
			auto myf = SndfileHandle(str);

			this->conf.samplingRate = myf.samplerate();

			std::vector<data_type> vec(myf.frames() * myf.channels());

			int parity = (myf.frames() % 2 != 0) ? -1 : 0;
			for(int i = 0; i < myf.channels(); ++i)
				myf.read(vec.data() + i * (myf.frames() + parity),  myf.frames() + parity);

			this->v() = MathUtil::deinterleave(vec, (unsigned int) myf.channels(), (unsigned int) myf.frames());

			std::cerr << "Filename: " << str << std::endl;
			std::cerr << "Channels: " << this->v().size() << std::endl;
		}
};
