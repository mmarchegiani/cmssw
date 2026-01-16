#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/HGCalReco/interface/HGCalTopoCluster.h"

class HGCTopoClusterTableProducer : public edm::global::EDProducer<> {
public:
  explicit HGCTopoClusterTableProducer(const edm::ParameterSet& iConfig)
      : src_(consumes<reco::HGCalTopoClusterCollection>(iConfig.getParameter<edm::InputTag>("src"))),
        name_(iConfig.getParameter<std::string>("name")),
        doc_(iConfig.getParameter<std::string>("doc")) {
    produces<nanoaod::FlatTable>();
  }

  ~HGCTopoClusterTableProducer() override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("Input TopoCluster collection");
    desc.add<std::string>("name")->setComment("Name of the FlatTable");
    desc.add<std::string>("doc")->setComment("Documentation string");
    descriptions.add("hgcTopoClusterTable", desc);
  }

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  const edm::EDGetTokenT<reco::HGCalTopoClusterCollection> src_;
  const std::string name_;
  const std::string doc_;
};

void HGCTopoClusterTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const {
  edm::Handle<reco::HGCalTopoClusterCollection> clusters;
  iEvent.getByToken(src_, clusters);

  unsigned int nclusters = clusters->size();
  auto out = std::make_unique<nanoaod::FlatTable>(nclusters, name_, false, false);

  // Basic properties
  std::vector<float> energy, time, x, y, z, eta, phi;
  std::vector<float> etaWidth, phiWidth;
  std::vector<uint32_t> nHits;
  
  energy.reserve(nclusters);
  time.reserve(nclusters);
  x.reserve(nclusters);
  y.reserve(nclusters);
  z.reserve(nclusters);
  eta.reserve(nclusters);
  phi.reserve(nclusters);
  etaWidth.reserve(nclusters);
  phiWidth.reserve(nclusters);
  nHits.reserve(nclusters);

  for (const auto& cluster : *clusters) {
    energy.push_back(cluster.energy());
    time.push_back(cluster.time());
    x.push_back(cluster.x());
    y.push_back(cluster.y());
    z.push_back(cluster.z());
    eta.push_back(cluster.eta());
    phi.push_back(cluster.phi());
    etaWidth.push_back(cluster.etaWidth());
    phiWidth.push_back(cluster.phiWidth());
    nHits.push_back(cluster.size());
  }

  out->addColumn<float>("energy", energy, "Cluster energy (GeV)", 14);
  out->addColumn<float>("time", time, "Cluster time (ns)", 14);
  out->addColumn<float>("x", x, "Cluster x position (cm)", 14);
  out->addColumn<float>("y", y, "Cluster y position (cm)", 14);
  out->addColumn<float>("z", z, "Cluster z position (cm)", 14);
  out->addColumn<float>("eta", eta, "Cluster eta", 14);
  out->addColumn<float>("phi", phi, "Cluster phi", 14);
  out->addColumn<float>("etaWidth", etaWidth, "Cluster RMS in eta", 14);
  out->addColumn<float>("phiWidth", phiWidth, "Cluster RMS in phi", 14);
  out->addColumn<uint32_t>("nHits", nHits, "Number of hits in cluster");

  iEvent.put(std::move(out));
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HGCTopoClusterTableProducer);
