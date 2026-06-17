#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"  // for raw::Uncompress

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace arcs {
  class RawDigitDumper;
}

class arcs::RawDigitDumper : public art::EDAnalyzer {
public:
  explicit RawDigitDumper(fhicl::ParameterSet const& p);
  void analyze(art::Event const& e) override;

private:
  std::string fInputLabel;   // e.g. "daq" or "tpcrawdecoder"
  std::string fOutputFile;
  std::ofstream fOut;
};

arcs::RawDigitDumper::RawDigitDumper(fhicl::ParameterSet const& p)
  : EDAnalyzer(p)
  , fInputLabel(p.get<std::string>("InputLabel", "daq"))
  , fOutputFile(p.get<std::string>("OutputFile", "rawdigits.csv"))
{
  fOut.open(fOutputFile);
  fOut << "run,subrun,event,channel,tick,adc\n";  // header
}

void arcs::RawDigitDumper::analyze(art::Event const& e)
{
     auto run    = e.run();
     auto subrun = e.subRun();
     auto event  = e.event();

     auto const& digitHandle = e.getValidHandle<std::vector<raw::RawDigit>>(fInputLabel);

     for (auto const& digit : *digitHandle) {
          unsigned int channel = digit.Channel();
          unsigned int nSamples = digit.Samples();

          // Uncompress ADC values
          std::vector<short> adcVec(nSamples);
          raw::Uncompress(digit.ADCs(), adcVec, digit.Compression());

          for (unsigned int tick = 0; tick < nSamples; ++tick) {
               fOut << run    << ","
                    << subrun << ","
                    << event  << ","
                    << channel << ","
                    << tick    << ","
                    << adcVec[tick] << "\n";
          }
     }
}

DEFINE_ART_MODULE(arcs::RawDigitDumper)
