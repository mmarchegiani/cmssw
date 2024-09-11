import FWCore.ParameterSet.Config as cms

from DQMOffline.RecoB.bTagGenericAnalysis_cff import bTagGenericAnalysisBlock
from DQMOffline.RecoB.cTagGenericAnalysis_cff import cTagGenericAnalysisBlock


# need to be replaced...couldn't find
UnifiedParticleTransformerWP = {
    'BvsAll': 0.001,#0.0490, # loose
    'CvsL':   0.002,#0.099,  # medium
    'CvsB':   0.003,#0.325,  # medium
}

UnifiedParticleTransformerDiscriminators = {
    'BvsAll': cms.PSet(
        bTagGenericAnalysisBlock,

        folder = cms.string('UnifiedParticleTransformer_BvsAll'),
        CTagPlots = cms.bool(False),
        discrCut = cms.double(UnifiedParticleTransformerWP['BvsAll']),
        numerator = cms.vstring(
            'pfUnifiedParticleTransformerAK4JetTags:probb',
            'pfUnifiedParticleTransformerAK4JetTags:probbb',
            'pfUnifiedParticleTransformerAK4JetTags:problepb',
        ),
        denominator = cms.vstring(
            'pfUnifiedParticleTransformerAK4JetTags:probb',
            'pfUnifiedParticleTransformerAK4JetTags:probbb',
            'pfUnifiedParticleTransformerAK4JetTags:probpb',
            'pfUnifiedParticleTransformerAK4JetTags:probc',
            'pfUnifiedParticleTransformerAK4JetTags:probu',
            'pfUnifiedParticleTransformerAK4JetTags:probd',
            'pfUnifiedParticleTransformerAK4JetTags:probs',
            'pfUnifiedParticleTransformerAK4JetTags:probg',
        ),
    ),

    'CvsL': cms.PSet(
        cTagGenericAnalysisBlock,

        folder = cms.string('UnifiedParticleTransformer_CvsL'),
        CTagPlots = cms.bool(True),
        discrCut = cms.double(UnifiedParticleTransformerWP['CvsL']),
        numerator = cms.vstring(
            'pfUnifiedParticleTransformerAK4JetTags:probc',
        ),
        denominator = cms.vstring(
            'pfUnifiedParticleTransformerAK4JetTags:probc',
            'pfUnifiedParticleTransformerAK4JetTags:probu',
            'pfUnifiedParticleTransformerAK4JetTags:probd',
            'pfUnifiedParticleTransformerAK4JetTags:probs',
            'pfUnifiedParticleTransformerAK4JetTags:probg',
        ),
    ),

    'CvsB': cms.PSet(
        cTagGenericAnalysisBlock,

        folder = cms.string('UnifiedParticleTransformer_CvsB'),
        CTagPlots = cms.bool(True),
        discrCut = cms.double(UnifiedParticleTransformerWP['CvsB']),
        numerator = cms.vstring(
            'pfUnifiedParticleTransformerAK4JetTags:probc',
        ),
        denominator = cms.vstring(
            'pfUnifiedParticleTransformerAK4JetTags:probb',
            'pfUnifiedParticleTransformerAK4JetTags:probbb',
            'pfUnifiedParticleTransformerAK4JetTags:probpb',
            'pfUnifiedParticleTransformerAK4JetTags:probc',
        ),
    ),
}
