"""
Example configuration to add topological clustering to nanoHGCML production

Usage:
  cmsRun test_topoClusters.py

This config demonstrates how to enable topological clustering for HGCAL RecHits.
"""

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9

process = cms.Process('NANO', Phase2C17I13M9)

# Import standard sequences
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4D110Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4D110_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

# Global Tag
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic_T21', '')

# Input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/m/mmarcheg/private/ml4reco/cms-pepr-port-CMSSW_15_0_0/CMSSW_15_0_0/src/production_tests/testRECO.root'),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))

# Load the nanoHGCML sequence using process.load()
process.load('DPGAnalysis.HGCalNanoAOD.nanoHGCML_cff')

# Load RecHits and Track modules
from DPGAnalysis.HGCalNanoAOD.hgcRecHits_cff import hgcRecHitsTask
from DPGAnalysis.TrackNanoAOD.tracks_cff import trackTables

# Import topoCluster components
from DPGAnalysis.HGCalNanoAOD.hgcTopoClusters_cff import hgcTopoClusters, hgcTopoClusterTable, hgcTopoClusterHitIndexTable, hgcTopoClusterSequence

# Add topoCluster modules to the process
process.hgcTopoClusters = hgcTopoClusters
process.hgcTopoClusterTable = hgcTopoClusterTable
process.hgcTopoClusterHitIndexTable = hgcTopoClusterHitIndexTable

# Customize thresholds if needed
process.hgcTopoClusters.seedThreshold = cms.double(4.0)
process.hgcTopoClusters.neighborThreshold = cms.double(2.0)
process.hgcTopoClusters.cellThreshold = cms.double(0.0)
process.hgcTopoClusters.noiseLevel = cms.double(0.001)  # 1 MeV
process.hgcTopoClusters.minClusterEnergy = cms.double(0.01)  # 10 MeV minimum

# Build the sequence - include RecHits tables and topoCluster sequence
process.nanoSequence = cms.Sequence(
    process.nanoHGCMLSequence +
    cms.Sequence(hgcRecHitsTask) +
    cms.Sequence(trackTables)+
    hgcTopoClusterSequence
)

# Output
process.NANOoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    fileName = cms.untracked.string('file:nanoHGC_withTopoClusters.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep nanoaodFlatTable_*_*_*',
        'keep nanoaodUniqueString_*_*_*',
        'keep nanoaodMergeableCounterTable_*_*_*',
    )
)

# Path
process.nanoAOD_step = cms.Path(process.nanoSequence)
process.NANOoutput_step = cms.EndPath(process.NANOoutput)

# Schedule
process.schedule = cms.Schedule(process.nanoAOD_step, process.NANOoutput_step)

# Setup FWK for multithreaded
process.options.numberOfThreads = 1
process.options.numberOfStreams = 0
