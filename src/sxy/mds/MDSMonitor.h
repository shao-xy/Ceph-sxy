 // -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef __SXY_MDS_MDSMONITOR_H__
#define __SXY_MDS_MDSMONITOR_H__

#include "common/Thread.h"

#include "mds/mdstypes.h"

#include "macroconfig.h"

class MDSRank;

namespace sxy {

class MDSMonitor : public Thread {
    MDSRank * mds;
    bool m_runFlag;
  
    int m_last_iocnt;
    int iops(); // update IOPS per second
    int m_last_iops; // for get_iops()
  public:
    int get_iops() { return m_last_iops; }

  private:
    mds_load_t mds_load();
  
  private:
    void writelog();
  protected:
    void * entry() override;
  
  public:
    MDSMonitor(MDSRank * mds = NULL);
    ~MDSMonitor();
  
    int terminate();
};

}; /* namespace: sxy */

#endif /* sxy/mds/MDSMonitor.h */
