// Copyright(C) 1999-2017 National Technology & Engineering Solutions
// of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
// NTESS, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
//     * Neither the name of NTESS nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <exo_fac/Ioex_IOFactory.h> // for Ioex IOFactory

#include <exo_fpp/Iofx_DatabaseIO.h> // for Iofx DatabaseIO
#if defined(SEACAS_HAVE_MPI) && !defined(NO_DOF_EXODUS_SUPPORT)
#include <exo_par/Iopx_DatabaseIO.h> // for Iopx DatabaseIO
#endif
#include <tokenize.h>

#include <cstddef> // for nullptr
#include <exodusII.h>
#include <string> // for string

#include "Ioss_CodeTypes.h" // for MPI_Comm
#include "Ioss_DBUsage.h"   // for DatabaseUsage
#include "Ioss_IOFactory.h" // for IOFactory

namespace Ioss {
  class DatabaseIO;
} // namespace Ioss

#if defined(SEACAS_HAVE_MPI) && !defined(NO_DOF_EXODUS_SUPPORT)
namespace {
  std::string check_decomposition_property(const Ioss::PropertyManager &properties,
                                           Ioss::DatabaseUsage          db_usage);
  bool        check_composition_property(const Ioss::PropertyManager &properties,
                                         Ioss::DatabaseUsage          db_usage);
} // namespace
#endif

namespace Ioex {

  const IOFactory *IOFactory::factory()
  {
    static IOFactory registerThis;
    return &registerThis;
  }

  IOFactory::IOFactory() : Ioss::IOFactory("exodus")
  {
    Ioss::IOFactory::alias("exodus", "exodusii");
    Ioss::IOFactory::alias("exodus", "exodusII");
    Ioss::IOFactory::alias("exodus", "genesis");
#if defined(SEACAS_HAVE_MPI) && !defined(NO_DOF_EXODUS_SUPPORT)
    Ioss::IOFactory::alias("exodus", "dof_exodus");
    Ioss::IOFactory::alias("exodus", "dof");
#endif
  }

  Ioss::DatabaseIO *IOFactory::make_IO(const std::string &filename, Ioss::DatabaseUsage db_usage,
                                       MPI_Comm                     communicator,
                                       const Ioss::PropertyManager &properties) const
  {
#if defined(SEACAS_HAVE_MPI) && !defined(NO_DOF_EXODUS_SUPPORT)
    // The "exodus" and "parallel_exodus" databases can both be accessed
    // from this factory.  The "parallel_exodus" is returned only if the following
    // are true:
    // 0. The db_usage is 'READ_MODEL' (not officially supported for READ_RESTART yet)
    // 1. Parallel run with >1 processor
    // 2. There is a DECOMPOSITION_METHOD specified in 'properties'
    // 3. The decomposition method is not "EXTERNAL"

    int proc_count = 1;
    if (communicator != MPI_COMM_NULL) {
      MPI_Comm_size(communicator, &proc_count);
    }

    bool decompose = false;
    if (proc_count > 1) {
      if (db_usage == Ioss::READ_MODEL || db_usage == Ioss::READ_RESTART) {
        std::string method = check_decomposition_property(properties, db_usage);
        if (!method.empty() && method != "EXTERNAL") {
          decompose = true;
        }
      }
      else if (db_usage == Ioss::WRITE_RESULTS || db_usage == Ioss::WRITE_RESTART) {
        if (check_composition_property(properties, db_usage)) {
          decompose = true;
        }
      }
    }

    // Could call Iopx::DatabaseIO constructor directly, but that leads to some circular
    // dependencies and other yuks.
    if (decompose)
      return new Iopx::DatabaseIO(nullptr, filename, db_usage, communicator, properties);
    else
#endif
      return new Iofx::DatabaseIO(nullptr, filename, db_usage, communicator, properties);
  }

  void IOFactory::show_config() const { ex_print_config(); }
} // namespace Ioex

#if defined(SEACAS_HAVE_MPI) && !defined(NO_DOF_EXODUS_SUPPORT)
namespace {
  std::string check_decomposition_property(const Ioss::PropertyManager &properties,
                                           Ioss::DatabaseUsage          db_usage)
  {
    std::string decomp_method;
    std::string decomp_property;
    if (db_usage == Ioss::READ_MODEL) {
      decomp_property = "MODEL_DECOMPOSITION_METHOD";
    }
    else if (db_usage == Ioss::READ_RESTART) {
      decomp_property = "RESTART_DECOMPOSITION_METHOD";
    }

    // Applies to either read_model or read_restart
    if (properties.exists("DECOMPOSITION_METHOD")) {
      std::string method = properties.get("DECOMPOSITION_METHOD").get_string();
      return Ioss::Utils::uppercase(method);
    }

    // Check for property...
    if (properties.exists(decomp_property)) {
      std::string method = properties.get(decomp_property).get_string();
      return Ioss::Utils::uppercase(method);
    }
    return decomp_method;
  }

  bool check_composition_property(const Ioss::PropertyManager &properties,
                                  Ioss::DatabaseUsage          db_usage)
  {
    bool        compose          = false;
    std::string compose_property = "COMPOSE_INVALID";
    if (db_usage == Ioss::WRITE_RESULTS) {
      compose_property = "COMPOSE_RESULTS";
    }
    else if (db_usage == Ioss::WRITE_RESTART) {
      compose_property = "COMPOSE_RESTART";
    }

    Ioss::Utils::check_set_bool_property(properties, compose_property, compose);
    return compose;
  }
} // namespace
#endif
