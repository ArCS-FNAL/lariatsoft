#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"

#include <vector>
#include <algorithm>

namespace arcs {
     class EmptyEventFilter;
}

class arcs::EmptyEventFilter : public art::EDFilter {
public:
     explicit EmptyEventFilter(fhicl::ParameterSet const& p);
     bool filter(art::Event& e) override;

private:
     std::string fInputLabel;
     int         fMinChannels;    // min number of signal channels to pass
     bool        fdebug;          // print debug info for each channel
};

arcs::EmptyEventFilter::EmptyEventFilter(fhicl::ParameterSet const& p)
     : EDFilter(p)
     , fInputLabel  (p.get<std::string>("InputLabel",   "daq"))
     , fMinChannels (p.get<int>        ("MinChannels",    3))
     , fdebug       (p.get<bool>       ("Debug",          true))
{}

bool arcs::EmptyEventFilter::filter(art::Event& e)
{
     auto const& digits = e.getValidHandle<std::vector<raw::RawDigit>>(fInputLabel);

     int nSignalChannels = 0;
     for (auto const& digit : *digits) {

          std::vector<short> adcVec(digit.Samples());
          raw::Uncompress(digit.ADCs(), adcVec, digit.Compression());

          // Median of full waveform — signal occupies few ticks so median ≈ baseline
          std::vector<short> sorted = adcVec;
          size_t mid = sorted.size() / 2;
          std::nth_element(sorted.begin(), sorted.begin() + mid, sorted.end());
          short baseline = sorted[mid];
          
          // Noise via MAD
          std::vector<short> residuals;
          residuals.reserve(adcVec.size());
          for (auto v : adcVec) residuals.push_back(std::abs(v - baseline));
          std::nth_element(residuals.begin(), residuals.begin() + mid, residuals.end());
          float noise = residuals[mid] * 1.4826f;

          float threshold = baseline + 3.f * noise;
          short maxADC = *std::max_element(adcVec.begin(), adcVec.end());

          if (fdebug){
               std::cout << "Channel " << digit.Channel() 
                         << ": baseline = " << baseline 
                         << ", noise = " << noise 
                         << ", threshold = " << threshold 
                         << ", maxADC = " << maxADC
                         << std::endl;
          }
          float maxSigma = (maxADC - baseline) / noise;
          if (maxSigma > 10.f) {
               ++nSignalChannels;  // 10σ is very unlikely from noise
               if (nSignalChannels >= fMinChannels) return true;
          }
          /*if (maxADC > threshold) {
               ++nSignalChannels;
               std::cout<<"here"<<std::endl;
          }*/
          if (fdebug){
               std::cout<<"number of signal channels above threshold: "<<nSignalChannels<<std::endl;
          }
          
     }

     mf::LogInfo("EmptyEventFilter")
          << "Rejecting event " << e.event()
          << " — only " << nSignalChannels << " signal channels above 3σ";
     return false;
}

DEFINE_ART_MODULE(arcs::EmptyEventFilter)

