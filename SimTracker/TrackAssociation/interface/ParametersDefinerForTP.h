#ifndef TrackAssociation_ParametersDefinerForTP_h
#define TrackAssociation_ParametersDefinerForTP_h

/**
 *
 *
 * \author Boris Mangano (UCSD)  5/7/2009
 */

#include <memory>

#include "DataFormats/Candidate/interface/Candidate.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "SimGeneral/TrackingAnalysis/interface/SimHitTPAssociationProducer.h"
#include <SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h>

class ParametersDefinerForTP {
public:
  ParametersDefinerForTP(){};
  ParametersDefinerForTP(const edm::ParameterSet &iConfig);
  virtual ~ParametersDefinerForTP(){};

  typedef int Charge;                              ///< electric charge type
  typedef math::XYZPointD Point;                   ///< point in the space
  typedef math::XYZTLorentzVectorD LorentzVector;  ///< Lorentz vector

  virtual TrackingParticle::Vector momentum(const edm::Event &iEvent,
                                            const edm::EventSetup &iSetup,
                                            const Charge ch,
                                            const Point &vtx,
                                            const LorentzVector &lv) const;

  virtual TrackingParticle::Vector momentum(const edm::Event &iEvent,
                                            const edm::EventSetup &iSetup,
                                            const TrackingParticleRef &tpr) const {
    return momentum(iEvent, iSetup, tpr->charge(), tpr->vertex(), tpr->p4());
  }

  virtual TrackingParticle::Vector momentum(const edm::Event &iEvent,
                                            const edm::EventSetup &iSetup,
                                            const reco::Candidate &tp) const {
    return momentum(iEvent, iSetup, tp.charge(), tp.vertex(), tp.p4());
  }

  virtual TrackingParticle::Point vertex(const edm::Event &iEvent,
                                         const edm::EventSetup &iSetup,
                                         const Charge ch,
                                         const Point &vtx,
                                         const LorentzVector &lv) const;

  virtual TrackingParticle::Point vertex(const edm::Event &iEvent,
                                         const edm::EventSetup &iSetup,
                                         const TrackingParticleRef &tpr) const {
    return vertex(iEvent, iSetup, tpr->charge(), tpr->vertex(), tpr->p4());
  }

  virtual TrackingParticle::Point vertex(const edm::Event &iEvent,
                                         const edm::EventSetup &iSetup,
                                         const reco::Candidate &tp) const {
    return vertex(iEvent, iSetup, tp.charge(), tp.vertex(), tp.p4());
  }

  virtual std::pair<TrackingParticle::Point, TrackingParticle::Vector> vertexAndMomentum(
      const edm::Event &iEvent, const edm::EventSetup &iSetup, const TrackingParticleRef &tpr) const {
    return vertexAndMomentum(iEvent, iSetup, tpr->charge(), tpr->vertex(), tpr->p4());
  }

  std::pair<TrackingParticle::Point, TrackingParticle::Vector> vertexAndMomentum(const edm::Event &iEvent,
                                                                                 const edm::EventSetup &iSetup,
                                                                                 const Charge ch,
                                                                                 const Point &vtx,
                                                                                 const LorentzVector &lv) const;

  virtual void initEvent(edm::Handle<SimHitTPAssociationProducer::SimHitTPAssociationList> simHitsTPAssocToSet) {}

  virtual std::unique_ptr<ParametersDefinerForTP> clone() const {
    return std::make_unique<ParametersDefinerForTP>(*this);
  }

  edm::InputTag beamSpotInputTag_;
};

#endif
