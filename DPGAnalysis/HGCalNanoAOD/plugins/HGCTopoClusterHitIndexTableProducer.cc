#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/HGCalReco/interface/HGCalTopoCluster.h"

class HGCTopoClusterHitIndexTableProducer : public edm::global::EDProducer<> {
public:
  explicit HGCTopoClusterHitIndexTableProducer(const edm::ParameterSet& iConfig)
      : src_(consumes<reco::HGCalTopoClusterCollection>(iConfig.getParameter<edm::InputTag>("src"))),
        name_(iConfig.getParameter<std::string>("name")),
        doc_(iConfig.getParameter<std::string>("doc")) {
    produces<nanoaod::FlatTable>();
  }

  ~HGCTopoClusterHitIndexTableProducer() override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("Input TopoCluster collection");
    desc.add<std::string>("name")->setComment("Name of the FlatTable");
    desc.add<std::string>("doc")->setComment("Documentation string");
    descriptions.add("hgcTopoClusterHitIndexTable", desc);
  }

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  const edm::EDGetTokenT<reco::HGCalTopoClusterCollection> src_;
  const std::string name_;
  const std::string doc_;
};

void HGCTopoClusterHitIndexTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const {
  edm::Handle<reco::HGCalTopoClusterCollection> clusters;
  iEvent.getByToken(src_, clusters);

  // Count total number of hit indices
  unsigned int totalHits = 0;
  for (const auto& cluster : *clusters) {
    totalHits += cluster.hitIndices().size();
  }

  auto out = std::make_unique<nanoaod::FlatTable>(totalHits, name_, false, false);

  std::vector<uint32_t> hitIndex;
  std::vector<uint32_t> clusterIndex;
  
  hitIndex.reserve(totalHits);
  clusterIndex.reserve(totalHits);

  for (size_t iCluster = 0; iCluster < clusters->size(); ++iCluster) {
    const auto& cluster = (*clusters)[iCluster];
    for (unsigned int idx : cluster.hitIndices()) {
      hitIndex.push_back(idx);
      clusterIndex.push_back(static_cast<uint32_t>(iCluster));
    }
  }

  out->addColumn<uint32_t>("hitIndex", hitIndex, "RecHit index in collection");
  out->addColumn<uint32_t>("clusterIndex", clusterIndex, "TopoCluster index");

  iEvent.put(std::move(out));
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HGCTopoClusterHitIndexTableProducer);
