#pragma once

#include "FFTProxy.h"
#include <io/inputs/InputManagerBase.h>
#include <io/copystyle/InputOLA.h>
#include "window/BartlettWindow.h"
#include "window/RectWindow.h"

template <typename data_type>
/**
 * @brief The FFTInputProxy class
 *
 * Réalise une STFT (short time fourier transform)
 */
class FFTInputProxy : public FFTProxy<data_type>, public InputManagerBase<data_type>
{
		using InputManagerBase<data_type>::channels;
		using InputManagerBase<data_type>::frames;

		using FFTProxy<data_type>::fft;
		using complex_type = typename Parameters<data_type>::complex_type;

	private:
		Input_p inputImpl = nullptr;
		Window_p<data_type> window = nullptr;

	public:
		FFTInputProxy(InputManagerBase<data_type>* input,
					  FFT_p<data_type> fftmanager,
					  Parameters<data_type>& cfg):
			FFTProxy<data_type>(fftmanager, cfg),
			InputManagerBase<data_type>(nullptr, cfg),
			inputImpl(input),
			window(new RectWindow<data_type>())
		{
		}

		FFTInputProxy(Input_p input,
					  FFT_p<data_type> fftmanager,
					  Parameters<data_type>& cfg):
			FFTProxy<data_type>(fftmanager, cfg),
			InputManagerBase<data_type>(nullptr, cfg),
			inputImpl(input),
			window(new RectWindow<data_type>())
		{
		}

		FFTInputProxy(WindowBase<data_type>* wnd,
					  InputManagerBase<data_type>* input,
					  FFT_p<data_type> fftmanager,
					  Parameters<data_type>& cfg):
			FFTProxy<data_type>(fftmanager, cfg),
			InputManagerBase<data_type>(nullptr, cfg),
			inputImpl(input),
			window(wnd)
		{
		}
		virtual ~FFTInputProxy() = default;

		virtual Audio_p getNextBuffer() final override
		{
			// 1. On get le buffer.
			Audio_p tmp = inputImpl->getNextBuffer();
			if(tmp == nullptr) return tmp;
			auto& inbuff = getAudio<double>(tmp);

			// 2. On fenêtre
			// channel.size() : on prend tout. Rajouter la possibilité de faire du 0-fill
			for(auto& channel : inbuff)
				window->apply(channel);

			// 3. On copie dans le buffer de la fft
			std::copy(inbuff.begin(), inbuff.end(), fft->input().begin());

			// 3. fft
			fft->forward();

			// 4. On crée un autre buffer pour le spectre
			// TODO mettre en cache
			auto outbuff = new CData<complex_type>{std::move(fft->spectrum())};

			// 5. On retourne
			return Audio_p(outbuff);
		}
};
