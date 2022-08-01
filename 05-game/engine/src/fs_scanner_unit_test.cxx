
#include <filesystem>
#include <fstream>
#include <limits>

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "fs_scanner.hxx"

#include <clocale>

namespace fs = std::filesystem;
using Catch::Matchers::Contains;

TEST_CASE("scanner test")
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    fs::path p1("test-folder/engine/src/om");
    fs::path p2("test-folder/engine/src/scanner/~.scanner");
    fs::path p3("test-folder/game/game.bkp");
    fs::path p4("test-folder/русский");

    fs::create_directories(p1);
    fs::create_directories(p2);
    fs::create_directories(p3);
    fs::create_directories(p4);

    std::ofstream fout("test-folder/appveyor.yml");
    fout << "The quick brown fox jumps over the lazy dog";
    fout.close();
    fout.open("test-folder/game/game.cxx");
    fout << "Who packed five dozen old quart jugs in my box?";
    fout.close();
    fout.open("test-folder/engine/src/scanner.hxx");
    fout << "Grumpy wizards make a toxic brew for the jovial queen";
    fout.close();
    fout.open("test-folder/engine/src/one.cxx");
    fout.close();
    fout.open("test-folder/engine/src/one.hxx");
    fout.close();
    fout.open("test-folder/engine/src/two.cxx");
    fout << "Hello World";
    fout.close();
    fout.open("test-folder/readme.md");
    fout << "Few black taxis drive up major roads on quiet hazy nights";
    fout.close();
    fout.open("test-folder/game/game.bkp/c++");
    fout << "C++ is a general-purpose programming language. It has imperative, "
            "object-oriented and generic programming features, while also "
            "providing facilities for low-level memory manipulation. ";
    fout.close();
    fout.open("test-folder/engine/src/scanner/~.scanner/.gitignore");
    fout << "Each line in a gitignore file specifies a pattern. When deciding "
            "whether to ignore a path, Git normally checks gitignore patterns "
            "from multiple sources, with the following order of precedence, "
            "from highest to lowest (within one level of precedence, the last "
            "matching pattern decides the outcome)";
    fout.close();
    fout.open("test-folder/русский/файл");
    fout << "Именованная область данных на носителе информации.";
    fout.close();

    SECTION("scanner initialization test")
    {
        fs::path current_path = fs::current_path();
        size_t   counter      = 0;
        for (auto p : fs::recursive_directory_iterator(current_path))
        {
            ++counter;
        }
        om::scanner first_scanner(u8"test-folder");
        om::scanner second_scanner(u8"test-folder/engine");
        // full path recognize check
        om::scanner third_scanner(current_path.generic_u8string());
        // empty path recognize check
        om::scanner        forth_scanner(u8"");
        om::scanner_report first_scanner_report  = first_scanner.get_report();
        om::scanner_report second_scanner_report = second_scanner.get_report();
        om::scanner_report third_scanner_report  = third_scanner.get_report();
        om::scanner_report forth_scanner_report  = forth_scanner.get_report();

        REQUIRE(first_scanner_report.initialized == true);
        REQUIRE(first_scanner_report.total_files == 10);
        REQUIRE(first_scanner_report.total_folders == 8);

        REQUIRE(second_scanner_report.initialized == true);
        REQUIRE(second_scanner_report.total_files == 5);
        REQUIRE(second_scanner_report.total_folders == 4);

        REQUIRE(third_scanner_report.initialized == true);
        REQUIRE((third_scanner_report.total_folders +
                 third_scanner_report.total_files) == counter);

        REQUIRE(forth_scanner_report.initialized == true);
        REQUIRE((forth_scanner_report.total_folders +
                 forth_scanner_report.total_files) == counter);
    }

    SECTION("get_file_size test")
    {
        om::scanner scnr(u8"test-folder");

        SECTION("valid request")
        {
            REQUIRE(scnr.get_file_size(u8"game/game.cxx") == 47);
            REQUIRE(scnr.get_file_size(u8"appveyor.yml") == 43);
            REQUIRE(scnr.get_file_size(u8"русский/файл") == 94);
            REQUIRE(scnr.get_file_size(
                        u8"engine/src/scanner/~.scanner/.gitignore") == 295);
            REQUIRE(scnr.get_file_size(u8"game/game.bkp/c++") == 183);
        }
        SECTION("invalid request")
        {
            size_t bad = std::numeric_limits<size_t>::max();
            REQUIRE(scnr.get_file_size(u8"engine/src/scanner") == bad);
            // no extension present
            REQUIRE(scnr.get_file_size(u8"readme") == bad);
            // no extension present
            REQUIRE(scnr.get_file_size(u8"") == bad);
            // no name and extension present
            REQUIRE(scnr.get_file_size(u8".hxx") == bad);
            //  no name present
            REQUIRE(scnr.get_file_size(u8"game/game.bkp") == bad);
            // file not found, game.bkp is a folder.
            REQUIRE(scnr.get_file_size(u8"main.cxx") == bad);
            // file not found
        }
    }

    SECTION("is_file_exists test")
    {
        om::scanner scnr(u8"test-folder");

        SECTION("valid request")
        {
            REQUIRE(scnr.is_file_exists(u8"game/game.cxx") == true);
            REQUIRE(scnr.is_file_exists(u8"game/game.bkp") == false);
            REQUIRE(scnr.is_file_exists(u8"appveyor.yml") == true);
            REQUIRE(scnr.is_file_exists(u8"main.cxx") == false);
            REQUIRE(scnr.is_file_exists(
                        u8"engine/src/scanner/~.scanner/.gitignore") == true);
            REQUIRE(scnr.is_file_exists(u8"русский/файл") == true);
        }
        SECTION("invalid request")
        {
            REQUIRE(scnr.is_file_exists(u8"engine/src/scanner") == false);
            // no such file as scanner, only has engine/src/scanner.hxx
            REQUIRE(scnr.is_file_exists(u8"readme") == false);
            // no such file as readme, only has readme.md
            REQUIRE(scnr.is_file_exists(u8".md") == false);
            // no such file as .md, only has readme.md
            REQUIRE(scnr.is_file_exists(u8"") == false);
            // no name and extension present
        }
    }

    SECTION("get_all_files_with_extension test")
    {
        om::scanner scnr(u8"test-folder");

        SECTION("valid request")
        {
            std::vector<om::file_info> inf;

            inf = scnr.get_files_with_extension(u8"engine/src", u8"wtf");
            REQUIRE(inf.size() == 0);
            REQUIRE(inf.empty());
            inf = scnr.get_files_with_extension(u8"engine/src", u8"cxx");
            REQUIRE(inf.size() == 2);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_files_with_extension(u8"", u8"yml");
            REQUIRE(inf.size() == 1);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_files_with_extension(u8"engine/src", u8"");
            REQUIRE(inf.size() == 0);
            inf = scnr.get_files_with_extension(u8"русский", u8"");
            REQUIRE(inf.size() == 1);
            inf = scnr.get_files_with_extension(u8"game", u8"bkp");
            REQUIRE(inf.size() == 0);
            inf = scnr.get_files_with_extension(u8"engine/src/scanner/~.scanner",
                                                u8"gitignore");
            REQUIRE(inf.size() == 0);
            inf = scnr.get_files_with_extension(u8"game/game.bkp", u8"");
            REQUIRE(inf.size() == 1);
        }
        SECTION("invalid request")
        {
            std::vector<om::file_info> inf;

            inf = scnr.get_files_with_extension(u8"engine/src/one.cxx", u8"cxx");
            REQUIRE(inf.size() == 0);
            // incorrect path, one.cxx is interpreted as a path's part.
            inf = scnr.get_files_with_extension(u8"engine/no_dir", u8"cxx");
            REQUIRE(inf.size() == 0);
            // path not exists

            /* XXX
             * inf = scnr.get_all_files_with_extension("engine//src","cxx");
             * REQUIRE(inf.size() == 0);
             * double "/" - This has implementation-dependent behavior.
             * std::filesystem and boost::filesystem can resolve "//" as
             * path separator, while dirent.h can not.
             */
        }
    }

    SECTION("get_all_files_with_name test")
    {
        om::scanner scnr(u8"test-folder");

        SECTION("valid request")
        {

            std::vector<om::file_info> inf;
            inf = scnr.get_files_with_name(u8"engine/src", u8"wtf");
            REQUIRE(inf.empty());
            inf = scnr.get_files_with_name(u8"engine/src", u8"one");
            REQUIRE(inf.size() == 2);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_files_with_name(u8"", u8"appveyor");
            REQUIRE(inf.size() == 1);
            REQUIRE_FALSE(inf.empty());
            inf = scnr.get_files_with_name(u8"engine/src/scanner/~.scanner",
                                           u8".gitignore");
            REQUIRE(inf.size() == 1);
            inf = scnr.get_files_with_name(u8"русский", u8"файл");
            REQUIRE(inf.size() == 1);
        }
        SECTION("invalid request")
        {
            std::vector<om::file_info> inf;
            inf = scnr.get_files_with_name(u8"engine/src/scanner.hxx", u8"readme");
            REQUIRE(inf.size() == 0);
            // incorrect path, scanner.hxx interpreted as a path's part
            inf = scnr.get_files_with_name(u8"engine/no_dir", u8"readme");
            // path not exists
            REQUIRE(inf.size() == 0);
            inf = scnr.get_files_with_name(u8"engine/src", u8"");
            // empty name. All the files DO have names. ".gitignore" - is name!
            REQUIRE(inf.size() == 0);
            inf = scnr.get_files_with_name(u8"", u8"");
            REQUIRE(inf.size() == 0);
        }
    }

    SECTION("get_all_files test")
    {
        om::scanner scnr(u8"test-folder");

        SECTION("valid request")
        {
            std::vector<om::file_info> inf;
            inf = scnr.get_files(u8"engine/src");
            REQUIRE(inf.size() == 4);
            inf = scnr.get_files(u8"");
            REQUIRE(inf.size() == 2);
            inf = scnr.get_files(u8"engine");
            REQUIRE(inf.size() == 0);
        }
        SECTION("invalid request")
        {
            std::vector<om::file_info> inf;
            inf = scnr.get_files(u8"//\nqwerty~=30 l,.-0k3///asd");
            REQUIRE(inf.size() == 0);
            // invalid input
            inf = scnr.get_files(u8"engine/no_dir");
            // path not exists
            REQUIRE(inf.size() == 0);
        }
    }

    fs::remove_all(u8"test-folder");
}
