/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: RPDataDigiProducer.h,v $: $
* $Revision: 9953 $
* $Date: 2014-12-16 12:03:45 +0100 (Tue, 16 Dec 2014) $
*
****************************************************************************/

#ifndef DataDigiProducer_h
#define DataDigiProducer_h

#include "FWCore/Framework/interface/EDProducer.h"

namespace edm {
  class ParameterSet;
  class EventSetup;
  class Event;
  class EventID;
}

namespace Totem {
  class FramePosition;
}


/**
 * \brief Converts raw event data to RP digi information.
**/
class RPDataDigiProducer : public edm::EDProducer
{
  public:

    RPDataDigiProducer(const edm::ParameterSet& conf);
    virtual  ~RPDataDigiProducer();

    virtual void beginJob(const edm::EventSetup&);
    virtual void endJob();
    virtual void produce(edm::Event&, const edm::EventSetup&);

  private:
    unsigned char verbosity;

    /// whether to throw an exception when the position of data frame is missing from the mapping
    bool stopOnMissing;
};

#endif

