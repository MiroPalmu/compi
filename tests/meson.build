####################################################################################################
# Tests
####################################################################################################


single_threaded_unit_tests = [
    'test_unit_test'
]

single_threaded_test_names_and_exes = {}

foreach test_name : single_threaded_unit_tests
    single_threaded_test_names_and_exes += {test_name :
        executable(
            test_name,
            files(test_name + '.cpp'),
            include_directories: project_include_directories,
            dependencies: project_dependencies
        )
    }

endforeach

####################################################################################################
# Running tests
####################################################################################################

foreach test_name, test_exe : single_threaded_test_names_and_exes
    test(
        test_name,
        test_exe,
        is_parallel: true
    )
endforeach
