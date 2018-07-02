#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "experimental/filesystem"
#include "scanner.hxx"
#include <fstream>

namespace fs = std::experimental::filesystem;
using Catch::Matchers::Contains;

TEST_CASE("scanner test")
{
    fs::create_directories("om/engine/src/om");
    fs::create_directories("om/engine/src/scanner");
    fs::create_directories("om/game");

    std::ofstream fout("om/appveyor.yml");
    fout << "The quick brown fox jumps over the lazy dog";
    fout.close();
    fout.open("om/game/game.cxx");
    fout << "Who packed five dozen old quart jugs in my box?";
    fout.close();
    fout.open("om/engine/src/scanner.hxx");
    fout << "Grumpy wizards make a toxic brew for the jovial queen";
    fout.close();
    fout.open("om/engine/src/one.cxx");
    fout.close();
    fout.open("om/engine/src/one.hxx");
    fout.close();
    fout.open("om/engine/src/two.cxx");
    fout << "Hello World";
    fout.close();
    fout.open("om/readme.md");
    fout << "Few black taxis drive up major roads on quiet hazy nights";
    fout.close();

    SECTION("scanner initialization test")
    {

        om::scanner        first_scanner("om");
        om::scanner        second_scanner("om/engine");
        om::scanner_report first_scanner_report  = first_scanner.getReport();
        om::scanner_report second_scanner_report = second_scanner.getReport();

        REQUIRE(first_scanner_report.is_initialized == true);
        REQUIRE(first_scanner_report.scan_perfomed == true);
        REQUIRE(first_scanner_report.total_files == 7);
        REQUIRE(first_scanner_report.total_folders == 5);

        REQUIRE(second_scanner_report.is_initialized == true);
        REQUIRE(second_scanner_report.scan_perfomed == true);
        REQUIRE(second_scanner_report.total_files == 4);
        REQUIRE(second_scanner_report.total_folders == 3);
    }

    SECTION("get_file_size test")
    {
        om::scanner scnr("om");

        SECTION("valid request")
        {
            REQUIRE(scnr.get_file_size("game/game.cxx") == 47);
            REQUIRE(scnr.get_file_size("appveyor.yml") == 43);
        }
        SECTION("invalid request")
        {
            REQUIRE(scnr.get_file_size("engine/src/scanner") == -1);
            // no extension present
            REQUIRE(scnr.get_file_size("readme") == -1);
            // no extension present
            REQUIRE(scnr.get_file_size("") == -1);
            // no name and extension present
            REQUIRE(scnr.get_file_size(".hxx") == -1);
            //  no name present
            REQUIRE(scnr.get_file_size("game/net.cxx") == -1);
            // file not found
            REQUIRE(scnr.get_file_size("main.cxx") == -1);
            // file not found
        }
    }

    SECTION("is_file_exists test")
    {
        om::scanner scnr("om");

        SECTION("valid request")
        {
            REQUIRE(scnr.is_file_exists("game/game.cxx") == true);
            REQUIRE(scnr.is_file_exists("game/net.cxx") == false);
            REQUIRE(scnr.is_file_exists("appveyor.yml") == true);
            REQUIRE(scnr.is_file_exists("main.cxx") == false);
        }
        SECTION("invalid request")
        {
            REQUIRE(scnr.is_file_exists("engine/src/scanner") == false);
            // no extension present
            REQUIRE(scnr.is_file_exists("readme") == false);
            // no extension present
            REQUIRE(scnr.is_file_exists(".md") == false);
            // no name present
            REQUIRE(scnr.is_file_exists("") == false);
            // no name and extension present
        }
    }

    SECTION("get_all_files_with_extension test")
    {
        om::scanner scnr("om");

        SECTION("valid request")
        {
            om::file_list inf;

            inf = scnr.get_all_files_with_extension("wtf", "engine/src");
            REQUIRE(inf.size() == 0);
            REQUIRE(inf.empty());
            inf = scnr.get_all_files_with_extension("cxx", "engine/src");
            REQUIRE(inf.size() == 2);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_all_files_with_extension("yml", "");
            REQUIRE(inf.size() == 1);
            REQUIRE_FALSE(inf.empty());
        }
        SECTION("invalid request")
        {
            om::file_list inf;

            inf =
                scnr.get_all_files_with_extension("cxx", "engine/src/one.cxx");
            REQUIRE(inf.size() == 0);
            // incorrect path
            inf = scnr.get_all_files_with_extension("cxx", "engine/no_dir");
            REQUIRE(inf.size() == 0);
            // path not found
            inf = scnr.get_all_files_with_extension("", "engine/src");
            REQUIRE(inf.size() == 0);
            //  no extension present
        }
    }

    SECTION("get_all_files_with_name test")
    {
        om::scanner scnr("om");

        SECTION("valid request")
        {

            om::file_list inf;
            inf = scnr.get_all_files_with_name("wtf", "engine/src");
            REQUIRE(inf.size() == 0);
            REQUIRE(inf.empty());
            inf = scnr.get_all_files_with_name("one", "engine/src");
            REQUIRE(inf.size() == 1);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_all_files_with_name("", "engine/src");
            REQUIRE(inf.size() == 4);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_all_files_with_name("appveyor", "");
            REQUIRE(inf.size() == 1);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_all_files_with_name("", "");
            REQUIRE(inf.size() == 2);
            REQUIRE_FALSE(inf.empty());
        }
        SECTION("invalid request")
        {
            om::file_list inf;
            inf = scnr.get_all_files_with_name("readme",
                                               "engine/src/scanner.hxx");
            REQUIRE(inf.size() == 0);
            // incorrect path
            inf = scnr.get_all_files_with_name("readme", "engine/no_dir");
            // path no  found
            REQUIRE(inf.size() == 0);
        }
    }

    fs::remove_all("om");
}
