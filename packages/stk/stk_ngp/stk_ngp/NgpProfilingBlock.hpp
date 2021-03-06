/*--------------------------------------------------------------------*/
/*    Copyright 2002 - 2008, 2010, 2011 National Technology &         */
/*    Engineering Solutions of Sandia, LLC (NTESS). Under the terms   */
/*    of Contract DE-NA0003525 with NTESS, there is a                 */
/*    non-exclusive license for use of this work by or on behalf      */
/*    of the U.S. Government.  Export of this program may require     */
/*    a license from the United States Government.                    */
/*--------------------------------------------------------------------*/
#ifndef STK_STK_NGP_STK_NGP_NGPPROFILINGBLOCK_H_
#define STK_STK_NGP_STK_NGP_NGPPROFILINGBLOCK_H_

#include <string>

namespace ngp
{

// This is an RAII helper for applying a Kokkos profiling region to a scope.
class ProfilingBlock
{
public:
  ProfilingBlock(const std::string & name);
  ~ProfilingBlock();
};
}

#endif /* STK_STK_NGP_STK_NGP_NGPPROFILINGBLOCK_H_ */
