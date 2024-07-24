import FWCore.ParameterSet.Config as cms

from DQMOffline.RecoB.bTagGenericAnalysis_cff import bTagGenericAnalysisBlock
from DQMOffline.RecoB.cTagGenericAnalysis_cff import cTagGenericAnalysisBlock

from RecoBTag.ONNXRuntime.pfUnifiedParticleTransformerAK4DiscriminatorsJetTags_cfi import pfUnifiedParticleTransformerAK4DiscriminatorsJetTags

# N.B.: placeholder values !! These values need to be updated when the official recommendations are available
UnifiedParticleTransformerWP = {
    'BvsAll': 0.0681, # loose (N.B.: placeholder)
    'CvsL':   0.099,  # medium (N.B.: placeholder)
    'CvsB':   0.325,  # medium (N.B.: placeholder)
}

keys_btag = ["BvsAll"]
keys_ctag = ["CvsL", "CvsB"]

UnifiedParticleTransformerDiscriminators = {discr.name.value() : cms.PSet(
    bTagGenericAnalysisBlock,
    folder=cms.string('ParticleTransformer_' + discr.name.value()),
    CTagPlots=cms.bool(False),
    discrCut=cms.double(UnifiedParticleTransformerWP[discr.name.value()]),
    numerator=discr.numerator,
    denominator=discr.denominator,
    )
    for discr in pfUnifiedParticleTransformerAK4DiscriminatorsJetTags.discriminators if discr.name in keys_btag
}

UnifiedParticleTransformerDiscriminators.update({discr.name.value() : cms.PSet(
    cTagGenericAnalysisBlock,
    folder=cms.string('ParticleTransformer_' + discr.name.value()),
    CTagPlots=cms.bool(True),
    discrCut=cms.double(UnifiedParticleTransformerWP[discr.name.value()]),
    numerator=discr.numerator,
    denominator=discr.denominator,
    )
    for discr in pfUnifiedParticleTransformerAK4DiscriminatorsJetTags.discriminators if discr.name in keys_ctag
})
