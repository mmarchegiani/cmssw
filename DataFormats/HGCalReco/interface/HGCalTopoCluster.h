#ifndef DataFormats_HGCalReco_TopoCluster_h
#define DataFormats_HGCalReco_TopoCluster_h

#include "DataFormats/Math/interface/Point3D.h"
#include <vector>

namespace reco {

  class HGCalTopoCluster {
  public:
    HGCalTopoCluster() 
        : energy_(0.0), 
          time_(0.0), 
          x_(0.0), 
          y_(0.0), 
          z_(0.0),
          eta_(0.0),
          phi_(0.0),
          etaWidth_(0.0),
          phiWidth_(0.0) {}

    // Setters
    void setEnergy(float energy) { energy_ = energy; }
    void setTime(float time) { time_ = time; }
    void setPosition(float x, float y, float z) { 
      x_ = x; 
      y_ = y; 
      z_ = z; 
    }
    void setEtaPhi(float eta, float phi) {
      eta_ = eta;
      phi_ = phi;
    }
    void setShapes(float etaWidth, float phiWidth) {
      etaWidth_ = etaWidth;
      phiWidth_ = phiWidth;
    }
    void addHitIndex(unsigned int idx) { hitIndices_.push_back(idx); }
    void setHitIndices(const std::vector<unsigned int>& indices) { hitIndices_ = indices; }

    // Getters
    float energy() const { return energy_; }
    float time() const { return time_; }
    float x() const { return x_; }
    float y() const { return y_; }
    float z() const { return z_; }
    float eta() const { return eta_; }
    float phi() const { return phi_; }
    float etaWidth() const { return etaWidth_; }
    float phiWidth() const { return phiWidth_; }
    unsigned int size() const { return hitIndices_.size(); }
    const std::vector<unsigned int>& hitIndices() const { return hitIndices_; }

  private:
    float energy_;
    float time_;
    float x_;
    float y_;
    float z_;
    float eta_;
    float phi_;
    float etaWidth_;
    float phiWidth_;
    std::vector<unsigned int> hitIndices_;
  };

  typedef std::vector<HGCalTopoCluster> HGCalTopoClusterCollection;

}  // namespace reco

#endif
