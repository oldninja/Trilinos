TRILINOS_DRIVER_SETUP()

TRILINOS_DRIVER_ADD_DASHBOARD(
  ZOLTAN_INT_IDS
  ctest_octopi_zoltan_int_ids.cmake
  CTEST_INSTALLER_TYPE release
)

TRILINOS_DRIVER_ADD_DASHBOARD(
  ZOLTAN_LONG_IDS
  ctest_octopi_zoltan_long_ids.cmake
  CTEST_INSTALLER_TYPE release
)

TRILINOS_DRIVER_ADD_DASHBOARD(
  ZOLTAN_LONGLONG_IDS
  ctest_octopi_zoltan_longlong_ids.cmake
  CTEST_INSTALLER_TYPE release
)

TRILINOS_DRIVER_ADD_DASHBOARD(
  ZOLTAN_PURIFY
  ctest_octopi_zoltan_purify.cmake
  CTEST_INSTALLER_TYPE release
)

TRILINOS_ADD_REQUIRED_CMAKE_INSTALLS()
