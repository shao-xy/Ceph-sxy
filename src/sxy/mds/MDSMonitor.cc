 // -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <sstream>

#include "MDSMonitor.h"

#include "common/debug.h"
#include "mds/MDSRank.h"
#include "mds/Server.h"
#include "mds/MDBalancer.h"

#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_mds
#undef dout_prefix
#define dout_prefix *_dout << "SXY.mds." << mds->get_nodeid() << ".mon "

namespace sxy {

FactorTracer::FactorTracer(MDSRank * mds, bool use_server, int factoridx)
  : DeltaTracerWatcher<int, int>(factoridx), mds(mds), use_server(use_server)
{
  init_now();
} 

int FactorTracer::check_now(int idx) {
  if (use_server)
    return mds->server->logger ? mds->server->logger->get(idx) : 0;
  else
    return mds->logger ? mds->logger->get(idx) : 0;
}

MDSMonitor::MDSMonitor(MDSRank * mds)
    : mds(mds), m_runFlag(true),
    iops_tracer(mds, false, l_mds_request),
    fwps_tracer(mds, false, l_mds_forward)
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

mds_load_t MDSMonitor::mds_load()
{
  return mds->balancer->get_load(ceph_clock_now());
}

void MDSMonitor::update_and_writelog()
{
  std::stringstream ss;
  ss << "MDS_MONITOR IOPS " << iops_tracer.get(true); // IOPS
  ss << " FWPS " << fwps_tracer.get(true); // FWPS
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
    update_and_writelog();
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
