/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "RecoLocalCTPPS/TotemRP/interface/TotemRPLocalTrackFitterAlgorithm.h"

#include "TMath.h"
#include "TMatrixD.h"

//----------------------------------------------------------------------------------------------------

TotemRPLocalTrackFitterAlgorithm::TotemRPLocalTrackFitterAlgorithm(const edm::ParameterSet &)
{
}

//----------------------------------------------------------------------------------------------------


void TotemRPLocalTrackFitterAlgorithm::Reset()
{
  cout << ">> TotemRPLocalTrackFitterAlgorithm::Reset" << endl;
  det_data_map_.clear();
}

//----------------------------------------------------------------------------------------------------

RPDetCoordinateAlgebraObjs TotemRPLocalTrackFitterAlgorithm::PrepareReconstAlgebraData(unsigned int det_id,
  const TotemRPGeometry& tot_rp_geom)
{
  //cout << ">> TotemRPLocalTrackFitterAlgorithm::PrepareReconstAlgebraData: " << det_id << endl;

  RPDetCoordinateAlgebraObjs det_algebra_obj;

  det_algebra_obj.centre_of_det_global_position_ = convert3vector(tot_rp_geom.GetDetTranslation(det_id));
 
  HepMC::ThreeVector rp_topology_stripaxis = rp_topology_.GetStripReadoutAxisDir();
  CLHEP::Hep3Vector rp_topology_stripaxis_clhep;
  
  rp_topology_stripaxis_clhep.setX(rp_topology_stripaxis.x());
  rp_topology_stripaxis_clhep.setY(rp_topology_stripaxis.y());
  rp_topology_stripaxis_clhep.setZ(rp_topology_stripaxis.z());
 
  TVector3 rd_dir = convert3vector( 
      tot_rp_geom.LocalToGlobalDirection(det_id, rp_topology_stripaxis_clhep ) );
  
  TVector2 v(rd_dir.X(), rd_dir.Y());
  det_algebra_obj.readout_direction_ = v.Unit();
  det_algebra_obj.rec_u_0_ = 0.0;
  det_algebra_obj.available_ = det_availability_.IsRPDetAvailableForTrackFitting(det_id);
  det_algebra_obj.rec_u_0_ = - (det_algebra_obj.readout_direction_*
      det_algebra_obj.centre_of_det_global_position_.XYvector());
  
  return det_algebra_obj;
}

//----------------------------------------------------------------------------------------------------

RPDetCoordinateAlgebraObjs * TotemRPLocalTrackFitterAlgorithm::GetDetAlgebraData(
    unsigned int det_id, const TotemRPGeometry& tot_rp_geom)
{
  DetReconstructionDataMap::iterator it;
  it = det_data_map_.find(det_id);
  if(it!=det_data_map_.end())
    return &(it->second);
  else
  {
    det_data_map_[det_id] = PrepareReconstAlgebraData(det_id, tot_rp_geom);
    return &det_data_map_[det_id];
  }
}

//----------------------------------------------------------------------------------------------------

#if 0
TVector2 TotemRPLocalTrackFitterAlgorithm::ComputeXYPointInZDir(const TotemRPRecHit& hit_0, const TotemRPRecHit& hit_1, const TotemRPGeometry &tot_geom)
{
  RPDetCoordinateAlgebraObjs *alg_obj_0 = GetDetAlgebraData(hit_0.DetId(), tot_geom);
  RPDetCoordinateAlgebraObjs *alg_obj_1 = GetDetAlgebraData(hit_1.DetId(), tot_geom);
  if(!alg_obj_0->available_ || !alg_obj_1->available_)
    return TVector2();
  
  TMatrixD point_rec_matrix(2,2);
  point_rec_matrix(0,0) = alg_obj_0->readout_direction_.X();
  point_rec_matrix(0,1) = alg_obj_0->readout_direction_.Y();
  point_rec_matrix(1,0) = alg_obj_1->readout_direction_.X();
  point_rec_matrix(1,1) = alg_obj_1->readout_direction_.Y();
  point_rec_matrix.Invert();
  TVectorD vect(2);
  vect[0] = hit_0.Position() - alg_obj_0->rec_u_0_;
  vect[1] = hit_1.Position() - alg_obj_1->rec_u_0_;
  vect*=point_rec_matrix;
  return TVector2(vect[0], vect[1]);
}
#endif

//----------------------------------------------------------------------------------------------------

#if 0
TVector2 TotemRPLocalTrackFitterAlgorithm::ComputeXYPointOfTheGivenLine(const TotemRPRecHit& hit_0, const TotemRPRecHit& hit_1, double tx, double ty, double z0, const TotemRPGeometry &tot_geom)
{
  RPDetCoordinateAlgebraObjs *alg_obj_0 = GetDetAlgebraData(hit_0.DetId(), tot_geom);
  RPDetCoordinateAlgebraObjs *alg_obj_1 = GetDetAlgebraData(hit_1.DetId(), tot_geom);
  if(!alg_obj_0->available_ || !alg_obj_1->available_)
    return TVector2();
    
  TMatrixD point_rec_matrix(2,2);
  point_rec_matrix(0,0) = alg_obj_0->readout_direction_.X();
  point_rec_matrix(0,1) = alg_obj_0->readout_direction_.Y();
  point_rec_matrix(1,0) = alg_obj_1->readout_direction_.X();
  point_rec_matrix(1,1) = alg_obj_1->readout_direction_.Y();
  point_rec_matrix.Invert();
  TVectorD vect(2);
  double delta_U_0;
  double delta_U_1;
  TVector2 direct(tx, ty);
  delta_U_0 = (alg_obj_0->readout_direction_*=direct)*(alg_obj_0->centre_of_det_global_position_.Z()-z0);
  delta_U_1 = (alg_obj_1->readout_direction_*=direct)*(alg_obj_1->centre_of_det_global_position_.Z()-z0);
  
  vect[0] = hit_0.Position() - alg_obj_0->rec_u_0_ - delta_U_0;
  vect[1] = hit_1.Position() - alg_obj_1->rec_u_0_ - delta_U_1;
  vect*=point_rec_matrix;
  return TVector2(vect[0], vect[1]);
}
#endif

//----------------------------------------------------------------------------------------------------

bool TotemRPLocalTrackFitterAlgorithm::FitTrack(const vector<const TotemRPRecHit *> &hits, double z_0,
    const TotemRPGeometry &tot_geom, TotemRPLocalTrack &fitted_track)
{
  fitted_track.IsValid(false);
  
  std::vector<const TotemRPRecHit*> applicable_hits;
  for (auto hp : hits)
  {
    if (GetDetAlgebraData(hp->DetId(), tot_geom)->available_)
      applicable_hits.push_back(hp);
  }
  
  if(applicable_hits.size()<5)
    return false;
  
  TMatrixD H(applicable_hits.size(), 4);
  TVectorD V(applicable_hits.size());
  TVectorD V_inv(applicable_hits.size());
  TVectorD U(applicable_hits.size());
  
  for(unsigned int i=0; i<applicable_hits.size(); ++i)
  {
    RPDetCoordinateAlgebraObjs *alg_obj = GetDetAlgebraData(applicable_hits[i]->DetId(), tot_geom);
    H(i,0) = alg_obj->readout_direction_.X();
    H(i,1) = alg_obj->readout_direction_.Y();
    double delta_z = alg_obj->centre_of_det_global_position_.Z()-z_0;
    //std::cout<<"det id:"<<applicable_hits[i]->DetId()<<" readout dir:"<<H[i][0]<<","<<H[i][1]<<" delta z:"<<delta_z;
    H(i,2) = alg_obj->readout_direction_.X()*delta_z;
    H(i,3) = alg_obj->readout_direction_.Y()*delta_z;
    double var = applicable_hits[i]->Sigma();
    var*=var;
    //std::cout<<" var:"<<var;
    V[i] = var;
    V_inv[i] = 1.0/var;
    U[i] = applicable_hits[i]->Position() - alg_obj->rec_u_0_;
    //std::cout<<" pos:"<<U[i]<<" u0:"<<alg_obj->rec_u_0_<<std::endl;
  }

  //std::cout<<"Matrix calculations started..."<<std::endl;
  TMatrixD H_T_V_inv(TMatrixD::kTransposed, H);
  MultiplyByDiagonalInPlace(H_T_V_inv, V_inv);
  TMatrixD V_a(H_T_V_inv);
  //std::cout<<"V_a"<<std::endl;
  //tot_rp::Print(std::cout, V_a);
  //std::cout<<"H"<<std::endl;
  //tot_rp::Print(std::cout, H);
  TMatrixD V_a_mult(V_a, TMatrixD::kMult, H);
  //V_a*=H;
  //std::cout<<"V_a_mult"<<std::endl;
  //V_a.Invert();
  try
  {
    V_a_mult.Invert();
  }

  // TODO: put here specifically the exception type that ROOT throws
  catch (...)
  {
    edm::LogProblem("TotemRPLocalTrackFitterAlgorithm") << ">> TotemRPLocalTrackFitterAlgorithm > Fit matrix is singular.";
    return false;
  }
  //tot_rp::Print(std::cout, V_a_mult);
  //TMatrixD u_to_a(V_a, TMatrixD::kMult, H_T_V_inv);
  TMatrixD u_to_a(V_a_mult, TMatrixD::kMult, H_T_V_inv);
  TVectorD a(U);
  a *= u_to_a;
  
  //std::cout<<"fitted track vector:"<<a[0]<<","<<a[1]<<","<<a[2]<<","<<a[3]<<","<<std::endl;
  
  fitted_track.Reset();
  fitted_track.Z0(z_0);
  fitted_track.ParameterVector(a);
  //fitted_track.CovarianceMatrix(V_a);
  fitted_track.CovarianceMatrix(V_a_mult);
  
  double Chi_2 = 0;
  for(unsigned int i=0; i<applicable_hits.size(); ++i)
  {
    RPDetCoordinateAlgebraObjs *alg_obj = GetDetAlgebraData(applicable_hits[i]->DetId(), tot_geom);
    TVector2 readout_dir = alg_obj->readout_direction_;
    double det_z = alg_obj->centre_of_det_global_position_.Z();
    double sigma_str = applicable_hits[i]->Sigma();
    double sigma_str_2 = sigma_str*sigma_str;
    TVector2 fited_det_xy_point = fitted_track.GetTrackPoint(det_z);
    double U_readout = applicable_hits[i]->Position() - alg_obj->rec_u_0_;
    double U_fited = (readout_dir*=fited_det_xy_point);
    double residual = U_fited - U_readout;
    TMatrixD V_T_Cov_X_Y(1,2);
    V_T_Cov_X_Y(0,0) = readout_dir.X();
    V_T_Cov_X_Y(0,1) = readout_dir.Y();
    //V_T_Cov_X_Y *= fitted_track.TrackPointInterpolationCovariance(det_z);
    TMatrixD V_T_Cov_X_Y_mult(V_T_Cov_X_Y, TMatrixD::kMult, fitted_track.TrackPointInterpolationCovariance(det_z));
    //std::cout<<"TrackPointInterpolationCovariance(det_z)"<<std::endl;
    //tot_rp::Print(std::cout, fitted_track.TrackPointInterpolationCovariance(det_z));
    double fit_strip_var = V_T_Cov_X_Y_mult(0,0)*readout_dir.X() + V_T_Cov_X_Y_mult(0,1)*readout_dir.Y();
    double pull_normalization = TMath::Sqrt(sigma_str_2 - fit_strip_var);
    double pull = residual/pull_normalization;
    //std::cout<<"u read:"<<U_readout<<" u fitted:"<<U_fited<<" residual:"<<residual
    //  <<" pull norm:"<<pull_normalization<<" normalised pull:"<<pull
    //  <<" fit_strip_var:"<<fit_strip_var<<std::endl;
    
    Chi_2+=residual/sigma_str_2;

    RPDetHitPoint hit_point(*(applicable_hits[i]), TVector3(fited_det_xy_point.X(), fited_det_xy_point.Y(), det_z), 
        residual, pull);
    fitted_track.AddHit(hit_point);
  }
  
  fitted_track.ChiSquared(Chi_2);
  fitted_track.IsValid(true);
  return true;
}

//----------------------------------------------------------------------------------------------------

void TotemRPLocalTrackFitterAlgorithm::MultiplyByDiagonalInPlace(TMatrixD &mt, const TVectorD &diag)
{
  if(mt.GetNcols()!=diag.GetNrows())
  {
    std::cout<<"TotemRPLocalTrackFitterAlgorithm::MultiplyByDiagonalinPlace: mt.GetNcols()!=diag.GetNrows()"<<std::endl;
    exit(0);
  }
    
  for(int i=0; i<mt.GetNrows(); ++i)
  {
    for(int j=0; j<mt.GetNcols(); ++j)
    {
      //std::cout<<"i="<<i<<" j="<<j<<" mt[i][j]="<<mt[i][j];
      mt[i][j]*=diag[j];
      //std::cout<<" diag[j]="<<diag[j]<<" mt[i][j]="<<mt[i][j]<<std::endl;
    }
  }
}