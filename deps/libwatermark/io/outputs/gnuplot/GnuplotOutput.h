#pragma once

#include <io/outputs/OutputManagerBase.h>
#include <mathutils/math_util.h>
#include "gnuplot_private/gnuplot_i.h"

template <typename data_type>
/**
 * @brief The GnuplotOutput class
 *
 * Affichage de tableaux de samples avec gnuplot.
 * (les buffers sont passés en sortie)
 */
class GnuplotOutput:public OutputManagerBase<data_type>
{
		using IOManagerBase<data_type>::v;
		using IOManagerBase<data_type>::channels;
		using IOManagerBase<data_type>::frames;

	private:
		gnuplot_ctrl *h = nullptr;
		Output_p outputImpl = nullptr;

	public:
		GnuplotOutput(OutputManagerBase<data_type>* output, Parameters<data_type>& cfg):
			OutputManagerBase<data_type>(nullptr, cfg),
			outputImpl(output)
		{
			h = gnuplot_init();
			gnuplot_setstyle(h, "lines");
		}

		virtual ~GnuplotOutput()
		{
			gnuplot_close(h);
		}

		GnuplotOutput(const GnuplotOutput&) = delete;
		GnuplotOutput& operator=(const GnuplotOutput&) = delete;

		virtual void writeNextBuffer(Audio_p& data) override
		{
			auto& buffer{getAudio<data_type>(data)};

			for(auto& channel : buffer)
			{
				gnuplot_resetplot(h);
				gnuplot_plot_x(h, channel.data(), channel.size(), "Plot");
				sleep(3);
			}

			outputImpl->writeNextBuffer(data);
		}
};
