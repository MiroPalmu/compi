doxygen = find_program('doxygen', version : '>=1.9.1', required : false)
if not doxygen.found()
    error('Skipping docs due to missing Doxygen')
    subdir_done()
endif

subdir('doxygen')


sphinx = find_program('sphinx-build', version : '>=4.3.2', required: false)

if not sphinx.found()
    error('Skipping docs due to missing Sphinx')
    subdir_done()
endif

# Check if Breathe is installed using python module of meson
breathe = import('python').find_installation(
    'python3',
    modules : ['breathe'],
    required : false
)

if not breathe.found()
    error('Skipping docs due to missing Breathe')
    subdir_done()
endif

subdir('sphinx')
