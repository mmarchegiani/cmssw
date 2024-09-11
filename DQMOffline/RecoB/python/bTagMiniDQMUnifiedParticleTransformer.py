import FWCore.ParameterSet.Config as cms

from DQMOffline.RecoB.bTagGenericAnalysis_cff import bTagGenericAnalysisBlock
from DQMOffline.RecoB.cTagGenericAnalysis_cff import cTagGenericAnalysisBlock


# The working point values need to be updated
UnifiedParticleTransformerWP = {
    'BvsAll': 0.09, # PLACEHOLDER
    'CvsL':   0.117,  # PLACEHOLDER
    'CvsB':   0.128,  # PLACEHOLDER
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
