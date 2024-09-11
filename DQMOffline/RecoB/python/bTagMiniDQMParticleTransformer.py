import FWCore.ParameterSet.Config as cms

from DQMOffline.RecoB.bTagGenericAnalysis_cff import bTagGenericAnalysisBlock
from DQMOffline.RecoB.cTagGenericAnalysis_cff import cTagGenericAnalysisBlock


# need to be replaced...couldn't find
ParticleTransformerWP = {
    'BvsAll': 0.0490, # loose
    'CvsL':   0.099,  # medium
    'CvsB':   0.325,  # medium
}

ParticleTransformerDiscriminators = {
    'BvsAll': cms.PSet(
        bTagGenericAnalysisBlock,

        folder = cms.string('ParticleTransformer_BvsAll'),
        CTagPlots = cms.bool(False),
        discrCut = cms.double(ParticleTransformerWP['BvsAll']),
        numerator = cms.vstring(
            'pfParticleTransformerAK4JetTags:probb',
            'pfParticleTransformerAK4JetTags:probbb',
            'pfParticleTransformerAK4JetTags:problepb',
        ),
        denominator = cms.vstring(),
    ),

    'CvsL': cms.PSet(
        cTagGenericAnalysisBlock,

        folder = cms.string('ParticleTransformer_CvsL'),
        CTagPlots = cms.bool(True),
        discrCut = cms.double(ParticleTransformerWP['CvsL']),
        numerator = cms.vstring('pfParticleTransformerAK4JetTags:probc'),
        denominator = cms.vstring(
            'pfParticleTransformerAK4JetTags:probc',
            'pfParticleTransformerAK4JetTags:probuds',
            'pfParticleTransformerAK4JetTags:probg',
        ),
    ),

    'CvsB': cms.PSet(
        cTagGenericAnalysisBlock,

        folder = cms.string('ParticleTransformer_CvsB'),
        CTagPlots = cms.bool(True),
        discrCut = cms.double(ParticleTransformerWP['CvsB']),
        numerator = cms.vstring('pfParticleTransformerAK4JetTags:probc'),
        denominator = cms.vstring(
            'pfParticleTransformerAK4JetTags:probc',
            'pfParticleTransformerAK4JetTags:probb',
            'pfParticleTransformerAK4JetTags:probbb',
            'pfParticleTransformerAK4JetTags:problepb',
        ),
    ),
}
