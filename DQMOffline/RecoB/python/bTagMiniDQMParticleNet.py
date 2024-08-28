import FWCore.ParameterSet.Config as cms

from DQMOffline.RecoB.bTagGenericAnalysis_cff import bTagGenericAnalysisBlock
from DQMOffline.RecoB.cTagGenericAnalysis_cff import cTagGenericAnalysisBlock


# need to be replaced...couldn't find
ParticleNetWP = {
    'BvsAll': 0.0358, # loose
    'CvsL':   0.160,  # OLD
    'CvsB':   0.304,  # OLD
}

ParticleNetDiscriminators = {
    'BvsAll': cms.PSet(
        bTagGenericAnalysisBlock,

        folder = cms.string('ParticleNet_BvsAll'),
        CTagPlots = cms.bool(False),
        discrCut = cms.double(ParticleNetWP['BvsAll']),
        numerator = cms.vstring(
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probb',
        ),
        denominator = cms.vstring(
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probb',
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probc',
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probuds',
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probg',
        ),
    ),

    'CvsL': cms.PSet(
        bTagGenericAnalysisBlock,

        folder = cms.string('ParticleNet_CvsL'),
        CTagPlots = cms.bool(False),
        discrCut = cms.double(ParticleNetWP['CvsL']),
        numerator = cms.vstring(
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probc',
        ),
        denominator = cms.vstring(
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probc',
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probuds',
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probg',
        ),
    ),

    'CvsB': cms.PSet(
        bTagGenericAnalysisBlock,

        folder = cms.string('ParticleNet_CvsB'),
        CTagPlots = cms.bool(False),
        discrCut = cms.double(ParticleNetWP['CvsB']),
        numerator = cms.vstring(
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probc',
        ),
        denominator = cms.vstring(
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probb',
            'pfParticleNetFromMiniAODAK4PuppiCentralJetTags:probc',
        ),
    ),
}
