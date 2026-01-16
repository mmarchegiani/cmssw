import FWCore.ParameterSet.Config as cms

# TopoCluster producer
hgcTopoClusters = cms.EDProducer("HGCTopoClusterProducer",
    src = cms.InputTag("hgcRecHits"),
    seedThreshold = cms.double(4.0),      # S/N > 4σ for seed cells
    neighborThreshold = cms.double(2.0),  # S/N > 2σ for neighbor cells
    cellThreshold = cms.double(0.0),      # S/N > 0σ for boundary cells
    noiseLevel = cms.double(0.1),         # Noise level in GeV (1 MeV)
    minClusterEnergy = cms.double(0.1)    # Minimum cluster energy in GeV
)

# TopoCluster main properties table
hgcTopoClusterTable = cms.EDProducer("HGCTopoClusterTableProducer",
    src = cms.InputTag("hgcTopoClusters"),
    name = cms.string("TopoClusterHGC"),
    doc = cms.string("HGCAL Topological Clusters (4-2-0 algorithm)")
)

# TopoCluster hit indices table
hgcTopoClusterHitIndexTable = cms.EDProducer("HGCTopoClusterHitIndexTableProducer",
    src = cms.InputTag("hgcTopoClusters"),
    name = cms.string("TopoClusterHGCHitIndex"),
    doc = cms.string("Hit indices for HGCAL Topological Clusters")
)

# Task for topological clustering
hgcTopoClusterTask = cms.Task(
    hgcTopoClusters,
    hgcTopoClusterTable,
    hgcTopoClusterHitIndexTable
)

# Sequence for topological clustering
hgcTopoClusterSequence = cms.Sequence(hgcTopoClusterTask)
