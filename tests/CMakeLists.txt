add_executable(mysql_unittest mysql_unittest.cc)
target_link_libraries(mysql_unittest mysqlcppconn boost_system)

add_executable(mysql_unittest2 mysql_unittest2.cc)
target_link_libraries(mysql_unittest2 mysqlcppconn)


add_executable(mysql_unittest3 mysql_unittest3.cc)
target_link_libraries(mysql_unittest3 mysqlcppconn)