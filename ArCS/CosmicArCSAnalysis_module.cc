////////////////////////////////////////////////////////////////////////
// Class:       CosmicArCSAnalysis
// Module Type: analyzer
// File:        CosmicArCSAnalysis_module.cc
//
// Generated at Fri Aug 8th 2025 by Li Jiaoyang
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

namespace lariat
{
  class CosmicArCSAnalysis;
}

class lariat::CosmicArCSAnalysis : public art::EDAnalyzer
{
public:
  explicit CosmicArCSAnalysis(fhicl::ParameterSet const & p);
  virtual ~CosmicArCSAnalysis();

  // Required functions.
  void analyze(art::Event const & e) override;

  // Selected optional functions.
  //void beginJob() override;
  //void reconfigure(fhicl::ParameterSet const & p);

private:
  bool _debug;
  TTree* _tree;
  int _run, _subrun, _event;
  std::vector<double> _mcp_startx; ///< G4 MCParticle start point X
  std::vector<double> _mcp_starty; ///< G4 MCParticle start point Y
  std::vector<double> _mcp_startz; ///< G4 MCParticle start point Z
  std::vector<double> _mcp_endx; ///< G4 MCParticle end point X
  std::vector<double> _mcp_endy; ///< G4 MCParticle end point Y
  std::vector<double> _mcp_endz; ///< G4 MCParticle end point Z
  std::vector<int> _mcp_pdg; ///< G4 MCParticle PDG code
  std::vector<int> _mcp_isprimary; ///< G4 MCParticle is primary flag
  std::vector<int> _mcp_trackid; ///< G4 MCParticle track ID
  std::vector<int> _mcp_mother; ///< G4 MCParticle mother
  std::vector<int> _mcp_ndaughters; ///< G4 MCParticle number of daughters
  std::vector<double> _mcp_dL; ///< G4 MCParticle start-to-end length
  std::vector<float> _mcp_KE0; ///< G4 MCParticle initial kinetic energy
  std::vector<float> _mcp_T0; ///< G4 MCParticle initial time
  std::vector<std::string> _mcp_process; ///< G4 MCParticle process
  std::vector<std::string> _mcp_endProcess; ///< G4 MCParticle end process
  std::vector<float> _mcp_Px; ///< G4 MCParticle momentum X
  std::vector<float> _mcp_Py; ///< G4 MCParticle momentum Y
  std::vector<float> _mcp_Pz; ///< G4 MCParticle momentum Z
};


lariat::CosmicArCSAnalysis::CosmicArCSAnalysis(fhicl::ParameterSet const & pset)
  : EDAnalyzer(pset)
{

  _debug = pset.get<bool>("Debug", false);
  art::ServiceHandle<art::TFileService> tfs;
  _tree = tfs->make<TTree>("tree","");
  _tree->Branch("run", &_run, "run/I");
  _tree->Branch("subrun", &_subrun, "subrun/I");
  _tree->Branch("event", &_event, "event/I");
  _tree->Branch("mcp_startx", "std::vector<double>", &_mcp_startx);
  _tree->Branch("mcp_starty", "std::vector<double>", &_mcp_starty);
  _tree->Branch("mcp_startz", "std::vector<double>", &_mcp_startz);
  _tree->Branch("mcp_endx", "std::vector<double>", &_mcp_endx);
  _tree->Branch("mcp_endy", "std::vector<double>", &_mcp_endy);
  _tree->Branch("mcp_endz", "std::vector<double>", &_mcp_endz);
  _tree->Branch("mcp_pdg", "std::vector<int>", &_mcp_pdg);
  _tree->Branch("mcp_isprimary", "std::vector<int>", &_mcp_isprimary);
  _tree->Branch("mcp_trackid", "std::vector<int>", &_mcp_trackid);
  _tree->Branch("mcp_mother", "std::vector<int>", &_mcp_mother);
  _tree->Branch("mcp_ndaughters", "std::vector<int>", &_mcp_ndaughters);
  _tree->Branch("mcp_dL", "std::vector<double>", &_mcp_dL);
  _tree->Branch("mcp_KE0", "std::vector<float>", &_mcp_KE0);
  _tree->Branch("mcp_T0", "std::vector<float>", &_mcp_T0);
  _tree->Branch("mcp_process", "std::vector<std::string>", &_mcp_process);
  _tree->Branch("mcp_endProcess", "std::vector<std::string>", &_mcp_endProcess);
  _tree->Branch("mcp_Px", "std::vector<float>", &_mcp_Px);
  _tree->Branch("mcp_Py", "std::vector<float>", &_mcp_Py);
  _tree->Branch("mcp_Pz", "std::vector<float>", &_mcp_Pz);
}

lariat::CosmicArCSAnalysis::~CosmicArCSAnalysis()
{
}

//void lariat::CosmicArCSAnalysis::reconfigure(fhicl::ParameterSet const & pset)
//{
//}

void lariat::CosmicArCSAnalysis::analyze(art::Event const & evt)
{
  
  bool isData = (bool)evt.isRealData();
  if( isData ) return;

  _run = evt.run();
  _subrun = evt.subRun();
  _event = evt.event();

  if (_debug) std::cout<<"CosmicArCSAnalysis: looking at run:subrun:event: "<<_run<<":"<<_subrun<<":"<<_event<<std::endl; 

  // #######################################
  // ### Get potentially useful services ###
  // #######################################
  art::ServiceHandle<cheat::ParticleInventoryService> pi_serv;
  const sim::ParticleList& plist = pi_serv->ParticleList();
  size_t n_mcp = plist.size();
  if (_debug) std::cout<<"Looping through particles: "<<n_mcp<<"\n";
  _mcp_pdg.resize(n_mcp);     _mcp_isprimary.resize(n_mcp);
  _mcp_startx.resize(n_mcp);  _mcp_starty.resize(n_mcp); _mcp_startz.resize(n_mcp);
  _mcp_endx.resize(n_mcp);    _mcp_endy.resize(n_mcp);   _mcp_endz.resize(n_mcp);
  _mcp_trackid.resize(n_mcp); _mcp_mother.resize(n_mcp); _mcp_ndaughters.resize(n_mcp);
  _mcp_dL.resize(n_mcp);      _mcp_KE0.resize(n_mcp);    _mcp_T0.resize(n_mcp);
  _mcp_process.resize(n_mcp); _mcp_endProcess.resize(n_mcp);
  _mcp_Px.resize(n_mcp);      _mcp_Py.resize(n_mcp);     _mcp_Pz.resize(n_mcp);

  for(size_t imcp = 0; imcp < n_mcp; ++imcp) {
    auto& particle = plist.Particle(imcp);

    // Exclude particles that are not propagated
    if (particle->StatusCode() != 1) continue;

    _mcp_pdg[imcp] = particle->PdgCode();
    _mcp_isprimary[imcp] = particle->Process() == "primary";
    _mcp_startx[imcp] = particle->Vx();
    _mcp_starty[imcp] = particle->Vy();
    _mcp_startz[imcp] = particle->Vz();
    _mcp_endx[imcp] = particle->EndX();
    _mcp_endy[imcp] = particle->EndY();
    _mcp_endz[imcp] = particle->EndZ();
    _mcp_trackid[imcp] = particle->TrackId();
    _mcp_mother[imcp] = particle->Mother();
    _mcp_ndaughters[imcp] = particle->NumberDaughters();
    _mcp_dL[imcp] = (particle->Position(0).Vect()-particle->Position(particle->NumberTrajectoryPoints()-1).Vect()).Mag();
    _mcp_KE0[imcp] = 1e3*(particle->E(0)-particle->Mass()); // MeV
    _mcp_T0[imcp] = 1e-3*particle->T(0);
    _mcp_process[imcp] = particle->Process();
    _mcp_endProcess[imcp] = particle->EndProcess();
    _mcp_Px[imcp] = 1e3*particle->Px(); // MeV
    _mcp_Py[imcp] = 1e3*particle->Py(); // MeV
    _mcp_Pz[imcp] = 1e3*particle->Pz(); // MeV

    if (_debug) std::cout << " Particle "<<imcp<<" TrackID: "<<_mcp_trackid[imcp]
                          << " PDG: "<<_mcp_pdg[imcp]
                          << " Start: ("<<_mcp_startx[imcp]<<", "<<_mcp_starty[imcp]<<", "<<_mcp_startz[imcp]<<") cm"
                          << " End: ("<<_mcp_endx[imcp]<<", "<<_mcp_endy[imcp]<<", "<<_mcp_endz[imcp]<<") cm"
                          << " Mother: " << _mcp_mother[imcp]
                          << " NDaughters: " << _mcp_ndaughters[imcp]
                          << " dL: " << _mcp_dL[imcp] <<" cm"
                          << " KE0: " << _mcp_KE0[imcp] <<" MeV"
                          << " T0: " << _mcp_T0[imcp] <<" s"
                          << " Process: " << _mcp_process[imcp]
                          << " EndProcess: " << _mcp_endProcess[imcp]
                          << "),\n Initial mom (Px,Py,Pz,E)=(" << _mcp_Px[imcp]
                          << "," << _mcp_Py[imcp]
                          << "," << _mcp_Pz[imcp]
                          << "," << 1e3*particle->E()
                          << ") MeV" << std::endl;
  }
  _tree->Fill();
}


//void lariat::CosmicArCSAnalysis::beginJob()
//{
//
//}


DEFINE_ART_MODULE(lariat::CosmicArCSAnalysis)
