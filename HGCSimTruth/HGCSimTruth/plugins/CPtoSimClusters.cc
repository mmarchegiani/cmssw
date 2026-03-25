#include <memory>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <map>
#include <sstream>
#include <utility>
#include <set>
#include <cmath>
#include <numeric>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Ref.h"

#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"


class CPtoSimClusters : public edm::stream::EDProducer<> {
    public:
        explicit CPtoSimClusters(const edm::ParameterSet&);
        ~CPtoSimClusters() {}


    private:
        void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

        edm::EDGetTokenT<CaloParticleCollection> cpCollectionToken_;

    };

CPtoSimClusters::CPtoSimClusters(const edm::ParameterSet &pset) :
              cpCollectionToken_(consumes<CaloParticleCollection>(pset.getParameter<edm::InputTag>("caloParticles")))
{
    produces<SimClusterCollection>();
}

void CPtoSimClusters::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {


    edm::Handle<CaloParticleCollection> cpCollection;
    iEvent.getByToken(cpCollectionToken_, cpCollection);

    auto output = std::make_unique<SimClusterCollection>();

    for(const auto& cp: *cpCollection){
        SimCluster cpsc;
        bool first = true;
        for(const auto& sc: cp.simClusters()){
            if(first){
                cpsc = *sc;
                first = false;
            }
            else{
                cpsc += *sc;
            }
        }

        cpsc.setImpactMomentum(cp.p4());
        
        // Use vertex position directly from SimTrack to avoid index lookup issues with pileup
        if (!cp.g4Tracks().empty()) {
            auto vertex_pos = cp.g4Tracks().at(0).trackerSurfacePosition();
            cpsc.setImpactPoint(math::XYZTLorentzVectorF(
                vertex_pos.x(), vertex_pos.y(), vertex_pos.z(), 0.));
        }
        
        // Set pileup fraction: 0 for hard-scatter, 1 for pileup.
        // Uses the same condition as removeCPFromPU in AssociatorTools.h.
        if (!cp.g4Tracks().empty()) {
            const auto& evId = cp.g4Tracks().at(0).eventId();
            bool isPU = (evId.event() != 0 || evId.bunchCrossing() != 0);
            cpsc.setPileupFraction(isPU ? 1.0f : 0.0f);
        }
        
        cpsc.setPdgId(cp.pdgId());
        output->push_back(cpsc);
    }

    iEvent.put(std::move(output));

}

DEFINE_FWK_MODULE(CPtoSimClusters);
