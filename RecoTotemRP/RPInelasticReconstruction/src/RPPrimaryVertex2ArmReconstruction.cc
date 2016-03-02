#include "RecoTotemRP/RPInelasticReconstruction/interface/RPPrimaryVertex2ArmReconstruction.h"
#include "SimG4Core/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimG4Core/Notification/interface/SimG4Exception.h"
#include "HepMC/GenEvent.h"
#include "RecoTotemRP/RPRomanPotResolutionService/interface/RPFitResolution.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPair.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPairCollection.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include <string>
#include <map>
#include "TFile.h"
#include <cassert>
#include <vector>


RPPrimaryVertex2ArmReconstruction::RPPrimaryVertex2ArmReconstruction(const edm::ParameterSet& conf)
 : conf_(conf), resol_degrad_service_(conf)
{
  rpFittedTrackCollectionLabel = conf.getParameter<edm::InputTag>("RPFittedTrackCollectionLabel");
  rpFittedTrackCollectionToken = consumes<RPFittedTrackCollection>(rpFittedTrackCollectionLabel);

  produces< RPReconstructedProtonPairCollection > ();
}


void RPPrimaryVertex2ArmReconstruction::InitInverseParametrizationFitter()
{
  typedef std::map<RPId, LHCOpticsApproximator> rp_param_map_type;
  rp_param_map_type rp_param_map_right;
  rp_param_map_type rp_param_map_left;
  std::string par_name;
  
  if(verbosity_)
    std::cout<<"RPPrimaryVertex2ArmReconstruction::InitInverseParametrizationFitter"<<std::cout;
    
  if(station_220_mandatory_in_reconstruction_ || any_station_in_reconstruction_)
  {
    //220 station right
    char *cmsswPath_220 = getenv("CMSSW_BASE");
    std::string fileName_220_r = std::string(cmsswPath_220) + std::string("/src/") + param_file_name_220_right_;
    TFile *f_220_r = TFile::Open(fileName_220_r.c_str(), "read");
    par_name = param_prefix_220_right_+"_v_1_"+right_beam_postfix_;
    rp_param_map_right[120] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
    
    rp_param_map_right[121] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
    
    par_name = param_prefix_220_right_+"_h_1_"+right_beam_postfix_;
    rp_param_map_right[122] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
    
    par_name = param_prefix_220_right_+"_h_2_"+right_beam_postfix_;
    rp_param_map_right[123] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
    
    par_name = param_prefix_220_right_+"_v_2_"+right_beam_postfix_;
    rp_param_map_right[124] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
    rp_param_map_right[125] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
    f_220_r->Close();
    
    //220 station left
    std::string fileName_220_l = std::string(cmsswPath_220) + std::string("/src/") + param_file_name_220_left_;
    TFile *f_220_l = TFile::Open(fileName_220_l.c_str(), "read");
    par_name = param_prefix_220_left_+"_v_1_"+left_beam_postfix_;
    rp_param_map_left[20] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
    rp_param_map_left[21] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
    
    par_name = param_prefix_220_left_+"_h_1_"+left_beam_postfix_;
    rp_param_map_left[22] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
    
    par_name = param_prefix_220_left_+"_h_2_"+left_beam_postfix_;
    rp_param_map_left[23] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
    
    par_name = param_prefix_220_left_+"_v_2_"+left_beam_postfix_;
    rp_param_map_left[24] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
    rp_param_map_left[25] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
    f_220_l->Close();
  }
  
  if(station_150_mandatory_in_reconstruction_ || any_station_in_reconstruction_)
  {
    //150 station right
    char *cmsswPath_150 = getenv("CMSSW_BASE");
    std::string fileName_150_r = std::string(cmsswPath_150) + std::string("/src/") + param_file_name_150_right_;
    TFile *f_150_r = TFile::Open(fileName_150_r.c_str(), "read");
    par_name = param_prefix_150_right_+"_v_1_"+right_beam_postfix_;
    rp_param_map_right[100] = *((LHCOpticsApproximator*) f_150_r->Get(par_name.c_str()));
    rp_param_map_right[101] = *((LHCOpticsApproximator*) f_150_r->Get(par_name.c_str()));
    
    par_name = param_prefix_150_right_+"_h_1_"+right_beam_postfix_;
    rp_param_map_right[102] = *((LHCOpticsApproximator*) f_150_r->Get(par_name.c_str()));
    
    par_name = param_prefix_150_right_+"_h_2_"+right_beam_postfix_;
    rp_param_map_right[103] = *((LHCOpticsApproximator*) f_150_r->Get(par_name.c_str()));
    
    par_name = param_prefix_150_right_+"_v_2_"+right_beam_postfix_;
    rp_param_map_right[104] = *((LHCOpticsApproximator*) f_150_r->Get(par_name.c_str()));
    rp_param_map_right[105] = *((LHCOpticsApproximator*) f_150_r->Get(par_name.c_str()));
    f_150_r->Close();
    
    //150 station left
    std::string fileName_150_l = std::string(cmsswPath_150) + std::string("/src/") + param_file_name_150_left_;
    TFile *f_150_l = TFile::Open(fileName_150_l.c_str(), "read");
    par_name = param_prefix_150_left_+"_v_1_"+left_beam_postfix_;
    rp_param_map_left[0] = *((LHCOpticsApproximator*) f_150_l->Get(par_name.c_str()));
    rp_param_map_left[1] = *((LHCOpticsApproximator*) f_150_l->Get(par_name.c_str()));
    
    par_name = param_prefix_150_left_+"_h_1_"+left_beam_postfix_;
    rp_param_map_left[2] = *((LHCOpticsApproximator*) f_150_l->Get(par_name.c_str()));
    
    par_name = param_prefix_150_left_+"_h_2_"+left_beam_postfix_;
    rp_param_map_left[3] = *((LHCOpticsApproximator*) f_150_l->Get(par_name.c_str()));
    
    par_name = param_prefix_150_left_+"_v_2_"+left_beam_postfix_;
    rp_param_map_left[4] = *((LHCOpticsApproximator*) f_150_l->Get(par_name.c_str()));
    rp_param_map_left[5] = *((LHCOpticsApproximator*) f_150_l->Get(par_name.c_str()));
    f_150_l->Close();
  }
  
  inv_param_->AddParameterizationsRight(rp_param_map_right);
  inv_param_->AddParameterizationsLeft(rp_param_map_left);
  
  if(verbosity_)
  {
    for(rp_param_map_type::iterator it = rp_param_map_right.begin(); 
      it!=rp_param_map_right.end(); ++it)
    {
      std::cout<<"RPId : "<<it->first;
      it->second.PrintOpticalFunctions();
      std::cout<<std::endl;
    }
    
    for(rp_param_map_type::iterator it = rp_param_map_left.begin(); 
      it!=rp_param_map_left.end(); ++it)
    {
      std::cout<<"RPId : "<<it->first;
      it->second.PrintOpticalFunctions();
      std::cout<<std::endl;
    }
  }
}


RPPrimaryVertex2ArmReconstruction::~RPPrimaryVertex2ArmReconstruction()
{
}


void RPPrimaryVertex2ArmReconstruction::beginRun(edm::Run const& r, edm::EventSetup const& es)
{
  edm::ESHandle<BeamOpticsParams> BOParH;
  es.get<BeamOpticsParamsRcd>().get(BOParH);
  if(!BOParH.isValid())
    throw cms::Exception("ParamMADRefTransport") << " edm::ESHandle<BeamOpticsParams> is invalid";
  BOPar_ = *BOParH;
  
  inv_param_ = std::auto_ptr<RPInverse2SidedParameterization>(new RPInverse2SidedParameterization(conf_, BOPar_));

  edm::LogInfo("RPPrimaryVertex2ArmReconstruction") << "[RPPrimaryVertex2ArmReconstruction::RPPrimaryVertex2ArmReconstruction] Constructing object...";
  verbosity_ = conf_.getParameter<int>("Verbosity");
  inv_param_->Verbosity(verbosity_);
  
  station_150_mandatory_in_reconstruction_ = conf_.getParameter<bool>("Station150MandatoryInReconstruction");
  station_220_mandatory_in_reconstruction_ = conf_.getParameter<bool>("Station220MandatoryInReconstruction");
  any_station_in_reconstruction_ = conf_.getParameter<bool>("AnyStationInReconstruction");
  external_primary_vertex_ = conf_.getParameter<bool>("ExternalPrimaryVertex");
  set_primary_vertex_to_zero_ = conf_.getParameter<bool>("ConstrainPrimaryVertex");
  elastic_scattering_reconstruction_ = conf_.getParameter<bool>("ElasticScatteringReconstruction"); 
  
  external_primary_vertex_ = external_primary_vertex_ && !set_primary_vertex_to_zero_;
  
  if(station_150_mandatory_in_reconstruction_ || any_station_in_reconstruction_)
  {
    param_file_name_150_right_ = conf_.getParameter<std::string>("ParameterizationFileName150Right");
    param_file_name_150_left_ = conf_.getParameter<std::string>("ParameterizationFileName150Left");
    param_prefix_150_right_ = conf_.getParameter<std::string>("ParameterizationNamePrefix150Right");
    param_prefix_150_left_ = conf_.getParameter<std::string>("ParameterizationNamePrefix150Left");
  }
  
  if(station_220_mandatory_in_reconstruction_ || any_station_in_reconstruction_)
  {
    param_file_name_220_right_ = conf_.getParameter<std::string>("ParameterizationFileName220Right");
    param_file_name_220_left_ = conf_.getParameter<std::string>("ParameterizationFileName220Left");
    param_prefix_220_right_ = conf_.getParameter<std::string>("ParameterizationNamePrefix220Right");
    param_prefix_220_left_ = conf_.getParameter<std::string>("ParameterizationNamePrefix220Left");
  }
  
  if(external_primary_vertex_)
  {
    HepMCProductLabel = conf_.getParameter<edm::InputTag>("HepMCProductLabel");
    HepMCProductToken = consumes<edm::HepMCProduct>(HepMCProductLabel);
    primary_vertex_error_.SetX(conf_.getParameter<double>("PrimaryVertexXSigma"));
    primary_vertex_error_.SetY(conf_.getParameter<double>("PrimaryVertexYSigma"));
    primary_vertex_error_.SetZ(conf_.getParameter<double>("PrimaryVertexZSigma"));
  }
  
  if(set_primary_vertex_to_zero_)
  {
    //conversion from meters to mm
    primary_vertex_error_.SetX(BOPar_.GetPrimVertSizeX()*1000.0);
    primary_vertex_error_.SetY(BOPar_.GetPrimVertSizeY()*1000.0);
    primary_vertex_error_.SetZ(BOPar_.GetPrimVertSizeZ()*1000.0);
    primary_vertex_nom_pos_.SetX(BOPar_.GetBeamDisplacementX()*1000.0);
    primary_vertex_nom_pos_.SetY(BOPar_.GetBeamDisplacementY()*1000.0);
    primary_vertex_nom_pos_.SetZ(BOPar_.GetBeamDisplacementZ()*1000.0);
  }
  
  right_beam_postfix_ = conf_.getParameter<std::string>("RightBeamPostfix");
  left_beam_postfix_ = conf_.getParameter<std::string>("LeftBeamPostfix");
  
  rp_multiple_scattering_sigma_ = conf_.getParameter<double>("RPMultipleScatteringSigma");
  
  //init rp ids for the stations
  //order: from ip to outside, then top bottom
  //top first vertical, bottom first vertical, first horizontal, second horizontal, last top vertical, lat bottom vertical 
  for(int i=100; i<=105; ++i)
    station_150_right_ids_.push_back(i);
  
  for(int i=0; i<=5; ++i)
    station_150_left_ids_.push_back(i);
  
  for(int i=120; i<=125; ++i)
    station_220_right_ids_.push_back(i);
  
  for(int i=20; i<=25; ++i)
    station_220_left_ids_.push_back(i);
  
  left_arm_top_pot_ids_.push_back(0);
  left_arm_top_pot_ids_.push_back(4);
  left_arm_top_pot_ids_.push_back(20);
  left_arm_top_pot_ids_.push_back(24);

  left_arm_bot_pot_ids_.push_back(1);
  left_arm_bot_pot_ids_.push_back(5);
  left_arm_bot_pot_ids_.push_back(21);
  left_arm_bot_pot_ids_.push_back(25);
  
  right_arm_top_pot_ids_.push_back(100);
  right_arm_top_pot_ids_.push_back(104);
  right_arm_top_pot_ids_.push_back(120);
  right_arm_top_pot_ids_.push_back(124);
  
  right_arm_bot_pot_ids_.push_back(101);
  right_arm_bot_pot_ids_.push_back(105);
  right_arm_bot_pot_ids_.push_back(121);
  right_arm_bot_pot_ids_.push_back(125);
  
  InitInverseParametrizationFitter();
}

void RPPrimaryVertex2ArmReconstruction::produce(edm::Event& e, const edm::EventSetup& c)
{
  edm::Handle< RPFittedTrackCollection > input; 
  RPReconstructedProtonPairCollection reconstructed_proton_pair_collection;
  e.getByToken(rpFittedTrackCollectionToken, input);

  if(external_primary_vertex_)
    if(!FindPrimaryVertex(e))
    {
      throw SimG4Exception("Primary vertex not found");
    }
  
  if(set_primary_vertex_to_zero_)
  {
    primary_vertex_ = primary_vertex_nom_pos_;
  }
  
  rec_tracks_collection hits_220_r;
  rec_tracks_collection hits_220_l;
  rec_tracks_collection hits_150_r;
  rec_tracks_collection hits_150_l; 
  
  int no_of_pots_220_r = 0;
  int no_of_pots_150_r = 0;
  int no_of_pots_220_l = 0;
  int no_of_pots_150_l = 0;
  
  if(station_150_mandatory_in_reconstruction_ || any_station_in_reconstruction_)
  {
    no_of_pots_150_r = Select150RightHits(*input, hits_150_r);
    no_of_pots_150_l = Select150LeftHits(*input, hits_150_l);
  }
  
  if(station_220_mandatory_in_reconstruction_ || any_station_in_reconstruction_)
  {
    no_of_pots_220_r = Select220RightHits(*input, hits_220_r);
    no_of_pots_220_l = Select220LeftHits(*input, hits_220_l);
  }
  
  bool right_reconstructable = true;
  if(station_150_mandatory_in_reconstruction_)
    right_reconstructable = right_reconstructable && no_of_pots_150_r;
  if(station_220_mandatory_in_reconstruction_)
    right_reconstructable = right_reconstructable && no_of_pots_220_r;
  if(any_station_in_reconstruction_)
    right_reconstructable = right_reconstructable && (no_of_pots_220_r || no_of_pots_150_r);
      
  bool left_reconstructable = true;
  if(station_150_mandatory_in_reconstruction_)
    left_reconstructable = left_reconstructable && no_of_pots_150_l;
  if(station_220_mandatory_in_reconstruction_)
    left_reconstructable = left_reconstructable && no_of_pots_220_l;
  if(any_station_in_reconstruction_)
    left_reconstructable = left_reconstructable && (no_of_pots_220_l || no_of_pots_150_l);
  
  //check for symmetric location of vertical roman pots in elastic reconstruction
  bool allow_elastic_recon = false;
  if(elastic_scattering_reconstruction_)
  {
    allow_elastic_recon = allow_elastic_recon || (CollectionContainsBotRP(hits_150_r) && CollectionContainsTopRP(hits_150_l));
    allow_elastic_recon = allow_elastic_recon || (CollectionContainsBotRP(hits_150_l) && CollectionContainsTopRP(hits_150_r));
    allow_elastic_recon = allow_elastic_recon || (CollectionContainsBotRP(hits_220_r) && CollectionContainsTopRP(hits_220_l));
    allow_elastic_recon = allow_elastic_recon || (CollectionContainsBotRP(hits_220_l) && CollectionContainsTopRP(hits_220_r));    
  }
  else
  {
    allow_elastic_recon = true;
  }
  
  if(right_reconstructable && left_reconstructable && allow_elastic_recon)
  {
    // quick and dirty - copy 150 to 220 and clear 150
    // 150 and 220 are treated like a single station now
    for (rec_tracks_collection::iterator it = hits_150_r.begin(); it != hits_150_r.end(); ++it) 
        hits_220_r[it->first] = it->second;
    hits_150_r.clear();
    for (rec_tracks_collection::iterator it = hits_150_l.begin(); it != hits_150_l.end(); ++it) 
        hits_220_l[it->first] = it->second;
    hits_150_l.clear();
    
    AddInStationMultipleScatteringContribution(hits_220_r, rp_multiple_scattering_sigma_);
    AddInStationMultipleScatteringContribution(hits_220_l, rp_multiple_scattering_sigma_);
    
    Reconstruct(hits_150_r, hits_220_r, hits_150_l, hits_220_l, *inv_param_, 
        reconstructed_proton_pair_collection, 
        external_primary_vertex_ || set_primary_vertex_to_zero_);
  }
  
  std::auto_ptr<RPReconstructedProtonPairCollection> 
        output(new RPReconstructedProtonPairCollection(reconstructed_proton_pair_collection) );
  e.put(output);
}

bool RPPrimaryVertex2ArmReconstruction::CollectionContainsTopRP(const rec_tracks_collection& track_coll)
{
  rec_tracks_collection::const_iterator it;
  rec_tracks_collection::const_iterator beg = track_coll.begin();
  rec_tracks_collection::const_iterator end = track_coll.end();
  
  for(it = beg; it!=end; ++it)
  {
    int rp_pos = it->first % 10;
    if(rp_pos == 0 || rp_pos == 4)
      return true;
  }
  return false;
}

bool RPPrimaryVertex2ArmReconstruction::CollectionContainsBotRP(const rec_tracks_collection& track_coll)
{
  rec_tracks_collection::const_iterator it;
  rec_tracks_collection::const_iterator beg = track_coll.begin();
  rec_tracks_collection::const_iterator end = track_coll.end();
  
  for(it = beg; it!=end; ++it)
  {
    int rp_pos = it->first % 10;
    if(rp_pos == 1 || rp_pos == 5)
      return true;
  }
  return false;
}


void RPPrimaryVertex2ArmReconstruction::
    AddInStationMultipleScatteringContribution(rec_tracks_collection &col, 
    double rp_multiple_scattering_sigma)
{
  typedef rec_tracks_collection::iterator it_type;
  typedef rec_tracks_collection::reverse_iterator rit_type;
  double sig2 = rp_multiple_scattering_sigma*rp_multiple_scattering_sigma;
  
  for(rit_type rit_set = col.rbegin(); rit_set!=col.rend(); ++rit_set)
  {
    double scat_var_contrib = 0.0;
    for(it_type it_comp = col.begin(); it_comp->first < rit_set->first; ++it_comp)
    {
      double dist = rit_set->second.Z() - it_comp->second.Z();
      scat_var_contrib += sig2*dist*dist;
    }
    double vx = rit_set->second.Vx() + scat_var_contrib;
    double vy = rit_set->second.Vy() + scat_var_contrib;
    if(verbosity_)
      std::cout<<"rp:"<<rit_set->first<<" multiple_scat_position_sigma:"
          <<TMath::Sqrt(scat_var_contrib)<<std::endl;
    rit_set->second.Vx(vx);
    rit_set->second.Vy(vy);
  }
}


bool RPPrimaryVertex2ArmReconstruction::FindPrimaryVertex(edm::Event& e)
{
  edm::Handle<edm::HepMCProduct> HepMCEvt;
  e.getByToken(HepMCProductToken, HepMCEvt);

  if(!HepMCEvt.isValid())
  {
     throw SimG4Exception("Unable to find HepMCProduct(HepMC::GenEvent) in edm::Event  ");
  }
  
  const HepMC::GenEvent *evt = HepMCEvt->GetEvent();
  
  int vertex_number = 0;
  for(HepMC::GenEvent::vertex_const_iterator vitr = evt->vertices_begin(); 
        vitr != evt->vertices_end(); ++vitr )
  {
    ++vertex_number;
    const HepMC::FourVector &xvtx = (*vitr)->position();
    primary_vertex_.SetX(xvtx.x() + rand_.Gaus(0, primary_vertex_error_.X()) );
    primary_vertex_.SetY(xvtx.y() + rand_.Gaus(0, primary_vertex_error_.Y()) );
    primary_vertex_.SetZ(xvtx.z() + rand_.Gaus(0, primary_vertex_error_.Z()) );
    if(verbosity_)
    {
      std::cout<<"Vertex found: [" << xvtx.x() << ", " << xvtx.y() << ", " << xvtx.z() << ", " << xvtx.t()
    		<< "]  smeared:("<<primary_vertex_.X()<<", " <<primary_vertex_.Y()<<", "<<
    		primary_vertex_.Z()<<")"<<std::endl;
    }
  }

  return vertex_number==1;
}


//return number of pots through which the proton goes
int RPPrimaryVertex2ArmReconstruction::SelectHits(const RPFittedTrackCollection &tracks, 
    rec_tracks_collection & coll, const station_rp_ids_type& st_ids)
{
  RPFittedTrackCollection::const_iterator it;
  RPFittedTrackCollection::const_iterator low_it;
  RPFittedTrackCollection::const_iterator high_it;
  
  assert(st_ids.size() == 6);
  
  low_it = tracks.lower_bound(st_ids[0]);
  high_it = tracks.upper_bound(st_ids[5]);
  
  for(it = low_it; it!=high_it; ++it)
  {
    if(!it->second.IsValid())
      continue;
    coll[it->first]=resol_degrad_service_.Create2DHit(it->first, it->second);
  }
  
  rec_tracks_collection::const_iterator end;
  end = coll.end();
  
  bool station_accepted = 
    (coll.find(st_ids[0])!=end || coll.find(st_ids[1])!=end || coll.find(st_ids[2])!=end) && 
    (coll.find(st_ids[3])!=end || coll.find(st_ids[4])!=end || coll.find(st_ids[5])!=end) &&
    !(coll.find(st_ids[0])!=end && coll.find(st_ids[1])!=end) &&
    !(coll.find(st_ids[4])!=end && coll.find(st_ids[5])!=end);
  
  if(!station_accepted)
    coll.clear();
  
  return coll.size();
}


int RPPrimaryVertex2ArmReconstruction::Select220RightHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll)
{
  return SelectHits(tracks, coll, station_220_right_ids_);
}


int RPPrimaryVertex2ArmReconstruction::Select220LeftHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll)
{
  return SelectHits(tracks, coll, station_220_left_ids_);
}


int RPPrimaryVertex2ArmReconstruction::Select150RightHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll)
{
  return SelectHits(tracks, coll, station_150_right_ids_);
}


int RPPrimaryVertex2ArmReconstruction::Select150LeftHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll)
{
  return SelectHits(tracks, coll, station_150_left_ids_);
}


void RPPrimaryVertex2ArmReconstruction::Reconstruct(const rec_tracks_collection &rec_col_1, 
    const rec_tracks_collection &rec_col_2, const rec_tracks_collection &rec_col_3, 
    const rec_tracks_collection &rec_col_4, RPInverse2SidedParameterization &inv_par, 
    RPReconstructedProtonPairCollection & rec_prot_pair_col, bool external_prim_vertex)
{
  inv_par.ClearEvent();
  inv_par.AddProtonAtRPCollection(rec_col_1);
  inv_par.AddProtonAtRPCollection(rec_col_2);
  inv_par.AddProtonAtRPCollection(rec_col_3);
  inv_par.AddProtonAtRPCollection(rec_col_4);
  
  if(external_prim_vertex)
    inv_par.SetPrimaryVertex(primary_vertex_, primary_vertex_error_);
  
  RPReconstructedProtonPair rec_prot_pair;
  if(verbosity_)
    inv_par.PrintFittedHitsInfo(std::cout);
    
//  bool fit_valid = 
  inv_par.Fit(rec_prot_pair);
  
//  if(fit_valid)
  {
    rec_prot_pair_col.push_back(rec_prot_pair);
  }
}

DEFINE_FWK_MODULE(RPPrimaryVertex2ArmReconstruction);

