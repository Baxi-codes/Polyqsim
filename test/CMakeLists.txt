configure_lit_site_cfg(
        ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
        ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
        MAIN_CONFIG
        ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
)

set(POLYQSIM_OPT_TEST_DEPENDS
        FileCheck count not
        polyqsim-opt
        )

add_lit_testsuite(check-polyqsim-opt "Running the polyqsim-opt regression tests"
        ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${POLYQSIM_OPT_TEST_DEPENDS}
        )
set_target_properties(check-polyqsim-opt PROPERTIES FOLDER "Tests")

add_lit_testsuites(POLYQSIM_OPT ${CMAKE_CURRENT_SOURCE_DIR} DEPENDS ${POLYQSIM_OPT_TEST_DEPENDS})
