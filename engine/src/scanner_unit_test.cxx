#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "catch.hpp"
#include "scanner.hxx"

using Catch::Matchers::Contains;

TEST_CASE("scanner initialization test")
{
    om::scanner        scnr;
    om::scanner_report report = scnr.getReport();
    REQUIRE(report.is_initialized == true);
    REQUIRE(report.scan_perfomed == true);
    REQUIRE(report.total_files == 50);
    REQUIRE(report.total_folders == 8);
}

TEST_CASE("get_file_size test")
{
    om::scanner scnr;

    SECTION("valid request")
    {
        REQUIRE(om::get_file_size(&scnr, "game/game.cxx") == 100500);
        REQUIRE(om::get_file_size(&scnr, "appveyor.yml") == 123);
    }
    SECTION("invalid request")
    {
        REQUIRE_THROWS_WITH(om::get_file_size(&scnr, "engine/src/scanner"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::get_file_size(&scnr, "readme"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::get_file_size(&scnr, ""),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::get_file_size(&scnr, ".hxx"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::get_file_size(&scnr, "game/net.cxx"),
                            Contains("not found"));
        REQUIRE_THROWS_WITH(om::get_file_size(&scnr, "main.cxx"),
                            Contains("not found"));
    }
}

TEST_CASE("is_file_exists test")
{
    om::scanner scnr;
    SECTION("valid request")
    {
        REQUIRE(om::is_file_exists(&scnr, "game/game.cxx") == true);
        REQUIRE(om::is_file_exists(&scnr, "game/net.cxx") == false);
        REQUIRE(om::is_file_exists(&scnr, "appveyor.yml") == true);
        REQUIRE(om::is_file_exists(&scnr, "main.cxx") == false);
    }
    SECTION("invalid request")
    {
        REQUIRE_THROWS_WITH(om::is_file_exists(&scnr, "engine/src/scanner"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::is_file_exists(&scnr, "readme"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::is_file_exists(&scnr, ".md"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::is_file_exists(&scnr, ""),
                            Contains("no name or/and extension present"));
    }
}

TEST_CASE("get_all_files_with_extension test")
{
    om::scanner scnr;

    SECTION("valid request")
    {
        om::file_list inf;
        inf = om::get_all_files_with_extension(&scnr, "wtf", "engine/src");
        REQUIRE(inf.size() == 0);
        REQUIRE(inf.empty());
        inf = om::get_all_files_with_extension(&scnr, "cxx", "engine/src");
        REQUIRE(inf.size() == 2);
        REQUIRE_FALSE(inf.empty());
        inf = om::get_all_files_with_extension(&scnr, "yml", "");
        REQUIRE(inf.size() == 2);
        REQUIRE_FALSE(inf.empty());
    }
    SECTION("invalid request")
    {
        REQUIRE_THROWS_WITH(om::get_all_files_with_extension(
                                &scnr, "cxx", "engine/src/readme.md"),
                            Contains("path not found"));
        REQUIRE_THROWS_WITH(om::get_all_files_with_extension(
                                &scnr, "cxx", "engine/invalid_dir"),
                            Contains("path not found"));
        REQUIRE_THROWS_WITH(
            om::get_all_files_with_extension(&scnr, "", "engine/src"),
            Contains("no extension present"));
    }
}

TEST_CASE("get_all_files_with_name test")
{
    om::scanner scnr;

    SECTION("valid request")
    {

        om::file_list inf;
        inf = om::get_all_files_with_name(&scnr, "wtf", "engine/src");
        REQUIRE(inf.size() == 0);
        REQUIRE(inf.empty());
        inf = om::get_all_files_with_name(&scnr, "readme", "engine/src");
        REQUIRE(inf.size() == 1);
        REQUIRE_FALSE(inf.empty());
        inf = om::get_all_files_with_name(&scnr, "", "engine/src");
        REQUIRE(inf.size() == 8);
        REQUIRE_FALSE(inf.empty());
        inf = om::get_all_files_with_name(&scnr, "appveyor", "");
        REQUIRE(inf.size() == 1);
        REQUIRE_FALSE(inf.empty());
        inf = om::get_all_files_with_name(&scnr, "", "");
        REQUIRE(inf.size() == 6);
        REQUIRE_FALSE(inf.empty());
    }
    SECTION("invalid request")
    {
        REQUIRE_THROWS_WITH(om::get_all_files_with_name(&scnr, "readme",
                                                        "engine/src/readme.md"),
                            Contains("path not found"));
        REQUIRE_THROWS_WITH(
            om::get_all_files_with_name(&scnr, "readme", "engine/invalid_dir"),
            Contains("path not found"));
    }
}
