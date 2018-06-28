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

TEST_CASE("getFileSize test")
{
    om::scanner scnr;
    SECTION("full  path")
    {
        REQUIRE_THROWS_WITH(
            om::getFileSize(&scnr, "C:/Windows/System32/kernel32"),
            Contains("no extension present"));
        REQUIRE_THROWS_WITH(
            om::getFileSize(&scnr, "C:/Windows/System32/kernel100.dll"),
            Contains("not found"));
        REQUIRE(om::getFileSize(&scnr, "C:/Windows/System32/kernel32.dll") ==
                100500);
    }
    SECTION("relative path from root directory")
    {
        REQUIRE_THROWS_WITH(om::getFileSize(&scnr, "engine/src/scanner"),
                            Contains("no extension present"));
        REQUIRE_THROWS_WITH(om::getFileSize(&scnr, "game/net.cxx"),
                            Contains("not found"));
        REQUIRE(om::getFileSize(&scnr, "game/game.cxx") == 100500);
    }
    SECTION("name only")
    {
        REQUIRE_THROWS_WITH(om::getFileSize(&scnr, "readme"),
                            Contains("no extension present"));
        REQUIRE_THROWS_WITH(om::getFileSize(&scnr, "main.cxx"),
                            Contains("not found"));
        REQUIRE(om::getFileSize(&scnr, "appveyor.yml") == 100500);
    }
    SECTION("empty string") { REQUIRE(om::getFileSize(&scnr, "") == false); }
}

TEST_CASE("isFileExists test")
{
    om::scanner scnr;
    SECTION("full  path")
    {
        REQUIRE_THROWS_WITH(
            om::isFileExists(&scnr, "C:/Windows/System32/kernel32"),
            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::isFileExists(&scnr, "C:/Windows/System32/.dll"),
                            Contains("no name or/and extension present"));
        REQUIRE(om::isFileExists(&scnr, "C:/Windows/System32/kernel32.dll") ==
                true);
        REQUIRE(om::isFileExists(&scnr, "C:/Windows/System32/kernel100.dll") ==
                false);
    }
    SECTION("relative path from root directory")
    {
        REQUIRE_THROWS_WITH(om::isFileExists(&scnr, "engine/src/scanner"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::isFileExists(&scnr, "engine/src/.cxx"),
                            Contains("no name or/and extension present"));
        REQUIRE(om::isFileExists(&scnr, "game/game.cxx") == true);
        REQUIRE(om::isFileExists(&scnr, "game/net.cxx") == false);
    }
    SECTION("name only")
    {
        REQUIRE_THROWS_WITH(om::isFileExists(&scnr, "readme"),
                            Contains("no name or/and extension present"));
        REQUIRE_THROWS_WITH(om::isFileExists(&scnr, ".md"),
                            Contains("no name or/and extension present"));
        REQUIRE(om::isFileExists(&scnr, "appveyor.yml") == true);
        REQUIRE(om::isFileExists(&scnr, "main.cxx") == false);
    }
    SECTION("empty string") { REQUIRE(om::isFileExists(&scnr, "") == false); }
}

TEST_CASE("getAllFilesWithExtension test")
{
    om::scanner scnr;
    SECTION("full  path")
    {
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithExtension(&scnr, "cxx",
                                         "C:/Windows/System32/kernel32.dll"),
            Contains("path not found"));
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithExtension(&scnr, "wtf", "C:/Windows/System32"),
            Contains("files not found"));
        om::file_list inf =
            om::getAllFilesWithExtension(&scnr, "dll", "C:/Windows/System32");
        REQUIRE(inf.size() == 100500);
        REQUIRE_FALSE(inf.empty());
    }
    SECTION("relative path from root directory")
    {
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithExtension(&scnr, "cxx", "engine/src/readme.md"),
            Contains("path not found"));
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithExtension(&scnr, "wtf", "engine/src"),
            Contains("files not found"));
        om::file_list inf =
            om::getAllFilesWithExtension(&scnr, "cxx", "engine/src");
        REQUIRE(inf.size() == 2);
        REQUIRE_FALSE(inf.empty());
    }
    SECTION("empty string")
    {
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithExtension(&scnr, "", "engine/src"),
            Contains("files not found"));
        om::file_list inf = om::getAllFilesWithExtension(&scnr, "yml", "");
        REQUIRE(inf.size() == 2);
        REQUIRE_FALSE(inf.empty());
    }
}

TEST_CASE("getAllFilesWithName test")
{
    om::scanner scnr;
    SECTION("full  path")
    {
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithName(&scnr, "main",
                                    "C:/Windows/System32/kernel32.dll"),
            Contains("path not found"));
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithName(&scnr, "wtf", "C:/Windows/System32"),
            Contains("files not found"));
        om::file_list inf =
            om::getAllFilesWithName(&scnr, "winlogon", "C:/Windows/System32");
        REQUIRE(inf.size() == 4);
        REQUIRE_FALSE(inf.empty());
    }
    SECTION("relative path from root directory")
    {
        REQUIRE_THROWS_WITH(
            om::getAllFilesWithName(&scnr, "readme", "engine/src/readme.md"),
            Contains("path not found"));
        REQUIRE_THROWS_WITH(om::getAllFilesWithName(&scnr, "wtf", "engine/src"),
                            Contains("files not found"));
        om::file_list inf =
            om::getAllFilesWithName(&scnr, "readme", "engine/src");
        REQUIRE(inf.size() == 1);
        REQUIRE_FALSE(inf.empty());
    }
    SECTION("empty string")
    {
        REQUIRE_THROWS_WITH(om::getAllFilesWithName(&scnr, "", "engine/src"),
                            Contains("files not found"));
        om::file_list inf = om::getAllFilesWithName(&scnr, "appveyor", "");
        REQUIRE(inf.size() == 1);
        REQUIRE_FALSE(inf.empty());
    }
}
