 // -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <sstream>

#include "MDSMonitor.h"

#include "common/debug.h"
#include "mds/MDSRank.h"
#include "mds/MDBalancer.h"

#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_mds
#undef dout_prefix
#define dout_prefix *_dout << "SXY.mds." << mds->get_nodeid() << ".mon "

namespace sxy {

MDSMonitor::MDSMonitor(MDSRank * mds)
    : mds(mds), m_runFlag(true), m_last_iocnt(0), m_last_iops(0)
{
  // We start immediately in constructor function
  dout(0) << "Launching monitor thread " << dendl;
  create("SXY-MDSMonitor");
  dout(0) << "Launched monitor thread " << dendl;
}

MDSMonitor::~MDSMonitor()
{
  m_runFlag = false;
  if (am_self()) {
    // suicide?
    // It's not safe to destruct an object with class derived from Thread class
  }
  else {
    // join();
    kill(SIGTERM);
  }
}

int MDSMonitor::iops()
{
  int cnt_now = mds->get_req_rate();
  m_last_iops = cnt_now - m_last_iocnt;
  m_last_iocnt = cnt_now;
  return m_last_iops;
}

mds_load_t MDSMonitor::mds_load()
{
  return mds->balancer->get_load(ceph_clock_now());
}

void MDSMonitor::writelog()
{
  std::stringstream ss;
  ss << "MDS_MONITOR IOPS " << iops(); // IOPS
  ss << " Inodes " << mds->mdcache->lru.lru_get_size(); // Cached inodes size
  mds_load_t load(mds_load());
  ss << " MDSLoad " << load;
  dout(0) << ss.str() << dendl;
}

void * MDSMonitor::entry()
{
  dout(0) << "MDS_MONITOR thread start." << dendl;
  while (m_runFlag) {
    sleep(1);
    writelog();
  }
  dout(0) << "MDS_MONITOR thread stop." << dendl;
  return NULL;
}

int MDSMonitor::terminate()
{
  m_runFlag = false;
  if (!am_self())
    return this->join();
  else
    return 0;
}

}; /* namespace: sxy */
