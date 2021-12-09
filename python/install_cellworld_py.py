import httpimport
with httpimport.remote_repo(["cellworld_py_setup"], "https://raw.githubusercontent.com/germanespinosa/cellworld_py/master/"):
    import cellworld_py_setup
cellworld_py_setup.install(version="1.3", force=True)
