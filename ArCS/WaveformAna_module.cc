////////////////////////////////////////////////////////////////////////
// Class:       WaveformAna
// Module Type: analyzer
// File:        WaveformAna_module.cc
////////////////////////////////////////////////////////////////////////

// ##########################
// ### Framework includes ###
// ##########################
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
//#include "cetlib/maybe_ref.h"

// ########################
// ### LArSoft includes ###
// ########################
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/CryostatGeo.h"
#include "larcorealg/Geometry/TPCGeo.h"
#include "larcorealg/Geometry/PlaneGeo.h"
#include "larcorealg/Geometry/WireGeo.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/TrackHitMeta.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardata/ArtDataHelper/TrackUtils.h" // lar::util::TrackPitchInView()
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "lardata/Utilities/AssociationUtil.h"

//#include "RawData/ExternalTrigger.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "larevt/Filters/ChannelFilter.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "larreco/RecoAlg/TrackMomentumCalculator.h"
#include "LArIATRecoAlg/BeamlineMassAlg.h"
#include "LArIATDataProducts/WCTrack.h"
#include "LArIATDataProducts/TOF.h"
#include "LArIATDataProducts/AGCounter.h"
#include "RawDataUtilities/TriggerDigitUtility.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/EndPoint2D.h"
#include "lardataobj/MCBase/MCShower.h"
#include "lardataobj/MCBase/MCStep.h"
#include "larreco/Calorimetry/CalorimetryAlg.h"

// #####################
// ### ROOT includes ###
// #####################
#include "TComplex.h"
#include "TFile.h"
#include "TH2D.h"
#include "TF1.h"
#include "TTree.h"
#include "TTimeStamp.h"



namespace arcs
{
  class WaveformAna;
}

class arcs::WaveformAna : public art::EDAnalyzer
{
public:
  explicit WaveformAna(fhicl::ParameterSet const & p);
  virtual ~WaveformAna();

  // Required functions.
  void analyze(art::Event const & e) override;

  // Selected optional functions.
  void beginJob() override;

private:
  std::string _rawdigit_producer = "daq";

  TTree* _tree;

  int _run, _subrun, _event;
  std::vector<std::vector<float>> _waveforms;
 
};


arcs::WaveformAna::WaveformAna(fhicl::ParameterSet const & pset)
  : EDAnalyzer(pset)
{
  art::ServiceHandle<art::TFileService> fs;
  _tree = fs->make<TTree>("tree","");
  _tree->Branch("waveforms", "std::vector<std::vector<float>>", &_waveforms);
}

arcs::WaveformAna::~WaveformAna()
{
  // Clean up dynamic memory and other resources here.
}



void arcs::WaveformAna::analyze(art::Event const & evt)
{
  
  art::ServiceHandle<geo::Geometry> geom;
  
  art::Handle<std::vector<raw::RawDigit>> rawdigit_h;
  evt.getByLabel(_rawdigit_producer, rawdigit_h);
  if(!rawdigit_h.isValid()){
    std::cout << "RawDigit product " << _rawdigit_producer << " not found..." << std::endl;
    throw std::exception();
  }
  std::vector<art::Ptr<raw::RawDigit>> rawdigit_v;
  art::fill_ptr_vector(rawdigit_v, rawdigit_h); 


  std::cout << "Number of available RawDigits " << rawdigit_v.size() << std::endl;
  
  _waveforms.clear();
  _waveforms.resize(480);

  for (auto const &rawdigit : rawdigit_v)
  {

    unsigned int ch  = rawdigit->Channel();
    float        ped = rawdigit->GetPedestal();

    std::vector<geo::WireID> widVec = geom->ChannelToWire(ch);
    for (geo::WireID w_id : widVec) {
      unsigned int wire = w_id.Wire;
      unsigned int plane = w_id.Plane;
      unsigned int tpc = w_id.TPC;
      unsigned int cryo = w_id.Cryostat;

      std::cout << "RawDigit ch " << ch << ", wire " << wire << ", plane " << plane << ", tpc " << tpc << ", cryo " << cryo << std::endl;

      auto adcs = rawdigit->ADCs();
      _waveforms[ch].assign(adcs.begin(), adcs.end());
    }
  }

  _tree->Fill();
}


void arcs::WaveformAna::beginJob()
{     
}




DEFINE_ART_MODULE(arcs::WaveformAna)
