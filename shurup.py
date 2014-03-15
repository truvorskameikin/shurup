"""Unit test preprocessor"""

import os
import pickle
import re
import argparse

def get_modified_time(file_name):
    """Gets last modified time of a given file"""

    (_, _, _, _, _, _, _, _, mtime, _) = os.stat(file_name)
    return mtime

class CachedTests:
    """Recently processed test files"""

    VERSION = "5"

    def __init__(self):
        self.cached = {}

    def load(self, file_name):
        """Loads information about recently processed test files"""

        self.cached = {}

        try:
            with open(file_name) as in_file:
                content = pickle.load(in_file)

                if content.get("version", None) != self.VERSION:
                    return

                if "cached" in content:
                    self.cached = content["cached"]
        except IOError:
            pass

    def save(self, file_name):
        """Saves information about recently processed test files"""

        with open(file_name, "w+") as out_file:
            content = {"version": self.VERSION, "cached": self.cached}

            pickle.dump(content, out_file)

    def needs_processing(self, file_name):
        """Tests if given file needs to be processed"""

        cached_file = self.cached.get(file_name, None)
        if cached_file is None:
            return True

        mtime = get_modified_time(file_name)
        if cached_file["mtime"] < mtime:
            return True

        return False

    def add(self, file_name, mtime, test_cases):
        """Adds information about test cases for given file to cache"""

        self.cached[file_name] = {
            "mtime": mtime,
            "test_cases": test_cases
        }

    def get(self, file_name):
        """Returns tests for given file"""

        cached = self.cached.get(file_name, None)
        if not cached:
            return []

        return cached["test_cases"]

class Config:
    """Config"""

    def __init__(self):
        self.items = []

    def load_from_memory(self, memory):
        """Loads config from a content of a file"""

        self.items = []

        for line in memory.splitlines():
            line = line.strip()

            if line.startswith("#"):
                continue

            (path, sep, priority) = line.partition("=")

            if sep != "":
                try:
                    priority = int(priority.strip())
                except ValueError:
                    continue
            else:
                priority = 0

            path = path.strip()

            self.items.append({"path": path, "priority": priority})

        return True

    def load(self, file_name):
        """Loads config file"""

        try:
            with open(file_name) as in_file:
                return self.load_from_memory(in_file.read())
        except IOError:
            return False

    def include_items(self):
        """Returns items listed in config"""

        return self.items

def list_files(root_directory, expr):
    """Lists all files with given extensions"""

    pattern = re.compile(expr)

    def list_files_r(directory, root_priority, expr, files):
        """Lists all files with given extensions"""

        items = []

        config_file_name = directory + os.sep + ".shurup.conf"

        config = Config()
        if config.load(config_file_name):
            for i in config.include_items():
                items.append(i)
        else:
            for i in os.listdir(directory):
                items.append({"path": i, "priority": root_priority})

        for i in items:
            path = directory + os.sep + i["path"]
            priority = i["priority"]

            if os.path.isdir(path):
                list_files_r(path, priority, expr, files)
            if os.path.isfile(path):
                match = pattern.match(i["path"])
                if match:
                    files.append({"path": path, "priority": priority})

    result = []
    list_files_r(root_directory, 0, expr, result)

    return result

def list_files_in_directories(root_directories, expr):
    """Lists all files in directories"""

    result = []
    for directory in root_directories:
        result += list_files(directory, expr)

    return result

def read_test_from_line(line):
    """Reads test from single line"""

    pattern = re.compile(
        r"TEST_MAKE\s*\(\s*([a-zA-Z_][0-9a-zA-Z_]*)\s*,\s*([a-zA-Z_][0-9a-zA-Z_]*)\s*\)"
    )
    match = pattern.search(line)
    if not match:
        return None

    groups = match.groups()
    return {"test": groups[0], "case": groups[1]}

def read_tests_from_file(file_name):
    """Reads all test cases from given file"""

    tests = []

    with open(file_name) as in_file:
        for line in in_file.readlines():
            test = read_test_from_line(line)
            if test is None:
                continue

            tests.append(test)

    return tests

def format_main_file(all_tests):
    """Formats main file with RunTests function"""

    result = ""

    for test in all_tests:
        for case in test["cases"]:
            result += "TEST_DECLARE({0}, {1});\n".format(test["test"], case)

    result += "inline\n"
    result += "void RunTests(int argc = 0, char* argv[] = 0) {\n"
    result += "  shurup::Settings settings;\n"
    result += "  settings.ParseCommandLineArguments(argc, argv);\n"
    for test in all_tests:
        result += "  TEST_START_GROUP({0});\n".format(test["test"])
        for case in test["cases"]:
            result += "  TEST({0}, {1});\n".format(test["test"], case)
        result += "  TEST_END_GROUP({0});\n".format(test["test"])
    result += "}"

    return result

def process_tests(root_directories, expr, cache_file_name, out_file_name):
    """Processes test files and outputs *.cpp file with RunTests function"""

    cache = CachedTests()
    cache.load(cache_file_name)

    all_tests = []
    all_files = list_files_in_directories(root_directories, expr)
    has_new_test = False

    for test_file in all_files:
        file_name = test_file["path"]

        if cache.needs_processing(file_name):
            tests = read_tests_from_file(file_name)
            cache.add(file_name, get_modified_time(file_name), tests)

            for test in tests:
                test["priority"] = test_file["priority"]

            all_tests += tests

            has_new_test = True
        else:
            all_tests += cache.get(file_name)

    def find(list_to_search, cmp_func):
        """Finds list item, or None"""
        for item in list_to_search:
            if cmp_func(item):
                return item
        return None

    test_groups = []
    for test in all_tests:
        test_group = find(test_groups, lambda x: x["test"] == test["test"])
        if test_group is None:
            test_groups.append(
                {
                    "test": test["test"],
                    "priority": test["priority"],
                    "cases": [test["case"]]
                }
            )
        else:
            if test_group["priority"] < test["priority"]:
                test_group["priority"] = test["priority"]
            test_group["cases"].append(test["case"])

    all_tests = test_groups

    cmp_func = lambda x, y: cmp(y["priority"], x["priority"])
    all_tests = sorted(all_tests, cmp=cmp_func)

    write_new_file = False
    if has_new_test:
        write_new_file = True
    else:
        if not os.path.exists(out_file_name):
            write_new_file = True

    if write_new_file:
        body = format_main_file(all_tests)

        with open(out_file_name, "w+") as out_file:
            out_file.write(body)

    cache.save(cache_file_name)

def main():
    """Main function"""

    parser = argparse.ArgumentParser(
        description="Unit-test preprocessor"
    )

    default = ".+_[Tt]est.cpp"
    parser.add_argument(
        "-e",
        "--expr",
        default=default,
        help="Regexp for files with tests ({0})".format(default)
    )

    default = "tests_processed.cache"
    parser.add_argument(
        "-c",
        "--cache",
        default=default,
        help="Caching file ({0})".format(default)
    )

    default = "RunTests.h"
    parser.add_argument(
        "-o",
        "--out",
        default=default,
        help="Output file with RunTests function ({0})".format(default)
    )

    parser.add_argument(
        "root_directories",
        nargs="+",
        help="Directories to scan for tests"
    )

    args = vars(parser.parse_args())

    if "help" in args:
        print(parser.format_help())
        return

    root_directories = args["root_directories"]
    expr = args["expr"]
    cache_file = args["cache"]
    out_file_name = args["out"]

    process_tests(
        root_directories,
        expr,
        cache_file,
        out_file_name
    )

if __name__ == "__main__":
    main()