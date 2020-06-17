//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Source file containing tests for RAJA workgroup enqueue.
///

#include "tests/test-workgroup-enqueue.hpp"

#if defined(RAJA_ENABLE_TBB)
using TBBBasicWorkGroupEnqueueTypes =
  Test< camp::cartesian_product< TBBExecPolicyList,
                                 TBBOrderPolicyList,
                                 TBBStoragePolicyList,
                                 IndexTypeTypeList,
                                 XargsTypeList,
                                 HostAllocatorList > >::Types;

INSTANTIATE_TYPED_TEST_SUITE_P(TBBBasicTest,
                               WorkGroupBasicEnqueueUnitTest,
                               TBBBasicWorkGroupEnqueueTypes);
#endif
