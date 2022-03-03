#include <iostream>
#include <sstream>
#include "CondCore/Utilities/interface/PayloadInspector.h"
#include "CondCore/BeamSpotPlugins/plugins/BeamSpot_PayloadInspector.cc"
#include "CondCore/BeamSpotPlugins/plugins/BeamSpotOnline_PayloadInspector.cc"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/PluginManager/interface/PluginManager.h"
#include "FWCore/PluginManager/interface/standard.h"
#include "FWCore/PluginManager/interface/SharedLibrary.h"
#include "FWCore/ServiceRegistry/interface/ServiceRegistry.h"

int main(int argc, char** argv) {
  Py_Initialize();

  edmplugin::PluginManager::Config config;
  edmplugin::PluginManager::configure(edmplugin::standard::config());

  std::vector<edm::ParameterSet> psets;
  edm::ParameterSet pSet;
  pSet.addParameter("@service_type", std::string("SiteLocalConfigService"));
  psets.push_back(pSet);
  edm::ServiceToken servToken(edm::ServiceRegistry::createSet(psets));
  edm::ServiceRegistry::Operate operate(servToken);

  std::string connectionString("frontier://FrontierProd/CMS_CONDITIONS");

  // BeamSpot
  std::string tag = "BeamSpotObjects_PCL_byLumi_v0_prompt";
  cond::Time_t start = static_cast<unsigned long long>(1406876667347162);
  cond::Time_t end = static_cast<unsigned long long>(1488257707672138);

  edm::LogPrint("testBeamSpotPayloadInspector") << "## Exercising BeamSpot plots " << std::endl;

  BeamSpotParameters histoParameters;
  histoParameters.process(connectionString, PI::mk_input(tag, start, start));
  edm::LogPrint("testBeamSpotPayloadInspector") << histoParameters.data() << std::endl;

  BeamSpotParametersDiffSingleTag histoParametersDiff;
  histoParametersDiff.process(connectionString, PI::mk_input(tag, start, end));
  edm::LogPrint("testBeamSpotPayloadInspector") << histoParametersDiff.data() << std::endl;

  std::string tag1 = "BeamSpotObjects_Realistic25ns_900GeV_2021PilotBeams_v2_mc";
  std::string tag2 = "BeamSpotObjects_Realistic25ns_900GeV_2021PilotBeams_v1_mc";
  start = static_cast<unsigned long long>(1);

  BeamSpotParametersDiffTwoTags histoParametersDiffTwoTags;
  histoParametersDiffTwoTags.process(connectionString, PI::mk_input(tag1, start, start, tag2, start, start));
  edm::LogPrint("testBeamSpotPayloadInspector") << histoParametersDiffTwoTags.data() << std::endl;

  edm::LogPrint("testBeamSpotPayloadInspector") << "## Exercising BeamSpotOnline plots " << std::endl;

  tag = "BeamSpotOnlineTestLegacy";
  start = static_cast<unsigned long long>(1443392479297557);

  BeamSpotOnlineParameters histoOnlineParameters;
  histoOnlineParameters.process(connectionString, PI::mk_input(tag, start, start));
  edm::LogPrint("testBeamSpotPayloadInspector") << histoOnlineParameters.data() << std::endl;

  Py_Finalize();
}
