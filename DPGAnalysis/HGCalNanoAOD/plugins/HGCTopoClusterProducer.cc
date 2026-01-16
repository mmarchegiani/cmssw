#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/HGCalCommonData/interface/HGCalDDDConstants.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

#include "DataFormats/HGCalReco/interface/HGCalTopoCluster.h"

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

class HGCTopoClusterProducer : public edm::global::EDProducer<edm::RunCache<hgcal::RecHitTools>> {
public:
  explicit HGCTopoClusterProducer(const edm::ParameterSet&);
  ~HGCTopoClusterProducer() override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  std::shared_ptr<hgcal::RecHitTools> globalBeginRun(edm::Run const&, edm::EventSetup const&) const override;
  void globalEndRun(edm::Run const&, edm::EventSetup const&) const override {}

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  // Clustering helper functions
  std::vector<size_t> findSeedCells(const edm::View<CaloRecHit>& hits, 
                                     const std::vector<float>& signalToNoise) const;
  
  std::vector<std::set<size_t>> clusterMaker(const edm::View<CaloRecHit>& hits,
                                              const std::vector<float>& signalToNoise,
                                              const std::vector<size_t>& seeds,
                                              const hgcal::RecHitTools& rhtools) const;
  
  std::vector<std::set<size_t>> findNeighbors(size_t hitIdx, 
                                               const edm::View<CaloRecHit>& hits,
                                               const hgcal::RecHitTools& rhtools) const;
  
  reco::HGCalTopoCluster buildCluster(const std::set<size_t>& hitIndices,
                                       const edm::View<CaloRecHit>& hits,
                                       const hgcal::RecHitTools& rhtools) const;

  // Tokens and parameters
  const edm::EDGetTokenT<edm::View<CaloRecHit>> hitsToken_;
  const edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeomToken_;
  
  const float seedThreshold_;      // S/N threshold for seed cells (e.g., 4.0)
  const float neighborThreshold_;  // S/N threshold for neighbor cells (e.g., 2.0)
  const float cellThreshold_;      // S/N threshold for boundary cells (e.g., 0.0)
  const float noiseLevel_;         // Constant noise value in GeV
  const float minClusterEnergy_;   // Minimum cluster energy in GeV
};

HGCTopoClusterProducer::HGCTopoClusterProducer(const edm::ParameterSet& iConfig)
    : hitsToken_(consumes<edm::View<CaloRecHit>>(iConfig.getParameter<edm::InputTag>("src"))),
      caloGeomToken_(esConsumes<CaloGeometry, CaloGeometryRecord, edm::Transition::BeginRun>()),
      seedThreshold_(iConfig.getParameter<double>("seedThreshold")),
      neighborThreshold_(iConfig.getParameter<double>("neighborThreshold")),
      cellThreshold_(iConfig.getParameter<double>("cellThreshold")),
      noiseLevel_(iConfig.getParameter<double>("noiseLevel")),
      minClusterEnergy_(iConfig.getParameter<double>("minClusterEnergy")) {
  produces<reco::HGCalTopoClusterCollection>();
}

std::shared_ptr<hgcal::RecHitTools> HGCTopoClusterProducer::globalBeginRun(edm::Run const&, 
                                                                             edm::EventSetup const& iSetup) const {
  auto rhtools = std::make_shared<hgcal::RecHitTools>();
  rhtools->setGeometry(iSetup.getData(caloGeomToken_));
  return rhtools;
}

void HGCTopoClusterProducer::produce(edm::StreamID sid, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  auto const& rhtools = *runCache(iEvent.getRun().index());
  
  // Get input RecHits
  edm::Handle<edm::View<CaloRecHit>> hitsHandle;
  iEvent.getByToken(hitsToken_, hitsHandle);
  const auto& hits = *hitsHandle;

  // Calculate signal-to-noise ratio for all hits
  std::vector<float> signalToNoise(hits.size());
  for (size_t i = 0; i < hits.size(); ++i) {
    signalToNoise[i] = hits[i].energy() / noiseLevel_;
  }

  // Find seed cells
  auto seeds = findSeedCells(hits, signalToNoise);

  // Build clusters
  auto clusterHitSets = clusterMaker(hits, signalToNoise, seeds, rhtools);

  // Create output collection
  auto topoClusters = std::make_unique<reco::HGCalTopoClusterCollection>();
  
  for (const auto& hitSet : clusterHitSets) {
    auto cluster = buildCluster(hitSet, hits, rhtools);
    if (cluster.energy() > minClusterEnergy_) {
      topoClusters->push_back(cluster);
    }
  }

  // Sort clusters by energy (descending)
  std::sort(topoClusters->begin(), topoClusters->end(),
            [](const reco::HGCalTopoCluster& a, const reco::HGCalTopoCluster& b) {
              return a.energy() > b.energy();
            });

  iEvent.put(std::move(topoClusters));
}

std::vector<size_t> HGCTopoClusterProducer::findSeedCells(const edm::View<CaloRecHit>& hits,
                                                            const std::vector<float>& signalToNoise) const {
  std::vector<std::pair<size_t, float>> seedCandidates;
  
  for (size_t i = 0; i < hits.size(); ++i) {
    if (signalToNoise[i] > seedThreshold_) {
      seedCandidates.emplace_back(i, hits[i].energy());
    }
  }

  // Sort by energy (descending)
  std::sort(seedCandidates.begin(), seedCandidates.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

  std::vector<size_t> seeds;
  seeds.reserve(seedCandidates.size());
  for (const auto& seed : seedCandidates) {
    seeds.push_back(seed.first);
  }

  return seeds;
}

std::vector<std::set<size_t>> HGCTopoClusterProducer::clusterMaker(
    const edm::View<CaloRecHit>& hits,
    const std::vector<float>& signalToNoise,
    const std::vector<size_t>& seeds,
    const hgcal::RecHitTools& rhtools) const {
  
  std::vector<std::set<size_t>> clusters;
  std::map<size_t, size_t> hitToCluster;  // Maps hit index to cluster index

  for (size_t seedIdx : seeds) {
    // Skip if seed already belongs to a cluster
    if (hitToCluster.find(seedIdx) != hitToCluster.end()) {
      continue;
    }

    // Start new cluster
    size_t clusterIdx = clusters.size();
    clusters.emplace_back();
    auto& currentCluster = clusters.back();
    
    std::set<size_t> toProcess;
    toProcess.insert(seedIdx);

    // Grow cluster with neighbor threshold
    while (!toProcess.empty()) {
      size_t hitIdx = *toProcess.begin();
      toProcess.erase(toProcess.begin());

      // Check if hit already assigned to another cluster
      auto existingCluster = hitToCluster.find(hitIdx);
      if (existingCluster != hitToCluster.end()) {
        if (existingCluster->second != clusterIdx) {
          // Merge clusters
          size_t otherIdx = existingCluster->second;
          for (size_t idx : clusters[otherIdx]) {
            currentCluster.insert(idx);
            hitToCluster[idx] = clusterIdx;
          }
          clusters[otherIdx].clear();
        }
        continue;
      }

      // Add hit to cluster if it passes neighbor threshold
      if (signalToNoise[hitIdx] >= neighborThreshold_) {
        currentCluster.insert(hitIdx);
        hitToCluster[hitIdx] = clusterIdx;

        // Find neighbors and add to processing queue
        auto neighbors = findNeighbors(hitIdx, hits, rhtools);
        for (const auto& neighborSet : neighbors) {
          for (size_t neighborIdx : neighborSet) {
            if (hitToCluster.find(neighborIdx) == hitToCluster.end()) {
              toProcess.insert(neighborIdx);
            }
          }
        }
      }
    }

    // Add boundary cells (cellThreshold)
    std::set<size_t> boundaryToCheck;
    for (size_t hitIdx : currentCluster) {
      auto neighbors = findNeighbors(hitIdx, hits, rhtools);
      for (const auto& neighborSet : neighbors) {
        for (size_t neighborIdx : neighborSet) {
          if (currentCluster.find(neighborIdx) == currentCluster.end() &&
              signalToNoise[neighborIdx] > cellThreshold_) {
            boundaryToCheck.insert(neighborIdx);
          }
        }
      }
    }
    
    for (size_t hitIdx : boundaryToCheck) {
      if (hitToCluster.find(hitIdx) == hitToCluster.end()) {
        currentCluster.insert(hitIdx);
        hitToCluster[hitIdx] = clusterIdx;
      }
    }
  }

  // Remove empty clusters (from merging)
  clusters.erase(std::remove_if(clusters.begin(), clusters.end(),
                                 [](const std::set<size_t>& c) { return c.empty(); }),
                 clusters.end());

  return clusters;
}

std::vector<std::set<size_t>> HGCTopoClusterProducer::findNeighbors(
    size_t hitIdx,
    const edm::View<CaloRecHit>& hits,
    const hgcal::RecHitTools& rhtools) const {
  
  std::vector<std::set<size_t>> neighborSets;
  
  const CaloRecHit& hit = hits[hitIdx];
  DetId detId = hit.detid();
  
  // Use HGCalGeometry to find neighbors
  const HGCalGeometry* geom = static_cast<const HGCalGeometry*>(rhtools.getGeometry()->getSubdetectorGeometry(detId));
  std::vector<DetId> neighborDetIds = geom->topology().neighbors(detId);
  
  std::set<size_t> neighbors;
  for (const auto& neighborDetId : neighborDetIds) {
    // Find the hit index corresponding to this DetId
    for (size_t i = 0; i < hits.size(); ++i) {
      if (hits[i].detid() == neighborDetId) {
        neighbors.insert(i);
        break;
      }
    }
  }
  
  if (!neighbors.empty()) {
    neighborSets.push_back(neighbors);
  }
  
  return neighborSets;
}

reco::HGCalTopoCluster HGCTopoClusterProducer::buildCluster(
    const std::set<size_t>& hitIndices,
    const edm::View<CaloRecHit>& hits,
    const hgcal::RecHitTools& rhtools) const {
  
  reco::HGCalTopoCluster cluster;
  
  float totalEnergy = 0.0;
  float weightedX = 0.0;
  float weightedY = 0.0;
  float weightedZ = 0.0;
  float weightedTime = 0.0;
  float weightedEta = 0.0;
  float weightedPhi = 0.0;

  // First pass: calculate energy-weighted averages
  for (size_t idx : hitIndices) {
    const CaloRecHit& hit = hits[idx];
    float energy = hit.energy();
    totalEnergy += energy;

    auto position = rhtools.getPosition(hit.detid());
    weightedX += energy * position.x();
    weightedY += energy * position.y();
    weightedZ += energy * position.z();
    weightedTime += energy * hit.time();
    weightedEta += energy * position.eta();
    weightedPhi += energy * static_cast<float>(position.phi());
    
    cluster.addHitIndex(static_cast<unsigned int>(idx));
  }

  if (totalEnergy > 0) {
    weightedX /= totalEnergy;
    weightedY /= totalEnergy;
    weightedZ /= totalEnergy;
    weightedTime /= totalEnergy;
    weightedEta /= totalEnergy;
    weightedPhi /= totalEnergy;
  }

  // Second pass: calculate shape variables (RMS)
  float etaWidth = 0.0;
  float phiWidth = 0.0;
  
  for (size_t idx : hitIndices) {
    const CaloRecHit& hit = hits[idx];
    float energy = hit.energy();
    auto position = rhtools.getPosition(hit.detid());
    
    float deta = position.eta() - weightedEta;
    float dphi = static_cast<float>(position.phi()) - weightedPhi;
    
    // Handle phi wrapping
    while (dphi > M_PI) dphi -= 2 * M_PI;
    while (dphi < -M_PI) dphi += 2 * M_PI;
    
    etaWidth += energy * deta * deta;
    phiWidth += energy * dphi * dphi;
  }

  if (totalEnergy > 0) {
    etaWidth = std::sqrt(etaWidth / totalEnergy);
    phiWidth = std::sqrt(phiWidth / totalEnergy);
  }

  cluster.setEnergy(totalEnergy);
  cluster.setTime(weightedTime);
  cluster.setPosition(weightedX, weightedY, weightedZ);
  cluster.setEtaPhi(weightedEta, weightedPhi);
  cluster.setShapes(etaWidth, phiWidth);

  return cluster;
}

void HGCTopoClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("hgcRecHits"));
  desc.add<double>("seedThreshold", 4.0)->setComment("S/N threshold for seed cells");
  desc.add<double>("neighborThreshold", 2.0)->setComment("S/N threshold for neighbor cells");
  desc.add<double>("cellThreshold", 0.0)->setComment("S/N threshold for boundary cells");
  desc.add<double>("noiseLevel", 0.001)->setComment("Noise level in GeV");
  desc.add<double>("minClusterEnergy", 0.0)->setComment("Minimum cluster energy in GeV");
  descriptions.add("hgcTopoClusters", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HGCTopoClusterProducer);
